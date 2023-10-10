/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 18:00
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 18:00
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_assert.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>

#include "protocol.h"
#include "protocol_client_struct.h"
#include "protocol_server_struct.h"
#include "hy_protocol.h"

hy_s32_t HyProtocolVersion(void *handle, HyProtocolVersion_s *version)
{
    return -1;
}

void HyProtocolDestroy(void **handle_pp)
{
}

void *HyProtocolCreate(HyProtocolConfig_s *protocol_c)
{
    return NULL;
}

