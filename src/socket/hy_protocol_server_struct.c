/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol_server_struct.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 17:08
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 17:08
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_assert.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_thread.h>
#include <hy_utils/hy_socket.h>
#include <hy_utils/hy_fifo_lock.h>
#include <hy_utils/hy_file.h>

#include "protocol.h"
#include "hy_protocol_server.h"

struct HyProtocolServer_s {
    HyProtocolServerSaveConfig_s    save_c;

    hy_s32_t                        is_exit;
    HyThread_s                      *read_thread_h;
    HyThread_s                      *handle_thread_h;
    HyFifoLock_s                    *fifo_h;
};

static hy_s32_t _handle_loop_cb(void *args)
{
    HyProtocolServer_s *handle = args;
    HyProtocolServerSaveConfig_s *save_c = &handle->save_c;
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
                                                    head.len, save_c->args);
                break;
            }
        }
    }

    return -1;
}

static hy_s32_t _read_loop_cb(void *args)
{
    HyProtocolServer_s *handle = args;
    HyProtocolServerSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t socket_fd;
    hy_s32_t new_fd;
    hy_s32_t ret;
    char buf[1024];
    struct sockaddr_in client_addr;

    socket_fd = HySocketCreate(HY_SOCKET_DOMAIN_TCP);
    if (-1 == socket_fd) {
        LOGE("HySocketCreate failed \n");
        return -1;
    }

    if (-1 == HySocketListen(socket_fd, save_c->ip, save_c->port)) {
        HySocketDestroy(&socket_fd);

        LOGE("HySocketListen failed \n");
        return -1;
    }

    while (!handle->is_exit) {
        new_fd = HySocketAccept(socket_fd, &client_addr);

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

        HySocketDestroy(&new_fd);
    }

    if (socket_fd) {
        HySocketDestroy(&socket_fd);
    }

    return -1;
}

void HyProtocolServerDestroy(HyProtocolServer_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyProtocolServer_s *handle = *handle_pp;

    HyThreadDestroy(&handle->read_thread_h);

    LOGI("protocol server destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyProtocolServer_s *HyProtocolServerCreate(HyProtocolServerConfig_s *server_c)
{
    HY_ASSERT_RET_VAL(!server_c, NULL);
    HyProtocolServer_s *handle = NULL;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyProtocolServer_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &server_c->save_c, sizeof(handle->save_c));

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

        LOGI("protocol server create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("protocol server create failed \n");
    return NULL;
}
