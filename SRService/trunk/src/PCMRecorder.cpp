#include "PCMRecorder.h"

#define USE_AUDIO_MANAGER 0

namespace VA
{

PCMRecorder::PCMRecorder()
{
   stream_state = STREAM_CLOSED;
   stream_callback = NULL;
   user_data = NULL;
   pcm_handle = NULL;
   audioman_handle = 0;
   user_buffer = new char[1024*1024*16];//NULL;
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

   // Set the thread attributes for joinable and realtime scheduling
   // priority (optional).  The higher priority will only take affect
   // if the program is run as root or suid. Note, under Linux
   // processes with CAP_SYS_NICE privilege, a user can change
   // scheduling policy and priority (thus need not be root). See
   // POSIX "capabilities".
   int rtn = 0;
   pthread_attr_t pattr;
   pthread_attr_init(&pattr);
   pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);

#ifdef SCHED_RR // Undefined with some OSes (eg: NetBSD 1.6.x with GNU Pthread)
   int min = 0;
   int max = 0;
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
   rtn = pthread_create(&thread_id, &pattr, threadhandler, this);
   pthread_attr_destroy(&pattr);
   if (rtn)
   {
      std::cerr << "error creating callback thread!" << std::endl;
   }
}

PCMRecorder::~PCMRecorder()
{
   std::cout << "PCMRecorder::~PCMRecorder()" << std::endl;

   if (STREAM_RUNNING == stream_state || STREAM_PAUSED == stream_state)
   {
      StopStream();
   }
   
   if (STREAM_STOPPED == stream_state)
   {
      CloseStream();
   }

   pthread_kill(thread_id, SIGKILL);
   pthread_join(thread_id, NULL);

   if (user_buffer)
   {
      delete[] user_buffer;
      user_buffer = NULL;
   }
   
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&runnable_cv);
}

