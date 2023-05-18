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

#include "protocol.h"

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
        return 0;
    }
}

