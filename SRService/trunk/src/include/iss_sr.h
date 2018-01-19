/******************************************************************************
Copyright:      1999-2013, iFLYTEK Co., Ltd.
File name:      iss_sr.h
Description:    讯飞语音套件-语音识别.
                简写说明:
                ISS : iFLYTEK Speech Suite,讯飞语音套件;
                SR  : Speech Recognize,语音识别;
Author:         iFLYTEK
Version:        1.0
Date:           2013-10-10
History:        2013-10-10  创建
*******************************************************************************/
#ifndef _ISS_SR_H_
#define _ISS_SR_H_

#include "iss_types.h"
#include "iss_auth.h"
#include "iss_chipauth.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 语音识别句柄 */
typedef void*   HISSSR;

enum tagISS_SR_MSG{
/*******************************************************************************
MsgNum:      ISS_SR_MSG_InitStatus
Description: 初始化是否成功
wParam:      ISS_SUCCESS:初始化成功
             ISS_ERROR_FAIL:初始化失败 
             ISS_ERROR_OUT_OF_MEMORY:内存不足
lParam:      NULL
Others:      创建识别句柄后，会通过ISS_SR_MSG_InitStatus消息通知识别引擎初始化
             是否成功
*******************************************************************************/
    ISS_SR_MSG_InitStatus              =   20000,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_UpLoadDictToLocalStatus
Description: 本地个性化数据上传是否成功
wParam:      ISS_SUCCESS:本地个性化数据上传成功
             ISS_ERROR_INVALID_JSON_FMT:输入的Json格式有问题
             ISS_ERROR_INVALID_JSON_INFO:没有从Json输入中提取到必要的个性化数据
lParam:      NULL
Others:      用户调用ISSSRUpLoadDict异步上传个性化数据后，
             如果参数bOnlyUploadToCloud为零，
             会通过ISS_SR_MSG_UpLoadDictToLocalStatus消息通知本地数据上传是否成功
*******************************************************************************/
    ISS_SR_MSG_UpLoadDictToLocalStatus =   20001,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_UpLoadDictToCloudStatus
Description: 云端个性化数据上传是否成功
wParam:      ISS_SUCCESS:云端个性化数据上传成功
             ISS_ERROR_INVALID_JSON_FMT:输入的Json格式有问题
             ISS_ERROR_INVALID_JSON_INFO:没有从Json输入中提取到必要的个性化数据
             ISS_ERROR_NET_XXXX:网络错误
             ISS_ERROR_FAIL:云端个性化数据上传失败
lParam:      NULL
Others:      用户调用ISSSRUpLoadDict异步上传个性化数据后，
             会通过ISS_SR_MSG_UpLoadDictToCloudStatus消息通知云端数据上传是否成功
*******************************************************************************/
    ISS_SR_MSG_UpLoadDictToCloudStatus =   20002,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_VolumeLevel
Description: 声音能量，每计算25ms的录音返回一次(40Hz)。
wParam:      音量大小，最小为0，最大为931
lParam:      NULL
Others:      用于在界面中显示麦克风效果
*******************************************************************************/
    ISS_SR_MSG_VolumeLevel             =   20003,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_ResponseTimeout
Description: 响应超时,没有在预定的时间内检测到语音
wParam:      NULL
lParam:      NULL
Others:      识别引擎已经停止,不需要再写入数据
*******************************************************************************/
    ISS_SR_MSG_ResponseTimeout         =   20004,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_SpeechStart
Description: 检测到语音开始点
wParam:      NULL
lParam:      NULL
Others:      
*******************************************************************************/
    ISS_SR_MSG_SpeechStart             =   20005,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_SpeechTimeOut
Description: 语音超时
wParam:      NULL
lParam:      NULL
Others:      正在进行识别处理,不需要再写入数据
*******************************************************************************/
    ISS_SR_MSG_SpeechTimeOut           =   20006,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_SpeechEnd
Description: 检测到语音结束点
wParam:      NULL
lParam:      NULL
Others:      正在进行识别处理,不需要再写入数据
*******************************************************************************/
    ISS_SR_MSG_SpeechEnd               =   20007,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_Error
Description: 出现其他错误
wParam:      错误码
             ISS_ERROR_CHIP_LOAD_INFO:获取授权芯片信息时失败;
             ISS_ERROR_CHIP_CRYPT_FAIL:授权芯片解密失败.
lParam:      const char*类型，错误信息
Others:      识别引擎已经停止,不需要再写入数据
*******************************************************************************/
    ISS_SR_MSG_Error                   =   20008,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_Result
Description: 返回带语义的识别结果，识别结果的解析，参见《语义协议框架》。
             当返回的是本地识别结果时，会附加一个表示识别置信度得分的
             "nlocalconfidencescore"标签，本地识别的转写结果的置信度得分是0。
wParam:      NULL
lParam:      const char*类型，识别结果
Others:      一次识别开始后，只会出一次结果，识别引擎已经停止.
*******************************************************************************/
    ISS_SR_MSG_Result                  =   20009,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_LoadBigSrResStatus
Description: 延迟加载本地识别大的转写资源的状态
wParam:      ISS_ERROR_OUT_OF_MEMORY
             ISS_ERROR_FAIL
             ISS_SUCCESS
lParam:      NULL
Others:      
*******************************************************************************/
    ISS_SR_MSG_LoadBigSrResStatus      =   20010,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_ErrorDecodingAudio
Description: 如果套件为硬件加密版本，为了避免硬件加密的录音对影响云端识别效果，
             在往云端送录音时需要对录音解密，isssr.cfg中需要打开解密录音的配置：
             "DecodeAndioBeforeSendToCloud": 1。
             如果在解密时，发现送入的录音不是硬件加密的录音，会发出这个消息，警告
             用户可能在使用普通mic进行录音。
wParam:      NULL
lParam:      NULL
Others:      
*******************************************************************************/
    ISS_SR_MSG_ErrorDecodingAudio      =   20011,

/*******************************************************************************
MsgNum:      ISS_SR_MSG_PreResult
Description: 混合识别模式时，根据本地识别结果给出预结果的业务名，允许的业务名通过
             配置OutputFocusAsPreResultForHMITip来限定。如果本地早于云端给出了必
             须依赖服务端信源的focus的结果，则先给出focus名，用于HMI提示，如当前
             没有网络链接、正在查询XX。
wParam:      NULL
lParam:      const char*类型，focus name
Others:      
*******************************************************************************/
    ISS_SR_MSG_PreResult               =   20012,
};

