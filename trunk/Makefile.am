AM_CFLAGS = -Wextra -Wall -I /usr/src/linux-source-3.2/include/

#SUBDIRS = cgi_wrapper


bin_PROGRAMS = canary
canary_SOURCES = \
    test.c \
    pageinfo.c \
    bit_op.c

EXTRA_DIST = \
     pageinfo.h \
     bit_op.h
#     buffered_reader.h \
#     cgi.h \
#     embedded_auth.h \
#     listener.h \
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
    COPYING
#     Credits \
#     README \
#     TODOlist 
