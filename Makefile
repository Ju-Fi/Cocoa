##
# Cocoa
##
NAME = cocoa
CC = cc
CFLAGS = -g -Wall -Wextra -fsanitize=undefined
LFLAGS = -lm

SRC = $(wildcard src/*.c src/*/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

all: clean build

build: $(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(CFLAGS) $(LFLAGS)

obj/%.o: src/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%/%.o: src/%/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	@rm -f $(OBJ) $(NAME)

# end
