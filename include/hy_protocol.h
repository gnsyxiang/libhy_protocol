/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 17:42
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 17:42
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_H_
#define __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_utils/hy_type.h>

typedef enum {
    HY_PROTOCOL_CMD_HEARTBEAT,
    HY_PROTOCOL_CMD_VERSION,
} HyProtocolCmd_e;

typedef struct {
    char version[32];
} HyProtocolVersion_s;

hy_s32_t HyProtocolVersion(void **buf, HyProtocolVersion_s *version);

#ifdef __cplusplus
}
#endif

#endif

