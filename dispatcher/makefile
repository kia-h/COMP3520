# Host Dispatcher

# author: Internet's own boy

# Operating Systems Internals - Assignment 2
# Description: a makefile to compile source and header files into hostd
all: hostd sigtrap

.PHONY: all
.DEFAULT: all

hostd: src/hostd.c src/processblock.c header/processblock.h header/readinput.h src/readinput.c header/dispatcher.h src/dispatcher.c header/memoryblock.h src/memoryblock.c header/resource.h src/resource.c
		gcc -Wall -g src/hostd.c src/processblock.c src/readinput.c src/dispatcher.c src/resource.c src/memoryblock.c -o hostd

sigtrap:  src/sigtrap.c
		gcc -Wall src/sigtrap.c -o sigtrap



		
		