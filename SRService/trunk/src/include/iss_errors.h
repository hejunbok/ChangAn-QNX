/******************************************************************************
Copyright:      1999-2013, iFLYTEK Co., Ltd.
File name:      iss_errors.h
Description:    讯飞语音套件错误类型定义.
                简写说明:
                ISS : iFLYTEK Speech Suite,讯飞语音套件;
Author:         iFLYTEK
Version:        1.0
Date:           2013-10-09
History:        2013-10-09  创建
*******************************************************************************/

#ifndef __ISS_ERRORS_H__
#define __ISS_ERRORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 讯飞语音套件返回的错误码类型 */
typedef int ISSErrID;

enum ISS_EXCEPTION
{
    ISS_SUCCESS                             = 0,                            /* 函数执行成功*/
    ISS_ERROR_FAIL                          = -1,                           /* 失败 */
    ISS_ERROR_EXCEPTION                     = -2,                           /* 异常 */

    ISS_ERROR_INVALID_CALL                  = 10000,    /* 0x2710 */        /* 非法调用 */
    ISS_ERROR_INVALID_JSON_FMT              = 10001,    /* 0x2711 */        /* 非法JSON格式 */
    ISS_ERROR_INVALID_JSON_INFO             = 10002,    /* 0x2712 */        /* 无效的JSON信息内容 */
    ISS_ERROR_TTS_STOPPED                   = 10003,    /* 0x2713 */        /* Used */
    ISS_ERROR_TTS_COMPLETED                 = 10004,    /* 0x2714 */        /* Used */
    ISS_ERROR_CREATE_THREAD_FAIL            = 10005,    /* 0x2715 */        /* 创建线程失败 */
    ISS_ERROR_NO_SPEECH                     = 10006,    /* 0x2716 */        /* 没有检测到语音 */
    ISS_ERROR_GET_RESULT_TIMEOUT            = 10007,    /* 0x2717 */        /* 获取结果超时 */
    ISS_ERROR_BUILDGRM_INIT_ERROR           = 10008,    /* 0x2718 */        /* 构建词典初始化错误 */

    ISS_ERROR_INVALID_ACTIVE_KEY            = 10009,    /* 0x2719 */        /* 非法的激活码 */
    ISS_ERROR_INVALID_MACHINE_CODE          = 10010,    /* 0x271A */        /* 非法的机器码 */
    ISS_ERROR_ACTIVATE_TO_MANY_TIMES        = 10011,    /* 0x271B */        /* 激活次数已超过限制 */
    ISS_ERROR_ACCESS_TO_MANY_TIMES          = 10012,    /* 0x271C */        /* 访问次数已超过限制 */
    ISS_ERROR_MACHINE_CODE_NOT_SET          = 10013,    /* 0x271D */        /* 机器码还未设置 */
    ISS_ERROR_GET_ACTIVE_KEY_TIMEOUT        = 10014,    /* 0x271E */        /* 获取激活码超时 */
    ISS_ERROR_NO_RESULT                     = 10015,    /* 0x271F */        /* 识别结果拒识 */
    ISS_ERROR_BUILDING_GRM                  = 10016,    /* 0x2720 */        /* 配置文件
    CouldStartLocalRecogniseWhenBuildLocalGrm设为0后，如启动本地识别时 正在构建本地语法，则返回这一错误码 */

    ISS_ERROR_CHIP_LOAD_INFO                = 10017,    /* 0x2721 */        /* 获取授权芯片信息时失败 */
    ISS_ERROR_CHIP_CRYPT_FAIL               = 10018,    /* 0x2722 */        /* 授权芯片解密失败 */
    ISS_ERROR_CHIP_DVC_NOT_AVAILABLE		= 10019,	/* 0x2723 */		/* 打开芯片加密设备失败  */

    ISS_ERROR_MSC_COK_ERROR                 = 10020,    /* 0x2724 */		/* msc创建连接失败  */

