#ifndef FMHARDWAREINTERFACE_H
#define FMHARDWAREINTERFACE_H

class FmHardwareInterface
{
public:
   virtual ~FmHardwareInterface(){}

   virtual int open() = 0;
   virtual int close() = 0;
   virtual int mute() = 0;
   virtual int unmute() = 0;
   virtual int setBand(int band) = 0;
   virtual int getBand() = 0;
   virtual int setFreq(int freq) = 0;
   virtual int getFreq(int freqInfo[10]) = 0;
   virtual int seek(int direction) = 0;
   virtual int setDeEmphasisFilter(int filter) = 0;
   virtual int getDeEmphasisFilter() = 0;
   virtual int getRssi() = 0;
   virtual int setRssiThreshold(int threshold) = 0;
   virtual int getRssiThreshold() = 0;
   virtual int setChannelSpace(int space) = 0;
   virtual int getChannelSpace() = 0;
   virtual int setSeekStatus(int status) = 0;
   virtual int getSeekStatus() = 0;

   static FmHardwareInterface* create();
};

#endif
