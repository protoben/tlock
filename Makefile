#####################################################
# Makefile for tlock, the simple terminal locker.
#
# Requires sudo, as tlock must be suid root in order
# to auth.
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
	${CC} -o ${NAME} ${CFLAGS} ${OBJS}
ifeq (${USE_PAM},false)
	@echo -e "\n\n***\nTlock must be suid root if not using pam."
	@sudo echo -e "***\n"
	sudo chown root:root ./${NAME}
	sudo chmod 4755 ./${NAME}
endif

tlock:
	${CC} -c ${CFLAGS} tlock.c

clean:
	@for i in ${OBJS} ${NAME}; do \
		if [ -e $$i ]; then       \
			echo "rm $$i";        \
			rm -f $$i;            \
		fi                        \
	done
