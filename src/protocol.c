/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    protocol.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/05 2023 15:19
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/05 2023      create the file
 * 
 *     last modified: 18/05 2023 15:19
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

hy_u16_t protocol_generate_sum(protocol_msg_head_s *frame, hy_u32_t len)
{
    char *data = (char *)frame;
    data += sizeof(frame->magic);
    data += sizeof(frame->check_sum);
    len  -= sizeof(frame->magic);
    len  -= sizeof(frame->check_sum);
    hy_u64_t check_sum = 0;
    hy_u32_t i = 0;
    while (i < len) {
        if (i + 2 > len) {
            check_sum += data[i];
        } else {
            check_sum += data[i + 1] << 8 | data[i];
        }
        i += 2;
    }
    check_sum = (check_sum >> 16) + (check_sum & 0xffff);
    check_sum += (check_sum >>16);
    check_sum = (hy_u16_t)~check_sum;

    return ((check_sum >> 8) | (check_sum & 0xff) << 8);
}

hy_u16_t protocol_check_sum(protocol_msg_head_s *frame, hy_u32_t len)
{
    if (frame->check_sum == protocol_generate_sum(frame, len)) {
        return 1;
    } else {
        LOGW("protocol check sum failed \n");
        return 0;
    }
}

hy_s32_t protocol_version(char **buf, HyProtocolVersion_s *version)
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

    *buf = (char *)head;
    return frame_len;
}
