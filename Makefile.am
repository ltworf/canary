AM_CFLAGS = -Wextra -Wall
ACLOCAL_AMFLAGS = -I m4
#SUBDIRS = cgi_wrapper


#bin_PROGRAMS = canary
#canary_SOURCES = test.c ca_alloc.c ca_canary.c ca_monitor.c ca_pageinfo.c ca_tree.c error.c queue.c

#ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libcanary-1.0.la
libcanary_1_0_la_SOURCES = ca_alloc.c ca_canary.c ca_monitor.c ca_pageinfo.c ca_tree.c error.c queue.c


EXTRA_DIST = \
     ca_pageinfo.h \
     bit_op.h \
     ca_canary.h \
     error.h \
     types.h \
     ca_monitor.h \
     ca_tree.h \
     queue.h
#     examples \
#     daemon \
#     weborf.conf \
#     qweborf/main.ui \
#     qweborf.setup.py
 
dist-hook:
	rm -rf `find $(distdir)/ -name .svn`

# dist_man_MANS = weborf.1 weborf.conf.5
# 
# init_SCRIPTS = daemon/weborf
# 
# sysconf_DATA = weborf.conf
# 
# docdir = ${datadir}/doc/${PACKAGE}

dist_doc_DATA = \
    CHANGELOG \
    COPYING \
    README
#     Credits
#     TODOlist 
