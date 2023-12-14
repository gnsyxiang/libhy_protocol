/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    protocol.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/12 2023 16:48
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/12 2023      create the file
 * 
 *     last modified: 01/12 2023 16:48
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_H_
#define __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_log/hy_type.h>
#include <hy_log/hy_log.h>

#include <hy_utils/hy_compile.h>
#include <hy_utils/hy_fifo_lock.h>
#include <hy_utils/hy_thread.h>

#include "hy_protocol.h"

#define PROTOCOL_MAGIC (0x55aa)

typedef struct {
    hy_s16_t    magic;              ///< 魔素识别码
    hy_s16_t    check_sum;          ///< 除magic和check_sum以外的所有字段
    hy_u32_t    cmd;                ///< 标识后面data的命令
    hy_s32_t    len;                ///< 标识后面data的长度
    char        data[0];            ///< data数据，没有长度，方便程序处理用
} PACKED_4 protocol_head_s;

#define PROTOCOL_HEAD_INIT(_head, _cmd, _len)   \
do {                                            \
    (_head)->magic      = PROTOCOL_MAGIC;       \
    (_head)->cmd        = (_cmd);               \
    (_head)->len        = (_len);               \
} while (0)

typedef struct {
    protocol_head_s         head;
    HyProtocolVersion_s     version;
} protocol_version_s;

struct HyProtocol_s {
    HyProtocolSaveConfig_s  save_c;

    hy_s32_t                is_exit;
    HyThread_s              *thread_h;
    HyFifoLock_s            *fifo_lock_h;
};

hy_s16_t protocol_generate_sum(protocol_head_s *head, hy_u32_t len);

hy_s32_t protocol_check_sum(protocol_head_s *head, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

