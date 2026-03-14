# Compiler setup
CC:=gcc
CFLAGS:=-Wall -std=gnu23
TARGETS:=main

# Object files
OBJS:=${TARGETS:%=bin/%.o}

build: ${OBJS}
	${CC} ${CFLAGS} $(wildcard bin/*.o) -o bin/gen-macros

compile: ${OBJS}

${OBJS}: bin/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@

debug:
	echo ${OBJS}

cc:
	${CC} --version