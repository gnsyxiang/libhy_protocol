/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    protocol.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    08/05 2023 19:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        08/05 2023      create the file
 * 
 *     last modified: 08/05 2023 19:38
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_H_
#define __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_utils/hy_type.h>

#include "hy_protocol.h"

#define PROTOCOL_MAGIC (0x55aa)

typedef struct {
    hy_u16_t    magic;          ///< 魔素识别码
    hy_u16_t    check_sum;      ///< 除magic和check_sum以外的所有字段
    hy_u32_t    cmd;            ///< 标识后面data的命令
    hy_u32_t    len;            ///< 标识后面data的长度
    char        *data[0];       ///< data数据，没有长度，方便程序处理用
} protocol_msg_head_s;

hy_u16_t protocol_generate_sum(protocol_msg_head_s *frame, hy_u32_t len);
hy_u16_t protocol_check_sum(protocol_msg_head_s *frame, hy_u32_t len);

hy_s32_t protocol_version(char **buf, HyProtocolVersion_s *version);

#ifdef __cplusplus
}
#endif

#endif

