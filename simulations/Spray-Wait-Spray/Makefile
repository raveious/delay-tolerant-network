.SUFFIXES: .c .o
CC = gcc
EXEC = sim01
CCFLAGS = -g
OBJS = sim01.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<
	
run: ${EXEC}
	./${EXEC}
    
clean:
	rm -f ${EXEC} ${OBJS}
	
sim00.o: sim01.c