    /* General errors 10100(0x2774) */
    ISS_ERROR_GENERAL                       = 10100,     /* 0x2774 */        /* 基码 */ 
    ISS_ERROR_OUT_OF_MEMORY                 = 10101,     /* 0x2775 */        /* 内存越界 */
    ISS_ERROR_FILE_NOT_FOUND                = 10102,     /* 0x2776 */        /* 文件没有发现 */
    ISS_ERROR_NOT_SUPPORT                   = 10103,     /* 0x2777 */        /* 不支持 */
    ISS_ERROR_NOT_IMPLEMENT                 = 10104,     /* 0x2778 */        /* 没有实现 */
    ISS_ERROR_ACCESS                        = 10105,     /* 0x2779 */        /* 没有权限 */
    ISS_ERROR_INVALID_PARA                  = 10106,     /* 0x277A */        /* 无效的参数 */
    ISS_ERROR_INVALID_PARA_VALUE            = 10107,     /* 0x277B */        /* 无效的参数值 */
    ISS_ERROR_INVALID_HANDLE                = 10108,     /* 0x277C */        /* 无效的句柄 */
    ISS_ERROR_INVALID_DATA                  = 10109,     /* 0x277D */        /* 无效的数据 */
    ISS_ERROR_NO_LICENSE                    = 10110,     /* 0x277E */        /* 没有授权许可 */
    ISS_ERROR_NOT_INIT                      = 10111,     /* 0x277F */        /* 没有初始化 */
    ISS_ERROR_NULL_HANDLE                   = 10112,     /* 0x2780 */        /* 空句柄 */
    ISS_ERROR_OVERFLOW                      = 10113,     /* 0x2781 */        /* 溢出 */
    ISS_ERROR_TIME_OUT                      = 10114,     /* 0x2782 */        /* 超时 */
    ISS_ERROR_OPEN_FILE                     = 10115,     /* 0x2783 */        /* 打开文件出错 */
    ISS_ERROR_NOT_FOUND                     = 10116,     /* 0x2784 */        /* 没有发现 */
    ISS_ERROR_NO_ENOUGH_BUFFER              = 10117,     /* 0x2785 */        /* 没有足够的内存 */
    ISS_ERROR_NO_DATA                       = 10118,     /* 0x2786 */        /* 没有数据 */
    ISS_ERROR_NO_MORE_DATA                  = 10119,     /* 0x2787 */        /* 没有更多的数据 */
    ISS_ERROR_NO_RESPONSE_DATA              = 10120,     /* 0x2788 */        /* 没有反馈数据 */
    ISS_ERROR_ALREADY_EXIST                 = 10121,     /* 0x2789 */        /* 已经存在 */
    ISS_ERROR_LOAD_MODULE                   = 10122,     /* 0x278A */        /* 加载模块错误 */
    ISS_ERROR_BUSY                          = 10123,     /* 0x278B */        /* 忙碌 */
    ISS_ERROR_INVALID_CONFIG                = 10124,     /* 0x278C */        /* 无效的配置项 */
    ISS_ERROR_VERSION_CHECK                 = 10125,     /* 0x278D */        /* 版本错误 */
    ISS_ERROR_CANCELED                      = 10126,     /* 0x278E */        /* 取消 */
    ISS_ERROR_INVALID_MEDIA_TYPE            = 10127,     /* 0x278F */        /* 无效的媒体类型 */
    ISS_ERROR_CONFIG_INITIALIZE             = 10128,     /* 0x2790 */        /* 初始化Config实例 */
    ISS_ERROR_CREATE_HANDLE                 = 10129,     /* 0x2791 */        /* 建立句柄 */
    ISS_ERROR_CODING_LIB_NOT_LOAD           = 10130,     /* 0x2792 */        /* 库未加载 */

