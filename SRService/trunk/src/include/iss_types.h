/******************************************************************************
Copyright:      1999-2013, iFLYTEK Co., Ltd.
File name:      iss_types.h
Description:    讯飞语音套件类型定义.
                简写说明:
                ISS : iFLYTEK Speech Suite,讯飞语音套件;
Author:         讯飞车载组
Version:        1.0
Date:           2013-10-01
History:        2013-10-01  创建
*******************************************************************************/

#ifndef __ISS_TYPES_H__
#define __ISS_TYPES_H__


// #define ISS_SOFTWARE_ENCRYPTION

// 函数调用方式
#if !defined(ISSCALL)
#if defined(_WIN32)
    #define ISSCALL __stdcall
#else
    #define ISSCALL
#endif /* _WIN32 */
#endif /* ISSCALL */

// 函数导出符号
#if defined _WIN32 || defined __CYGWIN__
#define ISS_DLL_IMPORT __declspec(dllimport)
#define ISS_DLL_EXPORT __declspec(dllexport)
#define ISS_DLL_LOCAL
#define ISS_WCHAR_SUPPORT
#else
#if __GNUC__ >= 4
#define ISS_DLL_IMPORT __attribute__ ((visibility ("default")))
#define ISS_DLL_EXPORT __attribute__ ((visibility ("default")))
#define ISS_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define ISS_DLL_IMPORT
#define ISS_DLL_EXPORT
#define ISS_DLL_LOCAL
#endif
#endif

#ifdef ISS_DLL_EXPORTS // defined if we are building the DLL (instead of using it)
#define ISSAPI ISS_DLL_EXPORT ISSCALL
#else
#define ISSAPI ISS_DLL_IMPORT ISSCALL
#endif // ISS_DLL_EXPORTS

#define ISSLOCAL ISS_DLL_LOCAL ISSCALL

#define ISSCALLBACK ISSCALL

#define ISS_RES_DIR_MAX_PATH    256

#include "iss_errors.h"

#endif /* __ISS_TYPES_H__ */
