#include "tuner.h"
#include "i2c_io.h"

#include <unistd.h>

#define AM_STEP_9K   9  //AM step 9 KHz
#define AM_STEP_10K  10 //AM step 10 KHz
#define FM_STEP_100K 100 //FM step 100 KHz
#define FM_STEP_200K 200 //FM step 200 KHz

// HERO write subaddress 0x00--0x1F : FM/AM radio control, direct control
#define HERO_SUBADDRESS_MODE          (0x00)
#define HERO_SUBADDRESS_FREQM         (0x01)
#define HERO_SUBADDRESS_FREQL         (0x02)
#define HERO_SUBADDRESS_TUNER         (0x03)
#define HERO_SUBADDRESS_TUNER_OPT     (0x04)
#define HERO_SUBADDRESS_RADIO         (0x05)
#define HERO_SUBADDRESS_SIGNAL        (0x06)
#define HERO_SUBADDRESS_BWCTRL        (0x07)
#define HERO_SUBADDRESS_SPECIAL       (0x08)
#define HERO_SUBADDRESS_RDS           (0x09)
#define HERO_SUBADDRESS_LEVEL_OFFSET  (0x0C)
#define HERO_SUBADDRESS_SOFTMUTE_TIME (0x0D)
#define HERO_SUBADDRESS_SOFTMUTE_DET  (0x0E)
#define HERO_SUBADDRESS_SOFTMUTE_LEV  (0x0F)
#define HERO_SUBADDRESS_SOFTMUTE_N_M  (0x10)
#define HERO_SUBADDRESS_SOFTMUTE_LIM  (0x11)
#define HERO_SUBADDRESS_HIGHCUT_TIME  (0x12)
#define HERO_SUBADDRESS_HIGHCUT_DET   (0x13)
#define HERO_SUBADDRESS_HIGHCUT_LEV   (0x14)
#define HERO_SUBADDRESS_HIGHCUT_N_M   (0x15)
#define HERO_SUBADDRESS_HIGHCUT_LIM   (0x16)
#define HERO_SUBADDRESS_HIGHCUT_LOC   (0x17)
#define HERO_SUBADDRESS_STEREO_TIME   (0x18)
#define HERO_SUBADDRESS_STEREO_DET    (0x19)
#define HERO_SUBADDRESS_STEREO_LEV    (0x1A)
#define HERO_SUBADDRESS_STEREO_N_M    (0x1B)
#define HERO_SUBADDRESS_STEREO_LIM    (0x1C)
#define HERO_SUBADDRESS_STEREOHB_DET  (0x1D)
#define HERO_SUBADDRESS_STEREOHB_LNM  (0x1E)

//HERO write subaddress 0x20--0x3F : audio control, direct control
#define HERO_SUBADDRESS_P_INPUT (0x20)
#define HERO_SUBADDRESS_S_INPUT (0x28)
#define HERO_SUBADDRESS_SYSTEM  (0x3F)

//HERO write subaddress 0xA0--0xAF : audio special features, entry to additional control
#define HERO_SUBADDRESS_SPE_LIMITER (0xA6)
#define HERO_SUBADDRESS_SPE_AUDIO   (0xA9)

//HERO write subaddress 0xC0--0xCF : radio special features, entry to additional control
#define HERO_SUBADDRESS_SPE_GPIO_CONF  (0xC0)
#define HERO_SUBADDRESS_SPE_RADIO_CTRl (0xC2)
#define HERO_SUBADDRESS_SPE_SCANNING   (0xC3)
#define HERO_SUBADDRESS_SPE_RADIO_CONF (0xC9)

//HERO write subaddress 0xF2 : audio control, entry to low-level DSP registers
#define HERO_SUBADDRESS_AUDIO_DSP (0xF2)

// ctrl mode
#define CTRL_MODE_STANDBY   (0x00) //standby mode
#define CTRL_MODE_PRESET    (0x10) //tune to new program with short mute time
#define CTRL_MODE_SEARCH    (0x20) //tune to new program and stay muted
#define CTRL_MODE_AF_UPDATE (0x30) //tune to alternative frequency, store quality and tune back with inansudible mute
#define CTRL_MODE_JUMP      (0x40) //tune to alternative frequency with inaudible mute
#define CTRL_MODE_CHECK     (0x50) //tune to alternative frequency and stay muted
#define CTRL_MODE_END       (0x70) //release the mute of a search or check tune

#define BAND_PARAM_LEN 29

typedef enum _tuner_band {
  BAND_FM         = 0,
  BAND_AM_LW    = 1,
  BAND_AM_MW    = 2,
  BAND_AM_SW    = 3,
  BAND_WX         = 4,
  BAND_NUM_USED   = 5
} tuner_band_t;

