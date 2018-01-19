#ifndef PCMTYPE_H
#define PCMTYPE_H

#include <string>
#include <iostream>
#include <exception>

namespace VA
{

enum StreamState
{
      STREAM_CLOSED = 0,
      STREAM_STOPPED,
      STREAM_RUNNING,
      STREAM_PAUSED
};

struct StreamOptions
{
   int card; /*-2:use name, -1, use default, >=0:use card:x */
   int dev;
   int32_t format;
   int32_t sampleRate;
   int32_t channels;
   bool minimize_latency;
   int32_t num_frags; /* Number of stream buffers. */
   std::string name;
   
   // Default constructor.
   StreamOptions() :
      card(-1),
      dev(0),
      format(SND_PCM_SFMT_S16_LE),
      sampleRate(16000),
      channels(1),
      minimize_latency(false),
      num_frags(-1)
   {
   }
   
   // Copy constructor
   StreamOptions(const StreamOptions& src)
   {
      card = src.card;
      dev = src.dev;
      format = src.format;
      sampleRate = src.sampleRate;
      channels = src.channels;
      minimize_latency = src.minimize_latency;
      num_frags = src.num_frags;
      name = src.name;
   }
};

class PCMError : public std::exception
{
public:
   //! The constructor.
   PCMError(const std::string& message) throw () :
      message(message)
   {
   }
   
   //! The destructor.
   ~PCMError(void) throw ()
   {
   }
   
   //! Prints thrown error message to stderr.
   void printMessage(void) const throw ()
   {
      std::cerr << message << std::endl;
   }
   
   //! Returns the thrown error message string.
   const std::string& getMessage(void) const throw ()
   {
      return message;
   }
   
   //! Returns the thrown error message as a c-style string.
   const char* what(void) const throw ()
   {
      return message.c_str();
   }

protected:
   std::string message;
};

} /* namespace VA*/
#endif
