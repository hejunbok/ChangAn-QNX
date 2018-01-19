#ifndef CPCMRECORDER_H
#define CPCMRECORDER_H
#include <sys/asoundlib.h>
#include <audio/audio_manager_routing.h>
#include <pthread.h>
#include <sched.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <string>
#include <exception>
#include <sys/time.h>

#include "PCMType.h"

namespace VA
{

//return -2 to stop
typedef int (*PCMRecordCallback)(void *Buffer, int32_t BufferSize, bool xrun,
      void *userData);

class PCMRecorder
{
   volatile StreamState stream_state;
   PCMRecordCallback stream_callback;
   void* user_data;
   pthread_mutex_t mutex;
   pthread_cond_t runnable_cv;

   snd_pcm_t* pcm_handle;
   unsigned int audioman_handle;
   StreamOptions stream_options;
   char* user_buffer;
   int32_t user_buffer_size;
   volatile bool xrun;

   pthread_t thread_id;
   int thread_priority;

   static void *threadhandler(void * ptr);
   void thread_working_proc();

public:
   PCMRecorder();
   ~PCMRecorder();

   void OpenStream(StreamOptions options, int32_t *frag_size,
	 PCMRecordCallback callback, void *userData = NULL);
   void CloseStream();
   void StartStream();
   void StopStream();
   StreamState GetStreamState();
};

} /* namespace VA */

#endif /* CPCMRECORDER_H */
