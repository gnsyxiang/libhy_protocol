# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    select_socket_type.m4
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    18/05 2023 10:15
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        18/05 2023      create the file
# 
#     last modified: 18/05 2023 10:15
# ===============================================================

AC_DEFUN([SELECT_SOCKET_TYPE],
    [
        socket_type=""

        AC_ARG_WITH([socket_type],
            [AS_HELP_STRING([--with-socket_type=@<:@socket|libevent@:>@], [select socket_type about @<:@socket|log4cplus@:>@ @<:@default=socket@:>@])],
            [],
            [with_socket_type=socket])

        case "$with_socket_type" in
            socket)
                AC_DEFINE(HAVE_SELECT_SOCKET_TYPE_SOCKET,  1, [select socket socket_type])
                socket_type="socket"
            ;;
            libevent)
                AC_DEFINE(HAVE_SELECT_SOCKET_TYPE_LIBEVENT,  1, [select libevent socket_type])
                socket_type="libevent"
            ;;
            *)
                AC_MSG_ERROR([bad value ${with_socket_type} for --with-socket_type=@<:@socket|libevent@:>@])
            ;;
        esac

        AC_SUBST(socket_type)

        AM_CONDITIONAL([COMPILE_SELECT_SOCKET_TYPE_SOCKET],         [test "x$with_socket_type" = "xsocket"])
        AM_CONDITIONAL([COMPILE_SELECT_SOCKET_TYPE_LIBEVENT],       [test "x$with_socket_type" = "xlibevent"])
    ])

