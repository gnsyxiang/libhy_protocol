/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    client_node.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    13/12 2023 17:09
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        13/12 2023      create the file
 * 
 *     last modified: 13/12 2023 17:09
 */
#ifndef __LIBHY_PROTOCOL_INCLUDE_CLIENT_NODE_H_
#define __LIBHY_PROTOCOL_INCLUDE_CLIENT_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_log/hy_type.h>

#include <hy_utils/hy_thread.h>

#include "protocol.h"

typedef struct {
    hy_s32_t        fd;                 ///< 客户端fd
    hy_s32_t        is_exit;            ///< 控制线程是否退出
    HyThread_s      *thread_read_h;     ///< 读取数据的线程
    HyProtocol_s    *protocol_h;        ///< 协议句柄
    void            *args;              ///< 用户参数
} client_node_s;

client_node_s *client_node_create(hy_u32_t fd, HyProtocolSaveConfig_s *protocol_save_c);
void client_node_destroy(client_node_s **handle_pp);

hy_s32_t client_node_fd_get(client_node_s *handle);

#ifdef __cplusplus
}
#endif

#endif

