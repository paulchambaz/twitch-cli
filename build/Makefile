include config.mk

SRC = ../source/*.c

twitch-cli: clean
	@echo "Building" $@
	@echo "Flags  :" ${CFLAGS}
	@echo "Libs   :" ${LDFLAGS}
	@echo "Source :" ${SRC}
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${SRC}

debug:  clean
	@echo "Building debug"
	@echo "Flags  :" ${DEBUG_CFLAGS}
	@echo "Libs   :" ${LDFLAGS}
	@echo "Source :" ${SRC}
	@${CC} -o debug ${DEBUG_CFLAGS} ${LDFLAGS} ${SRC}

install: clean
	@echo "Installing twitch-cli"
	@echo "Flags  :" ${INSTALL_CFLAGS}
	@echo "Libs   :" ${LDFLAGS}
	@echo "Source :" ${SRC}
	@${CC} -o twitch-cli ${INSTALL_CFLAGS} ${LDFLAGS} ${SRC}
	@${STRIP} twitch-cli
	@echo "Installing executable file to ${BIN_DIR}"
	@cp -f twitch-cli ${BIN_DIR}/twitch-cli
	@chmod 755 ${BIN_DIR}/twitch-cli
	@echo "Installing manual page to ${MAN_DIR}/man1"
	@mkdir -p ${MAN_DIR}/man1
	@sed "s/VERSION/${VERSION}/g" < ../twitch-cli.1 > ${MAN_DIR}/man1/twitch-cli.1
	@chmod 644 ${MAN_DIR}/man1/twitch-cli.1
	@rm -f twitch-cli

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/twitch-cli
	@echo removing manual page from ${DESTDIR}${MANPREFIX}
	@rm -f ${DESTDIR}${MANPREFIX}/man1/twitch-cli.1

clean:
	@echo cleaning
	@rm -f twitch-cli debug twitch-cli.so
