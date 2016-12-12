.SUFFIXES: .c .o
CC = gcc
EXEC = sim00
CCFLAGS = -g
OBJS = sim00.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<
	
run: ${EXEC}
	./${EXEC}
    
clean:
	rm -f ${EXEC} ${OBJS}
	
sim00.o: sim00.c
