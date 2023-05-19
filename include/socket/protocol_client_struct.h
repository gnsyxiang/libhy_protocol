/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    protocol_client_struct.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 19:55
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 19:55
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_CLIENT_STRUCT_H_
#define __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_CLIENT_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_protocol.h"

void *protocol_client_create(HyProtocolConfig_s *protocol_c);
void protocol_client_destroy(void **handle_pp);

hy_s32_t protocol_client_protocol_version(void *handle, HyProtocolVersion_s *version);

#ifdef __cplusplus
}
#endif

#endif

