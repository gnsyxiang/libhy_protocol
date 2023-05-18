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

typedef enum {
    HY_PROTOCOL_TYPE_CLIENT,
    HY_PROTOCOL_TYPE_SERVER,
} HyProtocolType_e;

typedef void (*HyProtocolHandleCmdCb_t)(void *buf, hy_u32_t len, void *args);

typedef struct {
    hy_u32_t                    cmd;
    HyProtocolHandleCmdCb_t     handle_cmd_cb;
} HyProtocolHandleCmd_s;

typedef struct {
    HyProtocolType_e            type;

    const char                  *ip;
    hy_u16_t                    port;

    HyProtocolHandleCmd_s       *handle_cmd;
    void                        *handle_cmd_args;
    hy_u32_t                    handle_cmd_cnt;
} HyProtocolSaveConfig_s;

typedef struct {
    HyProtocolSaveConfig_s      save_c;
} HyProtocolConfig_s;

void *HyProtocolCreate(HyProtocolConfig_s *protocol_c);
void HyProtocolDestroy(void **handle_pp);

hy_s32_t HyProtocolVersion(void *handle, HyProtocolVersion_s *version);

#ifdef __cplusplus
}
#endif

#endif

