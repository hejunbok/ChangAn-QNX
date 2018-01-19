#include "TTSPlayer.h"
#include "assert.h"

namespace VA
{

#define PCMGET_BUFF_SIZE 2048

void TTSPlayer::OnTTSDataReady(void* pUsrArg)
{
   TTSPlayer* pThis = (TTSPlayer*) pUsrArg;
   pThis->m_Player.StartStream();
}

int TTSPlayer::PCMCallback(void *Buffer, int32_t BufferSize, bool xrun,
      void *userData)
{
   TTSPlayer* pThis = (TTSPlayer*) userData;
   if (pThis->m_bDataEnd)
   {
      std::cerr << "err: need Stop Play" << std::endl;
      return -2;
   }
   else
   {
      ISSErrID ret = 0;
      unsigned int nGet = 0;
      int nSize;
      nSize = PCMGET_BUFF_SIZE < BufferSize ? PCMGET_BUFF_SIZE : BufferSize;
      ret = ISSTTSGetAudioData(pThis->m_hTTS, pThis->m_PCMBuff,
	    nSize / 2, &nGet, NULL, NULL);
      if (ISS_ERROR_TTS_COMPLETED == ret)
      {
	 pThis->m_bDataEnd = true;
      }
      else if(ISS_SUCCESS == ret && nGet == 0)
      {
      }
      else if (ISS_SUCCESS == ret && nGet > 0)
      {
      }
      else
      {
      }

      memcpy(Buffer, pThis->m_PCMBuff, nSize);
      if (pThis->m_bDataEnd)
      {
	 std::cout << "---> call TTS Completed" << std::endl;
	 pThis->m_pListener->OnTTSPlayCompleted();
	 pThis->m_bPlaying = false;
	 return -1;
      }
      else
      {
	 return nGet*2;
      }
   }
}

TTSPlayer::TTSPlayer(const char* szResPath, const char* szMachineCode,
      ITTSPlayListener* pListener) :
   m_PCMBuff(NULL),
   m_hRes(NULL),
   m_hTTS(NULL),
   m_bPlaying(false),
   m_bPaused(false),
   m_bDataEnd(false),
   m_pListener(NULL)
{
   ISSErrID ret = 0;
   if (pListener)
   {
      m_pListener = pListener;
   }

   m_PCMBuff = new char[PCMGET_BUFF_SIZE];

#ifdef ISS_SOFTWARE_ENCRYPTION
   ret = ISSSetMachineCode(szMachineCode);
   ret = ISSActivate(szResPath);
   ret = ISSTTSInitRes(&m_hRes, szResPath, 0);
#else
   ret = ISSTTSInitRes(&m_hRes, szResPath, 0);
#endif
   assert(ISS_SUCCESS == ret);
   ret = ISSTTSCreate(&m_hTTS, m_hRes, OnTTSDataReady, this);
   assert(ISS_SUCCESS == ret);

   int frag_size = 320;

   try
   {
      m_Player.OpenStream(m_options, &frag_size, PCMCallback, this);
   }
   catch (VA::PCMError& e)
   {
      std::cerr << e.what() << std::endl;
   }
}

TTSPlayer::~TTSPlayer()
{
   ISSErrID ret = 0;
   ret = ISSTTSDestroy(m_hTTS);
   assert(ISS_SUCCESS == ret);
   ret = ISSTTSUnInitRes(m_hRes);
   assert(ISS_SUCCESS == ret);
   if (m_PCMBuff)
   {
      delete[] m_PCMBuff;
   }
}

void TTSPlayer::Start(const char* szText)
{
   ISSErrID ret = 0;
   if (m_bPlaying)
   {
      Stop();
   }

   m_bPlaying = true;
   int nlen = strlen(szText);
   ret = ISSTTSStart(m_hTTS, szText, nlen, ISS_TTS_CODEPAGE_UTF8);
   assert(ISS_SUCCESS == ret);
   m_bDataEnd = false;
}

void TTSPlayer::Stop()
{
   if (m_bPlaying)
   {
      ISSErrID ret = 0;
      m_Player.StopStream();
      ret = ISSTTSStop(m_hTTS);
      assert(ISS_SUCCESS == ret || ISS_ERROR_TTS_STOPPED == ret);
      m_bPlaying = false;
      m_bPaused = false;
      m_bDataEnd = true;
   }
}

void TTSPlayer::Pause()
{
   if (m_bPlaying)
   {
      m_Player.PauseStream();
      m_bPaused = true;
   }
}

void TTSPlayer::Resume()
{
   if (m_bPaused)
   {
      m_Player.ResumeStream();
      m_bPaused = false;
   }
}

} /* namespace VA */
