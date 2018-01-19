#include "kanzi.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <pthread.h>

namespace KANZI {

#define FILE_UI  "/pps/ui"
#define FILE_SCREEN  "/pps/screen"
#define FILE_MEDIA "/pps/qnxcar/media"
#define FILE_APP "/pps/qnxcar/app"
#define FILE_VOICE "/pps/services/asr/control"

int fl_state = 0;
int fr_state = 0;
int rl_state = 0;
int rr_state = 0;
int bd_state = 0;
int frontLight_state = 0;
int windows_state = 0;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void toggleDoor(const char* door, int state);

static void set_mediaId ( const int idx )
{
  int pps_fd, cnt, length;
  char buf[32];

  sprintf ( buf, "mediaClickedId:n:%d\n", idx );
  length = strlen ( buf );

  if ( ( pps_fd = open ( FILE_MEDIA, O_WRONLY ) ) <= 0 )
  {
      if ( ( pps_fd = open ( FILE_MEDIA, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
      {
          printf("open %s error:%s\n", FILE_MEDIA, strerror(errno));
          return;
      }
  }

  cnt = 0;
  if ( (cnt = write ( pps_fd, buf, length )) == length )
  {
      printf("wirte mediaId ok\n");
  }

  close ( pps_fd );
}

static void set_appId ( const int idx )
{
  int pps_fd, cnt, length;
  char buf[32];

  sprintf ( buf, "appClickedId:n:%d\n", idx );
  length = strlen ( buf );

  if ( ( pps_fd = open ( FILE_APP, O_WRONLY ) ) <= 0 )
  {
      if ( ( pps_fd = open ( FILE_APP, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
      {
          printf("open %s error:%s\n", FILE_APP, strerror(errno));
          return;
      }
  }

  cnt = 0;
  if ( (cnt = write ( pps_fd, buf, length )) == length )
  {
      printf("wirte appId ok\n");
  }

  close ( pps_fd );
}

void set_ui ( const idxUI idx )
{
  int pps_fd, cnt, length;
  char buf[32];

#if 1
  pthread_mutex_lock(&mutex);

  sprintf ( buf, "set_ui:: %d\n", idx );
  length = strlen ( buf );

  if ( ( pps_fd = open ( FILE_SCREEN, O_WRONLY ) ) <= 0 )
        if ( ( pps_fd = open ( FILE_SCREEN, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
        {
            return;
        }
  }

  cnt = 0;
 // do {
    if ( (cnt = write ( pps_fd, buf, length )) == length )
        printf("wirte ok\n");
 //   sleep ( 1 );
 // } while ( cnt++ < 3 );

  close ( pps_fd );
  pthread_mutex_unlock(&mutex);
#endif
}

idxUI get_ui ( void )
{
  int pps_fd, cnt, length;
  fd_set rfd;
  char buf[32];

  if ( ( pps_fd = open ( FILE_SCREEN, O_RDONLY ) ) <= 0 )
    return (idxUI)0;

  FD_ZERO ( &rfd );
  FD_SET ( pps_fd, &rfd );

  while ( 1 ) {
    if ( ( cnt = select ( 1 + pps_fd, &rfd, 0, 0, NULL ) ) < 0 )
      return (idxUI)0;;
    if ( cnt > 0 && FD_ISSET ( pps_fd, &rfd ) ) {
      length = read ( pps_fd, buf, sizeof ( buf ) - 1 );
      buf[length] = 0;
      for ( cnt = 0; cnt < length; cnt++ ) {
        if ( strncmp ( "set_ui:: ", buf + cnt, strlen ( "set_ui:: " ) ) == 0 )
          break;
        do {} while ( ( buf[cnt++] != 0x0a ) && ( cnt < length ) );
        cnt--;
      }
      if ( cnt < length ) {
        close ( pps_fd );
        return (idxUI)(buf[cnt + strlen ( "set_ui:: " )] - '0');
      }
      return (idxUI)0;
    }
  }
}

void runIPCLoop(Kanzi_CB *callbacks)
{
#if 1
  int ui_fd;
  int screen_fd;
  int max_fd;
  int length, scnt;
  int init = 1;
  fd_set rfd;
  char buf[512];
  //char iconPath[64];
  char *p = NULL;
  Kanzi_STATE state={NAVI, //uiidx
                     0, //fl_state
                     0, //fr_state
                     0, //rl_state
                     0, //rr_state
                     0, //misc_state;
                     0, //navi_daynight_state;
                     1, //navi_infobar_state;
                     0, //navi_camera_state;
                     DemoMode, //navi_mode;
                     0, //radio_amfm_state;
                     0, //window_fl_state;
                     0, //window_fr_state;
                     0, //window_rl_state;
                     0, //window_rr_state;
                     0, //backdoor_state;
                     Ringing, //phone_mode;
                     0  //frontlights_state;
                    };

//  while(1) printf("runIPCLoop runIPCLoop runIPCLoop\n");
#if 1
  printf("enter runIPCLoop!\n");

#if 0
retry:

  try
  {
  //Set App Icon

  printf("start pSetAppIconWithIndex\n");
  for (int i = 0; i < 9; i++)
  {
      sprintf(iconPath, "/extra/kanzi_app/icon/%d.jpg", i);
      printf("pSetAppIconWithIndex %d\n", i);
      if (callbacks->pSetAppIconWithIndex != NULL)
      {
          if (access(iconPath, F_OK) == 0)
          {
                (*(callbacks->pSetAppIconWithIndex))(i, iconPath);
          }
          else
          {
              printf("file:%s is not exist!\n", iconPath);
          }
      }
      else
      {
          printf("pSetAppIconWithIndex is not implemented\n");
      }
  }
  printf("end pSetAppIconWithIndex\n");

  (*(callbacks->pSetMainMenuAppIcon))("/extra/kanzi_app/icon/mat_menu_media.jpg");

  if (callbacks->pSetAppIconTextWithIndex != NULL)
  {
      printf("start pSetAppIconTextWithIndex\n");

      (*(callbacks->pSetAppIconTextWithIndex))(0, "计算器");
      (*(callbacks->pSetAppIconTextWithIndex))(1, "浏览器");
      (*(callbacks->pSetAppIconTextWithIndex))(2, "视频");
      (*(callbacks->pSetAppIconTextWithIndex))(3, "购物");
      (*(callbacks->pSetAppIconTextWithIndex))(4, "日历");
      (*(callbacks->pSetAppIconTextWithIndex))(5, "天气");
      (*(callbacks->pSetAppIconTextWithIndex))(6, "新闻");
      (*(callbacks->pSetAppIconTextWithIndex))(7, "财经");
      (*(callbacks->pSetAppIconTextWithIndex))(8, "音乐");

      printf("end pSetAppIconTextWithIndex\n");
  }
  else
  {
      printf("pSetAppIconTextWithIndex is not implemented\n");
  }
  }
  catch(...)
  {
      printf("retry to enter runIPCLoop!\n");
      goto retry;
  }

#endif

  toggleDoor("misc", 0);

  if ( ( ui_fd = open ( FILE_UI, O_RDONLY ) ) <= 0 ){
      printf("file open error!\n");
      return;
  }

  if ( ( screen_fd = open ( FILE_SCREEN, O_RDONLY ) ) <= 0 ){
      printf("file open error!\n");
      return;
  }


  //FD_ZERO ( &rfd );
  //FD_SET ( pps_fd, &rfd );

  max_fd = screen_fd > ui_fd ? screen_fd : ui_fd;


  while ( 1 ) {
    FD_ZERO ( &rfd );
    FD_SET ( ui_fd, &rfd );
    FD_SET ( screen_fd, &rfd );
    if ( ( scnt = select ( 1 + max_fd, &rfd, 0, 0, NULL ) ) < 0 )
    {
      printf("failed select!\n");
      continue;
    }


    if (scnt > 0 && FD_ISSET ( screen_fd, &rfd ))
    {
      printf("SCREEN Set selected\n");
      length = read ( screen_fd, buf, sizeof ( buf ) - 1 );

      buf[length] = 0;
      printf("read: %d: %s\n",length,buf);

      if ( init != 1 )
      {
          char* p;
          if((p = strstr(buf,"set_ui:: ")) != NULL)
          {
              KANZI::idxUI uiidx = (KANZI::idxUI)(*(p + strlen ( "set_ui:: " )) - '0');
              printf("pps ui value: %d\n",(int)uiidx);
              if (true /*state.uiidx != uiidx*/)
              {
                  printf("do set ui: %d\n",(int)uiidx);
                  state.uiidx = uiidx;
                  if(callbacks->pSetUI != NULL)
                  {
                    (*(callbacks->pSetUI))(uiidx);
                  }
                  else
                  {
                      printf("pSetUI is not implemented\n");
                  }
              }
          }
       }
    }

    if ( scnt > 0 && FD_ISSET ( ui_fd, &rfd ) ) {
        printf("Kanzi Car setting\n");
        length = read ( ui_fd, buf, sizeof ( buf ) - 1 );

        buf[length] = 0;
        printf("read: %d: %s\n",length,buf);
        if  (init != 1 )
        {
          if((p = strstr(buf,"fl::")) != NULL)
          {
              int toggle = (*(p + strlen ( "fl::" )) - '0');
              printf("fl value: %d\n",toggle);
              if (state.fl_state != toggle)
              {
                  printf("do fl: %d\n",toggle);
                  state.fl_state = toggle;
                  if(callbacks->pSettingToggleFL != NULL)
                  {
                    (*(callbacks->pSettingToggleFL))();

                  }
                  else
                  {
                      printf("pSettingToggleFL is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"fr::")) != NULL)
          {
              int toggle = (*(p + strlen ( "fr::" )) - '0');
              printf("fr value: %d\n",toggle);
              if (state.fr_state != toggle)
              {
                  printf("do fr: %d\n",toggle);
                  state.fr_state = toggle;
                  if(callbacks->pSettingToggleFR != NULL)
                  {
                    (*(callbacks->pSettingToggleFR))();
                  }
                  else
                  {
                      printf("pSettingToggleFR is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"rl::")) != NULL)
          {
              int toggle = (*(p + strlen ( "rl::" )) - '0');
              printf("rl value: %d\n",toggle);
              if (state.rl_state != toggle)
              {
                  printf("do rl: %d\n",toggle);
                  state.rl_state = toggle;
                  if(callbacks->pSettingToggleRL != NULL)
                  {
                    (*(callbacks->pSettingToggleRL))();
                  }
                  else
                  {
                      printf("pSettingToggleRL is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"rr::")) != NULL)
          {
              int toggle = (*(p + strlen ( "rr::" )) - '0');
              printf("rr value: %d\n",toggle);
              if (state.rr_state != toggle)
              {
                  printf("do rr: %d\n",toggle);
                  state.rr_state = toggle;
                  if(callbacks->pSettingToggleRR != NULL)
                  {
                    (*(callbacks->pSettingToggleRR))();
                  }
                  else
                  {
                      printf("pSettingToggleRR is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"misc::")) != NULL)
          {
              int toggle = (*(p + strlen ( "misc::" )) - '0');
              printf("misc value: %d\n",toggle);
              if (state.misc_state != toggle)
              {
                  printf("do misc: %d\n",toggle);
                  state.misc_state = toggle;
                  if(callbacks->pVoiceToggleMisc != NULL)
                  {
                    (*(callbacks->pVoiceToggleMisc))();

                  }
                  else
                  {
                      printf("pVoiceToggleMisc is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"navi_daynight::")) != NULL)
          {
              int toggle = (*(p + strlen ( "navi_daynight::" )) - '0');
              printf("navi_daynight value: %d\n",toggle);
              if (state.navi_daynight_state != toggle)
              {
                  printf("do navi_daynight: %d\n",toggle);
                  state.navi_daynight_state = toggle;
                  if(callbacks->pNaviToggleDayNight != NULL)
                  {
                    (*(callbacks->pNaviToggleDayNight))();
                  }
                  else
                  {
                      printf("pNaviToggleDayNight is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"navi_infobar::")) != NULL)
          {
              int toggle = (*(p + strlen ( "navi_infobar::" )) - '0');
              printf("navi_infobar value: %d\n",toggle);
              if (state.navi_infobar_state != toggle)
              {
                  printf("do navi_infobar: %d\n",toggle);
                  state.navi_infobar_state = toggle;
                  if(callbacks->pNaviToggleInfoBar != NULL)
                  {
                    (*(callbacks->pNaviToggleInfoBar))();
                  }
                  else
                  {
                      printf("pNaviToggleInfoBar is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"navi_camera::")) != NULL)
          {
              int toggle = (*(p + strlen ( "navi_camera::" )) - '0');
              printf("navi_camera value: %d\n",toggle);
              if (state.navi_camera_state != toggle)
              {
                  printf("do navi_camera: %d\n",toggle);
                  state.navi_camera_state = toggle;
                  if(callbacks->pNaviToggleCamera != NULL)
                  {
                    (*(callbacks->pNaviToggleCamera))();

                  }
                  else
                  {
                      printf("pNaviToggleCamera is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"navi_mode::")) != NULL)
          {
              int toggle = (*(p + strlen ( "navi_mode::" )) - '0');
              printf("navi_mode value: %d\n",toggle);
              if (toggle >= DemoMode && toggle <= NaviRunningMode)
              {
                  if (state.navi_mode != (NaviMode)toggle)
                  {
                      printf("do navi_mode: %d\n",toggle);
                      state.navi_mode = (NaviMode)toggle;
                      if(callbacks->pNaviSetMode != NULL)
                      {
                        (*(callbacks->pNaviSetMode))(state.navi_mode);
                      }
                      else
                      {
                          printf("pNaviSetMode is not implemented\n");
                      }
                  }
              }
          }

          if((p = strstr(buf,"radio_amfm::")) != NULL)
          {
              int toggle = (*(p + strlen ( "radio_amfm::" )) - '0');
              printf("radio_amfm value: %d\n",toggle);
              if (state.radio_amfm_state != toggle)
              {
                  printf("do radio_amfm: %d\n",toggle);
                  state.radio_amfm_state = toggle;
                  if(callbacks->pRadioToggleAMFM != NULL)
                  {
                    (*(callbacks->pRadioToggleAMFM))();
                  }
                  else
                  {
                      printf("pRadioToggleAMFM is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"window_fl::")) != NULL)
          {
              int toggle = (*(p + strlen ( "window_fl::" )) - '0');
              printf("window_fl value: %d\n",toggle);
              if (state.window_fl_state != toggle)
              {
                  printf("do window_fl: %d\n",toggle);
                  state.window_fl_state = toggle;
                  if(callbacks->pSettingToggleWindowFL != NULL)
                  {
                    (*(callbacks->pSettingToggleWindowFL))();
                  }
                  else
                  {
                      printf("pSettingToggleWindowFL is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"window_fr::")) != NULL)
          {
              int toggle = (*(p + strlen ( "window_fr::" )) - '0');
              printf("window_fr value: %d\n",toggle);
              if (state.window_fr_state != toggle)
              {
                  printf("do window_fl: %d\n",toggle);
                  state.window_fr_state = toggle;
                  if(callbacks->pSettingToggleWindowFR != NULL)
                  {
                    (*(callbacks->pSettingToggleWindowFR))();
                  }
                  else
                  {
                      printf("pSettingToggleWindowFR is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"window_rl::")) != NULL)
          {
              int toggle = (*(p + strlen ( "window_rl::" )) - '0');
              printf("window_rl value: %d\n",toggle);
              if (state.window_rl_state != toggle)
              {
                  printf("do window_rl: %d\n",toggle);
                  state.window_rl_state = toggle;
                  if(callbacks->pSettingToggleWindowRL != NULL)
                  {
                    (*(callbacks->pSettingToggleWindowRL))();
                  }
                  else
                  {
                      printf("pSettingToggleWindowRL is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"window_rr::")) != NULL)
          {
              int toggle = (*(p + strlen ( "window_rr::" )) - '0');
              printf("window_rr value: %d\n",toggle);
              if (state.window_rr_state != toggle)
              {
                  printf("do window_rr: %d\n",toggle);
                  state.window_rr_state = toggle;
                  if(callbacks->pSettingToggleWindowRR != NULL)
                  {
                    (*(callbacks->pSettingToggleWindowRR))();
                  }
                  else
                  {
                      printf("pSettingToggleWindowRR is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"backdoor::")) != NULL)
          {
              int toggle = (*(p + strlen ( "backdoor::" )) - '0');
              printf("backdoor value: %d\n",toggle);
              if (state.backdoor_state != toggle)
              {
                  printf("do backdoor: %d\n",toggle);
                  state.backdoor_state = toggle;
                  if(callbacks->pSettingToggleBackDoor != NULL)
                  {
                    (*(callbacks->pSettingToggleBackDoor))();
                  }
                  else
                  {
                      printf("pSettingToggleBackDoor is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"phone_mode::")) != NULL)
          {
              int toggle = (*(p + strlen ( "phone_mode::" )) - '0');
              printf("phone_mode value: %d\n",toggle);
              if (toggle >= Ringing && toggle <= LeavePhone)
              {
                  if (state.phone_mode != (PhoneMode)toggle)
                  {
                      printf("do phone_mode: %d\n",toggle);
                      state.phone_mode = (PhoneMode)toggle;
                      if(callbacks->pPhoneSetMode != NULL)
                      {
                        (*(callbacks->pPhoneSetMode))(state.phone_mode);
                      }
                      else
                      {
                          printf("pPhoneSetMode is not implemented\n");
                      }
                  }
              }
          }

          if((p = strstr(buf,"frontlights::")) != NULL)
          {
              int toggle = (*(p + strlen ( "frontlights::" )) - '0');
              printf("frontlights value: %d\n",toggle);
              if (state.frontlights_state != toggle)
              {
                  printf("do frontlights: %d\n",toggle);
                  state.frontlights_state = toggle;
                  if(callbacks->pSettingToggleFrontLights != NULL)
                  {
                    (*(callbacks->pSettingToggleFrontLights))();
                  }
                  else
                  {
                      printf("pSettingToggleFrontLights is not implemented\n");
                  }
              }
          }

          continue;
      }
    }

    if(init == 1) init=0;
  }

  close ( ui_fd );
  close ( screen_fd );
#endif

#endif
}

void on_media_clicked(const int idx)
{
    printf("on_media_clicked, idx=%d\n", idx);
    if (idx >= 0 && idx <= 14)
    {
       set_mediaId(idx);
    }


}
void on_media_focus(const int idx)
{
    printf("on_media_focus, idx=%d\n", idx);
}

void on_app_clicked(const int idx)
{
    printf("on_app_clicked, idx=%d\n", idx);
    set_appId(idx);
}

void on_voice_clicked()
{
    printf("on_voice_clicked\n");
    int tfd, cnt, length;
    char buf[32];

    //    std::cout<<door<<" set to "<<state<<std::endl;
    sprintf ( buf, "strobe::on\n");
    length = strlen ( buf );
    cnt = 0;

    if ( ( tfd = open ( FILE_VOICE, O_WRONLY ) ) <= 0 )
      if ( ( tfd = open ( FILE_VOICE, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
        return;

   // do {
      if ( (cnt = write ( tfd, buf, length )) == length )
          printf("wirte door ok\n");
   //   sleep ( 1 );
   // } while ( cnt++ < 3 );

    close ( tfd );
}

void toggleDoor(const char* door, int state)
{
    int tfd, cnt, length;
    char buf[32];

//    std::cout<<door<<" set to "<<state<<std::endl;
    sprintf ( buf, "%s::%d\n", door, state );
    length = strlen ( buf );
    cnt = 0;

    if ( ( tfd = open ( FILE_UI, O_WRONLY ) ) <= 0 )
      if ( ( tfd = open ( FILE_UI, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
        return;

   // do {
      if ( (cnt = write ( tfd, buf, length )) == length )
          printf("wirte door ok\n");
   //   sleep ( 1 );
   // } while ( cnt++ < 3 );

    close ( tfd );

}

int toggleState(const char* key)
{
    int tfd, length;
    char buf[512];
    char ppskey[32];
    char *p = NULL;
    int state = 0;

    sprintf (ppskey, "%s::", key);

    if ((tfd = open (FILE_UI, O_RDONLY )) <= 0)
    {
        return 0;
    }

    if ((length = read (tfd, buf, sizeof(buf) - 1)) <= 0)
    {
        return 0;
    }

    close ( tfd );

    buf[length] = 0;

    if((p = strstr(buf, ppskey)) != NULL)
    {
        state = (*(p + strlen(ppskey)) - '0');
    }

    return state;
}


void on_vehicle_clicked(VehicleSection section, bool opened)
{
    printf("on_voice_clicked, section=%d, opended=%d\n", section, opened);
    switch (section)
    {
    case CAR_DOOR_FRONT_LEFT:
        toggleDoor("fl", opened ? 0 : 1);
        break;
    case CAR_DOOR_RONT_RIGHT:
        toggleDoor("fr", opened ? 0 : 1);
        break;
    case CAR_DOOR_BACK_LEFT:
        toggleDoor("rl", opened ? 0 : 1);
        break;
    case CAR_DOOR_BACK_RIGHT:
        toggleDoor("rr", opened ? 0 : 1);
        break;
    case CAR_LIGHT:
        toggleDoor("frontlights", opened ? 1 : 0);
        break;
    case CAR_BACK_DOOR:
        toggleDoor("backdoor", opened ? 0 : 1);
        break;
    default:
            break;
    }
}

void toggleFL(void)
{
    fl_state = toggleState("fl");
    (fl_state==0) ? (fl_state=1) : (fl_state=0);
    toggleDoor("fl", fl_state);
}

void toggleFR(void)
{
    fr_state = toggleState("fr");
    (fr_state==0) ? (fr_state=1) : (fr_state=0);
    toggleDoor("fr", fr_state);
}
void toggleRL(void)
{
    rl_state = toggleState("rl");
    (rl_state==0) ? (rl_state=1) : (rl_state=0);
    toggleDoor("rl", rl_state);
}
void toggleRR(void)
{
    rr_state = toggleState("rr");
    (rr_state==0) ? (rr_state=1) : (rr_state=0);
    toggleDoor("rr", rr_state);
}

void toggleBackDoor(void)
{
    bd_state = toggleState("backdoor");
    (bd_state==0) ? (bd_state=1) : (bd_state=0);
    toggleDoor("backdoor", bd_state);
}

void toggleWindows(void)
{
    windows_state = toggleState("window_fl");
    (windows_state==0) ? (windows_state=1) : (windows_state=0);
    toggleDoor("window_fl", windows_state);
    toggleDoor("window_fr", windows_state);
    toggleDoor("window_rl", windows_state);
    toggleDoor("window_rr", windows_state);
}

void toggleFrontLight(void)
{
    frontLight_state = toggleState("frontlights");
    (frontLight_state==0) ? (frontLight_state=1) : (frontLight_state=0);
    toggleDoor("frontlights", frontLight_state);
}

}
