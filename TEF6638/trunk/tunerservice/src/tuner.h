#ifndef TUNER_H
#define TUNER_H

#include "FmHardwareInterface.h"

class tuner : public FmHardwareInterface
{
public:
   tuner();
   virtual ~tuner();

   virtual int open();
   virtual int close();
   virtual int mute();
   virtual int unmute();
   virtual int setBand(int band);
   virtual int getBand();
   virtual int setFreq(int freq);
   virtual int getFreq(int freqInfo[10]);
   virtual int seek(int direction);
   virtual int setDeEmphasisFilter(int filter);
   virtual int getDeEmphasisFilter();
   virtual int getRssi();
   virtual int setRssiThreshold(int threshold);
   virtual int getRssiThreshold();
   virtual int setChannelSpace(int space);
   virtual int getChannelSpace();
   virtual int setSeekStatus(int status);
   virtual int getSeekStatus();

private:
   int getFmAmBand();
   int standbyMode();
   int presetMode();
   int searchMode();
   int endMode();
   int checkStation();
   int getStatusAndQuality();
   int setBandParameters();

private:
   int mCurrentBand;
   int mCurrentFreq[2];
   int mSeekDirection;
   int mDeEmphasisFilter[2];
   int mRssiThreshold[2];
   int mUsnThreshold[2];
   int mWamThreshold[2];
   int mFofThreshold[2];
   int mChannelSpace[2];
   int mSeekStatus;
   int mRSSI;
   int mUSN;
   int mWAM;
   int mFOF;
   int mSTIN;
};

#endif
