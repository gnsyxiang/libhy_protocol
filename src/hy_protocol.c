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
#include "hy_protocol.h"

#define _PROTOCOL_INI_HEAD(_head, _cmd, _len) \
do { \
    _head->magic = PROTOCOL_MAGIC; \
    _head->cmd = _cmd; \
    _head->len = _len; \
} while(0);

#define _PROTOCOL_INI_HEAD_CHECKSUM(_head, _check_sum) \
do { \
    _head->check_sum = _check_sum; \
} while(0);

hy_s32_t HyProtocolVersion(void **buf, HyProtocolVersion_s *version)
{
    HY_ASSERT_RET_VAL(!buf || !version, -1);

    protocol_msg_head_s *head = NULL;
    hy_u32_t version_len = sizeof(HyProtocolVersion_s);
    hy_s32_t frame_len = sizeof(protocol_msg_head_s) + version_len;
    hy_u16_t check_sum;

    head = HY_MEM_CALLOC_RETURN_VAL(protocol_msg_head_s *, frame_len, -1);

    _PROTOCOL_INI_HEAD(head, HY_PROTOCOL_CMD_VERSION, version_len);

    HY_MEMCPY(head->data, version, sizeof(*version));

    check_sum = protocol_generate_sum(head, frame_len);

    _PROTOCOL_INI_HEAD_CHECKSUM(head, check_sum);

    *buf = head;
    return frame_len;
}

