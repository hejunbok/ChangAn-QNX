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
int fogLightState = 1;
int windows_state = 0;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void toggleDoor(const char* door, int state);

static void set_mediaVideoId ( const int idx )
{
  int pps_fd, cnt, length;
  char buf[32];

  sprintf ( buf, "videoClickedId:n:%d\n", idx );
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
  }

  close ( pps_fd );
}

static void set_mediaMusicId ( const int idx )
{
    int pps_fd, cnt, length;
    char buf[32];

    sprintf ( buf, "musicClickedId:n:%d\n", idx );
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
    }

    close ( pps_fd );
}

#if 0
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
      printf("write appId ok\n");
  }

  close ( pps_fd );
}
#endif

void set_ui ( const idxUI idx )
{
  int pps_fd, cnt, length;
  char buf[32];

  pthread_mutex_lock(&mutex);

  sprintf ( buf, "set_ui:: %d\n", idx );
  length = strlen ( buf );

  if ( ( pps_fd = open ( FILE_SCREEN, O_WRONLY ) ) <= 0 )
  {
        if ( ( pps_fd = open ( FILE_SCREEN, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
        {
            /* add bye henson **/
            pthread_mutex_unlock(&mutex);
            return;
        }
  }

  cnt = 0;

    if ( (cnt = write ( pps_fd, buf, length )) == length )
    {
    }

  close ( pps_fd );
  pthread_mutex_unlock(&mutex);
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
  int ui_fd;
  int screen_fd;
  int max_fd;
  int length, scnt;
  int init = 1;
  fd_set rfd;
  char buf[512];

  char *p = NULL;
  Kanzi_STATE state={SettingCar, //uiidx
                     0, //fl_state
                     0, //fr_state
                     0, //rl_state
                     0, //rr_state
                     0, //window_fl_state
                     0, //window_fr_state
                     0, //window_rl_state
                     0, //window_rr_state
                     0, //backdoor_state
                     0, //radio_amfm_state
                     0, //radio_amfm_value
                     LightNone, //lights_state
                     NormalStyle,  //ui_style
                     0.0, //chair_goback
                     0.0, //chair_updown
                     0,    //chair_rotate
                     White,    //pm_2_5_state
                     0      //misc
                    };


  printf("enter runIPCLoop!\n");


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

  state.ui_style = (UIStyle)2;
  state.pm_2_5_state = White;
  (*(callbacks->pSetCCFunc))(state.pm_2_5_state);
  if(callbacks->pSetUIStyle != NULL)
  {
    (*(callbacks->pSetUIStyle))(state.ui_style);
  }
  else
  {
      printf("pSetUIStyle is not implemented\n");
  }

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
        length = read ( ui_fd, buf, sizeof ( buf ) - 1 );

        buf[length] = 0;
        if  (init != 1 )
        {
          if((p = strstr(buf,"fl::")) != NULL)
          {
              int toggle = (*(p + strlen ( "fl::" )) - '0');
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
              if (state.fr_state != toggle)
              {
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
              if (state.rl_state != toggle)
              {
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
              if (state.rr_state != toggle)
              {
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
              if (state.misc != toggle)
              {
                  state.misc = toggle;
                  if(callbacks->pMiscToggleONOFF != NULL)
                  {
                    (*(callbacks->pMiscToggleONOFF))();
                  }
                  else
                  {
                      printf("pMiscToggleONOFF is not implemented\n");
                  }
              }
          }
#if 0
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
#endif
          if((p = strstr(buf,"backdoor::")) != NULL)
          {
              int toggle = (*(p + strlen ( "backdoor::" )) - '0');
              if (state.backdoor_state != toggle)
              {
                  state.backdoor_state = toggle;
                  if(callbacks->pSettingToggleBackDoor != NULL)
                  {
                      printf("do backdoor in callbacks: %d\n",state.backdoor_state);
                    (*(callbacks->pSettingToggleBackDoor))();
                  }
                  else
                  {
                      printf("pSettingToggleBackDoor is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"radio_amfm::")) != NULL)
          {
              int toggle = (*(p + strlen ( "radio_amfm::" )) - '0');
              if (state.radio_amfm_state != toggle)
              {
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

          if((p = strstr(buf,"radio_amfm_value::")) != NULL)
          {
              float toggle = strtof(p + strlen ( "radio_amfm_value::" ), NULL);
              if (state.radio_amfm_value != toggle)
              {
                  state.radio_amfm_value = toggle;
                  if(callbacks->pSetAMFMFunc != NULL)
                  {
                    (*(callbacks->pSetAMFMFunc))(state.radio_amfm_value);
                  }
                  else
                  {
                      printf("pSetAMFMFunc is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"pm2_5_state::")) != NULL)
          {
              int toggle = strtof(p + strlen ( "pm2_5_state::" ), NULL);
              if (state.pm_2_5_state != toggle)
              {
                  //toggle = 2;
                  state.pm_2_5_state = (CCState)toggle;
                  if(callbacks->pSetCCFunc != NULL)
                  {
                    (*(callbacks->pSetCCFunc))(state.pm_2_5_state);
                  }
                  else
                  {
                      printf("pSetCCFunc is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"lights_state::")) != NULL)
          {
              int toggle = (*(p + strlen ( "lights_state::" )) - '0');
              if (state.lights_state != toggle)
              {
                  state.lights_state = (LightState)toggle;
                  if(callbacks->pSetCarLight != NULL)
                  {
                    (*(callbacks->pSetCarLight))(state.lights_state);
                  }
                  else
                  {
                      printf("pSetCarLight is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"ui_style::")) != NULL)
          {
              int toggle = (*(p + strlen ( "ui_style::" )) - '0');
              if (state.ui_style != toggle)
              {
                  state.ui_style = (UIStyle)toggle;
                  if(callbacks->pSetUIStyle != NULL)
                  {
                    (*(callbacks->pSetUIStyle))(state.ui_style);
                  }
                  else
                  {
                      printf("pSetUIStyle is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"chair_goback::")) != NULL)
          {
              float toggle = strtof(p + strlen ( "chair_goback::" ), NULL);
              if (state.chair_goback != toggle)
              {
                  state.chair_goback = toggle;
                  if(callbacks->pSetChairGoBackFunc != NULL)
                  {
                    (*(callbacks->pSetChairGoBackFunc))(state.chair_goback);
                  }
                  else
                  {
                      printf("pSetChairGoBackFunc is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"chair_updown::")) != NULL)
          {
              float toggle = strtof(p + strlen ( "chair_updown::" ), NULL);
              if (state.chair_updown != toggle)
              {
                  state.chair_updown = toggle;
                  if(callbacks->pSetChairUpDownFunc != NULL)
                  {
                    (*(callbacks->pSetChairUpDownFunc))(state.chair_updown);
                  }
                  else
                  {
                      printf("pSetChairUpDownFunc is not implemented\n");
                  }
              }
          }

          if((p = strstr(buf,"chair_rotate::")) != NULL)
          {
              float toggle = atoi(p + strlen ( "chair_rotate::" ));
              if (state.chair_rotate != toggle)
              {
                  state.chair_rotate = toggle;
                  if(callbacks->pSetChairRotateFunc != NULL)
                  {
                    (*(callbacks->pSetChairRotateFunc))(state.chair_rotate);
                  }
                  else
                  {
                      printf("pSetChairRotateFunc is not implemented\n");
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
}

void on_media_video_clicked(const int idx)
{
    if (idx >= 0 && idx < 10)
    {
       set_mediaVideoId(idx);
    }
}
void on_media_video_focus(const int idx)
{
    if(idx > 0)
    {

    }
    else
    {

    }
}

void on_media_music_clicked(const int idx)
{
    if (idx >= 0 && idx < 17)
    {
       set_mediaMusicId(idx);
    }
}

void on_media_music_focus(const int idx)
{
    if(idx > 0)
    {

    }
    else
    {

    }
}

void toggleDoor(const char* door, int state)
{
    int tfd, cnt, length;
    char buf[32];

    sprintf ( buf, "%s::%d\n", door, state );
    length = strlen ( buf );
    cnt = 0;

    if ( ( tfd = open ( FILE_UI, O_WRONLY ) ) <= 0 )
      if ( ( tfd = open ( FILE_UI, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
        return;

      if ( (cnt = write ( tfd, buf, length )) == length )
      {
      }

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
    switch (section)
    {
    case CAR_DOOR_FRONT_LEFT:
        toggleDoor("fl", opened);
        break;
    case CAR_DOOR_RONT_RIGHT:
        toggleDoor("fr", opened);
        break;
    case CAR_DOOR_BACK_LEFT:
        toggleDoor("rl", opened);
        break;
    case CAR_DOOR_BACK_RIGHT:
        toggleDoor("rr", opened);
        break;
    case CAR_LIGHT:
        toggleDoor("lights_state", opened == 1 ? 2 : 1);
        break;
    case CAR_FOG_LIGHT:
        toggleDoor("lights_state", opened == 1 ? 3 : 1);
        break;
    case CAR_BACK_DOOR:
        toggleDoor("backdoor", opened);
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
    frontLight_state = toggleState("lights_state");
    (frontLight_state==1) ? (frontLight_state=2) : (frontLight_state=1);
    toggleDoor("lights_state", frontLight_state);
}

void toggleFogLights(void)
{
    fogLightState = toggleState("lights_state");
    (fogLightState==1) ? (fogLightState=3) : (fogLightState=1);
    toggleDoor("lights_state", fogLightState);
}

}
