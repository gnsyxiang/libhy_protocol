/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    08/05 2023 11:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        08/05 2023      create the file
 * 
 *     last modified: 08/05 2023 11:12
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

typedef void (*HyProtocolHandleCmdCb_t)(void *buf, hy_u32_t len, void *args);

typedef struct {
    hy_u32_t                    cmd;
    HyProtocolHandleCmdCb_t     handle_cmd_cb;
} HyProtocolHandleCmd_s;

typedef struct {
    const char                  *ip;
    hy_u16_t                    port;

    HyProtocolHandleCmd_s       *handle_cmd;
    void                        *args;
    hy_u32_t                    handle_cmd_cnt;
} HyProtocolSaveConfig_s;

typedef struct {
    HyProtocolSaveConfig_s      save_c;
} HyProtocolConfig_s;

typedef struct HyProtocol_s HyProtocol_s;

HyProtocol_s *HyProtocolCreate(HyProtocolConfig_s *protocol_c);
void HyProtocolDestroy(HyProtocol_s **handle_pp);

#ifdef __cplusplus
}
#endif

#endif

