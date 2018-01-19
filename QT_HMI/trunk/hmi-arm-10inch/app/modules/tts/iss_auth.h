/******************************************************************************
Copyright:      1999-2013, iFLYTEK Co., Ltd.
File name:      iss_auth.h
Description:    讯飞语音套件-授权.
                简写说明:
                ISS  : iFLYTEK Speech Suite,讯飞语音套件;
                auth : Authorization,授权;
Author:         iFLYTEK
Version:        1.0
Date:           2013-12-04
History:        2013-12-04  创建
*******************************************************************************/
#ifndef __ISS_AUTH_H__
#define __ISS_AUTH_H__

#include "iss_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
Function:       ISSSetMachineCode
Description:    输入机器码.
                在调用ISSGetActiveKey获取激活码前、在调用ISSActivate激活前、
                在调用ISSSRCreate创建识别句柄前, 都需要至少调用一次此接口.
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音套件授权激活的错误码.
                ISS_SUCCESS:输入机器码成功;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_FAIL:输入机器码失败;
Others:         每个客户端必须输入不同的机器码,机器码的长度不能超过1024.
                每个机器码每天最多只能访问云端进行1000次识别.
                机器码必须唯一, 用于标识个性化资源.
*******************************************************************************/
ISSErrID ISSAPI ISSSetMachineCode(
    const char* szMachineCode      /* [In] 机器码 */
);

#ifdef ISS_SOFTWARE_ENCRYPTION

/*******************************************************************************
Function:       ISSGetActiveKey
Description:    获取激活码，只需要在联网状态下成功调用一次.
                在调用ISSGetActiveKey获取激活码前, 必须已经调用ISSSetMachineCode
                设置机器码.
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音套件授权激活的错误码.
                ISS_SUCCESS:获取激活码成功;
                ISS_ERROR_FAIL:获取激活码失败;
                ISS_ERROR_MACHINE_CODE_NOT_SET:还未设置机器码;
                ISS_ERROR_INVALID_MACHINE_CODE:此机器码未经授权;
                ISS_ERROR_ACTIVATE_TO_MANY_TIMES:次机器码授权超出;
                ISS_ERROR_NET_XXXX:网络错误;
Others:         同步接口, 出错会返回 错误码.
*******************************************************************************/
ISSErrID ISSAPI ISSGetActiveKey(
    const char*     szResourceDir       /* [In] 资源目录,一般情况下:
                                                在windows中文环境下为GBK编码,
                                                在linux环境下为UTF-8编码 */
);

/*******************************************************************************
Function:       ISSActivate
Description:    输入激活码目录,激活. 
                在调用ISSSRCreate创建识别句柄前、在调用ISSTTSInitRes创建
                语音合成资源句柄前, 都需要至少调用一次此接口.
                在调用ISSActivate激活前, 必须已经调用ISSSetMachineCode设置机器码.
Calls:          略
Called By:      用户程序
Return:         ISSErrID    语音套件授权激活的错误码.
                ISS_SUCCESS:授权激活成功;
                ISS_ERROR_INVALID_PARA:无效的参数值;
                ISS_ERROR_MACHINE_CODE_NOT_SET:还未设置机器码;
                ISS_ERROR_INVALID_ACTIVE_KEY:无效的激活码;
Others:         
*******************************************************************************/
ISSErrID ISSAPI ISSActivate(
    const char*  szResourceDir          /* [In] 资源目录 */
);

#endif

#ifdef __cplusplus
}
#endif

#endif  /* __ISS_AUTH_H__ */