typedef struct _tuner_freq_limit {
  int lower;
  int upper;
} tuner_freq_limit_t;


static tuner_freq_limit_t g_freq_limit[5] = {
  {87500, 108000},
  {144, 288},
  {880, 1710},
  {2300, 27000},
  {1, 7}
};

static uint8_t g_hero_defParameters[BAND_PARAM_LEN] = {
  0x00, //SA=0x03,TUNER
  0x00, //SA=0x04,TUNER_OPT
  0x4A, //SA=0x05, RADIO
  0x01, //SA=0x06, SIGNAL
  0x05, //SA=0x07, BWCTRL
  0x00, //SA=0x08, SPECIAL
  0x41, //SA=0x09, RDS
  0x00, //SA=0x0A, reserved
  0x00, //SA=0x0B, reserved
  0x30, //SA=0x0C, LEVEL_OFFSET
  0x65, //SA=0x0D, SOFTMUTE_TIME
  0x10, //SA=0x0E, SOFTMUTE_DET
  0x3A, //SA=0x0F, SOFTMUTE_LEV
  0x33, //SA=0x10, SOFTMUTE_N_M
  0x0C, //SA=0x11, SOFTMUTE_LIM
  0x6E, //SA=0x12, HIGHCUT_TIME
  0x35, //SA=0x13, HIGHCUT_DET
  0x33, //SA=0x14, HIGHCUT_LEV
  0x33, //SA=0x15, HIGHCUT_N_M
  0x02, //SA=0x16, HIGHCUT_LIM
  0x04, //SA=0x17, HIGHCUT_LOC
  0x72, //SA=0x18, STEREO_TIME
  0x3F, //SA=0x19, STEREO_DET
  0x4A, //SA=0x1A, STEREO_LEV
  0x33, //SA=0x1B, STEREO_N_M
  0x00, //SA=0x1C, STEREO_LIM
  0x3F, //SA=0x1D, STEREOHB_DET
  0x13, //SA=0x1E, STEREOHB_LNM
  0x00  //SA=0x1F, reserved
};

tuner::tuner()
{
  mCurrentBand = 0; //0 FM, 1 AM
  mCurrentFreq[0] = 107700; //107.7MHz
  mCurrentFreq[1] = 990; //990KHz
  mSeekDirection = 0; //0 forward, 1 backward
  mDeEmphasisFilter[0] = 1;
  mDeEmphasisFilter[1] = 2;

  mRssiThreshold[0] = 76;
  mRssiThreshold[1] = 126;

  mUsnThreshold[0] = 70;
  mUsnThreshold[1] = 70;

  mFofThreshold[0] = 10;
  mFofThreshold[1] = 15;

  mWamThreshold[0] = 60;
  mWamThreshold[1] = 60;

  mChannelSpace[0] = FM_STEP_100K;
  mChannelSpace[1] = AM_STEP_9K;

  mSeekStatus = 0; //0 next, 1 stop

  i2c_open();
}

tuner::~tuner()
{
  i2c_close();
}

int tuner::open()
{
  uint8_t data[2] = {0};
  int ret = 0;

  //start Active mode
  presetMode();

/*
  //enable audio output(s)
  data[0] = 0x28;
  data[1] = 0x00;
  i2c_write_high ( HERO_SUBADDRESS_SPE_AUDIO, data, 2 );

  data[0] = 0x32;
  data[1] = 0x00;
  i2c_write_high ( HERO_SUBADDRESS_SPE_AUDIO, data, 2 );

  data[0] = 0x33;
  data[1] = 0x00;
  i2c_write_high ( HERO_SUBADDRESS_SPE_AUDIO, data, 2 );
*/
  //Primary input select
  //data[0] = 0x00;
  //i2c_write_high ( HERO_SUBADDRESS_P_INPUT, data, 1 );

  return setBand ( mCurrentBand );
}

int tuner::close()
{
  return standbyMode();
}

int tuner::mute()
{
  //ADSP_Y_Mute_P = 0xF2433B
  uint8_t ydata[5] = {0xF2, 0x43, 0x3B,
                      0x00, 0x00
                     };

  return i2c_write ( ydata, 5 );
}

int tuner::unmute()
{
  //ADSP_Y_Mute_P = 0xF2433B
  uint8_t ydata[5] = {0xF2, 0x43, 0x3B,
                      0x07, 0xFF
                     };

  return i2c_write ( ydata, 5 );
}

int tuner::setBand ( int band )
{
  if ( band < 0 || band > 1 ) {
    return -1;
  }

  mCurrentBand = band;

  setBandParameters();

  return presetMode();
}

int tuner::getBand()
{
  return mCurrentBand;
}