/*******************************************************************************
Function:       Proc_OnMsgProc
Description:    消息回调
Calls:          略
Called By:      语音识别引擎
Return:         无
Others:         引擎通过Proc_OnMsgProc回调发出各种消息
*******************************************************************************/
typedef void (ISSCALLBACK *Proc_OnMsgProc)(
    const void*         pUsrArg,      /* [In] 用户自定义参数 */
    unsigned int        uMsg,         /* [In] 消息标示符 */
    unsigned int        wParam,       /* [In] 消息附加信息 */
    const void*         lParam        /* [In] 消息附加信息 */
);

#define ISS_SR_SCENE_ALL              "all"                 // 全场景
#define ISS_SR_SCENE_POI              "poi"                 // POI导航场景
#define ISS_SR_SCENE_CONTACTS         "contacts"            // 联系人场景
#define ISS_SR_SCENE_SELECT           "select"              // 选择场景
#define ISS_SR_SCENE_CONFIRM          "confirm"             // 确认场景
#define ISS_SR_SCENE_ANSWER_CALL      "answer_call"         // 接听来电场景

// mode为ISS_SR_MODE_LOCAL_CMDLIST时，本地使用全场景，并开启云端识别
#define ISS_SR_SCENE_CMDLIST_WITHALL  "cmdlist_withall"