    /* Error codes of network 10200(0x27D8)*/
    ISS_ERROR_NET_GENERAL                   = 10200,     /* 0x27D8 */   /* 网络一般错误 */
    ISS_ERROR_NET_OPENSOCK                  = 10201,     /* 0x27D9 */   /* 打开套接字 */
    ISS_ERROR_NET_CONNECTSOCK               = 10202,     /* 0x27DA */   /* 套接字连接 */
    ISS_ERROR_NET_ACCEPTSOCK                = 10203,     /* 0x27DB */   /* 套接字接收 */
    ISS_ERROR_NET_SENDSOCK                  = 10204,     /* 0x27DC */   /* 发送错误 */
    ISS_ERROR_NET_RECVSOCK                  = 10205,     /* 0x27DD */   /* 接收错误 */
    ISS_ERROR_NET_INVALIDSOCK               = 10206,     /* 0x27DE */   /* 无效的套接字 */
    ISS_ERROR_NET_BADADDRESS                = 10207,     /* 0x27EF */   /* 无效的地址 */
    ISS_ERROR_NET_BINDSEQUENCE              = 10208,     /* 0x27E0 */   /* 绑定次序 */
    ISS_ERROR_NET_NOTOPENSOCK               = 10209,     /* 0x27E1 */   /* 套接字没有打开 */
    ISS_ERROR_NET_NOTBIND                   = 10210,     /* 0x27E2 */   /* 没有绑定 */
    ISS_ERROR_NET_NOTLISTEN                 = 10211,     /* 0x27E3 */   /* 没有监听 */
    ISS_ERROR_NET_CONNECTCLOSE              = 10212,     /* 0x27E4 */   /* 连接关闭 */
    ISS_ERROR_NET_NOTDGRAMSOCK              = 10213,     /* 0x27E5 */   /* 非数据报套接字 */
    ISS_ERROR_NET_DNS                       = 10214,     /* 0x27E6 */   /* DNS解析错误 */

    /* Error codes of mssp message 10300(0x283C) */
    ISS_ERROR_MSG_GENERAL                   = 10300,     /* 0x283C */   /* 消息一般错误消息一般错误 */
    ISS_ERROR_MSG_PARSE_ERROR               = 10301,     /* 0x283D */   /* 解析出错 */
    ISS_ERROR_MSG_BUILD_ERROR               = 10302,     /* 0x283E */   /* 构建出错 */
    ISS_ERROR_MSG_PARAM_ERROR               = 10303,     /* 0x283F */   /* 参数出错 */
    ISS_ERROR_MSG_CONTENT_EMPTY             = 10304,     /* 0x2840 */   /* Content为空 */
    ISS_ERROR_MSG_INVALID_CONTENT_TYPE      = 10305,     /* 0x2841 */   /* Content类型无效 */
    ISS_ERROR_MSG_INVALID_CONTENT_LENGTH    = 10306,     /* 0x2842 */   /* Content长度无效 */
    ISS_ERROR_MSG_INVALID_CONTENT_ENCODE    = 10307,     /* 0x2843 */   /* Content编码无效 */
    ISS_ERROR_MSG_INVALID_KEY               = 10308,     /* 0x2844 */   /* Key无效 */
    ISS_ERROR_MSG_KEY_EMPTY                 = 10309,     /* 0x2845 */   /* Key为空 */
    ISS_ERROR_MSG_SESSION_ID_EMPTY          = 10310,     /* 0x2846 */   /* 会话ID为空 */
    ISS_ERROR_MSG_LOGIN_ID_EMPTY            = 10311,     /* 0x2847 */   /* 登录ID为空 */
    ISS_ERROR_MSG_SYNC_ID_EMPTY             = 10312,     /* 0x2848 */   /* 同步ID为空 */
    ISS_ERROR_MSG_APP_ID_EMPTY              = 10313,     /* 0x2849 */   /* 应用ID为空 */
    ISS_ERROR_MSG_EXTERN_ID_EMPTY           = 10314,     /* 0x284A */   /* 扩展ID为空 */
    ISS_ERROR_MSG_INVALID_CMD               = 10315,     /* 0x284B */   /* 无效的命令 */
    ISS_ERROR_MSG_INVALID_SUBJECT           = 10316,     /* 0x284C */   /* 无效的主题 */
    ISS_ERROR_MSG_INVALID_VERSION           = 10317,     /* 0x284D */   /* 无效的版本 */
    ISS_ERROR_MSG_NO_CMD                    = 10318,     /* 0x284E */   /* 没有命令 */
    ISS_ERROR_MSG_NO_SUBJECT                = 10319,     /* 0x284F */   /* 没有主题 */
    ISS_ERROR_MSG_NO_VERSION                = 10320,     /* 0x2850 */   /* 没有版本号 */
    ISS_ERROR_MSG_MSSP_EMPTY                = 10321,     /* 0x2851 */   /* 消息为空 */
    ISS_ERROR_MSG_NEW_RESPONSE              = 10322,     /* 0x2852 */   /* 新建响应消息失败 */
    ISS_ERROR_MSG_NEW_CONTENT               = 10323,     /* 0x2853 */   /* 新建Content失败 */
    ISS_ERROR_MSG_INVALID_SESSION_ID        = 10324,     /* 0x2854 */   /* 无效的会话ID */

