/*
 * SRService.cpp
 *
 *  Created on: 2015-1-15
 *      Author: ryan
 */

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "iss_sr.h"

//#include <kanzi/kanzi.h>
#include <libxml/parser.h>
#include <semaphore.h>
#include <sys/asoundlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "PCMRecorder.h"
#include "TTSPlayer.h"
#include "DoorControl.h"
#include "AirConditionControl.h"
#include "LightControl.h"
#include "WindowControl.h"

//using namespace std;

#if 1
#define DEBUG_TRACE(...) fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n")
#else
#define DEBUG_TRACE(...) ((void)0)
#endif

#define PPS_FILE_SR "/pps/services/asr/control"
#define FILE_UI  "/pps/ui"

struct TTSPlayListener: VA::ITTSPlayListener
{
   void OnTTSPlayCompleted()
   {
      fprintf(stdout, "OnTTSPlayCompleted\n");
      fflush(stdout);
   }
} g_Listener;

void toggleDoor(const char* door, int state)
{
    int tfd, cnt, length;
    char buf[32];

//    std::cout<<door<<" set to "<<state<<std::endl;
    sprintf (buf, "%s::%d\n", door, state);
    length = strlen(buf);

    if ((tfd = open(FILE_UI, O_WRONLY)) <= 0 )
      if ((tfd = open(FILE_UI, O_WRONLY | O_CREAT, 0666) ) <= 0 )
        return;

    cnt = 0;
   // do {
      if (write (tfd, buf, length ) == length)
          printf("wirte ok\n");
   //   sleep ( 1 );
   // } while ( cnt++ < 3 );

   close(tfd);
}


VA::StreamOptions g_options;
VA::PCMRecorder g_recorder;
VA::TTSPlayer* g_player = NULL;
DoorControl g_doorControl;
AirConditionControl g_airConditionControl;
LightControl g_lightControl;
WindowControl g_windowControl;

HISSSR g_hIssSr = NULL;
sem_t g_sem;
int g_frames = 8;

