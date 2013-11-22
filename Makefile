GCC=gcc
S_HEADERS=sock/ssock.h sock/data.h 
C_HEADERS=sock/csock.h sock/data.h sock/ssock.h
S_OBJS=sock/ssock.o sock/data.o fops/fileop.o 
C_OBJS=sock/csock.o sock/data.o fops/fileop.o 
SEN_OBJ=sender.o
REC_OBJ=receiver.o 
EXEC_PATH=exec
PLIB=-lpthread

FLAGS=-g -Werror -DAPP -DDROP -DDELAY #-DGRAN1

all: sender receiver
	mv sock/*.o sock/objs/
	@echo "-----------------compilation success :)---------------"

receiver: ${S_OBJS} ${S_HEADERS}
	${GCC} -o $(EXEC_PATH)/receiver receiver.c ${S_OBJS} ${FLAGS}

sender: ${C_OBJS} ${C_HEADERS}
	${GCC} -o $(EXEC_PATH)/sender sender.c ${C_OBJS} $(PLIB) ${FLAGS}

%.o: %.c ${C_HEADERS} ${S_HEADERS}
	${GCC} -c $< -o $@ ${FLAGS}

clean: 
	rm -f *.o
	rm -r exec/*
	cd sock; make clean	
        