int tuner::setFreq ( int freq )
{
  if ( mCurrentBand == BAND_FM ) {
    if ( ( freq < g_freq_limit[BAND_FM].lower ) || ( freq > g_freq_limit[BAND_FM].upper ) ) {
      return -1;
    }
  } else {
    if ( ( freq < g_freq_limit[BAND_AM_LW].lower || freq > g_freq_limit[BAND_AM_MW].upper )
         && ( freq < g_freq_limit[BAND_AM_MW].lower || freq > g_freq_limit[BAND_AM_MW].upper )
         && ( freq < g_freq_limit[BAND_AM_SW].lower || freq > g_freq_limit[BAND_AM_SW].upper ) ) {
      return -1;
    }

  }

  mCurrentFreq[mCurrentBand] = freq;

  return presetMode();
}

int tuner::getFreq ( int freqInfo[10] )
{
  if ( getStatusAndQuality() < 0 ) {
    return -1;
  }

  freqInfo[0] = mCurrentFreq[mCurrentBand];
  freqInfo[1] = mRSSI;
  freqInfo[2] = mUSN;
  freqInfo[3] = mSTIN;
  freqInfo[4] = mFOF;
  freqInfo[5] = mRSSI < mRssiThreshold[mCurrentBand] ? 0 : 1;

  return 0;
}

int tuner::seek ( int direction )
{
  int freq = 0;
  int stopFreq = mCurrentFreq[mCurrentBand];
  int band = getFmAmBand();
  bool repeat = false;

  while ( mSeekStatus != 1 ) {
    if ( direction == 0 ) {
      if ( mCurrentFreq[mCurrentBand] + mChannelSpace[mCurrentBand] > g_freq_limit[band].upper ) {
        mCurrentFreq[mCurrentBand] = g_freq_limit[band].lower;
        repeat = true;
      } else {
        mCurrentFreq[mCurrentBand] += mChannelSpace[mCurrentBand];
      }

      if ( repeat && mCurrentFreq[mCurrentBand] >= stopFreq ) { //repeat, stop seek
        break;
      }
    } else {
      if ( mCurrentFreq[mCurrentBand] - mChannelSpace[mCurrentBand] < g_freq_limit[band].lower ) {
        mCurrentFreq[mCurrentBand] = g_freq_limit[band].upper;
        repeat = true;
      } else {
        mCurrentFreq[mCurrentBand] -= mChannelSpace[mCurrentBand];
      }

      if ( repeat && mCurrentFreq[mCurrentBand] <= stopFreq ) { //repeat, stop seek
        break;
      }
    }

    searchMode();

    if ( checkStation() == 0 ) {
      freq = mCurrentFreq[mCurrentBand];
      break;
    }
  }

  return freq;
}

int tuner::setDeEmphasisFilter ( int filter )
{
  mDeEmphasisFilter[mCurrentBand] = filter;
  uint8_t data[1] = {0};
  data[0] = mCurrentBand == BAND_FM ? ( filter & 0x3 ) : ( 0x08 | ( filter & 0x3 ) );

  return i2c_write_high ( HERO_SUBADDRESS_SIGNAL, data, 1 );
}

int tuner::getDeEmphasisFilter()
{
  return mDeEmphasisFilter[mCurrentBand];
}

int tuner::getRssi()
{
  int rssi = -1;

  if ( getStatusAndQuality() == 0 ) {
    rssi = mRSSI;
  }

  return rssi;
}

int tuner::setRssiThreshold ( int threshold )
{
  mRssiThreshold[mCurrentBand] = threshold;
  return 0;
}

int tuner::getRssiThreshold()
{
  return mRssiThreshold[mCurrentBand];
}

int tuner::setChannelSpace ( int space )
{
  mChannelSpace[mCurrentBand] = space;
  return 0;
}

int tuner::getChannelSpace()
{
  return mChannelSpace[mCurrentBand];
}

int tuner::setSeekStatus ( int status )
{
  if ( status != 0 && status != 1 ) { // 0 next, 1 stop
    return -1;
  }

  mSeekStatus = status;
  return 0;
}

int tuner::getSeekStatus()
{
  return mSeekStatus;
}

int tuner::getFmAmBand()
{
  int freq = mCurrentFreq[1];

  if ( mCurrentBand == BAND_FM ) {
    return BAND_FM;
  } else if ( freq >= g_freq_limit[BAND_AM_LW].lower && freq <= g_freq_limit[BAND_AM_LW].upper ) {
    return BAND_AM_LW;
  } else if ( freq >= g_freq_limit[BAND_AM_MW].lower && freq <= g_freq_limit[BAND_AM_MW].upper ) {
    return BAND_AM_MW;
  } else if ( freq >= g_freq_limit[BAND_AM_SW].lower && freq <= g_freq_limit[BAND_AM_SW].upper ) {
    return BAND_AM_SW;
  } else {
    return BAND_WX;
  }
}