    /* Error codes of DataBase 10400(0x28A0)*/
    ISS_ERROR_DB_GENERAL                    = 10400,     /* 0x28A0 */   /* 数据库一般错误 */
    ISS_ERROR_DB_EXCEPTION                  = 10401,     /* 0x28A1 */   /* 异常 */
    ISS_ERROR_DB_NO_RESULT                  = 10402,     /* 0x28A2 */   /* 没有结果 */
    ISS_ERROR_DB_INVALID_USER               = 10403,     /* 0x28A3 */   /* 无效的用户 */
    ISS_ERROR_DB_INVALID_PWD                = 10404,     /* 0x28A4 */   /* 无效的密码 */
    ISS_ERROR_DB_CONNECT                    = 10405,     /* 0x28A5 */   /* 连接出错 */
    ISS_ERROR_DB_INVALID_SQL                = 10406,     /* 0x28A6 */   /* 无效的SQL */
    ISS_ERROR_DB_INVALID_APPID              = 10407,     /* 0x28A7 */   /* 无效的应用ID */

    /* Error codes of Resource 10500(0x2904)*/
    ISS_ERROR_RES_GENERAL                   = 10500,     /* 0x2904 */   /* 资源一般错误 */
    ISS_ERROR_RES_LOAD                      = 10501,     /* 0x2905 */   /* 没有加载 */
    ISS_ERROR_RES_FREE                      = 10502,     /* 0x2906 */   /* 空闲 */
    ISS_ERROR_RES_MISSING                   = 10503,     /* 0x2907 */   /* 缺失 */
    ISS_ERROR_RES_INVALID_NAME              = 10504,     /* 0x2908 */   /* 无效的名称 */
    ISS_ERROR_RES_INVALID_ID                = 10505,     /* 0x2909 */   /* 无效的ID */
    ISS_ERROR_RES_INVALID_IMG               = 10506,     /* 0x290A */   /* 无效的映像 */
    ISS_ERROR_RES_WRITE                     = 10507,     /* 0x290B */   /* 写操作错误 */
    ISS_ERROR_RES_LEAK                      = 10508,     /* 0x290C */   /* 泄露 */
    ISS_ERROR_RES_HEAD                      = 10509,     /* 0x290D */   /* 资源头部错误 */
    ISS_ERROR_RES_DATA                      = 10510,     /* 0x290E */   /* 数据出错 */
    ISS_ERROR_RES_SKIP                      = 10511,     /* 0x290F */   /* 跳过 */

    /* Error codes of TTS 10600(0x2968)*/
    ISS_ERROR_TTS_GENERAL                   = 10600,     /* 0x2968 */   /* 合成一般错误  */
    ISS_ERROR_TTS_TEXTEND                   = 10601,     /* 0x2969 */   /* 文本结束 */
    ISS_ERROR_TTS_TEXT_EMPTY                = 10602,     /* 0x296A */   /* 文本为空 */

    /* Error codes of Recognizer 10700(0x29CC) */
    ISS_ERROR_REC_GENERAL                   = 10700,     /* 0x29CC */   /* 一般错误 */
    ISS_ERROR_REC_INACTIVE                  = 10701,     /* 0x29CD */   /* 处于不活跃状态 */
    ISS_ERROR_REC_GRAMMAR_ERROR             = 10702,     /* 0x29CE */   /* 语法错误 */
    ISS_ERROR_REC_NO_ACTIVE_GRAMMARS        = 10703,     /* 0x29CF */   /* 没有活跃的语法 */
    ISS_ERROR_REC_DUPLICATE_GRAMMAR         = 10704,     /* 0x29D0 */   /* 语法重复 */
    ISS_ERROR_REC_INVALID_MEDIA_TYPE        = 10705,     /* 0x29D1 */   /* 无效的媒体类型 */
    ISS_ERROR_REC_INVALID_LANGUAGE          = 10706,     /* 0x29D2 */   /* 无效的语言 */
    ISS_ERROR_REC_URI_NOT_FOUND             = 10707,     /* 0x29D3 */   /* 没有对应的URI */
    ISS_ERROR_REC_URI_TIMEOUT               = 10708,     /* 0x29D4 */   /* 获取URI内容超时 */
    ISS_ERROR_REC_URI_FETCH_ERROR           = 10709,     /* 0x29D5 */   /* 获取URI内容时出错 */

