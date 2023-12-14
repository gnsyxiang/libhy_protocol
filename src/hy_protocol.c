/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/10 2023 10:48
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/10 2023      create the file
 * 
 *     last modified: 12/10 2023 10:48
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_assert.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_utils.h>

#include "protocol.h"

#include "hy_protocol.h"

static void _handle_cmd_version_cb(HyProtocol_s *handle)
{
    hy_s32_t ret = -1;
    protocol_head_s head;
    protocol_version_s protocol_version;
    HyProtocolSaveConfig_s *save_c = &handle->save_c;

    HyFifoLockRead(handle->fifo_lock_h, &head, sizeof(head));

    HY_MEMSET(&protocol_version, sizeof(protocol_version));

    PROTOCOL_HEAD_INIT(&protocol_version.head, HY_PROTOCOL_CMD_VERSION_ACK, sizeof(HyProtocolVersion_s));

    if (save_c->version_cb) {
        save_c->version_cb(&protocol_version.version, save_c->args);
    }

    protocol_version.head.check_sum = protocol_generate_sum(&protocol_version.head, sizeof(protocol_version));
    if (save_c->data_write_cb) {
        ret = save_c->data_write_cb(&protocol_version, sizeof(protocol_version), save_c->args);
        if (-1 == ret) {
            LOGE("protocol data write fadild \n");
        }
    }
}

static void _handle_cmd_version_ack_cb(HyProtocol_s *handle)
{
    protocol_version_s protocol_version;
    HyProtocolSaveConfig_s *save_c = &handle->save_c;

    HY_MEMSET(&protocol_version, sizeof(protocol_version));

    HyFifoLockRead(handle->fifo_lock_h, &protocol_version, sizeof(protocol_version));
    if (save_c->version_ack_cb) {
        save_c->version_ack_cb(&protocol_version.version, save_c->args);
    }
}

static hy_s32_t _thread_loop_cb(void *args)
{
    HyProtocol_s *handle = args;
    protocol_head_s head;
    hy_u32_t i;

    while (!handle->is_exit) {
        HyFifoLockReadPeek(handle->fifo_lock_h, &head, sizeof(head));

        if (head.magic != PROTOCOL_MAGIC) {
            HyFifoLockRead(handle->fifo_lock_h, &head, 1);
            continue;
        }

        struct {
            hy_u32_t cmd;
            void (*handle_cmd_cb)(HyProtocol_s *handle);
        } _handle_cmd_arr[] = {
            {HY_PROTOCOL_CMD_VERSION,       _handle_cmd_version_cb},
            {HY_PROTOCOL_CMD_VERSION_ACK,   _handle_cmd_version_ack_cb},
        };
        for (i = 0; i < HY_UTILS_ARRAY_CNT(_handle_cmd_arr); i++) {
            if (head.cmd == _handle_cmd_arr[i].cmd) {
                if (_handle_cmd_arr[i].handle_cmd_cb) {
                    _handle_cmd_arr[i].handle_cmd_cb(handle);
                    break;
                }
            }
        }
        if (i >= HY_UTILS_ARRAY_CNT(_handle_cmd_arr)) {
            LOGE("error cmd: %d<%d> \n", head.cmd, head.cmd);
        }
    }

    return -1;
}

hy_s32_t HyProtocolVersionGet(HyProtocol_s *handle)
{
    hy_s32_t ret = -1;
    protocol_head_s head;
    HyProtocolSaveConfig_s  *save_c = &handle->save_c;

    HY_ASSERT_RET_VAL(!handle, -1);

    PROTOCOL_HEAD_INIT(&head, HY_PROTOCOL_CMD_VERSION, 0);

    if (save_c->data_write_cb) {
        head.check_sum = protocol_generate_sum(&head, sizeof(head));

        ret = save_c->data_write_cb(&head, sizeof(head), save_c->args);
    }

    return ret;
}

hy_s32_t HyProtocolDataInsert(HyProtocol_s *handle, const void *buf, hy_u32_t len)
{
    hy_u32_t ret;

    HY_ASSERT(handle);
    HY_ASSERT(buf);

    ret = HyFifoLockWrite(handle->fifo_lock_h, buf, len);

    return (ret == len) ? 0 : -1;
}

void HyProtocolDestroy(HyProtocol_s **handle_pp)
{
    HyProtocol_s *handle = *handle_pp;

    HY_ASSERT_RET(!handle_pp || !handle);

    handle->is_exit = 1;

    HyThreadDestroy(&handle->thread_h);

    HyFifoLockDestroy(&handle->fifo_lock_h);

    LOGI("hy protocol create, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyProtocol_s *HyProtocolCreate(HyProtocolConfig_s *protocol_c)
{
    HyProtocol_s *handle = NULL;

    HY_ASSERT_RET_VAL(!protocol_c, NULL);

    do {
        handle = HY_MEM_CALLOC_BREAK(HyProtocol_s *, sizeof(HyProtocol_s));

        HY_MEMCPY(&handle->save_c, &protocol_c->save_c, sizeof(handle->save_c));

        handle->fifo_lock_h = HyFifoLockCreate_m(protocol_c->fifo_capacity);
        if (!handle->fifo_lock_h) {
            LOGE("protocol create fifo lock failed \n");
            break;
        }

        handle->thread_h = HyThreadCreate_m("HyProtocolLoop",
                                            _thread_loop_cb, handle);
        if (!handle->thread_h) {
            LOGE("create protocol thread failed \n");
            break;
        }

        LOGI("hy protocol create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("hy protocol create failed \n");
    return handle;
}