/*
    使用本地多唤醒实现的选择场景, 可以说: 第一个, 第二个, 第三个, 
    第四个, 第五个, 第六个, 最后一个, 取消
*/
#define ISS_SR_SCENE_SELECT_MVW         "select_mvw"

/*
    使用本地多唤醒实现的确认场景, 可以说: 确定, 取消
*/
#define ISS_SR_SCENE_CONFIRM_MVW        "confirm_mvw"

/*
    使用本地多唤醒实现的接听来电场景, 可以说: 接听, 挂断, 取消
*/
#define ISS_SR_SCENE_ANSWER_CALL_MVW    "answer_call_mvw"

#define ISS_SR_MODE_CLOUD_REC       (0)              // 纯网络端识别
#define ISS_SR_MODE_LOCAL_REC       (1)              // 纯本地端识别
#define ISS_SR_MODE_MIX_REC         (2)              // 云加端混合识别
#define ISS_SR_MODE_LOCAL_CMDLIST   (3)              // 纯本地命令词
#define ISS_SR_MODE_LOCAL_NLP       (4)              // 纯本地语义

#define ISS_SR_MODE_LOCAL_MVW       (5)

#define ISS_SR_PARAM_IAT_EXTEND_PARAMS  "iatextendparams"
#define ISS_SR_PARAM_SPEECH_TIMEOUT     "speechtimeout"
#define ISS_SR_PARAM_RESPONSE_TIMEOUT   "responsetimeout"
#define ISS_SR_PARAM_SPEECH_TAIL        "speechtail"
#define ISS_SR_PARAM_LONGTITUDE         "longitude"
#define ISS_SR_PARAM_LATITUDE           "latitude"
#define ISS_SR_PARAM_CITY               "city"

#define ISS_SR_PARAM_TRACE_LEVEL              "tracelevel"
#define ISS_SR_PARAM_TRACE_LEVEL_VALUE_NONE   "none"
#define ISS_SR_PARAM_TRACE_LEVEL_VALUE_ERROR  "error"
#define ISS_SR_PARAM_TRACE_LEVEL_VALUE_INFOR  "infor"
#define ISS_SR_PARAM_TRACE_LEVEL_VALUE_DEBUG  "debug"

/*******************************************************************************
Function:       ISSTTSCreate
Description:    创建识别句柄.
                在调用ISSTTSCreate创建识别句柄前,
                必须已经调用ISSSetMachineCode设置机器码.
Calls:          C标准函数库
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功创建语音识别句柄;
                ISS_ERROR_MACHINE_CODE_NOT_SET:还未设置机器码;
                ISS_ERROR_FILE_NOT_FOUND:没有找到相应的资源文件;
                ISS_ERROR_INVALID_PARA:无效的参数值;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSSRCreate(
    HISSSR*         phISSSRObj,         /* [Out] 识别句柄 */
    const char*     szResourceDir,      /* [In] 资源目录,一般情况下:
                                                在windows中文环境下为GBK编码,
                                                在linux环境下为UTF-8编码 */
    Proc_OnMsgProc  pfnOnMsgProc,       /* [In] 消息回调 */
    void*           pUsrArg,            /* [In] 用户自定义参数 */
    int             bLoadBigSrResDelay  /* [In] 是否延迟加载本地识别大的转写资源 */
);
#ifdef ISS_WCHAR_SUPPORT
ISSErrID ISSAPI ISSSRCreateW(
    HISSSR*         phISSSRObj,         /* [Out] 识别句柄 */
    const wchar_t*  szResourceDir,      /* [In] 资源目录,一般情况下:
                                                在windows vc环境下为UNICODE-16编码,
                                                在linux gcc环境下为UNICODE-32编码 */
    Proc_OnMsgProc  pfnOnMsgProc,       /* [In] 消息回调 */
    void*           pUsrArg,            /* [In] 用户自定义参数 */
    int             bLoadBigSrResDelay  /* [In] 是否延迟加载本地识别大的转写资源 */
);
#endif

