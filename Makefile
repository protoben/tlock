#####################################################
# Makefile for tlock, the simple terminal locker.
#
# Requires sudo, as tlock must be suid root in order
# to auth.
#####################################################

CC=gcc
CFLAGS=-lcurses -lcrypt '-DCOLORVALS=${COLORVALS}'

COLORVALS={{250, 500, 750}, {500, 750, 250}, {750, 250, 500}, NULL}
OBJS=tlock.o
CHOWN=sudo chown root:root
CHMOD=sudo chmod 4755
NAME=tlock

all: ${OBJS}
	${CC} -o ${NAME} ${CFLAGS} ${OBJS}
	@echo -e "\n\n***\nTlock must be suid root."
	@sudo echo -e "***\n"
	${CHOWN} ./${NAME}
	${CHMOD} ./${NAME}

tlock:
	${CC} -c ${CFLAGS} tlock.c

clean:
	@for i in ${OBJS} ${NAME}; do \
		if [ -e $$i ]; then       \
			echo "rm $$i";        \
			rm -f $$i;            \
		fi                        \
	done
