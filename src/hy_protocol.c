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

#include "hy_protocol.h"

struct HyProtocol {
    HyProtocolSaveConfig_s  save_c;
};

void HyProtocolDestroy(HyProtocol **handle_pp)
{
    HyProtocol *handle = *handle_pp;

    HY_ASSERT_RET(!handle_pp || !handle);

    LOGI("hy protocol create, handle: %p \n", handle);
}

HyProtocol *HyProtocolCreate(HyProtocolConfig_s *protocol_c)
{
    HyProtocol *handle = NULL;

    HY_ASSERT_RET_VAL(!protocol_c, NULL);

    do {
        handle = HY_MEM_CALLOC_BREAK(HyProtocol *, sizeof(HyProtocol));

        HY_MEMCPY(&handle->save_c, &protocol_c->save_c, sizeof(handle->save_c));

        LOGI("hy protocol create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("hy protocol create failed \n");
    return handle;
}
