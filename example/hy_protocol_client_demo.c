/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protocol_client_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    08/05 2023 16:31
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        08/05 2023      create the file
 * 
 *     last modified: 08/05 2023 16:31
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include <hy_utils/hy_type.h>
#include <hy_utils/hy_mem.h>
#include <hy_utils/hy_string.h>
#include <hy_utils/hy_signal.h>
#include <hy_utils/hy_module.h>
#include <hy_utils/hy_utils.h>

#include "hy_protocol.h"

#define _APP_NAME           "hy_protocol_client_demo"
#define _SERVER_IP          "192.168.0.15"
#define _SERVER_PORT        (8899)

typedef struct {
    hy_s32_t            is_exit;
    void                *protocol_client_h;
} _main_context_s;

static void _handle_cmd_version(void *buf, hy_u32_t len, void *args)
{

}

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_s *context = args;
    context->is_exit = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_context_s *context = args;
    context->is_exit = 1;
}

static void _bool_module_destroy(void)
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
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
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

static void _handle_module_destroy(_main_context_s *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"protocol client", (void **)&context->protocol_client_h, (HyModuleDestroyHandleCb_t)HyProtocolDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyProtocolConfig_s client_c;
    HY_MEMSET(&client_c, sizeof(client_c));
    HyProtocolHandleCmd_s handle_cmd[] = {
        {HY_PROTOCOL_CMD_VERSION,       _handle_cmd_version},
    };
    client_c.save_c.ip = _SERVER_IP;
    client_c.save_c.port = _SERVER_PORT;
    client_c.save_c.handle_cmd = handle_cmd;
    client_c.save_c.handle_cmd_cnt = HY_UTILS_ARRAY_CNT(handle_cmd);
    client_c.save_c.handle_cmd_args = context;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"protocol client", (void **)&context->protocol_client_h, (void *)&client_c, (HyModuleCreateHandleCb_t)HyProtocolCreate, (HyModuleDestroyHandleCb_t)HyProtocolDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        while (!context->is_exit) {
            sleep(1);
        }
    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}
