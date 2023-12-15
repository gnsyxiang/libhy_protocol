/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol_server_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/10 2023 10:52
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/10 2023      create the file
 * 
 *     last modified: 12/10 2023 10:52
 */
#include <stdio.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_signal.h>
#include <hy_utils/hy_module.h>
#include <hy_utils/hy_utils.h>
#include <hy_utils/hy_socket.h>
#include <hy_utils/hy_file.h>

#include "config.h"

#include "client_node.h"
#include "hy_protocol.h"

#define _APP_NAME           "hy_protocol_server_demo"
#define _APP_SOFT_VERSION   "hy_protocol_server_demo-0.1.0"
#define _APP_HARD_VERSION   "hy_protocol_server_demo-1.0.0"
#define _PROTOCOL_PORT      (34567)

typedef struct {
    hy_s32_t        is_exit;
    hy_s32_t        listen_fd;

    client_node_s   *client_node;
} _main_context_s;

static hy_s32_t _data_write_cb(const void *buf, hy_u32_t len, void *args)
{
    _main_context_s *context = args;

    return HyFileWrite(client_node_fd_get(context->client_node), buf, len);
}

static void _version_cb(HyProtocolVersion_s *version, void *args)
{
    version->project = 0xff;
    HY_STRNCPY(version->soft_version, sizeof(version->soft_version),
               _APP_SOFT_VERSION, HY_STRLEN(_APP_SOFT_VERSION));
    HY_STRNCPY(version->hard_version, sizeof(version->hard_version),
               _APP_HARD_VERSION, HY_STRLEN(_APP_HARD_VERSION));
    version->force_upgrade = HY_PROTOCOL_FORCE_UPGRADE_OFF;
}

static void _version_ack_cb(HyProtocolVersion_s *version, void *args)
{
    LOGI("project: %x \n", version->project);
    LOGI("soft_version: %s \n", version->soft_version);
    LOGI("hard_version: %s \n", version->hard_version);
    LOGI("force_upgrade: %d \n", version->force_upgrade);
}

static void _signal_error_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;

    LOGE("------error cb\n");
}

static void _signal_user_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;

    LOGW("------user cb\n");
}

static void _bool_module_destroy(_main_context_s **context_pp)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_context_s *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(HyLogConfig_s));
    log_c.port                      = 0;
    log_c.fifo_len                  = 10 * 1024;
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.save_c.level              = HY_LOG_LEVEL_INFO;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;

    hy_s8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    hy_s8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    HY_MEMSET(&signal_c, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name        = _APP_NAME;
    signal_c.save_c.coredump_path   = "./";
    signal_c.save_c.error_cb        = _signal_error_cb;
    signal_c.save_c.user_cb         = _signal_user_cb;
    signal_c.save_c.args            = context;

    HyModuleCreateBool_s bool_module[] = {
        {"log",         &log_c,         (HyModuleCreateBoolCb_t)HyLogInit,          HyLogDeInit},
        {"signal",      &signal_c,      (HyModuleCreateBoolCb_t)HySignalCreate,     HySignalDestroy},
    };

    HY_MODULE_RUN_CREATE_BOOL(bool_module);
}

static void _handle_module_destroy(_main_context_s **context_pp)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {NULL, NULL, NULL},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {NULL, NULL, NULL, NULL, NULL},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        struct {
            const char *name;
            hy_s32_t (*create)(_main_context_s *context);
        } create_arr[] = {
            {"_bool_module_create",     _bool_module_create},
            {"_handle_module_create",   _handle_module_create},
        };
        for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(create_arr); i++) {
            if (create_arr[i].create) {
                if (0 != create_arr[i].create(context)) {
                    LOGE("%s failed \n", create_arr[i].name);
                }
            }
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        context->listen_fd = HySocketCreate(HY_SOCKET_DOMAIN_TCP);
        if (-1 == context->listen_fd) {
            LOGE("socket create failed \n");
            break;
        }
        LOGI("listen_fd: %d \n", context->listen_fd);

        if (-1 == HySocketListen(context->listen_fd, NULL, _PROTOCOL_PORT)) {
            LOGE("socket listen failed \n");
            break;
        }

        struct sockaddr_in client_addr;
        hy_s32_t client_fd;

        while (!context->is_exit) {
            client_fd = HySocketAccept(context->listen_fd, &client_addr);
            if (-1 == client_fd) {
                LOGE("socket accept failed \n");
                break;
            }
            LOGI("client_fd: %d \n", client_fd);

            HyProtocolSaveConfig_s protocol_save_c;
            HY_MEMSET(&protocol_save_c, sizeof(protocol_save_c));
            protocol_save_c.data_write_cb   = _data_write_cb;
            protocol_save_c.version_cb      = _version_cb;
            protocol_save_c.version_ack_cb  = _version_ack_cb;
            protocol_save_c.args            = context;
            context->client_node = client_node_create(client_fd, &protocol_save_c);
            if (!context->client_node) {
                LOGE("create client node failed \n");
                break;
            }

            if (-1 == HyProtocolVersionGet(context->client_node->protocol_h)) {
                LOGE("HyProtocolVersionGet failed \n");
                break;
            }
        }
    } while (0);

    HySocketDestroy(&context->listen_fd);

    void (*destroy_arr[])(_main_context_s **context_pp) = {
        _handle_module_destroy,
        _bool_module_destroy
    };
    for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }

    HY_MEM_FREE_PP(&context);

    return 0;
}
