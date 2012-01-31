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

#AC_CHECK_HEADERS([unistd.h])
#AC_CHECK_FUNCS([fork])

AC_HEADER_STDBOOL
AC_C_INLINE
AC_SYS_LARGEFILE
AC_TYPE_PID_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

#AC_CHECK_LIB([pthread], [pthread_create], , \
#    AC_MSG_ERROR([libpthread is needed])
#)
#AC_CHECK_LIB([magic], [magic_load], , )

#AC_CONFIG_HEADERS([config.h options.h])

AC_CONFIG_FILES([Makefile])
#    cgi_wrapper/Makefile
#    options.h

AC_OUTPUT
