GCC=gcc
HEADERS=csock.h ssock.h data.h
OBJS=csock.o ssock.o data.o
FLAGS=-DSOCK

sock: ${OBJS}
	mv *.o objs/
%.o : %.c ${HEADERS}
	${GCC} -c $<

clean:
	rm -f *.o
	rm -f objs/*o
