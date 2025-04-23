TARGET ?= aarch64-linux-musl

.PHONY: all
all: piss

.PHONY: piss
piss:
	zig build -Dtarget=$(TARGET)

.PHONY: compile_commands.json
compile_commands.json:
	zig build cdb -Dtarget=$(TARGET)
