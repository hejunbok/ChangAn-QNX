/******************************************************************************
Copyright:      1999-2013, iFLYTEK Co., Ltd.
File name:      iss_tts.h
Description:    讯飞语音套件-语音合成.
                简写说明:
                ISS : iFLYTEK Speech Suite,讯飞语音套件;
                TTS : Text To Speech,语音合成;
Author:         iFLYTEK
Version:        1.0
Date:           2013-10-10
History:        2013-10-10  创建
*******************************************************************************/
#ifndef _ISS_TTS_H_
#define _ISS_TTS_H_

#include "iss_types.h"
#include "iss_chipauth.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 语音合成资源句柄 */
typedef void* HISSTTSRES;

/* 语音合成句柄 */
typedef void* HISSTTS;

// 输入TTS文本的代码页
typedef enum ISS_TTS_CODEPAGE {
    ISS_TTS_CODEPAGE_GBK = 936,
    ISS_TTS_CODEPAGE_UTF16LE = 1200,
    ISS_TTS_CODEPAGE_UTF8 = 65001
} ISSTTSCodePage;

#define ISS_TTS_PARAM_SPEAKER (0x00000500)
typedef enum tagESpeaker {
    ivTTS_ROLE_TIANCHANG = 1,   /* Tianchang (female, Chinese) */
    ivTTS_ROLE_WENJING = 2,     /* Wenjing (female, Chinese) */
    ivTTS_ROLE_XIAOYAN = 3,     /* Xiaoyan (female, Chinese) */
    ivTTS_ROLE_YANPING = 3,     /* Xiaoyan (female, Chinese) */
    ivTTS_ROLE_XIAOFENG = 4,    /* Xiaofeng (male, Chinese) */
    ivTTS_ROLE_YUFENG = 4,      /* Xiaofeng (male, Chinese) */
    ivTTS_ROLE_SHERRI = 5,      /* Sherri (female, US English) */
    ivTTS_ROLE_XIAOJIN = 6,     /* Xiaojin (female, Chinese) */
    ivTTS_ROLE_NANNAN = 7,      /* Nannan (child, Chinese) */
    ivTTS_ROLE_JINGER = 8,      /* Jinger (female, Chinese) */
    ivTTS_ROLE_JIAJIA = 9,      /* Jiajia (girl, Chinese) */
    ivTTS_ROLE_YUER = 10,       /* Yuer (female, Chinese) */
    ivTTS_ROLE_XIAOQIAN = 11,   /* Xiaoqian (female, Chinese Northeast) */
    ivTTS_ROLE_LAOMA = 12,      /* Laoma (male, Chinese) */
    ivTTS_ROLE_BUSH = 13,       /* Bush (male, US English) */
    ivTTS_ROLE_XIAORONG = 14,   /* Xiaorong (female, Chinese Szechwan) */
    ivTTS_ROLE_XIAOMEI = 15,    /* Xiaomei (female, Cantonese) */
    ivTTS_ROLE_ANNI = 16,       /* Anni (female, Chinese) */
    ivTTS_ROLE_JOHN = 17,       /* John (male, US English) */
    ivTTS_ROLE_ANITA = 18,      /* Anita (female, British English) */
    ivTTS_ROLE_TERRY = 19,      /* Terry (female, US English) */
    ivTTS_ROLE_CATHERINE = 20,  /* Catherine (female, US English) */
    ivTTS_ROLE_TERRYW = 21,     /* Terry (female, US English Word) */
    ivTTS_ROLE_XIAOLIN = 22,    /* Xiaolin (female, Chinese) */
    ivTTS_ROLE_XIAOMENG = 23,   /* Xiaomeng (female, Chinese) */
    ivTTS_ROLE_XIAOQIANG = 24,  /* Xiaoqiang (male, Chinese) */
    ivTTS_ROLE_XIAOKUN = 25,    /* XiaoKun (male, Chinese) */
    ivTTS_ROLE_JIUXU = 51,      /* Jiu Xu (male, Chinese) */
    ivTTS_ROLE_DUOXU = 52,      /* Duo Xu (male, Chinese) */
    ivTTS_ROLE_XIAOPING = 53,   /* Xiaoping (female, Chinese) */
    ivTTS_ROLE_DONALDDUCK = 54, /* Donald Duck (male, Chinese) */
    ivTTS_ROLE_BABYXU = 55,     /* Baby Xu (child, Chinese) */
    ivTTS_ROLE_DALONG = 56,     /* Dalong (male, Cantonese) */
    ivTTS_ROLE_TOM = 57,        /* Tom (male, US English) */
    ivTTS_ROLE_USER = 99        /* user defined */
} ESpeaker;

