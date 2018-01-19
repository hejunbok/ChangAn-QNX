#ifndef CTTSPLAYER_H
#define CTTSPLAYER_H
#include "PCMPlayer.h"
#include "iss_tts.h"

namespace VA
{

struct ITTSPlayListener
{
    virtual void OnTTSPlayCompleted() = 0;
};

class TTSPlayer
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
   TTSPlayer(const char* szResPath, const char* szMachineCode, ITTSPlayListener* pListener = NULL);
   ~TTSPlayer();

   void Start(const char* szText);
   void Stop();
   void Pause();
   void Resume();

   bool isPlaying()
   {
      return m_bPlaying;
   }
   bool isPaused()
   {
      return m_bPaused;
   }
};

} /* namespace VA */

#endif /* CTTSPLAYER_H */