void PCMRecorder::OpenStream(StreamOptions options, int32_t *frag_size,
      PCMRecordCallback callback, void *userData)
{
   __label__ OpenStreamUnlock;
   std::cerr << "-->PCMRecorder OpenStream" << std::endl;
   std::string errMessage;

   if (STREAM_CLOSED != stream_state)
   {
      CloseStream();
   }

   int rtn = 0;
   int card = -1;
   int dev = 0;
   int fsize = 0;
   snd_pcm_channel_info_t pi;
   snd_mixer_group_t group;
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
   strcpy(dev_name, "/dev/snd/pcmPreferredc");
   if ((rtn = audio_manager_snd_pcm_open_name(AUDIO_TYPE_VOICE_RECOGNITION,
	       &pcm_handle,
	       &audioman_handle,
	       dev_name,
	       SND_PCM_OPEN_CAPTURE)) < 0)
   {
      errMessage = "audio_manager_snd_pcm_open_name error:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }
#else
   if (-1 == stream_options.card)
   {
      if ((rtn = snd_pcm_open_preferred(&pcm_handle, &card, &dev,
		  SND_PCM_OPEN_CAPTURE)) < 0)
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
		  SND_PCM_OPEN_CAPTURE)) < 0)
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
		  stream_options.dev, SND_PCM_OPEN_CAPTURE)) < 0)
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

   if ((rtn = snd_pcm_plugin_set_disable(pcm_handle, PLUGIN_DISABLE_MMAP)) < 0)
   {
      snd_pcm_close(pcm_handle);
      pcm_handle = NULL;
      errMessage = "snd_pcm_plugin_set_disable failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }
	
   memset(&pi, 0, sizeof(pi));
   pi.channel = SND_PCM_CHANNEL_CAPTURE;
   snd_pcm_plugin_info(pcm_handle, &pi);
	
   memset(&pp, 0, sizeof(pp));
   pp.mode = SND_PCM_MODE_BLOCK;
   pp.channel = SND_PCM_CHANNEL_CAPTURE;
   pp.start_mode = SND_PCM_START_DATA;
   pp.stop_mode = SND_PCM_STOP_STOP;
   pp.time = 1;
   pp.buf.block.frag_size = pi.max_fragment_size;
#if 0
   if (stream_options.minimize_latency)
   {
      pp.buf.block.frags_max = 2;
   }
   else
   {
      if (stream_options.num_frags > 0)
      {
	 pp.buf.block.frags_max = stream_options.num_frags;
      }
      else
      {
	 pp.buf.block.frags_max = -1;
      }
   }
#else
   pp.buf.block.frags_max = -1;
#endif
   pp.buf.block.frags_min = 1;

   pp.format.interleave = 1;
   pp.format.rate = stream_options.sampleRate;//16000;
   pp.format.voices = stream_options.channels;//1; //mono
   pp.format.format = stream_options.format;//SND_PCM_SFMT_S16_LE;
	
   snd_pcm_plugin_set_src_method(pcm_handle, 0);
   if ((rtn = snd_pcm_plugin_params(pcm_handle, &pp)) < 0)
   {
      snd_pcm_close(pcm_handle);
      pcm_handle = NULL;
      errMessage = "snd_pcm_plugin_params failed:";
      memset(&pp, 0, sizeof(pp));
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }
	
   memset (&setup, 0, sizeof (setup));
   memset (&group, 0, sizeof (group));
   setup.channel = SND_PCM_CHANNEL_CAPTURE;
   setup.mixer_gid = &group.gid;
   if ((rtn = snd_pcm_plugin_setup(pcm_handle, &setup)) < 0)
   {
      snd_pcm_close(pcm_handle);
      pcm_handle = NULL;
      errMessage = "snd_pcm_plugin_setup failed:";
      errMessage += snd_strerror(rtn);
      goto OpenStreamUnlock;
   }

   std::cout << "Format " << snd_pcm_get_format_name(setup.format.format) << std::endl;
   std::cout << "Frag Size " << setup.buf.block.frag_size << std::endl;
   std::cout << "Total Frags " << setup.buf.block.frags << std::endl;
   std::cout << "Rate " << setup.format.rate << std::endl;
   std::cout << "Voices " << setup.format.voices << std::endl;

   //user_buffer_size = setup.buf.block.frag_size;

   //pthread_mutex_lock(&mutex);
   user_buffer_size = setup.buf.block.frag_size;

   //if (setup.buf.block.frag_size > (1024*1024*16))
   //{
   //   delete[] user_buffer;
    //  user_buffer = NULL;
    //  user_buffer = new char[user_buffer_size];
   //}
   //pthread_mutex_unlock(&mutex);

   stream_state = STREAM_STOPPED;

#if 0
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
#endif

OpenStreamUnlock:
   pthread_mutex_unlock(&mutex);
   if (!errMessage.empty())
   {
      throw(PCMError(errMessage));
   }
   
   return;
}

void PCMRecorder::CloseStream()
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

   pthread_mutex_lock(&mutex);

   if (pcm_handle)
   {
      snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_CAPTURE);
      snd_pcm_close(pcm_handle);
      pcm_handle = NULL;
   }
#if USE_AUDIO_MANAGER
   audio_manager_free_handle(audioman_handle);
#endif

   stream_state = STREAM_CLOSED;

   pthread_cond_signal(&runnable_cv);

   pthread_mutex_unlock(&mutex);
   
   std::cerr << "-->CloseStream end" << std::endl;
}

void PCMRecorder::StartStream()
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
   status.channel = SND_PCM_CHANNEL_CAPTURE;
   rtn = snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_CAPTURE); //clear all data in driver buffer
   if (snd_pcm_plugin_status(pcm_handle, &status) < 0)
   {
      std::cerr << "snd_pcm_plugin_status: record channel status error" << std::endl;
   }
   
   if (status.status != SND_PCM_STATUS_PREPARED)
   {
      rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_CAPTURE);
      if (rtn < 0)
      {
	 std::cerr << "snd_pcm_plugin_prepare: record channel prepare error: "
	    << snd_strerror(rtn) << std::endl;
      }
   }
   
   xrun = false;
   stream_state = STREAM_RUNNING;
   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&runnable_cv);
}

void PCMRecorder::StopStream()
{
   std::cerr << "-->StopStream" << std::endl;

   pthread_cond_signal(&runnable_cv);
   pthread_mutex_lock(&mutex);
   int result = 0;
   if (pcm_handle)
   {
      result = snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_CAPTURE);
      if (result < 0)
      {
         std::cerr << "error draining output pcm device: "
	    << snd_strerror(result) << std::endl;
      }
   }
   
   pthread_mutex_unlock(&mutex);

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

   CloseStream();
}