/*******************************************************************************
Function:       ISSSRDestroy
Description:    销毁语音识别句柄
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功销毁语音识别句柄并释放资源;
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSSRDestroy(
    HISSSR hISSSRObj   /* [In] 识别句柄 */
);

/*******************************************************************************
Function:       ISSSRSetParam
Description:    参数设置
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功设置参数;
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
                ISS_ERROR_INVALID_PARA:无效的参数;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSSRSetParam(
    HISSSR hISSSRObj,         /* [In] 识别句柄 */
    const char* szParam,      /* [In] 参数,支持
                                      ISS_SR_PARAM_XXXX
                               */
    const char* szParamValue  /* [In] 参数值 */
);

/*******************************************************************************
Function:       ISSSRLoadBigSrRes
Description:    主动加载本地识别大的转写资源,加载是否成功会通过消息
                ISS_SR_MSG_LoadBigSrResStatus返回
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:
                ISS_ERROR_INVALID_CALL:
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSSRLoadBigSrRes(
    HISSSR hISSSRObj         /* [In] 识别句柄 */
);

/*******************************************************************************
Function:       ISSSRStart
Description:    开始一次语音识别,如果已经处于识别状态,会停止之前的识别,重新开始这次
                新的识别
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功启动一次语音识别;
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值,输入了错误的场景或识别模式;
				ISS_ERROR_INVALID_JSON_FMT:输入的Json格式有问题
				ISS_ERROR_INVALID_JSON_INFO:没有从Json输入中提取到必要的语法信息
				ISS_ERROR_BUILDING_GRM:配置文件CouldStartLocalRecogniseWhenBuildLocalGrm
				设为0后，如启动本地识别时 正在构建本地语法，则返回这一错误码
Others:         当i_Mode = ISS_SR_MODE_LOCAL_CMDLIST时,编译命令词表的动作是在套件维护的线程中完成的，
                不会阻塞ISSSRStart接口。
				在Proc_OnInitStatus返回成功后才能调用ISSSRStart开启一次识别。
                ISSSRStart返回ISS_SUCCESS后，就可以调用ISSSRAppendAudioData送录音了。
szCmd的示例:
i_Mode = ISS_SR_MODE_LOCAL_CMDLIST(废弃 但仍支持)、i_Mode = ISS_SR_MODE_LOCAL_REC时、
或者 i_Mode = ISS_SR_MODE_MIX_REC时:
{
    "grm": [
        {
            "dictname": "cmd",
            "dictcontant": [
                { "name": "第一个", "id": 0 },
                { "name": "第二个", "id": 1 },
                { "name": "第三个", "id": 2 },
                { "name": "最后一个", "id": 3 }
            ]
        }
    ]
}

i_Mode =  ISS_SR_MODE_LOCAL_NLP时:
{
    "nlptext": "今天天气怎么样？"
}
*******************************************************************************/
ISSErrID ISSAPI ISSSRStart(
    HISSSR      hISSSRObj, /* [In] 识别句柄 */
    const char* szScene,   /* [In] 指定场景，限定识别范围。
                                   支持：ISS_SR_SCENE_ALL、
                                         ISS_SR_SCENE_POI、
                                         ISS_SR_SCENE_CONTACTS、
                                         ISS_SR_SCENE_SELECT、
                                         ISS_SR_SCENE_CONFIRM、
										 ISS_SR_SCENE_CMDLIST_WITHALL(废弃 但仍支持)
										 (iMode为ISS_SR_MODE_LOCAL_CMDLIST时，并联全场景)、
                                         ISS_SR_SCENE_SELECT_MVW(此时忽略iMode参数)、
                                         ISS_SR_SCENE_CONFIRM_MVW(此时忽略iMode参数)、
                                         ISS_SR_SCENE_ANSWER_CALL_MVW(此时忽略iMode参数)
                            */
    int         iMode,     /* [In] i_Mode = ISS_SR_MODE_CLOUD_REC,纯网络端识别;
                                   i_Mode = ISS_SR_MODE_LOCAL_REC,纯本地端识别;
                                   i_Mode = ISS_SR_MODE_MIX_REC,云加端识别;
                                   i_Mode = ISS_SR_MODE_LOCAL_CMDLIST(废弃 但仍支持),本地命令词识别
                                            ,此时忽略szScene参数;
                                   i_Mode = ISS_SR_MODE_LOCAL_NLP,纯本地语义,
                                            ,此时忽略szScene参数.
                            */
    const char* szCmd      /* [In] i_Mode = ISS_SR_MODE_LOCAL_CMDLIST(废弃 但仍支持)、
                                   i_Mode = ISS_SR_MODE_LOCAL_REC时、
                                   或者 i_Mode = ISS_SR_MODE_MIX_REC时,
                                   为本地识别所用并联的命令词列表,json格式;
								   i_Mode = ISS_SR_MODE_LOCAL_NLP时,
                                   为送给语义解析的字符串 UTF8编码,json格式 */
);