bool procMsgResult(const char *result, int len)
{
   bool ret = false;
   
   if (!result || len <= 0)
   {
      return ret;
   }
   
   xmlDocPtr doc = xmlReadMemory(result, len, NULL, "utf-8", XML_PARSE_RECOVER);
   
   if (doc)
   {
      std::string command;
      std::string voice = "不要问人家这么难的问题嘼";

      xmlChar* rawtext = NULL;
      xmlChar* focus = NULL;
      xmlChar* operation = NULL;
      xmlChar* subject = NULL;
      xmlChar* name = NULL;
      xmlChar* content = NULL;

      xmlNodePtr rootNode = xmlDocGetRootElement(doc);
      if (!rootNode)
      {
         return ret;
      }
      
      xmlNodePtr nlpNode = rootNode->xmlChildrenNode;
      while (nlpNode)
      {				
         if ((!xmlStrcmp(nlpNode->name, (const xmlChar *)"rawtext")))
	 {
	    rawtext = xmlNodeGetContent(nlpNode);
            command += (char*)rawtext;
	 } //end rawtext

	 else if ((!xmlStrcmp(nlpNode->name, (const xmlChar *)"result")))
         {
            xmlNodePtr resultNode = nlpNode->xmlChildrenNode;
            
            while (resultNode)
            {
	       if ((!xmlStrcmp(resultNode->name, (const xmlChar *)"focus")))
	       {
                  focus = xmlNodeGetContent(resultNode);
	       } //end result->focus
	       else if ((!xmlStrcmp(resultNode->name, (const xmlChar *)"action")))
	       {
		  xmlNodePtr actionNode = resultNode->xmlChildrenNode;
		  while (actionNode)
		  {
		     if ((!xmlStrcmp(actionNode->name, (const xmlChar *)"operation")))
		     {
			operation = xmlNodeGetContent(actionNode);
			break;
		     } //end result->action->operation

		     actionNode = actionNode->next;
		  }
	       } //end result->action

	       else if ((!xmlStrcmp(resultNode->name, (const xmlChar *)"subject")))
	       {
                  subject = xmlNodeGetContent(resultNode);
	       } //end result->subject

	       else if ((!xmlStrcmp(resultNode->name, (const xmlChar *)"object")))
               {
                  xmlNodePtr objectNode = resultNode->xmlChildrenNode;
                  while (objectNode)
                  {
                     if ((!xmlStrcmp(objectNode->name, (const xmlChar *)"name")))
                     {
                        name = xmlNodeGetContent(objectNode);
                        break;
                     } //end result->object->name
                     
                     objectNode = objectNode->next;
                  } //end while (objectNode)
               } //end result->object node

	       else if ((!xmlStrcmp(resultNode->name, (const xmlChar *)"content")))
	       {
                  content = xmlNodeGetContent(resultNode);
	       } //end result->content
               
               resultNode = resultNode->next;
            } //end while (resultNode)
            
            break;
         } //end result node
					
         nlpNode = nlpNode->next;
      }			
      xmlFreeDoc(doc);

      if (focus)
      {
	 command += (char*)focus;
      }

      if (operation)
      {
	 command += (char*)operation;
      }

      if (subject)
      {
	 command += (char*)subject;
      }

      if (name)
      {
	 command += (char*)name;
      }

      std::cout << "command = " << command << std::endl;
      
      if (g_airConditionControl.match(command))
      {
	 voice = "您说的是，";
	 if (rawtext)
	 {
	    voice += (char*) rawtext;
	 }
	 voice += ",操作已完成";
	 g_player->Start("空调操作已完成"/*voice.c_str()*/);
      }
      else if (g_doorControl.match(command))
      {
         voice = "您说的是，";
	 if (rawtext)
	 {
	    voice += (char*) rawtext;
	 }
	 voice += ",操作已完成";
	 g_player->Start("车门操作已完成"/*voice.c_str()*/);
      }
      else if (g_lightControl.match(command))
      {
         voice = "您说的是，";
	 if (rawtext)
	 {
	    voice += (char*) rawtext;
	 }
	 voice += ",操作已完成";
	 g_player->Start("车灯操作已完成"/*voice.c_str()*/);
      }
      else if (g_windowControl.match(command))
      {
         voice = "您说的是，";
	 if (rawtext)
	 {
	    voice += (char*) rawtext;
	 }
	 voice += ",操作已完成";
	 g_player->Start("车窗操作已完成"/*voice.c_str()*/);
      }
      else
      {
	 if (content)
	 {
	    voice = (char*)content;
	 }
	 g_player->Start(voice.c_str());
      }
   }
   
   return ret;
}

static void ProcOnMsgProc(const void* pUsrArg, unsigned int uMsg,
		unsigned int wParam, const void* lParam) {
   switch (uMsg)
   {
   case ISS_SR_MSG_InitStatus:
      DEBUG_TRACE("ISS_SR_MSG_InitStatus: %d", wParam);
      sem_post(&g_sem);
      break;
      
   case ISS_SR_MSG_UpLoadDictToLocalStatus:
      DEBUG_TRACE("ISS_SR_MSG_UpLoadDictToLocalStatus: %d", wParam);
      break;
      
   case ISS_SR_MSG_UpLoadDictToCloudStatus:
      DEBUG_TRACE("ISS_SR_MSG_UpLoadDictToCloudStatus: %d", wParam);
      break;
      
   case ISS_SR_MSG_VolumeLevel:
      break;
      
   case ISS_SR_MSG_ResponseTimeout:
      DEBUG_TRACE("ISS_SR_MSG_ResponseTimeout");
      g_recorder.StopStream();
      g_player->Start("你好像什么也没说");
      ISSSRStop(g_hIssSr);
      break;
      
   case ISS_SR_MSG_SpeechStart:
      DEBUG_TRACE("ISS_SR_MSG_SpeechStart");
      break;
      
   case ISS_SR_MSG_SpeechTimeOut:
      DEBUG_TRACE("ISS_SR_MSG_SpeechTimeOut");
      g_player->Start("你好像什么也没说");
      ISSSRStop(g_hIssSr);
      break;
      
   case ISS_SR_MSG_SpeechEnd:
      DEBUG_TRACE("ISS_SR_MSG_SpeechEnd");
      break;
      
   case ISS_SR_MSG_Error:
      DEBUG_TRACE("ISS_SR_MSG_Error:%d:%s", wParam, (char* ) lParam);
      g_player->Start((char*)lParam);
      ISSSRStop(g_hIssSr);
      break;
      
   case ISS_SR_MSG_Result:
      DEBUG_TRACE("ISS_SR_MSG_Result:%s", (char* ) lParam);
      procMsgResult((const char*)lParam, strlen((char* )lParam));
      ISSSRStop(g_hIssSr);
      break;
      
   case ISS_SR_MSG_LoadBigSrResStatus:
      DEBUG_TRACE("ISS_SR_MSG_LoadBigSrResStatus wParam: %d.\n", wParam);
      break;
      
   default:
      DEBUG_TRACE("Unhandled Msg: %d", uMsg);
      break;
   }
}

