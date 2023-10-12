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

typedef enum {
    HY_PROTOCOL_DEVICE_TYPE_CLIENT,
    HY_PROTOCOL_DEVICE_TYPE_SERVER,
} HyProtocolDeviceType_e;

typedef struct {
    hy_s32_t demo;
} HyProtocolSaveConfig_s;

typedef struct {
    HyProtocolSaveConfig_s  save_c;

    HyProtocolDeviceType_e  device_type;
} HyProtocolConfig_s;

typedef struct HyProtocol HyProtocol;

HyProtocol *HyProtocolCreate(HyProtocolConfig_s *protocol_c);
void HyProtocolDestroy(HyProtocol **handle_pp);

#ifdef __cplusplus
}
#endif

#endif

