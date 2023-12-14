/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    client_node.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    13/12 2023 17:10
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        13/12 2023      create the file
 * 
 *     last modified: 13/12 2023 17:10
 */
#include <stdio.h>

#include <hy_utils/hy_assert.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_file.h>
#include <hy_utils/hy_hex.h>

#include "client_node.h"
#include "hy_protocol.h"

static hy_s32_t _thread_read_loop_cb(void *args)
{
    client_node_s *handle = args;
    char buf[HY_PROTOCOL_STRUCT_LEN_MAX];
    hy_s32_t ret;

    while (!handle->is_exit) {
        HY_MEMSET(buf, sizeof(buf));
        ret = HyFileRead(handle->fd, buf, sizeof(buf));
        if (-1 == ret) {
            LOGE("socket read failed \n");
            break;
        }

        HY_HEX_ASCII(buf, ret);

        if (-1 == HyProtocolDataInsert(handle->protocol_h, buf, ret)) {
            LOGE("insert data to protocol failed \n");
            break;
        }
    }

    return -1;
}

hy_s32_t client_node_fd_get(client_node_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->fd;
}

void client_node_destroy(client_node_s **handle_pp)
{
    client_node_s *handle = *handle_pp;

    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    handle->is_exit = 1;
    HyThreadDestroy(&handle->thread_read_h);

    HyProtocolDestroy(&handle->protocol_h);

    LOGI("destroy client node, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

client_node_s *client_node_create(hy_u32_t fd, HyProtocolSaveConfig_s *protocol_save_c)
{
    client_node_s *handle = NULL;
    HyProtocolConfig_s protocol_c;

    HY_ASSERT_RET_VAL(!protocol_save_c, NULL);

    do {
        handle = HY_MEM_CALLOC_BREAK(client_node_s *, sizeof(*handle));

        handle->fd = fd;
        handle->args = protocol_save_c->args;

        HY_MEMSET(&protocol_c, sizeof(protocol_c));
        HY_MEMCPY(&protocol_c.save_c, protocol_save_c, sizeof(*protocol_save_c));
        protocol_c.fifo_capacity = HY_PROTOCOL_STRUCT_LEN_MAX * 5;
        handle->protocol_h = HyProtocolCreate(&protocol_c);
        if (!handle->protocol_h) {
            LOGE("create protocol failed \n");
            break;
        }

        handle->thread_read_h = HyThreadCreate_m("HyProtocolRead",
                                                 _thread_read_loop_cb, handle);
        if (!handle->thread_read_h) {
            LOGE("create protocol read thread failed \n");
            break;
        }

        LOGI("create client node, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("create client node failed \n");
    client_node_destroy(&handle);
    return NULL;
}