void sigalrm_fn(int sig)
{
   toggleDoor("misc", 0);
   g_recorder.StopStream();
   ISSSREndAudioData(g_hIssSr);
}

int Callback(void *Buffer, int32_t BufferSize, bool xrun, void *userData)
{
   unsigned int NumWritten = 0;
   ISSErrID err;

   if (xrun)
   {
      err = ISSSRAppendAudioData(g_hIssSr, (short *) Buffer, BufferSize / 2, &NumWritten);
      
      DEBUG_TRACE("ISSSRAppendAudioData ret err: %d", err);

      if (err == ISS_SUCCESS && NumWritten != (unsigned int) (BufferSize / 2))
      {
	 DEBUG_TRACE("ISSSRAppendAudioData Buffer Full\r\n");
      }
   }

   return 0;
}

int main(int argc, char *argv[]) {
   ISSErrID err;
   int quit = 0;

   char fake_machine_id_buf[1024] =
			"ThisIsAFakeMachineIdWhichNotLongerThan1024";
   err = ISSSetMachineCode(fake_machine_id_buf);
   DEBUG_TRACE("ISSSetMachineCode ret: %d", err);
   
   char res_path[PATH_MAX] = {0};
   char sr_path[PATH_MAX] = {0};
   char tts_path[PATH_MAX] = {0};

   if (argc == 1)
   {
      getcwd(res_path, PATH_MAX);
   }
   else
   {
      strcat(res_path, argv[argc - 1]);
   }

   strcat(sr_path, res_path);
   strcat(sr_path, "/SRRes");

   strcat(tts_path, res_path);
   strcat(tts_path, "/TTSRes");

   //strcat(str_path, "/SRRes");
   DEBUG_TRACE("SRRes: %s", sr_path);
   DEBUG_TRACE("TTSRes: %s", tts_path);
   
   int iMode = ISS_SR_MODE_MIX_REC;
   const char* sz_scene = ISS_SR_SCENE_ALL;
	
   sem_init(&g_sem, 0, 0);

#ifdef ISS_SOFTWARE_ENCRYPTION
   err = ISSActivate(sr_path);
   if(err != ISS_SUCCESS)
   {
      err = ISSGetActiveKey(sr_path);
      if(err == ISS_SUCCESS)
      {
         err = ISSActivate(sr_path);
      }
      else
      {
         DEBUG_TRACE("Auth Failed!");
      }
   }

#else
#endif

   err = ISSSRCreate(&g_hIssSr, sr_path, ProcOnMsgProc, NULL, 0);
   DEBUG_TRACE("ISSSRCreate ret: %d", err);

   err = ISSSRSetParam(g_hIssSr, ISS_SR_PARAM_TRACE_LEVEL,
      ISS_SR_PARAM_TRACE_LEVEL_VALUE_DEBUG);

   //g_player = new VA::TTSPlayer(tts_path,
   //   "ThisIsAFakeMachineIdWhichNotLongerThan1024",
   //   &g_Listener);

   signal(SIGALRM, sigalrm_fn);

#if 0  
   try
   {
      g_recorder.OpenStream(g_options, &g_frames, Callback);
   }
   catch (VA::PCMError& e)
   {
      std::cerr << e.what() << std::endl;
   }
#endif
	
   sem_wait(&g_sem);

   g_player = new VA::TTSPlayer(tts_path,
      "ThisIsAFakeMachineIdWhichNotLongerThan1024",
      &g_Listener);

   g_player->Start("语音识别已启动");

   int pps_fd, cnt, length;
   fd_set rfd;
   char buf[32];
   int once = false;

   if ((pps_fd = open(PPS_FILE_SR, O_RDONLY)) <= 0)
   {
      if ((pps_fd = open(PPS_FILE_SR, O_RDONLY | O_CREAT, 0666)) <= 0)
      {
         DEBUG_TRACE("open %s error: %s", PPS_FILE_SR, strerror(errno));
         goto out_start;
      }
   }

   while (!quit)
   {
      FD_ZERO(&rfd );
      FD_SET(pps_fd, &rfd);

      if ((cnt = select(1 + pps_fd, &rfd, 0, 0, NULL)) < 0)
      {
         DEBUG_TRACE("select error [%d]: %s\n", errno, strerror(errno));
         if (errno == EINTR)
         {
            continue;
         }
         
	 close(pps_fd);
         if ((pps_fd = open(PPS_FILE_SR, O_RDONLY | O_CREAT, 0666)) <= 0)
         {
            DEBUG_TRACE("open %s error: %s", PPS_FILE_SR, strerror(errno));
            goto out_start;
         }
         
         continue;
      }

      if (cnt > 0 && FD_ISSET(pps_fd, &rfd))
      {
         length = read(pps_fd, buf, sizeof(buf) - 1);
	 
	 if (!once)
	 {
	    once = true;
	    continue;
	 }

         buf[length] = 0;
         for (cnt = 0; cnt < length; cnt++)
         {
            if (strncmp("strobe::", buf + cnt, strlen ("strobe::")) == 0)
            {
               break;
            }
            
            do {} while ( ( buf[cnt++] != 0x0a) && (cnt < length));
            cnt--;
         }
         if (cnt < length)
         {
            if (strncmp(buf + cnt + strlen("strobe::"), "on", strlen("on")) == 0) //start
            {
	       ISSSRStop(g_hIssSr);

               try
	       {
                  g_recorder.OpenStream(g_options, &g_frames, Callback);
	       }
	       catch (VA::PCMError& e)
	       {
		  std::cerr << e.what() << std::endl;
                  g_player->Start("打开录音设备失败，请重新尝试");
                  toggleDoor("misc", 0);
                  continue;
	       }

               toggleDoor("misc", 1);
	       g_player->Start("有什么可以帮您");
               DEBUG_TRACE("Before ISSSRStart. iMode = %d", iMode);		
               err = ISSSRStart(g_hIssSr, sz_scene, iMode, NULL);			
               DEBUG_TRACE("After ISSSRStart. iMode = %d", iMode);
               DEBUG_TRACE("ISSSRStart ret: %d", err);
	
               if (err != ISS_SUCCESS)
               {
                  toggleDoor("misc", 0);
                  g_player->Start("请重新尝试");
                  continue;
               }

               try
	       {
		  g_recorder.StartStream();
	       }
	       catch (VA::PCMError& e)
	       {
		  std::cerr << e.what() << std::endl;
                  g_player->Start("录音失败，请重新尝试");
	       }

               alarm(5);
            }
         }
      }
   }
	
out_start:
   //g_recorder.CloseStream();
   err = ISSSRDestroy(g_hIssSr);
   if (err != ISS_SUCCESS)
   {
      DEBUG_TRACE("ISSSRDestroy ret %d", err);
   }
   g_hIssSr = NULL;

   DEBUG_TRACE("SRService exit......");
	
   return 0;
}
