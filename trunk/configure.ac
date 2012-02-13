AC_INIT([canary], [0.1], [tiposchi@tiscali.it],[canary])
AC_DEFINE([PACKAGE_URL], ["http://galileo.dmi.unict.it/wiki/canary/"],[Define to the home page for this package.])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([pageinfo.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_TARGET

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AM_INIT_AUTOMAKE([foreign -Wall -Werror ])
AM_MAINTAINER_MODE

AC_PROG_CC

AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([syslog.h])
AC_CHECK_HEADERS([stddef.h])

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([pipe2])

AC_HEADER_STDBOOL
AC_C_INLINE
AC_SYS_LARGEFILE
AC_TYPE_PID_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

AC_CHECK_LIB([dl], [dlsym], , \
    AC_MSG_ERROR([dl (dynamic linking) is needed])
)

AC_CHECK_LIB([pthread], [pthread_once], , \
    AC_MSG_ERROR([pthread is needed])
)

#AC_CHECK_LIB([magic], [magic_load], , )

#AC_CONFIG_HEADERS([config.h options.h])

AC_CONFIG_FILES([Makefile])
#    cgi_wrapper/Makefile
#    options.h


#Stuff for the library
LT_INIT([dlopen shared pic-only])
AC_PROG_LIBTOOL
AC_ENABLE_SHARED



AC_OUTPUT
