CC=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
NAME=interp
FILES=main.c lexer.h

PKGS=sdl2 SDL2_image

CFLAGS=-Wall -Wextra -pedantic

tests: lexer_test parser_test

all: $(FILES) 
	$(CC) $(CFLAGS) -o $(NAME) $(FILES) 
lexer_test: tests/lexer.c 
	$(CC) $(CFLAGS) -o bin/tests/lexer_test -ggdb3 tests/lexer.c
parser_test: tests/parser.c
	$(CC) $(CFLAGS) -o bin/tests/parser_test -ggdb3 tests/parser.c
