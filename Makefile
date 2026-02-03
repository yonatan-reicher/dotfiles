C_FLAGS = -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code

.PHONY : all
all : aliases/generated.nu installer
	./installer

installer : installer.c cwalk.c
	gcc $(C_FLAGS) -g -o $@ $^

aliases/generated.nu : aliases/generate.nu
	@# Don't use the config fie... because... we are generating a config file!
	cd aliases && nu --no-config-file ../$< > ../$@
