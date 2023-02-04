CC=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
NAME=interp
FILES=main.c lexer.h

PKGS=sdl2 SDL2_image

CFLAGS=-Wall -Wextra -pedantic


all: $(FILES) 
	$(CC) $(CFLAGS) -o $(NAME) $(FILES) 
lexer_test: tests/lexer.c 
	$(CC) $(CFLAGS) -o lexer_test -g tests/lexer.c
