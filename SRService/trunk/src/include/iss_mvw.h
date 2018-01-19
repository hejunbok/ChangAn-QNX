/******************************************************************************
Copyright:      1999-2014, iFLYTEK Co., Ltd.
File name:      iss_mvw.h
Description:    讯飞语音套件-多语音唤醒.
                简写说明:
                ISS : iFLYTEK Speech Suite,讯飞语音套件;
                MVW : Multi Voice Wakeup,多语音唤醒;
Author:         iFLYTEK
Version:        1.0
Date:           2014-3-14
History:        2014-3-14  创建
*******************************************************************************/
#ifndef _ISS_MVW_H_
#define _ISS_MVW_H_

#include "iss_types.h"
#include "iss_chipauth.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 多语音唤醒句柄 */
typedef void*   HISSMVW;

// 场景ID信息及唤醒词ID信息参见配置文件,套件保留场景:
#define ISS_MVW_SCENE_GLOBAL         (0)
#define ISS_MVW_SCENE_CONFIRM        (1)
#define ISS_MVW_SCENE_SELECT         (2)
#define ISS_MVW_SCENE_ANSWER_CALL    (3)

/*******************************************************************************
Function:       Proc_ISSMVWOnWakeup
Description:    多唤醒回调接口
Calls:          略
Called By:      多唤醒引擎
Return:         无
Others:         在用户说出唤醒词时,多唤醒引擎会通过此回调函数通知调用者有一次唤醒.
                如果用户多次说出唤醒词时,会多次调用此回调函数,但不会同时调用,用
                户程序可以在此回调函数中,调用ISSMVWStop进行终止多唤醒引擎.
*******************************************************************************/
typedef void (ISSCALLBACK *Proc_ISSMVWOnWakeup)(
    void*         pUsrArg,     /* [In] 用户自定义参数 */
    unsigned int  nMvwScene,   /* [In] 多唤醒场景 */
    unsigned int  nMvwId,      /* [In] 唤醒词ID */
    signed int    nMvwScore    /* [In] 唤醒的置信度得分 */
);

/*******************************************************************************
Function:       ISSMVWCreate
Description:    创建多语音唤醒句柄
Calls:          C标准函数库,pthread
Called By:      用户程序
Return:         ISSErrID    多语音唤醒引擎返回的错误码.
                ISS_SUCCESS:成功创建多语音唤醒句柄;
                ISS_ERROR_FILE_NOT_FOUND:没有找到相应的资源文件;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_OUT_OF_MEMORY:内存不够;
                ISS_ERROR_INVALID_DATA:资源数据出错;
Others:         不能创建多实例.
*******************************************************************************/
ISSErrID ISSAPI ISSMVWCreate(
    HISSMVW*             phISSMVW,        /* [Out] 多唤醒句柄 */
    const char*          szResourceDir,   /* [In] 资源目录,一般情况下:
                                                  在windows中文环境下为GBK编码,
                                                  在linux环境下为UTF-8编码 */
    Proc_ISSMVWOnWakeup  pfnOnWakeup,     /* [In] 回调接口 */
#ifdef ISS_CHIP_CRYPT
    Proc_OnISSChipCryptError pcbOnISSChipCryptError, /* [In]
                                              芯片授权失败的回调函数指针 */
#endif
    void*                pUsrArg          /* [in] 用户自定义参数 */
);
#ifdef ISS_WCHAR_SUPPORT
ISSErrID ISSAPI ISSMVWCreateW(
    HISSMVW*             phISSMVW,         /* [Out] 多唤醒句柄 */
    const wchar_t*       szResourceDir,    /* [In] 资源目录,一般情况下:
                                                   在windows vc环境下为UNICODE-16编码,
                                                   在linux gcc环境下为UNICODE-32编码 */
    Proc_ISSMVWOnWakeup  pfnOnWakeup,      /* [In] 回调接口 */
#ifdef ISS_CHIP_CRYPT
    Proc_OnISSChipCryptError pcbOnISSChipCryptError, /* [In]
                                              芯片授权失败的回调函数指针 */
#endif
    void*                pUsrArg           /* [in] 用户自定义参数 */
);
#endif

