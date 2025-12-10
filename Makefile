C_FLAGS = -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code

.PHONY : default
default : installer
	./installer

installer : installer.c cwalk.c
	gcc $(C_FLAGS) -g -o $@ $^
