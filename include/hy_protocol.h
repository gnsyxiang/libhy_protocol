/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/10 2023 10:42
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/10 2023      create the file
 * 
 *     last modified: 12/10 2023 10:42
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_H_
#define __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_log/hy_type.h>

#include <hy_utils/hy_compile.h>

/**
 * @brief 命令字所对应的结构体的最大长度
 *
 * @note 里面使用的是栈空间，如果结构体太大则会引起栈空间溢出
 */
#define HY_PROTOCOL_STRUCT_LEN_MAX      (1024)

/**
 * @brief 协议命令字
 *
 * @note 如果需要传递参数，则相应命令字对应一个结构体(把相关数据封装在结构体内，方便后期维护)
 */
typedef enum {
    HY_PROTOCOL_CMD_VERSION,                        ///< 获取版本信息
    HY_PROTOCOL_CMD_VERSION_ACK,                    ///< 版本信息回复
} HyProtocolCmd_e;

/**
 * @brief 强制升级标志
 */
typedef enum {
    HY_PROTOCOL_FORCE_UPGRADE_OFF,                  ///< 强制升级关闭
    HY_PROTOCOL_FORCE_UPGRADE_ON,                   ///< 强制升级开启
} HyProtocolForceUpgrade_e;

/**
 * @brief 版本信息
 */
typedef struct {
    hy_s32_t    project;                            ///< 项目编号
    char        soft_version[64];                   ///< 软件版本号，如：1.0.0
    char        hard_version[64];                   ///< 硬件版本号，如：1.0.0
    hy_s8_t     force_upgrade;                      ///< 是否强制升级
    char        reserve[16];                        ///< 预留
} PACKED_4 HyProtocolVersion_s;

/**
 * @brief 数据发送回调
 *
 * @param buf 数据地址
 * @param len 数据长度
 * @param args 用户数据
 *
 * @return 成功返回写入的长度，失败返回-1
 */
typedef hy_s32_t (*HyProtocolDataWriteCb_t)(const void *buf, hy_u32_t len, void *args);

/**
 * @brief 版本信息回调
 *
 * @param version 版本信息
 * @param args 用户数据
 */
typedef void (*HyProtocolVersionCb_t)(HyProtocolVersion_s *version, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    HyProtocolDataWriteCb_t     data_write_cb;      ///< 数据发送回调
    HyProtocolVersionCb_t       version_cb;         ///< 版本信息回调
    void                        *args;              ///< 用户数据
} HyProtocolSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyProtocolSaveConfig_s      save_c;             ///< 配置参数

    hy_u32_t                    fifo_capacity;      ///< fifo大小
} HyProtocolConfig_s;

typedef struct HyProtocol HyProtocol;

/**
 * @brief 创建协议模块
 *
 * @param protocol_c 配置参数，详见HyProtocolConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyProtocol *HyProtocolCreate(HyProtocolConfig_s *protocol_c);

/**
 * @brief 销毁协议模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyProtocolDestroy(HyProtocol **handle_pp);

/**
 * @brief 协议总体流程处理
 *
 * @param handle 句柄
 */
void HyProtocolDataDispatch(HyProtocol *handle);

/**
 * @brief 向协议模块添加接收到的数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyProtocolDataInsert(HyProtocol *handle, const void *buf, hy_u32_t len);

/**
 * @brief 获取版本
 *
 * @param handle 句柄
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyProtocolVersionGet(HyProtocol *handle);

#ifdef __cplusplus
}
#endif

#endif