/*******************************************************************************
Function:       ISSMVWDestroy
Description:    销毁多语音唤醒句柄
Calls:          略
Called By:      用户程序
Return:         ISSErrID    多语音唤醒引擎返回的错误码.
                ISS_SUCCESS:成功销毁多语音唤醒句柄并释放资源;
                ISS_ERROR_INVALID_HANDLE:无效多语音唤醒句柄;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSMVWDestroy(
    HISSMVW  hISSMVW    /* [In] 唤醒句柄 */
);

/*******************************************************************************
Function:       ISSMVWSetThreshold
Description:    设置多语音唤醒门限
Calls:          略
Call Sequence:  ISSMVWCreate、ISSMVWSetThreshold
                可以在唤醒开始后调用ISSMVWSetThreshold设置门限
Called By:      用户程序
Return:         ISSErrID    多语音唤醒引擎返回的错误码.
                ISS_SUCCESS:成功设置门限;
                ISS_ERROR_INVALID_HANDLE:无效多语音唤醒句柄;
Others:
*******************************************************************************/
ISSErrID ISSAPI ISSMVWSetThreshold(
    HISSMVW       hISSMVW,         /* [In] 唤醒句柄 */
    unsigned int  nMvwScene,       /* [In] 多唤醒场景 */
    unsigned int  nMvwId,          /* [In] 唤醒词ID */
    int           nThreshold       /* [In] 门限值 */
);

/*******************************************************************************
Function:       ISSMVWStart
Description:    启动多语音唤醒
Calls:          略
Called By:      用户程序
Return:         ISSErrID    多语音唤醒引擎返回的错误码.
                ISS_SUCCESS:成功启动多语音唤醒;
                ISS_ERROR_INVALID_HANDLE:无效多语音唤醒句柄;
Others:         接口是同步的
*******************************************************************************/
ISSErrID ISSAPI ISSMVWStart(
    HISSMVW  hISSMVW,              /* [In] 多唤醒句柄 */
    unsigned int  nMvwScene        /* [In] 多唤醒场景 */
);

/*******************************************************************************
Function:       ISSMVWAppendAudioData
Description:    输入录音数据,支持输入16K采样率、S16-LE、单声道的PCM录音
                只能在一个线程中调用此接口
Calls:          略
Called By:      用户程序
Return:         ISSErrID    多语音唤醒引擎返回的错误码.
                ISS_SUCCESS:成功写入录音，但写入的录音的大小可能为0，需要判断
                *pNumberOfWritten != nNumberOfToWrite 来判断是否全部写入;
                ISS_ERROR_INVALID_HANDLE:无效语音唤醒句柄;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_INVALID_CALL:错误调用(还未调用ISSVWStart,唤醒还没有开始).;
Others:         必须处于唤醒启动状态下才能输入音频数据.
                为了避免语音唤醒引擎内部缓冲区溢出,增加了pNumberOfWritten参数.用
                户程序在发生*pNumberOfWritten != nNumberOfToWrite时,可以等待一会
                再写入或者丢弃这个缓冲区数据(一般智能手机,车机处理器不会出现语音
                唤醒处理跟不上效率的情况,有可能一次写入的数据太大,或者是写测试程
                序时).
                请注意有可能在写入数据期间,多语音唤醒引擎回调唤醒函数.
*******************************************************************************/
ISSErrID ISSAPI ISSMVWAppendAudioData(
    HISSMVW          hISSMVW,             /* [In] 多唤醒句柄 */
    short*           pSamples,            /* [In] 输入的音频buffer地址 */
    unsigned int     nNumberOfToWrite,    /* [In] 要写入的采样点的个数,非字节数 */
    unsigned int*    pNumberOfWritten     /* [Out] 实际写入的采样点的个数 */
);

/*******************************************************************************
Function:       ISSMVWStop
Description:    停止多语音唤醒
                用户程序可以在Proc_ISSMVWOnWakeup回调中,调用ISSMVWStop进行终止唤醒引擎.
Calls:          略
Called By:      用户程序
Return:         ISSErrID    多语音唤醒引擎返回的错误码.
                ISS_SUCCESS:成功停止语音唤醒;
                ISS_ERROR_INVALID_HANDLE:无效语音唤醒句柄;
Others:         接口是同步的
*******************************************************************************/
ISSErrID ISSAPI ISSMVWStop(
    HISSMVW  hISSMVW    /* [In] 唤醒句柄 */
);

#ifdef __cplusplus
}
#endif

#endif  /* _ISS_MVW_H_ */