/*******************************************************************************
Function:       ISSSRAppendAudioData
Description:    输入录音数据,支持输入16K采样率、S16-LE、单声道的PCM录音
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功写入录音，但写入的录音的大小可能为0，需要判断
                *pNumberOfWritten != nNumberOfToWrite 来判断是否全部写入
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_INVALID_CALL:错误调用(还未调用ISSSRStart,识别还没有开始);
Others:         必须处于识别启动状态下才能输入音频数据.
                为了避免语音识别引擎内部缓冲区溢出,增加了pNumberOfWritten参数.用
                户程序在发生*pNumberOfWritten != nNumberOfToWrite时,可以等待一会
                再写入或者丢弃这个缓冲区数据(一般智能手机,车机处理器不会出现语音
                识别处理跟不上效率的情况,有可能一次写入的数据太大,或者是写测试程
                序时).
                请注意有可能在写入数据期间,语音识别引擎回调识别函数.
*******************************************************************************/
ISSErrID ISSAPI ISSSRAppendAudioData(
    HISSSR          hISSSRObj,          /* [In] 识别句柄 */
    short*          pSamples,           /* [In] 输入的音频buffer地址 */
    unsigned int    nNumberOfToWrite,   /* [In] 要写入的采样点的个数,非字节数 */
    unsigned int*   pNumberOfWritten    /* [Out] 实际写入的采样点的个数 */
);

/*******************************************************************************
Function:       ISSSREndAudioData
Description:    用户点击了界面上的 “我说完了”，表示主动停止传送录音，
                不需要再写入数据
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS: 成功;
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
                ISS_ERROR_INVALID_CALL:错误调用(还未调用ISSSRStart,识别还没有开始);
				ISS_ERROR_NO_SPEECH:没有检测到使用者语音,
				识别引擎已经停止,不需要再写入数据;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSSREndAudioData(
    HISSSR hISSSRObj           /* [In] 识别句柄 */
);

/*******************************************************************************
Function:       ISSSRStop
Description:    停止此次识别
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功停止了此次识别;
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
                ISS_ERROR_INVALID_CALL:错误调用(还未调用ISSSRStart,识别还没有开始);
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSSRStop(
    HISSSR hISSSRObj        /* [In] 识别句柄 */
);

