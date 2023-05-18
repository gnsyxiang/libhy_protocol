/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    08/05 2023 16:27
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        08/05 2023      create the file
 * 
 *     last modified: 08/05 2023 16:27
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_assert.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_thread.h>
#include <hy_utils/hy_hex.h>
#include <hy_utils/hy_fifo.h>

#include "hy_protocol.h"
#include "protocol.h"

struct HyProtocolClient_s {
    HyProtocolClientSaveConfig_s    save_c;

    hy_s32_t            socket_fd;
    struct event_base   *base;
    struct bufferevent  *event;
    struct evbuffer     *evbuf;

    hy_s32_t            is_exit;
    HyThread_s          *libevent_thread_h;
    HyThread_s          *handle_data_thread_h;
};

static hy_s32_t _libevent_loop_cb(void *args)
{
    HyProtocolClient_s *handle = args;
    struct sockaddr_in addr;
    hy_s32_t ret;

    HY_MEMSET(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_aton(handle->save_c.ip, &addr.sin_addr);
    addr.sin_port = htons(handle->save_c.port);
    ret = bufferevent_socket_connect(handle->event,
                                     (struct sockaddr*)(&addr),
                                     sizeof(addr));
    if (0 != ret) {
        LOGW("bufferevent_socket_connect failed \n");
    }

    event_base_dispatch(handle->base);

    return -1;
}

static hy_s32_t _handle_data_loop_cb(void *args)
{
    HyProtocolClient_s *handle = args;

    while (!handle->is_exit) {
    }

    return -1;
}

static void _read_cb(struct bufferevent *bev, void *args)
{
    HyProtocolClient_s *handle = args;
    char buf[1024];
    hy_s32_t n;
    hy_u32_t buffer_len;
    protocol_msg_head_s msg_head;

    while (n = bufferevent_read(bev, buf, 1024), n > 0) {
        evbuffer_add(handle->evbuf, buf, n);
    }

    buffer_len = evbuffer_get_length(handle->evbuf);
    if (buffer_len < sizeof(msg_head)) {
        return;
    }

    evbuffer_copyout(handle->evbuf, &msg_head, sizeof(msg_head));
    HY_HEX_ASCII((char *)&msg_head, (hy_u32_t)sizeof(msg_head));

    if (msg_head.magic != PROTOCOL_MAGIC) {
        LOGW("drain one byte \n");
        evbuffer_drain(handle->evbuf, 1);
        return;
    }
}

static void _write_cb(struct bufferevent *bev, void *args)
{
   //  size_t buffer_len = evbuffer_get_length(buf);
   //  ev_uint32_t record_len;
   //  char *record;
   //
   //  if (buffer_len < 4)
   //     return 0; /* The size field hasn't arrived. */
   //
   // /* We use evbuffer_copyout here so that the size field will stay on
   //     the buffer for now. */
   //  evbuffer_copyout(buf, &record_len, 4);
   //  /* Convert len_buf into host order. */
   //  record_len = ntohl(record_len);
   //  if (buffer_len < record_len + 4)
   //      return 0; /* The record hasn't arrived */
   //
   //  /* Okay, _now_ we can remove the record. */
   //  record = malloc(record_len);
   //  if (record == NULL)
   //      return -1;
   //
   //  evbuffer_drain(buf, 4);
   //  evbuffer_remove(buf, record, record_len);
   //
   //  *record_out = record;
   //  *size_out = record_len;

}

static void _event_cb(struct bufferevent *bev, short events, void *args)
{
    HY_ASSERT_RET(!bev || !args);
    HyProtocolClient_s *handle = args;

    evutil_socket_t fd = bufferevent_getfd(bev);
    LOGI("socket fd: %d \n", fd);

    if (events & BEV_EVENT_CONNECTED) {
        LOGI("protocol client connect to the server \n");
        return;
    }

    if (events & BEV_EVENT_TIMEOUT) {
        LOGW("time out \n");
    } else if (events & BEV_EVENT_EOF) {
        LOGI("connection closed \n");
    } else if (events & BEV_EVENT_ERROR) {
        LOGE("error \n");
    }

    bufferevent_free(bev);
    handle->socket_fd = -1;
}

static void _libevent_client_destroy(HyProtocolClient_s **handle_pp)
{
    HyProtocolClient_s *handle = *handle_pp;

    // 1, 由于创建bufferevent时指定了BEV_OPT_CLOSE_ON_FREE参数，bufferevent释放时，会自动关闭底层传输端口
    // 2, 当初始化失败时，需要关闭底层传输端口
    // 3，bufferevent_free后，指定了socket_fd = -1，所以不会有问题
    if (handle->socket_fd) {
        close(handle->socket_fd);
    }

    if (handle->evbuf) {
        evbuffer_free(handle->evbuf);
    }

    if (handle->base) {
        event_base_free(handle->base);
    }
}

static hy_s32_t _libevent_client_create(HyProtocolClient_s *handle)
{
    do {
        handle->base = event_base_new();
        if (!handle->base) {
            LOGE("event_base_new failed \n");
            break;
        }

        handle->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == handle->socket_fd) {
            LOGE("socket failed \n");
            break;
        }
        evutil_make_socket_nonblocking(handle->socket_fd);

        handle->evbuf = evbuffer_new();
        if (!handle->evbuf) {
            LOGE("evbuffer_new faield \n");
            break;
        }

        handle->event = bufferevent_socket_new(handle->base,
                                               handle->socket_fd,
                                               BEV_OPT_CLOSE_ON_FREE);
        if (!handle->event) {
            LOGE("bufferevent_socket_new failed \n");
            break;
        }
        bufferevent_enable(handle->event, EV_READ | EV_WRITE);
        bufferevent_setcb(handle->event, _read_cb, _write_cb, _event_cb, handle);

        return 0;
    } while(0);

    _libevent_client_destroy(&handle);
    return -1;
}

