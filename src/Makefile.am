# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:35
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:35
# ===============================================================

SUBDIRS =

if COMPILE_SELECT_SOCKET_TYPE_SOCKET
SUBDIRS += socket
endif

if COMPILE_SELECT_SOCKET_TYPE_LIBEVENT
SUBDIRS += libevent
endif

lib_LTLIBRARIES = lib@project_name@.la

# ------------------
# defining variables
# ------------------
_no_error_com   =
_define_com     =
_include_com    = $(include_com)
_sources_com    =
_cppflags_com   =
_cflags_com     =
_cxxflags_com   =
_ldflags_com    =
_ldadd_com      =

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     +=

_include_com    += -I$(top_srcdir)/include

_sources_com    += \
                   protocol.c

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    += -version-info @current@:@revision@:@age@

if !COMPILE_DEBUG_INFO
_ldflags_com    += -export-symbols-regex 'Hy*'
endif

_ldadd_com      += $(ldadd_com)

if COMPILE_SELECT_SOCKET_TYPE_LIBEVENT
_ldadd_com      += $(top_builddir)/src/libevent/libhy_libevent.la
endif

if COMPILE_SELECT_SOCKET_TYPE_SOCKET
_ldadd_com      += $(top_builddir)/src/socket/libhy_socket.la
endif

lib@project_name@_la_SOURCES    = $(_sources_com)
lib@project_name@_la_CPPFLAGS   = $(_cppflags_com)
lib@project_name@_la_CFLAGS     = $(_cflags_com)
lib@project_name@_la_CXXFLAGS   = $(_cxxflags_com)
lib@project_name@_la_LDFLAGS    = $(_ldflags_com)
lib@project_name@_la_LIBADD     = $(_ldadd_com)
