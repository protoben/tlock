#####################################################
# Makefile for tlock, the simple terminal locker.
#
# Requires sudo, as tlock must be suid root in order
# to auth.
#####################################################

CC=gcc
CCARGS=-lcurses -lcrypt
OBJS=tlock.o
CHOWN=sudo chown root:root
CHMOD=sudo chmod 4755
NAME=tlock

all: ${OBJS}
	${CC} -o ${NAME} ${CCARGS} ${OBJS}
	@echo -e "\n\n***\nTlock must be suid root."
	@sudo echo -e "***\n"
	${CHOWN} ./${NAME}
	${CHMOD} ./${NAME}

tlock:
	${CC} -c ${CCARGS} tlock.c

clean:
	@for i in ${OBJS} ${NAME}; do \
		if [ -e $$i ]; then       \
			echo "rm $$i";        \
			rm -f $$i;            \
		fi                        \
	done
