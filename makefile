BIN     = ./bin
OUT     = $(BIN)/app
INSTALL = /usr/bin/fembed

SRC  = $(wildcard src/*.c)
DEPS = $(wildcard src/*.h)
OBJ  = $(addsuffix .o,$(subst src/,$(BIN)/,$(basename $(SRC))))

CC     = gcc
CSTD   = c11
CFLAGS = -Og -std=$(CSTD) -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -I./lib

shared: $(BIN) $(OBJ) $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

static: $(BIN) $(OBJ) $(SRC)
	$(CC) -static $(CFLAGS) -o $(OUT) $(OBJ)

bin/%.o: src/%.c $(DEPS)
	$(CC) -c $< $(CFLAGS) -o $@

$(BIN):
	mkdir -p $(BIN)

install:
	cp $(OUT) $(INSTALL)

clean:
	rm -r $(BIN)/*

all:
	@echo shared, static, install, clean