void* PCMRecorder::threadhandler(void * ptr)
{
   PCMRecorder *pThis = (PCMRecorder *)ptr;

#ifdef SCHED_RR // Undefined with some OSes (eg: NetBSD 1.6.x with GNU Pthread)
   pthread_t tID = pthread_self();  // ID of this thread
   sched_param prio = { pThis->thread_priority }; // scheduling priority of thread
   pthread_setschedparam(tID, SCHED_RR, &prio);
#endif
   
   pThis->thread_working_proc();
   
   return (NULL);
}

void PCMRecorder::thread_working_proc()
{
   while (1/*STREAM_CLOSED != stream_state*/)
   {
      //pthread_testcancel();

      pthread_mutex_lock(&mutex);

      if (STREAM_RUNNING != stream_state)
      {
	 //pthread_mutex_lock(&mutex);
	 std::cerr << "PCMRecorder in wait" << std::endl;
	 pthread_cond_wait(&runnable_cv, &mutex);

	 if (STREAM_RUNNING != stream_state || !pcm_handle)
	 {
	    pthread_mutex_unlock(&mutex);
	    continue;
	 }

	 snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_CAPTURE);
	 snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_CAPTURE);

	 std::cerr << "PCMRecorder out wait" << std::endl;

	 
	 //pthread_mutex_unlock(&mutex);
      }
      
      int doStopStream = 0;
      int rtn = 0;
      doStopStream = stream_callback(user_buffer, user_buffer_size, xrun, user_data); 

      xrun = false;
      if (doStopStream == -2)
      {
         pthread_mutex_unlock(&mutex);
	 StopStream();  //立刻停止
	 continue;
      }
      
      snd_pcm_channel_status_t status;
      memset(&status, 0, sizeof(status));
      status.channel = SND_PCM_CHANNEL_CAPTURE;
      //pthread_mutex_lock(&mutex);
      // The state might change while waiting on a mutex.
      if (STREAM_STOPPED == stream_state || STREAM_PAUSED == stream_state)
      {
	 goto WorkingUnlock;
      }

      snd_pcm_plugin_status(pcm_handle, &status);
      if (SND_PCM_STATUS_OVERRUN == status.status
	    || SND_PCM_STATUS_READY == status.status)
      {
	 //xrun = true;
	 std::cerr << "preparing device after resume" << std::endl;
	 rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_CAPTURE);
	 if (rtn < 0)
	 {
	    std::cerr << "error preparing device after resume: "
	       << snd_strerror(rtn) << std::endl;
	 }
      }
      else if (SND_PCM_STATUS_RUNNING != status.status
	    && SND_PCM_STATUS_PREPARED != status.status)
      {
	 std::cerr << "1:error, current state is " << status.status << ", "
	    << snd_strerror(rtn) << std::endl;
      }
     
      rtn = snd_pcm_plugin_read(pcm_handle, user_buffer, user_buffer_size);

      if (rtn < user_buffer_size)
      {
	 memset (&status, 0, sizeof (status));
	 snd_pcm_plugin_status(pcm_handle, &status);
	 if (status.status == SND_PCM_STATUS_READY
	       || SND_PCM_STATUS_OVERRUN == status.status)
	 {
	    //xrun = true;
	    rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_CAPTURE);
	    if (rtn < 0)
	    {
	       std::cerr << "error preparing device after overrun: "
		  << snd_strerror(rtn) << std::endl;
	    }
	 }
	 else
	 {
	    std::cerr << "2:error, current state is " << status.status << ", "
	       << snd_strerror(rtn) << std::endl;
	 }
	 
	 goto WorkingUnlock;
      }
      else
      {
	 xrun = true;
      }

WorkingUnlock:
      pthread_mutex_unlock(&mutex);
   }

   std::cerr << "PCMRecorder::threadhandler() end......" << std::endl;
}

StreamState PCMRecorder::GetStreamState()
{
   return stream_state;
}

} /* namespace VA */
