/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol_server.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 17:04
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 17:04
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_SERVER_H_
#define __LIBHY_PROTOCOL_INCLUDE_HY_PROTOCOL_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_utils/hy_type.h>

typedef void (*HyProtocolServerHandleCmdCb_t)(void *buf, hy_u32_t len, void *args);

typedef struct {
    hy_u32_t                        cmd;
    HyProtocolServerHandleCmdCb_t   handle_cmd_cb;
} HyProtocolServerHandleCmd_s;

typedef struct {
    const char                      *ip;
    hy_u16_t                        port;

    HyProtocolServerHandleCmd_s     *handle_cmd;
    void                            *handle_cmd_args;
    hy_u32_t                        handle_cmd_cnt;
} HyProtocolServerSaveConfig_s;

typedef struct {
    HyProtocolServerSaveConfig_s    save_c;
} HyProtocolServerConfig_s;

typedef struct HyProtocolServer_s HyProtocolServer_s;

HyProtocolServer_s *HyProtocolServerCreate(HyProtocolServerConfig_s *server_c);
void HyProtocolServerDestroy(HyProtocolServer_s **handle_pp);

#ifdef __cplusplus
}
#endif

#endif
