/******************************************************************************
Copyright:      1999-2014, iFLYTEK Co., Ltd.
File name:      iss_chipauth.h
Description:    讯飞语音套件-访问硬件授权芯片模块.
                简写说明:
                ISS : iFLYTEK Speech Suite,讯飞语音套件;
Author:         iFLYTEK
Version:        1.0
Date:           2014-8-27
History:        2014-8-27  创建
*******************************************************************************/
#ifndef _ISS_CHIPAUTH_H_
#define _ISS_CHIPAUTH_H_

#include "iss_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
Function:       Proc_ISSChipLoadinfo
Description:    获取授权芯片信息的回调
Calls:          访问硬件授权芯片
Called By:      采用芯片加密版本的套件
Return:         ISSErrID    获取芯片信息成功还是失败
                ISS_SUCCESS:获取芯片信息成功;
Others:         在不同的客户平台，访问硬件授权芯片的接口不完全一致，需要客户在
                回调中获取授权芯片信息。
*******************************************************************************/
typedef ISSErrID (ISSCALLBACK *Proc_ISSChipLoadinfo) (
    const unsigned char* pHint,    /* [In]  8字节的解密提示信息 */
    unsigned char* pChipinfo       /* [Out] 从授权芯片读取的32字节解密提示信息 */
);

/*******************************************************************************
Function:       Proc_OnISSChipCryptError
Description:    芯片授权失败的回调
Calls:          
Called By:      采用芯片加密版本的套件
Return:
Others:         芯片授权失败时会回调这一接口
*******************************************************************************/
typedef void (ISSCALLBACK *Proc_OnISSChipCryptError) (
    void* pUsrArg,
    ISSErrID err    /* [In] 芯片授权失败的错误码
                        ISS_ERROR_INVALID_PARA:没有设置获取授权芯片信息的回调;
                        ISS_ERROR_CHIP_LOAD_INFO:获取授权芯片信息时失败;
                        ISS_ERROR_CHIP_CRYPT_FAIL:授权芯片解密失败.
                      */
);

/*******************************************************************************
Function:       ISSSetChipLoadinfoCb
Description:    设置 获取授权芯片信息的回调
Calls:          略
Called By:      用户程序
Return:         ISSErrID    回调函数设置成功还是失败
                ISS_SUCCESS:回调函数设置成功;
                ISS_ERROR_INVALID_PARA:无效的参数值，如回调函数指针为空;
Others:         采用芯片加密版本的套件，在调用套件其他接口前，必须先调用这一接口
*******************************************************************************/
ISSErrID ISSAPI ISSSetChipLoadinfoCb(
    Proc_ISSChipLoadinfo pfnChipLoadinfo
);

#ifdef __cplusplus
}
#endif

#endif /* _ISS_CHIPAUTH_H_ */