/*******************************************************************************
Function:       ISSSRUpLoadDict
Description:    异步上传个性化词典。
                本地个性化词典支持songs、singer、contact、apps、videos，
                云端个性化词典支持contact、hotwords.
                个性化词典上传是否成功，会通过回调返回消息:
                ISS_SR_MSG_UpLoadDictToLocalStatus表示上传到本地是否成功;
                ISS_SR_MSG_UpLoadDictToCloudStatus表示上传到云端是否成功;
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音识别引擎返回的错误码.
                ISS_SUCCESS:成功添加了一个异步编语法的任务;
                ISS_ERROR_INVALID_HANDLE:无效语音识别句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
Others:         异步接口。在创建好句柄后，不用等ISS_SR_MSG_InitStatus的消息，
                就可以调用ISSSRUpLoadDict上传词典了。
                如果最后一个参数bOnlyUploadToCloud为0, 则:
                每次调用ISSSRUpLoadDict接口后,
                返回消息ISS_SR_MSG_UpLoadDictToLocalStatus,标识本地个性化数据上传是否成功,
                返回消息ISS_SR_MSG_UpLoadDictToCloudStatus,表示云端个性化数据上传是否成功;
                如果最后一个参数bOnlyUploadToCloud为非0, 则:
                每次调用ISSSRUpLoadDict接口后,
                返回消息ISS_SR_MSG_UpLoadDictToCloudStatus,表示云端个性化数据上传是否成功.
语法示例:
{
    "grm": [
        {
            "dictname": "contact",
            "dictcontant": [
                { "name": "科大讯飞", "id": 0 },
                { "name": "白雪", "id": 1 },
                { "name": "刘俊峰",    "id": 2 },
                { "name": "白亚伟", "id": 3 }
            ]
        },
        {
            "dictname": "singers",
            "dictcontant": [
                { "name": "凤凰传奇", "id": 0 },
                { "name": "那英", "id": 1 },
                { "name": "杨坤", "id": 2 },
                { "name": "S.H.E", "id": 3 }
            ]
        },
        {
            "dictname": "songs",
            "dictcontant": [
                { "name": "恭喜发财", "id": 0 },
                { "name": "偏爱", "id": 1 },
                { "name": "铿锵玫瑰", "id": 2 },
                { "name": "孤枕难眠", "id": 3 }
            ]
        },
		{
            "dictname": "apps",
            "dictcontant": [
                { "name": "讯飞语点", "id": 0 },
                { "name": "愤怒的小鸟", "id": 1 },
                { "name": "浏览器", "id": 2 }
            ]
        },
        {
            "dictname": "videos",
            "dictcontant": [
                { "name": "笔仙", "id": 0 },
                { "name": "冰封重生之门", "id": 1 },
                { "name": "五月天诺亚方舟", "id": 2 },
                { "name": "美国骗局", "id": 3 },
                { "name": "城市游戏", "id": 4 },
                { "name": "催眠大师", "id": 5 },
                { "name": "整容日记", "id": 6 },
                { "name": "疯狂72小时", "id": 7 },
                { "name": "窃听风云", "id": 8 },
                { "name": "同桌的你", "id": 9 }
            ]
        },
        {
            "dictname": "hotwords",
            "dictcontant": [
                { "name": "导航到沸腾渔乡", "id": 0 },
                { "name": "我想去沸腾渔乡", "id": 1 },
                { "name": "导航到锦明大厦", "id": 2 },
                { "name": "我想去锦明大厦", "id": 3 },
                { "name": "导航到星华村", "id": 4 },
                { "name": "我想去星华村", "id": 5 },
                { "name": "导航到兴龙会馆", "id": 6 },
                { "name": "我想去兴龙会馆", "id": 7 },
                { "name": "导航到乐嘉商务宾馆", "id": 8 },
                { "name": "我想去乐嘉商务宾馆", "id": 9 },
                { "name": "导航到青松园药房", "id": 10 },
                { "name": "我要去青松园药房", "id": 11 }
            ]
        }
    ]
}
*******************************************************************************/
ISSErrID ISSAPI ISSSRUpLoadDict(
    HISSSR hISSSRObj,           /* [In] 识别句柄 */
    const char *szList,         /* [In] 语法信息，Json格式 */
    int bOnlyUploadToCloud      /* [In] 是否只上传词典到云端 */
);

#ifdef __cplusplus
}
#endif

#endif