    /* Error codes of Speech Detector 10800(0x2A30) */
    ISS_ERROR_EP_GENERAL                    = 10800,     /* 0x2A30 */   /* （EP）一般错误 */
    ISS_ERROR_EP_NO_SESSION_NAME            = 10801,     /* 0x2A31 */   /* （EP）链接没有名字 */
    ISS_ERROR_EP_INACTIVE                   = 10802,     /* 0x2A32 */   /* （EP）不活跃 */
    ISS_ERROR_EP_INITIALIZED                = 10803,     /* 0x2A33 */   /* （EP）初始化出错 */

    /* Error codes of TUV */  
    ISS_ERROR_TUV_GENERAL                   = 10900,     /* 0x2A94 */
    ISS_ERROR_TUV_GETHIDPARAM               = 10901,     /* 0x2A95 */   /* Get Busin Param huanid*/
    ISS_ERROR_TUV_TOKEN                     = 10902,     /* 0x2A96 */   /* Get Token */
    ISS_ERROR_TUV_CFGFILE                   = 10903,     /* 0x2A97 */   /* Open cfg file */ 
    ISS_ERROR_TUV_RECV_CONTENT              = 10904,     /* 0x2A98 */   /* received content is error */
    ISS_ERROR_TUV_VERFAIL                   = 10905,     /* 0x2A99 */   /* Verify failure */

    /* Error codes of IMTV */
    ISS_ERROR_LOGIN_SUCCESS                 = 11000,     /* 0x2AF8 */   /* 登录成功 */
    ISS_ERROR_LOGIN_NO_LICENSE              = 11001,     /* 0x2AF9 */   /* 无授权 */
    ISS_ERROR_LOGIN_SESSIONID_INVALID       = 11002,     /* 0x2AFA */   /* 无效的SessionID */ 
    ISS_ERROR_LOGIN_SESSIONID_ERROR         = 11003,     /* 0x2AFB */   /* 错误的SessionID */
    ISS_ERROR_LOGIN_UNLOGIN                 = 11004,     /* 0x2AFC */   /* 未登录 */
    ISS_ERROR_LOGIN_INVALID_USER            = 11005,     /* 0x2AFD */   /* 无效的用户 */
    ISS_ERROR_LOGIN_INVALID_PWD             = 11006,     /* 0x2AFE */   /* 无效的密码 */
    ISS_ERROR_LOGIN_SYSTEM_ERROR            = 11099,     /* 0x2B5B */   /* 系统错误 */

    /* Error codes of HCR */
    ISS_ERROR_HCR_GENERAL                   = 11100,
    ISS_ERROR_HCR_RESOURCE_NOT_EXIST        = 11101,
    ISS_ERROR_HCR_CREATE                    = 11102,
    ISS_ERROR_HCR_DESTROY                   = 11103,
    ISS_ERROR_HCR_START                     = 11104,
    ISS_ERROR_HCR_APPEND_STROKES            = 11105,
    ISS_ERROR_HCR_GET_RESULT                = 11106,
    ISS_ERROR_HCR_SET_PREDICT_DATA          = 11107,
    ISS_ERROR_HCR_GET_PREDICT_RESULT        = 11108,


    /* Error codes of http 12000(0x2EE0) */
    ISS_ERROR_HTTP_BASE                     = 12000,    /* 0x2EE0 */  /* HTTP错误基码 */

    /*Error codes of ISV */
    ISS_ERROR_ISV_NO_USER                   = 13000,    /* 32C8 */    /* 用户不存在 */

    /* Error codes of Lua scripts */
    ISS_ERROR_LUA_BASE                      = 14000,    /* 0x36B0 */
    ISS_ERROR_LUA_YIELD                     = 14001,    /* 0x36B1 */
    ISS_ERROR_LUA_ERRRUN                    = 14002,    /* 0x36B2 */
    ISS_ERROR_LUA_ERRSYNTAX                 = 14003,    /* 0x36B3 */
    ISS_ERROR_LUA_ERRMEM                    = 14004,    /* 0x36B4 */
    ISS_ERROR_LUA_ERRERR                    = 14005,    /* 0x36B5 */
};

#ifdef __cplusplus
}
#endif

#endif /* __ISS_ERRORS_H__ */
