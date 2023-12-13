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
#include <hy_utils/hy_fifo.h>

#include "protocol.h"

#include "hy_protocol.h"

hy_s32_t HyProtocolVersionGet(HyProtocol *handle)
{
    hy_s32_t ret = -1;
    protocol_head_s head;
    HyProtocolSaveConfig_s  *save_c = &handle->save_c;

    HY_ASSERT_RET_VAL(!handle, -1);

    protocol_head_init(&head, HY_PROTOCOL_CMD_VERSION, 0);

    if (save_c->data_write_cb) {
        head.check_sum = protocol_generate_sum(&head, sizeof(head));

        ret = save_c->data_write_cb(&head, sizeof(head), save_c->args);
    }

    return ret;
}

hy_s32_t HyProtocolDataInsert(HyProtocol *handle, const void *buf, hy_u32_t len)
{
    hy_u32_t ret;

    HY_ASSERT(handle);
    HY_ASSERT(buf);

    ret = HyFifoWrite(handle->fifo_h, buf, len);

    return (ret == len) ? 0 : -1;
}

void HyProtocolDataDispatch(HyProtocol *handle)
{
    hy_s32_t ret;
    hy_s16_t check_sum;
    protocol_head_s *head;
    HyProtocolSaveConfig_s *save_c = &handle->save_c;
    char buf[HY_PROTOCOL_STRUCT_LEN_MAX];

    HY_ASSERT(handle);

    if (HyFifoGetUsedLen(handle->fifo_h) < (hy_s32_t)sizeof(protocol_head_s)) {
        return;
    }

    HyFifoReadPeek(handle->fifo_h, buf, sizeof(*head));

    head = (protocol_head_s *)buf;

    if (head->magic != PROTOCOL_MAGIC) {
        HyFifoRead(handle->fifo_h, buf, 1);
        return;
    }

    ret = HyFifoRead(handle->fifo_h, buf, sizeof(*head));
    if (ret != sizeof(*head)) {
        LOGE("impossible error, please check!!! \n");
    }

    // note: 应该可以去掉
    head = (protocol_head_s *)buf;

    if (head->len > 0) {
        ret = HyFifoRead(handle->fifo_h, &buf[sizeof(*head)], head->len);
        if (ret != head->len) {
            LOGE("impossible error, please check!!! \n");
        }

        check_sum = protocol_generate_sum(head, ret + sizeof(*head));
        if (check_sum != head->check_sum) {
            LOGE("impossible error, please check!!! \n");
        } else {
            switch (head->cmd) {
                case HY_PROTOCOL_CMD_VERSION:
                    if (save_c->version_cb) {
                        save_c->version_cb((HyProtocolVersion_s *)head->data, save_c->args);
                    }
                    break;
                default:
                    LOGE("error type, cmd: %d \n", head->cmd);
                    break;
            }
        }
    }
}

void HyProtocolDestroy(HyProtocol **handle_pp)
{
    HyProtocol *handle = *handle_pp;

    HY_ASSERT_RET(!handle_pp || !handle);

    HyFifoDestroy(&handle->fifo_h);

    LOGI("hy protocol create, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyProtocol *HyProtocolCreate(HyProtocolConfig_s *protocol_c)
{
    HyProtocol *handle = NULL;

    HY_ASSERT_RET_VAL(!protocol_c, NULL);

    do {
        handle = HY_MEM_CALLOC_BREAK(HyProtocol *, sizeof(HyProtocol));

        HY_MEMCPY(&handle->save_c, &protocol_c->save_c, sizeof(handle->save_c));

        handle->fifo_h = HyFifoCreate_m(protocol_c->fifo_capacity);
        if (!handle->fifo_h) {
            LOGE("protocol create fifo failed \n");
            break;
        }

        LOGI("hy protocol create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("hy protocol create failed \n");
    return handle;
}
