# See LICENSE file for copyright and license details.
# VERSION = 0

# SOURCE
SOURCE = main.c sndio.c
OBJ = main.o sndio.o
HDR = midi.h config.h arg.h

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

# libs
XLIBS = `pkg-config --libs x11` -lXext
XFLAGS = `pkg-config --cflags x11`

SOUNDLIBS = -lsndio
SOUNDFLAGS = -DSNDIO
# TODO: implement #
#uncomment for ALSA
#SOUNDLIBS = -lalsa
#SOUNDFLAGS = -DALSA

LIBS = ${XLIBS} ${SOUNDLIBS}
CFLAGS = -g -std=c99 -pedantic -Wall -O0 ${XFLAGS} ${SOUNDFLAGS}

BIN = virtmidi

all: ${SOURCE} ${BIN}

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: midi.h sndio.h

${BIN}: ${OBJ} ${HDR}
	${CC} ${CFLAGAS} -o ${BIN} ${OBJ} ${LIBS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${BIN} ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	cp -f ${BIN}.1 ${DESTDIR}${MANPREFIX}/man1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${BIN}
	rm -f ${DESTDIR}${MANPREFIX}/man1/${BIN}.1

clean:
	rm -f ${BIN} ${OBJ}
