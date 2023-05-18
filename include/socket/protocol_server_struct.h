/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    protocol_server_struct.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 20:15
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 20:15
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_SERVER_STRUCT_H_
#define __LIBHY_PROTOCOL_INCLUDE_PROTOCOL_SERVER_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_protocol.h"

void *protocol_server_create(HyProtocolConfig_s *server_c);
void protocol_server_destroy(void **handle_pp);

#ifdef __cplusplus
}
#endif

#endif

