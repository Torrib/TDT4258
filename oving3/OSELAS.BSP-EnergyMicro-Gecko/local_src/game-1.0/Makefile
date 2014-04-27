#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CFLAGS += -O2 -g -std=c99 -Wall -D_POSIX_C_SOURCE=199309L #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
CPPFLAGS +=
LDFLAGS +=


all: game

game: game.c game.h cross.h circle.h
	${CC} ${CFLAGS} game.c game.h cross.h circle.h -o game

clean:
	-rm -f game

install:


.PHONY: all install clean

# end of development makefile
