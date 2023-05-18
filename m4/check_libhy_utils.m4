# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    check_libhy_utils.m4
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    19/04 2023 09:53
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        19/04 2023      create the file
# 
#     last modified: 19/04 2023 09:53
# ===============================================================

AC_DEFUN([CHECK_LIBHY_UTILS],
    [

        AC_ARG_ENABLE([hy_utils],
            [AS_HELP_STRING([--disable-hy_utils],
                [disable support for hy_utils])],
                [],
                [enable_hy_utils=yes])

        case "$enable_hy_utils" in
            yes)
                have_hy_utils=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        HY_UTILS_LIBS=`$PKG_CONFIG --libs hy_utils 2>/dev/null`

                        case "$HY_UTILS_LIBS" in
                            '') ;;
                            *)
                                HY_UTILS_LIBS="$HY_UTILS_LIBS"
                                HY_UTILS_INCS=`$PKG_CONFIG --cflags hy_utils 2>/dev/null`
                                have_hy_utils=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_hy_utils" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        HY_UTILS_LIBS=""

                        # clear cache
                        unset ac_cv_search_HyUtilsIpStr2Int
                        AC_SEARCH_LIBS([HyUtilsIpStr2Int],
                                [hy_utils],
                                [have_hy_utils=yes HY_UTILS_LIBS="$LIBS"],
                                [have_hy_utils=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $HY_UTILS_INCS"
                AC_CHECK_HEADERS([hy_utils/hy_utils.h], [], [have_hy_utils=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(HY_UTILS_INCS)
                AC_SUBST(HY_UTILS_LIBS)

                case "$have_hy_utils" in
                    yes)
                        AC_CHECK_LIB([hy_utils], [HyUtilsIpStr2Int])
                        AC_DEFINE(HAVE_HY_UTILS, 1, [Define if the system has hy_utils])
                    ;;
                    *)
                        AC_MSG_ERROR([hy_utils is a must but can not be found. You should add the \
directory containing `hy_utils.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for hy_utils, or use `--disable-hy_utils' \
to disable support for hy_utils encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use hy_utils
        AM_CONDITIONAL(COMPILE_LIBHY_UTILS, [test "$enable_hy_utils" != "no" && test "$have_hy_utils" = "yes"])
    ])

