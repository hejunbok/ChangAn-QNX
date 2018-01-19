#ifndef __TTSPLAYER_H
#define __TTSPLAYER_H

#include <QObject>

#include "qtqnxcar2_export.h"

#include "iss_tts.h"
#include "PCMPlayer.h"


namespace QnxCar
{

struct ITTSPlayListener
{
    virtual void OnTTSPlayCompleted() = 0;
};

class TTSPlayer: public QObject
{
   StreamOptions m_options;
   PCMPlayer m_Player;
   char* m_PCMBuff;

   HISSTTSRES m_hRes;
   HISSTTS m_hTTS;
   bool m_bPlaying;
   bool m_bPaused;
   volatile bool m_bDataEnd;
   ITTSPlayListener* m_pListener;

   static void OnTTSDataReady(void* pUsrArg);
   static int PCMCallback(void *Buffer, int32_t BufferSize, bool xrun, void *userData);

public:
   TTSPlayer(const char* szResPath, const char* szMachineCode, ITTSPlayListener* pListener = NULL, QObject *parent = 0);
   ~TTSPlayer();

   bool isPlaying()
   {
      return m_bPlaying;
   }
   bool isPaused()
   {
      return m_bPaused;
   }

public slots:
   void start(const char* szText);
   void stop();
   void pause();
   void resume();

};

} /* namespace VA */

#endif /* CTTSPLAYER_H */
