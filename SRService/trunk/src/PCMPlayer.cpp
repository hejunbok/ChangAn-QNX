#include "PCMPlayer.h"

#define USE_AUDIO_MANAGER 0

namespace VA
{

PCMPlayer::PCMPlayer()
{
   stream_state = STREAM_CLOSED;
   stream_callback = NULL;
   user_data = NULL;
   pcm_handle = NULL;
   audioman_handle = 0;
   user_buffer = NULL;
   user_buffer_size = 0;
   xrun = false;
   thread_id = 0;
   thread_priority = 0;
   pthread_mutex_init(&mutex, NULL);
   pthread_condattr_t attr;
   pthread_condattr_init(&attr);
   pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
   pthread_cond_init(&runnable_cv, &attr);
   pthread_condattr_destroy(&attr);
}

PCMPlayer::~PCMPlayer()
{
   if (STREAM_RUNNING == stream_state || STREAM_PAUSED == stream_state)
   {
      StopStream();
   }
   
   if (STREAM_STOPPED == stream_state)
   {
      CloseStream();
   }
   
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&runnable_cv);
}

void PCMPlayer::OpenStream(StreamOptions options, int32_t *frag_size,
      PCMPlayCallback callback, void *userData)
{
   __label__ OpenStreamUnlock;
   std::cerr << "-->OpenStream" << std::endl;
   std::string errMessage;

#ifdef SCHED_RR
   int min = 0;
   int max = 0;
#endif
   if (STREAM_CLOSED != stream_state)
   {
      errMessage = "stream already opened";
      throw(PCMError(errMessage));
      return;
   }
   
   int rtn = 0;
   int card = -1;
   int dev = 0;

   snd_pcm_channel_info_t pi;
   snd_pcm_channel_params_t pp;
   snd_pcm_channel_setup_t setup;

   pthread_mutex_lock(&mutex);

   stream_options = options;
   stream_callback = callback;
   user_data = userData;
   if (!stream_callback)
   {
      errMessage = "callback can not be NULL";
      goto OpenStreamUnlock;
   }

#if USE_AUDIO_MANAGER
   char dev_name[256] = {0};
   strcpy(dev_name, "/dev/snd/pcmPreferredp");
   if ((rtn = audio_manager_snd_pcm_open_name(
	       AUDIO_TYPE_PUSH_TO_TALK/*AUDIO_TYPE_TEXT_TO_SPEECH*/,
	       &pcm_handle,
	       &audioman_handle,
	       dev_name,
	       SND_PCM_OPEN_PLAYBACK)) < 0)
   {
      errMessage = "audio_manager_snd_pcm_open_name error:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }
#else
   if (-1 == stream_options.card)
   {
      if ((rtn = snd_pcm_open_preferred(&pcm_handle, &card, &dev,
		  SND_PCM_OPEN_PLAYBACK)) < 0)
      {
	 errMessage = "snd_pcm_open_preferred error:";
	 errMessage += snd_strerror(rtn);
	 goto OpenStreamUnlock;
      }
   }
   else if (-2 == stream_options.card) 
   {
      snd_pcm_info_t info;
      if ((rtn = snd_pcm_open_name(&pcm_handle, stream_options.name.c_str(),
		  SND_PCM_OPEN_PLAYBACK)) < 0)
      {
	 errMessage = "snd_pcm_open_name error:";
	 errMessage += snd_strerror(rtn);
	 goto OpenStreamUnlock;
      }

      rtn = snd_pcm_info(pcm_handle, &info);
      card = info.card;
   }
   else if (0 <= stream_options.card)
   {
      if ((rtn = snd_pcm_open(&pcm_handle, stream_options.card,
		  stream_options.dev, SND_PCM_OPEN_PLAYBACK)) < 0)
      {
	 errMessage = "snd_pcm_open error:";
	 errMessage += snd_strerror(rtn);
	 goto OpenStreamUnlock;
      }
   }
   else
   {
      errMessage = "device number can not be negative";
      goto OpenStreamUnlock;
   }
#endif

   /* disabling mmap is not actually required in this example but it is included to
    * demonstrate how it is used when it is required.
    */
   if ((rtn = snd_pcm_plugin_set_disable(pcm_handle, PLUGIN_DISABLE_MMAP)) < 0)
   {
      snd_pcm_close(pcm_handle);
      errMessage = "snd_pcm_plugin_set_disable failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }

   memset(&pi, 0, sizeof(pi));
   pi.channel = SND_PCM_CHANNEL_PLAYBACK;
   if ((rtn = snd_pcm_plugin_info(pcm_handle, &pi)) < 0)
   {
      snd_pcm_close(pcm_handle);
      errMessage = "snd_pcm_plugin_info failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }

   memset(&pp, 0, sizeof(pp));
   pp.mode = SND_PCM_MODE_BLOCK;
   pp.channel = SND_PCM_CHANNEL_PLAYBACK;
   pp.start_mode = SND_PCM_START_FULL;
   pp.stop_mode = SND_PCM_STOP_STOP;

   pp.buf.block.frag_size = pi.max_fragment_size;
   if (*frag_size > 0 && *frag_size < pi.max_fragment_size)
   {
      pp.buf.block.frag_size = *frag_size;
   }
   *frag_size = pp.buf.block.frag_size;

   if (stream_options.minimize_latency)
   {
      pp.buf.block.frags_max = 2;
   }
   else
   {
      if (stream_options.num_frags > 4)
      {
	 pp.buf.block.frags_max = stream_options.num_frags;
      }
      else
      {
	 pp.buf.block.frags_max = 4;  // a fairly safe default value
      }
   }
   
   pp.buf.block.frags_min = 1;

   pp.format.interleave = 1;
   pp.format.rate = stream_options.sampleRate;
   pp.format.voices = stream_options.channels;
   pp.format.format = stream_options.format;

   strcpy(pp.sw_mixer_subchn_name, "Wave playback channel");
   if ((rtn = snd_pcm_plugin_params(pcm_handle, &pp)) < 0)
   {
      snd_pcm_close(pcm_handle);
      errMessage = "snd_pcm_plugin_params failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }

   if ((rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
   {
      snd_pcm_close(pcm_handle);
      errMessage = "snd_pcm_plugin_prepare failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }
   
   memset(&setup, 0, sizeof(setup));

   setup.channel = SND_PCM_CHANNEL_PLAYBACK;

   if ((rtn = snd_pcm_plugin_setup(pcm_handle, &setup)) < 0)
   {
      snd_pcm_close(pcm_handle);
      errMessage = "snd_pcm_plugin_setup failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }

   std::cout << "Format " << snd_pcm_get_format_name(setup.format.format) << std::endl;
   std::cout << "Frag Size " << setup.buf.block.frag_size << std::endl;
   std::cout << "Total Frags " <<  setup.buf.block.frags << std::endl;
   std::cout << "Rate " << setup.format.rate << std::endl;
   std::cout << "Voices " << setup.format.voices << std::endl;

   user_buffer_size = setup.buf.block.frag_size;
   user_buffer = new char[user_buffer_size];

   stream_state = STREAM_STOPPED;

   // Set the thread attributes for joinable and realtime scheduling
   // priority (optional).  The higher priority will only take affect
   // if the program is run as root or suid. Note, under Linux
   // processes with CAP_SYS_NICE privilege, a user can change
   // scheduling policy and priority (thus need not be root). See
   // POSIX "capabilities".
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

#ifdef SCHED_RR // Undefined with some OSes (eg: NetBSD 1.6.x with GNU Pthread)
   min = sched_get_priority_min(SCHED_RR);
   max = sched_get_priority_max(SCHED_RR);
   if (thread_priority < min)
   {
      thread_priority = min;
   }
   else if (thread_priority > max)
   {
      thread_priority = max;
   }
#endif
   rtn = pthread_create(&thread_id, &attr, threadhandler, this);
   pthread_attr_destroy(&attr);
   if (rtn)
   {
      errMessage = "error creating callback thread!";
      goto OpenStreamUnlock;
   }

OpenStreamUnlock:
   pthread_mutex_unlock(&mutex);
   if (!errMessage.empty())
   {
      throw(PCMError(errMessage));
   }
   
   return;
}

void PCMPlayer::CloseStream() 
{
   std::cerr << "-->CloseStream" << std::endl;
   if (STREAM_CLOSED == stream_state)
   {
      std::cerr << "no open stream to close!" << std::endl;
      return;
   }
   if (STREAM_STOPPED != stream_state)
   {
      StopStream();
   }
   
   stream_state = STREAM_CLOSED;
   pthread_cond_signal(&runnable_cv);
   pthread_join(thread_id, NULL);
   snd_pcm_plugin_playback_drain(pcm_handle);
   snd_pcm_close(pcm_handle);

#if USE_AUDIO_MANAGER
   audio_manager_free_handle(audioman_handle);
#endif

   if (user_buffer)
   {
      delete[] user_buffer;
      user_buffer = NULL;
   }
   stream_state = STREAM_CLOSED;
}

void PCMPlayer::StartStream()
{
   std::cerr << "-->StartStream" << std::endl;
   if (STREAM_CLOSED == stream_state)
   {
      std::cerr << "no open stream!" << std::endl;
      return;
   }
   
   if (STREAM_RUNNING == stream_state || STREAM_PAUSED == stream_state)
   {
      std::cerr << "the stream is already running!" << std::endl;
      return;
   }
   
   pthread_mutex_lock(&mutex);
   int rtn = 0;
   snd_pcm_channel_status_t status;
   memset(&status, 0, sizeof(status));
   status.channel = SND_PCM_CHANNEL_PLAYBACK;
   if (snd_pcm_plugin_status(pcm_handle, &status) < 0)
   {
      std::cerr << "snd_pcm_plugin_status: playback channel status error" << std::endl;
   }
   
   if (status.status != SND_PCM_STATUS_PREPARED)
   {
      rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
      if (rtn < 0)
      {
	 std::cerr << "snd_pcm_plugin_prepare: playback channel prepare error: "
	    << snd_strerror(rtn) << std::endl;
      }
   }
   
   stream_state = STREAM_RUNNING;
   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&runnable_cv);
}

void PCMPlayer::PauseStream()
{
   std::cerr << "-->PauseStream" << std::endl;

   if (STREAM_RUNNING != stream_state)
   {
      std::cerr << "the stream is not running!" << std::endl;
      return;
   }

   stream_state = STREAM_PAUSED;
   pthread_mutex_lock(&mutex);
   int result = 0;
   result = snd_pcm_playback_pause(pcm_handle);
   if (result < 0)
   {
      std::cerr << "error pause output pcm device: "
	 << snd_strerror(result) << std::endl;
   }
   pthread_mutex_unlock(&mutex);
}

void PCMPlayer::ResumeStream()
{
   std::cerr << "-->ResumeStream" << std::endl;
   if (STREAM_PAUSED != stream_state)
   {
      std::cerr << "the stream is not paused!" << std::endl;
      return;
   }

   stream_state = STREAM_RUNNING;
   pthread_mutex_lock(&mutex);
   int result = 0;
   result = snd_pcm_playback_resume(pcm_handle);
   if (result < 0)
   {
      std::cerr << "error resume output pcm device: "
	 << snd_strerror(result) << std::endl;
   }
   
   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&runnable_cv);
}

void PCMPlayer::StopStream(bool synchronized)
{
   std::cerr << "-->StopStream" << std::endl;

   if (STREAM_CLOSED == stream_state)
   {
      std::cerr << "no open stream!" << std::endl;
      return;
   }

   if (STREAM_STOPPED == stream_state)
   {
      std::cerr << "the stream is already stopped!" << std::endl;
      return;
   }
   
   stream_state = STREAM_STOPPED;
   pthread_cond_signal(&runnable_cv);
   pthread_mutex_lock(&mutex);
   int result = 0;
   if (synchronized)
   {
      result = snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
   }
   else
   {
      result = snd_pcm_plugin_playback_drain(pcm_handle);
   }
   if (result < 0)
   {
      std::cerr << "error draining output pcm device: "
	 << snd_strerror(result) << std::endl;
   }
   
   pthread_mutex_unlock(&mutex);
}

void* PCMPlayer::threadhandler(void * ptr) 
{
   PCMPlayer *pThis = (PCMPlayer *)ptr;

#ifdef SCHED_RR // Undefined with some OSes (eg: NetBSD 1.6.x with GNU Pthread)
   pthread_t tID = pthread_self();  // ID of this thread
   sched_param prio = { pThis->thread_priority }; // scheduling priority of thread
   pthread_setschedparam(tID, SCHED_RR, &prio);
#endif
   pThis->thread_working_proc();
   return (NULL);
}

void PCMPlayer::thread_working_proc()
{
   while (STREAM_CLOSED != stream_state)
   {
      __label__ WorkingUnlock;
      pthread_testcancel();
      if (STREAM_RUNNING != stream_state)
      {
	 pthread_mutex_lock(&mutex);
	 std::cerr << "in wait" << std::endl;
	 pthread_cond_wait(&runnable_cv, &mutex);
	 std::cerr << "out wait" << std::endl;
	 if (STREAM_CLOSED == stream_state)
	 {
	    pthread_mutex_unlock(&mutex);
	    return;
	 }
	 pthread_mutex_unlock(&mutex);
      }
      
      if (STREAM_CLOSED == stream_state)
      {
	 std::cerr << "the stream is closed ... this shouldn't happen!" << std::endl;
	 return;
      }

      int doStopStream = 0;
      int rtn = 0;
      doStopStream = stream_callback(user_buffer, user_buffer_size, xrun, user_data);
      xrun = false;
      if (doStopStream == -2)
      {
	 StopStream();  //立刻停止
      } else if (doStopStream == -1)
      {
	 StopStream(false);  //播放完停止
      }
      else
      {
	 snd_pcm_channel_status_t status;
	 memset(&status, 0, sizeof(status));
	 status.channel = SND_PCM_CHANNEL_PLAYBACK;
	 pthread_mutex_lock(&mutex);
			
	 // The state might change while waiting on a mutex.
	 if (STREAM_STOPPED == stream_state || STREAM_PAUSED == stream_state)
	 {
            goto WorkingUnlock;
	 }
	 
	 snd_pcm_plugin_status(pcm_handle, &status);
	 if (SND_PCM_STATUS_UNDERRUN == status.status
	       || SND_PCM_STATUS_READY == status.status)
	 {
	    xrun = true;
	    std::cerr << "preparing device after resume" << std::endl;
	    rtn = snd_pcm_plugin_prepare(pcm_handle,SND_PCM_CHANNEL_PLAYBACK);
	    if (rtn < 0)
	    {
	       std::cerr << "error preparing device after resume: "
		  << snd_strerror(rtn) << std::endl;
	    }
	 }
	 else if (SND_PCM_STATUS_RUNNING != status.status
	       && SND_PCM_STATUS_PREPARED != status.status)
	 {
	    std::cerr << "error, current state is " << status.status << ", "
	       << snd_strerror(rtn) << std::endl;
	 }
	 rtn = snd_pcm_plugin_write(pcm_handle, user_buffer, doStopStream);
	 if (rtn < 0)
	 {
	    snd_pcm_plugin_status(pcm_handle, &status);
	    if (SND_PCM_STATUS_UNDERRUN == status.status)
	    {
	       xrun = true;
	       rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
	       if (rtn < 0)
	       {
		  std::cerr << "error preparing device after underrun: "
		     << snd_strerror(rtn) << std::endl;
	       }
	    }
	    else
	    {
	       std::cerr << "error, current state is " << status.status << ", "
		  << snd_strerror(rtn) << std::endl;
	    }
	    goto WorkingUnlock;
	 }

WorkingUnlock: pthread_mutex_unlock(&mutex);
      }
   }
}

} /* namespace VA */
