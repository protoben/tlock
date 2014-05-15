#####################################################
# Makefile for tlock, the simple terminal locker
# using pam or shadow.
#
# If using shadow, requires sudo, as tlock must be
# suid root in order to auth.
#####################################################

include ./Makefile.conf

CC=gcc
CFLAGS=-lcurses

OBJS=tlock.o
NAME=tlock

ifneq ($(strip ${COLORVALS}),)
CFLAGS+= '-DCOLORVALS=${COLORVALS}'
endif

ifeq (${USE_PAM},true)
CFLAGS+= -lpam -DUSE_PAM
else
CFLAGS+= -lcrypt
endif

all: ${OBJS}
	@${CC} -o ${NAME} ${CFLAGS} ${OBJS}
	@echo "cc tlock"
ifeq (${USE_PAM},false)
	@echo -e "\n\n***\nTlock must be suid root if not using pam."
	@sudo echo -e "***\n"
	sudo chown root:root ./${NAME}
	sudo chmod 4755 ./${NAME}
endif

%.o: %.c
	@${CC} -c ${CFLAGS} $<
	@echo "cc $@"

install: all
	@echo "install /usr/bin/tlock"
	@cp tlock /usr/bin
ifeq (${USE_PAM},true)
	@echo "install /etc/pam.d/tlock"
	@echo -e "auth\trequired\tpam_unix.so nodelay" > /etc/pam.d/tlock
endif

uninstall:
	rm -f /usr/bin/tlock /etc/pam.d/tlock

clean:
	rm -f ${OBJS} ${NAME}
