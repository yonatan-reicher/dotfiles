C_FLAGS = -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code

.PHONY : all install
all : aliases/generated.nu nvim install

install : installer
	./installer

nvim :
	git submodule update --init --recursive -- nvim

# jrc/% :
# 	git submodule update --init --recursive -- jrc

installer : installer.c jrc/include/*.h jrc/lib/debug/libjrc.a
	gcc $(C_FLAGS) -g -o $@ $(filter %.c,$^) -L./jrc/lib/debug -ljrc

aliases/generated.nu : aliases/generate.nu aliases/all.sh
	@# Don't use the config fie... because... we are generating a config file!
	@if type nu >/dev/null 2>/dev/null; then \
		echo "cd aliases && nu --no-config-file ../$< > ../$@"; \
		cd aliases && nu --no-config-file ../$< > ../$@; \
	else \
		echo "[Warning] Skipping nu configuration (not installed)"; \
	fi
