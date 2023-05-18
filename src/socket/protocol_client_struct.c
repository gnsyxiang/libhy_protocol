/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    protocol_client_struct.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 10:44
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 10:44
 */
#include <stdio.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_assert.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_socket.h>
#include <hy_utils/hy_file.h>
#include <hy_utils/hy_type.h>
#include <hy_utils/hy_thread.h>
#include <hy_utils/hy_fifo_lock.h>

#include "protocol.h"
#include "protocol_client_struct.h"

typedef struct HyProtocolClient_s {
    HyProtocolSaveConfig_s      save_c;

    hy_s32_t                        is_exit;
    HyThread_s                      *read_thread_h;
    HyThread_s                      *handle_thread_h;
    HyFifoLock_s                    *fifo_h;
} HyProtocolClient_s;

static hy_s32_t _handle_loop_cb(void *args)
{
    HyProtocolClient_s *handle = args;
    HyProtocolSaveConfig_s *save_c = &handle->save_c;
    protocol_msg_head_s head;
    hy_s32_t ret;
    char *buf = HY_MEM_CALLOC_RETURN_VAL(char *, 1024 * 32, -1);

    while (!handle->is_exit) {
        ret = HyFifoLockReadPeek(handle->fifo_h, &head, sizeof(head));
        if (ret != sizeof(head)) {
            LOGI("not enough data, ret: %d \n", ret);
            continue;
        }

        if (head.magic != PROTOCOL_MAGIC) {
            LOGI("the protocol magic error, magic:%x \n", head.magic);

            HyFifoLockReadDel(handle->fifo_h, 1);
            continue;
        }

        HyFifoLockRead(handle->fifo_h, buf, sizeof(head) + head.len);

        if (0 == protocol_check_sum((protocol_msg_head_s *)buf,
                                    sizeof(head) + head.len)) {
            LOGI("protocol check sum failed \n");
            continue;
        }

        for (size_t i = 0; i < save_c->handle_cmd_cnt; i++) {
            if (head.cmd == save_c->handle_cmd[i].cmd
                && save_c->handle_cmd[i].handle_cmd_cb) {
                save_c->handle_cmd[i].handle_cmd_cb(buf + sizeof(head),
                                                    head.len, save_c->handle_cmd_args);
                break;
            }
        }
    }

    return -1;
}

static hy_s32_t _read_loop_cb(void *args)
{
    HyProtocolClient_s *handle = args;
    HyProtocolSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t socket_fd;
    hy_s32_t ret;
    char buf[1024];

    while (!handle->is_exit) {
        socket_fd = HySocketCreate(HY_SOCKET_DOMAIN_TCP);
        if (-1 == socket_fd) {
            LOGE("HySocketCreate faield \n");
            continue;
        }

        if (-1 == HySocketConnect(socket_fd, save_c->ip, save_c->port)) {
            LOGE("HySocketConnect failed \n");
            HySocketDestroy(&socket_fd);
            continue;
        }
        LOGI("socket connect ok \n");

        while (!handle->is_exit) {
            ret = HyFileReadTimeout(socket_fd, buf, 1024, 1000);
            if (ret == -1) {
                LOGE("HyFileRead failed \n");
                break;
            } else if (ret == 0) {
            } else {
                HyFifoLockWrite(handle->fifo_h, buf, ret);
            }
        }

        HySocketDestroy(&socket_fd);

        if (handle->is_exit) {
            break;
        }
    }

    if (socket_fd) {
        HySocketDestroy(&socket_fd);
    }

    return -1;
}

void protocol_client_destroy(void **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyProtocolClient_s *handle = *handle_pp;

    handle->is_exit = 1;
    HyFifoLockDestroy(&handle->fifo_h);

    HyThreadDestroy(&handle->handle_thread_h);
    HyThreadDestroy(&handle->read_thread_h);

    LOGI("protocol client create, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

void *protocol_client_create(HyProtocolConfig_s *client_c)
{
    HY_ASSERT_RET_VAL(!client_c, NULL);
    HyProtocolClient_s *handle = NULL;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyProtocolClient_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &client_c->save_c, sizeof(handle->save_c));

        handle->fifo_h = HyFifoLockCreate_m(1024 * 64);
        if (!handle->fifo_h) {
            LOGE("HyFifoCreate_m failed \n");
            break;
        }

        handle->read_thread_h = HyThreadCreate_m("PC_read_loop",
                                                 _read_loop_cb, handle);
        if (!handle->read_thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        handle->handle_thread_h = HyThreadCreate_m("PC_handle_loop",
                                                   _handle_loop_cb, handle);
        if (!handle->handle_thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("protocol client create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("protocol client create failed \n");
    protocol_client_destroy((void **)&handle);
    return NULL;
}
