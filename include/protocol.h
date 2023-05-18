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

#define PROTOCOL_MAGIC (0x55aa)

typedef struct {
    hy_s16_t magic;
    hy_u16_t server;
    hy_u32_t version;
    hy_u32_t len;
} protocol_msg_head_s;

#ifdef __cplusplus
}
#endif

#endif

