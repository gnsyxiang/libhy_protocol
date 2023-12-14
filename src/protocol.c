/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    protocol.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/12 2023 17:03
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/12 2023      create the file
 * 
 *     last modified: 01/12 2023 17:03
 */
#include <stdio.h>

#include "protocol.h"

hy_s16_t protocol_generate_sum(protocol_head_s *head, hy_u32_t len)
{
    hy_u64_t check_sum = 0;
    hy_u32_t i = 0;
    char *data = (char *)head;

    data += sizeof(head->magic);
    data += sizeof(head->check_sum);
    len  -= sizeof(head->magic);
    len  -= sizeof(head->check_sum);

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

hy_s32_t protocol_check_sum(protocol_head_s *head, hy_u32_t len)
{
    if (head->check_sum == protocol_generate_sum(head, len)) {
        return 1;
    } else {
        LOGE("protocol check sum failed \n");

        return 0;
    }
}