int tuner::standbyMode()
{
  uint8_t data[1] = {0};

  data[0] = CTRL_MODE_STANDBY;

  return i2c_write_high ( HERO_SUBADDRESS_MODE, data, 1 );
}

int tuner::presetMode()
{
  uint8_t data[3] = {0};
  int freq = mCurrentFreq[mCurrentBand];
  if ( mCurrentBand == BAND_FM ) {
    freq = freq / 10;
  }

  data[0] = ( uint8_t ) ( CTRL_MODE_PRESET | getFmAmBand() );
  data[1] = ( uint8_t ) ( ( freq >> 8 ) & 0x7F );
  data[2] = ( uint8_t ) ( freq & 0xFF );

  return i2c_write_high ( HERO_SUBADDRESS_MODE, data, 3 );
}

int tuner::searchMode()
{
  uint8_t data[3] = {0};
  int freq = mCurrentFreq[mCurrentBand];
  if ( mCurrentBand == BAND_FM ) {
    freq = freq / 10;
  }

  data[0] = ( uint8_t ) ( CTRL_MODE_SEARCH | getFmAmBand() );
  data[1] = ( uint8_t ) ( ( freq >> 8 ) & 0x7F );
  data[2] = ( uint8_t ) ( freq & 0xFF );

  return i2c_write_high ( HERO_SUBADDRESS_MODE, data, 3 );
}

int tuner::endMode()
{
  uint8_t data[1] = {0};

  data[0] = ( uint8_t ) ( CTRL_MODE_END | getFmAmBand() );

  return i2c_write_high ( HERO_SUBADDRESS_MODE, data, 1 );
}

int tuner::checkStation()
{
  if ( getStatusAndQuality() < 0 ) {
    return -1;
  }

  if ( mRSSI > mRssiThreshold[mCurrentBand] && mFofThreshold[mCurrentBand] > ( mFOF & 0x7F ) ) {
    if ( mCurrentBand == BAND_FM ) {
      if ( mUSN < mUsnThreshold[mCurrentBand] && mWAM < mWamThreshold[mCurrentBand] ) {
        return 0;
      }
    } else {
      return 0;
    }
  }

  return -1;
}

int tuner::getStatusAndQuality()
{
  uint8_t qrs = 0;
  uint8_t data[5] = {0};
  while ( qrs < 5 ) {
    if ( i2c_read_high ( 0x00, data, 5 ) < 0 ) {
      return -1;
    }

    qrs = ( data[0] & 0xE0 ) >> 5;
    if ( qrs < 5 ) {
      usleep ( 1000 );
      continue;
    }

    mSTIN = ( data[0] & 0x08 ) == 0 ? 0 : 1;
    mRSSI = data[1];
    mUSN = data[2];
    mWAM = data[3];
    mFOF = ( data[4] & 0x80 ) == 0 ? ( data[4] & 0x7F ) : - ( data[4] & 0x7F );
  }

  return 0;
}

int tuner::setBandParameters()
{
  if ( mCurrentBand == BAND_FM ) {
    g_hero_defParameters[HERO_SUBADDRESS_TUNER - 0x3] = 0x40;
    g_hero_defParameters[HERO_SUBADDRESS_SIGNAL - 0x3] = mDeEmphasisFilter[0] & 0x3;
    g_hero_defParameters[HERO_SUBADDRESS_SOFTMUTE_LEV - 0x3] = 0x3A;
    g_hero_defParameters[HERO_SUBADDRESS_SOFTMUTE_LIM - 0x3] = 0x0A;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_TIME - 0x3] = 0x6E;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_LEV - 0x3] = 0x33;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_LIM - 0x3] = 0x04;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_LOC - 0x3] = 0x04;
  } else {
    g_hero_defParameters[HERO_SUBADDRESS_TUNER - 0x3] = 0x0A;
    g_hero_defParameters[HERO_SUBADDRESS_SIGNAL - 0x3] = 0x08 | ( mDeEmphasisFilter[1] & 0x3 );
    g_hero_defParameters[HERO_SUBADDRESS_SOFTMUTE_LEV - 0x3] = 0x40;
    g_hero_defParameters[HERO_SUBADDRESS_SOFTMUTE_LIM - 0x3] = 0x04;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_TIME - 0x3] = 0x72;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_LEV - 0x3] = 0x5B;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_LIM - 0x3] = 0x02;
    g_hero_defParameters[HERO_SUBADDRESS_HIGHCUT_LOC - 0x3] = 0x05;
  }

  return i2c_write_high ( HERO_SUBADDRESS_TUNER, g_hero_defParameters, BAND_PARAM_LEN );
}
