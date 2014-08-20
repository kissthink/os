COMPILER_TYPE = gnu

CC = gcc

PROG = os
SRCS = main.c OS.c
SRCS += VBE.c Graphics.c Bitmap.c Timer.c Keyboard.c KBC.c Mouse.c
SRCS += MouseOptionsMenu.c Point.c Task.c Utilities.c Window.c WindowContent.c

CCFLAGS= -Wall -O3

DPADD += ${LIBDRIVER} ${LIBSYS}
LDADD += -llm -ldriver -lsys

LDFLAGS += -L.

MAN=
BINDIR? = /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
