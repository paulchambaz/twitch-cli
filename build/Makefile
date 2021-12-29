include config.mk

SRC = ../source/main.c
OBJ = ${SRC:.c=.o}

all: clean options twitch-cli

options:
	@echo twitch-cli build options:
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

../include/config.h:
	cp ../include/config.def.h ../include/config.h

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<
	@mv *.o ../source/

${OBJ}: ../include/config.h config.mk

twitch-cli: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}
	@rm ../source/*.o

debug: clean
	@make CFLAGS='${DEBUG_CFLAGS}'

clean:
	@echo cleaning
	@rm -f twitch-cli ${OBJ} twitch-cli-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p twitch-cli-${VERSION}
	@cp -R ../LICENSE Makefile ../README.md ../include/config.def.h config.mk ${SRC} ../twitch-cli.1 twitch-cli-${VERSION}
	@tar -cf twitch-cli-${VERSION}.tar twitch-cli-${VERSION}
	@gzip twitch-cli-${VERSION}.tar
	@rm -rf twitch-cli-${VERSION}

install: twitch-cli
	@echo stripping executable
	@${STRIP} twitch-cli
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f twitch-cli ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < ../twitch-cli.1 > ${DESTDIR}${MANPREFIX}/man1/twitch-cli.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/twitch-cli.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/twitch-cli
	@echo removing manual page from ${DESTDIR}${MANPREFIX}
	@rm -f ${DESTDIR}${MANPREFIX}/man1/twitch-cli.1

.PHONY: all options clean dist install uninstall debug
