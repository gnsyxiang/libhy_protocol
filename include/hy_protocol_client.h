/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol_client.h
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
#ifndef __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_CLIENT_H_
#define __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_utils/hy_type.h>

typedef enum {
    HY_PROTOCOL_CLIENT_CMD_HEARTBEAT,
    HY_PROTOCOL_CLIENT_CMD_VERSION,
} HyProtocolClientCmd_e;

typedef struct {
    char version[32];
} HyProtocolClientVersion_s;

typedef void (*HyProtocolClientHandleCmdCb_t)(void *buf, hy_u32_t len, void *args);

typedef struct {
    hy_u32_t                        cmd;
    HyProtocolClientHandleCmdCb_t   handle_cmd_cb;
} HyProtocolClientHandleCmd_s;

typedef struct {
    const char                      *ip;
    hy_u16_t                        port;

    HyProtocolClientHandleCmd_s     *handle_cmd;
    void                            *args;
    hy_u32_t                        handle_cmd_cnt;
} HyProtocolClientSaveConfig_s;

typedef struct {
    HyProtocolClientSaveConfig_s    save_c;
} HyProtocolClientConfig_s;

typedef struct HyProtocolClient_s HyProtocolClient_s;

HyProtocolClient_s *HyProtocolClientCreate(HyProtocolClientConfig_s *client_c);
void HyProtocolClientDestroy(HyProtocolClient_s **handle_pp);

#ifdef __cplusplus
}
#endif

#endif