#define ISS_TTS_PARAM_VOICE_SPEED (0x00000502) /* 设置TTS播报的语速 */
#define ISS_TTS_SPEED_MIN (-32768)             /* 最慢语速 */
#define ISS_TTS_SPEED_NORMAL_DEFAULT (0)       /* 正常语速 默认值 */
#define ISS_TTS_SPEED_MAX (+32767)             /* 最快语速 */

#define ISS_TTS_PARAM_VOICE_PITCH (0x00000503) /* 设置TTS播报的声调 */
#define ISS_TTS_PITCH_MIN (-32768)             /* 最低声调 */
#define ISS_TTS_PITCH_NORMAL_DEFAULT (0)       /* 正常声调 默认值 */
#define ISS_TTS_PITCH_MAX (+32767)             /* 最高声调 */

#define ISS_TTS_PARAM_VOLUME (0x00000504)   /* 设置TTS播报的音量 */
#define ISS_TTS_VOLUME_MIN (-32768)         /* 最小音量 */
#define ISS_TTS_VOLUME_NORMAL (0)           /* 正常音量 */
#define ISS_TTS_VOLUME_MAX_DEFAULT (+32767) /* 最大音量 默认值 */

/*******************************************************************************
Function:       ISSTTSInitRes
Description:    创建语音合成资源句柄
Calls:
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功创建语音合成资源句柄;
                ISS_ERROR_FILE_NOT_FOUND:没有找到相应的资源文件;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_OUT_OF_MEMORY:内存不够;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSTTSInitRes(
    HISSTTSRES* phISSTTSRES,   /* [Out] TTS资源句柄 */
    const char* szResourceDir, /* [In] 资源目录,一般情况下:
                                       在windows中文环境下为GBK编码,
                                       在linux环境下为UTF-8编码 */
    int iResMode /* 载入资源的方式:
                    iResMode = 0   打开文件句柄
                    iResMode = 1   直接载入内存 */
    );
#ifdef ISS_WCHAR_SUPPORT
ISSErrID ISSAPI ISSTTSInitResW(
    HISSTTSRES* phISSTTSRES,      /* [Out] TTS资源句柄 */
    const wchar_t* szResourceDir, /* [In] 资源目录,一般情况下:
                                          在windows vc环境下为UNICODE-16编码,
                                          在linux gcc环境下为UNICODE-32编码 */
    int iResMode /* 载入资源的方式:
                iResMode = 0   打开文件句柄
                iResMode = 1   直接载入内存 */
    );
#endif

/*******************************************************************************
Function:       ISSTTSUnInitRes
Description:    销毁语音合成资源句柄
Calls:
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功销毁语音合成资源句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSTTSUnInitRes(HISSTTSRES hISSTTSRES /* [In] TTS资源句柄 */
                                );

/*******************************************************************************
Function:       Proc_OnTTSDataReady
Description:    合成开始后抛出此回调通知，可以调用ISSTTSGetAudioData取音频数据了
Calls:          略
Called By:      语音合成引擎
Return:         无
Others:
*******************************************************************************/
typedef void(ISSCALLBACK* Proc_OnTTSDataReady)(
    void* pUsrArg /* [in] 用户自定义参数 */
    );