void HyEventListenerExit(HyProtocolClient_s *handle)
{
    HY_ASSERT_RET(!handle);

    event_base_loopbreak(handle->base);
}

void HyProtocolClientDestroy(HyProtocolClient_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyProtocolClient_s *handle= *handle_pp;

    handle->is_exit = 1;

    HyThreadDestroy(&handle->libevent_thread_h);
    HyThreadDestroy(&handle->handle_data_thread_h);

    if (handle->save_c.handle_cmd) {
        HY_MEM_FREE_PP(&handle->save_c.handle_cmd);
    }

    LOGI("protocol client destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyProtocolClient_s *HyProtocolClientCreate(HyProtocolClientConfig_s *client_c)
{
    HY_ASSERT_RET_VAL(!client_c, NULL);
    HyProtocolClient_s *handle = NULL;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyProtocolClient_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &client_c->save_c, sizeof(handle->save_c));

        HyProtocolClientSaveConfig_s *save_c = &client_c->save_c;
        handle->save_c.handle_cmd = HY_MEM_CALLOC_BREAK(
                HyProtocolClientHandleCmd_s *, save_c->handle_cmd_cnt);

        HY_MEMCPY(&handle->save_c.handle_cmd, save_c->handle_cmd,
                  save_c->handle_cmd_cnt * sizeof(HyProtocolClientHandleCmd_s));

        if (0 != _libevent_client_create(handle)) {
            LOGE("_libevent_client_create failed \n");
            break;
        }

        handle->libevent_thread_h = HyThreadCreate_m(
            "libevent_handle_loop", _libevent_loop_cb, handle);
        if (!handle->libevent_thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        handle->handle_data_thread_h = HyThreadCreate_m(
            "handle_data_loop", _handle_data_loop_cb, handle);
        if (!handle->handle_data_thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        HyFifoConfig_s fifo_c;
        HY_MEMSET(&fifo_c, sizeof(fifo_c));
        fifo_c.save_c.len           = 25;

        LOGI("protocol client create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("protocol client create failed \n");
    HyProtocolClientDestroy(&handle);
    return NULL;
}

