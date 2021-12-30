VERSION = 0.1
PREFIX ?= /usr/local
MANPREFIX = ${PREFIX}/share/man
CONFPREFIX = .config
TERMINFO := ${DESTDIR}${PREFIX}/share/terminfo
INCS = -I ../include
LIBS = -lcurl
CFLAGS += -std=c99 ${INCS} -DVERSION=\"${VERSION}\" -DNDBUG -O3
LDFLAGS += ${LIBS}
DEBUG_CFLAGS = ${CFLAGS} -UNDEBUG -O0 -g -ggdb -Wall -Wextra -Wno-unused-parameter
CC ?= cc
STRIP ?= strip