/*******************************************************************************
Function:       ISSTTSCreate
Description:    创建语音合成句柄
Calls:          C标准函数库,pthread
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功创建语音合成句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_OUT_OF_MEMORY:内存不够;
Others:         可以使用同一TTS资源句柄创建多个TTS示例
*******************************************************************************/
ISSErrID ISSAPI ISSTTSCreate(
    HISSTTS* phISSTTS,                     /* [Out] TTS句柄 */
    HISSTTSRES hISSTTSRES,                 /* [In]  TTS资源句柄 */
    Proc_OnTTSDataReady pcbOnTTSDataReady, /* [In]
                                              用户OnTTSDataReady回调函数指针 */
#ifdef ISS_CHIP_CRYPT
    Proc_OnISSChipCryptError pcbOnISSChipCryptError, /* [In]
                                              芯片授权失败的回调函数指针 */
#endif
    void* pUsrArg                          /* [In] 用户自定义参数，将在Callback中提供给用户 */
    );

/*******************************************************************************
Function:       ISSTTSDestroy
Description:    销毁语音合成句柄
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功销毁语音合成句柄并释放资源;
                ISS_ERROR_INVALID_HANDLE:无效语音合成句柄;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSTTSDestroy(HISSTTS hISSTTS /* [In] TTS句柄 */
                              );

/*******************************************************************************
Function:       ISSTTSSetParam
Description:    设置参数
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功设置了参数;
                ISS_ERROR_INVALID_HANDLE:无效语音合成句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                                ISS_ERROR_INVALID_PARA_VALUE:无效的参数值
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSTTSSetParam(HISSTTS hISSTTS, /* [In] TTS句柄 */
                               int iParamID,    /* [In] ParamID */
                               int iParamValue  /* [In] ParamValue */
                               );

/*******************************************************************************
Function:       ISSTTSStart
Description:
合成指定文本,如果引擎已经在合成一段文本时，又调用ISSTTSStart去合成
                另一段文本，之前一次的合成任务会直接丢掉.
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功开始一次语音合成;
                ISS_ERROR_INVALID_HANDLE:无效语音合成句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
Others:         传入文本可以使用文本标注来是朗读更加符合预期，具体使用方式，请参考《Aisound5 简单文本标注用户手册.pdf》
*******************************************************************************/
ISSErrID ISSAPI
    ISSTTSStart(HISSTTS hISSTTS,            /* [In] TTS句柄 */
                const void* pText,          /* [In] 合成文本Buffer */
                int iSize,                  /* [In] 合成文本Buffer大小 */
                ISSTTSCodePage iTTSCodePage /* [In] 合成文本的代码页 */
                );

/*******************************************************************************
Function:       ISSTTSGetAudioData
Description:    取音频数据，支持获取16000采样率、S16-LE、单声道的音频，
                一个采样点的大小为2Bytes
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功获取了合成的语音;
                ISS_ERROR_INVALID_HANDLE:无效语音合成句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_TTS_STOPPED:错误调用,合成已经结束,
                                      却调用ISSTTSGetAudioData取录音
                ISS_ERROR_TTS_COMPLETED:取完了所有合成的音频，此次合成结束
Others:         同步阻塞函数
*******************************************************************************/
ISSErrID ISSAPI ISSTTSGetAudioData(
    HISSTTS hISSTTS,            /* [In] TTS句柄 */
    void* pOutBuffer,           /* [In] 音频输出Buff的地址 */
    unsigned int iBufferFrames, /* [In] 期望获取的采样点数,非字节数 */
    unsigned int* piBufferFramesGot, /* [Out] 实际输出的采样点数 */
    unsigned int* piProgBegin, /* [out] 当前进度相对传入文本首地址的偏移量,字节数 */
    unsigned int* piProgLen /* [out] 当前进度所在句子长度,字节数 */
    );

/*******************************************************************************
Function:       ISSTTSStop
Description:    停止合成
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音合成引擎返回的错误码.
                ISS_SUCCESS:成功取消了此次合成;
                ISS_ERROR_INVALID_HANDLE:无效语音合成句柄;
                ISS_ERROR_TTS_STOPPED:错误调用,合成已经结束
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSTTSStop(HISSTTS hISSTTS /* [In] TTS句柄 */
                           );

#ifdef __cplusplus
}
#endif

#endif
