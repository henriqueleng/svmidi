# See LICENSE file for copyright and license details.
# VERSION = 0

# SOURCE
SOURCE = main.c sndio.c alsa.c
OBJ = main.o
HDR = midi.h config.h

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

# libs
XLIBS = `pkg-config --libs x11` -lXext
XFLAGS = `pkg-config --cflags x11`

SOUNDLIBS = -lsndio
SOUNDFLAGS = -DSNDIO
SOUNDOBJ = sndio.o

#uncomment for ALSA
#SOUNDLIBS = -lasound
#SOUNDFLAGS = -DALSA
#SOUNDOBJ = alsa.o

OBJ += $(SOUNDOBJ)

LIBS = ${XLIBS} ${SOUNDLIBS}
CFLAGS = -g -std=c99 -pedantic -Wall -O0 ${XFLAGS} ${SOUNDFLAGS}

BIN = svmidi

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
