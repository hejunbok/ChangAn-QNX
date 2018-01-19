#ifndef KANZI_H
#define KANZI_H

namespace KANZI
{
typedef enum {
  NAVI = 1, MEDIA, SETTING,MainMenu,Radio,Phone,AppStore
} idxUI;

typedef void (*PTRSETUI)(const idxUI idx);
typedef idxUI (*PTRGETUI)(void);

typedef  void(*PTRToggleFunc)(void);

typedef enum {
    Ringing,TakePhone,LeavePhone
}PhoneMode;

typedef enum {
    DemoMode,NaviStartMode,NaviRunningMode
}NaviMode;

typedef enum {
    CAR_DOOR_FRONT_LEFT = 1,CAR_DOOR_RONT_RIGHT,CAR_DOOR_BACK_LEFT,CAR_DOOR_BACK_RIGHT,CAR_LIGHT,CAR_BACK_DOOR
}VehicleSection;

typedef void (*PTRSetNaviMode)(NaviMode naviMode);

typedef void (*PTRSetPhoneMode)(PhoneMode mode);

typedef void (*PTRSetAppIconWithIndex)(int index, const char* iconFileName);
typedef void (*PTRSetAppIconTextWithIndex)(int index, const char* iconName);

typedef void (*PTRSetMainMenuAppIcon)(const char* iconFileName);

typedef struct
{
    PTRSETUI pSetUI;
    PTRGETUI pGetUI;
    //toggle front left door
    PTRToggleFunc pSettingToggleFL;
    //toggle front right door
    PTRToggleFunc pSettingToggleFR;
    //toggle rear left door
    PTRToggleFunc pSettingToggleRL;
    //toggle rear right door
    PTRToggleFunc pSettingToggleRR;

    //the following already implemented
    //show or hide the misc
    PTRToggleFunc pVoiceToggleMisc;
    //switch between day or night mode of navi
    PTRToggleFunc pNaviToggleDayNight;
    //toggle navi info bar , show or hide
    PTRToggleFunc pNaviToggleInfoBar;
    //toggle navi camera
    PTRToggleFunc pNaviToggleCamera;
    //set navi mode
    PTRSetNaviMode pNaviSetMode;
    //Radio
    PTRToggleFunc pRadioToggleAMFM;
    //setting module
    PTRToggleFunc pSettingToggleWindowFL;
    PTRToggleFunc pSettingToggleWindowFR;
    PTRToggleFunc pSettingToggleWindowRL;
    PTRToggleFunc pSettingToggleWindowRR;
    PTRToggleFunc pSettingToggleBackDoor;
    //phone module
    PTRSetPhoneMode pPhoneSetMode;
    //the following to be done
    PTRToggleFunc pSettingToggleFrontLights;

    //set the icon image with index (0 - 8)
    PTRSetAppIconWithIndex pSetAppIconWithIndex;
    PTRSetAppIconTextWithIndex pSetAppIconTextWithIndex;
    PTRSetMainMenuAppIcon pSetMainMenuAppIcon;
}Kanzi_CB;

/*运行通信消息循环*/
void runIPCLoop(Kanzi_CB *callbacks);

/*通知一个媒体封面的点击事件*/
void on_media_clicked(const int idx);
/*通知一个媒体封面的焦点事件*/
void on_media_focus(const int idx);

/*通知一个应用的点击事件*/
void on_app_clicked(const int idx);

/*通知语音的点击事件*/
void on_voice_clicked();

/*通知车的点击事件*/
void on_vehicle_clicked(VehicleSection section, bool opened);


/****** Internal Used QNXCAR ******/

typedef struct
{
    idxUI uiidx;
    int fl_state;
    int fr_state;
    int rl_state;
    int rr_state;
    int misc_state;
    int navi_daynight_state;
    int navi_infobar_state;
    int navi_camera_state;
    NaviMode navi_mode;
    int radio_amfm_state;
    int window_fl_state;
    int window_fr_state;
    int window_rl_state;
    int window_rr_state;
    int backdoor_state;
    PhoneMode phone_mode;
    int frontlights_state;
}Kanzi_STATE;

void  set_ui ( const idxUI idx );
idxUI get_ui ( void );

void toggleFL(void);
void toggleFR(void);
void toggleRL(void);
void toggleRR(void);
void toggleBackDoor(void);
void toggleWindows(void);
void toggleFrontLight(void);
}

#endif
