CC=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
NAME=repl
FILES=main.c
CFLAGS=-Wall -Wextra -pedantic -Wno-incompatible-pointer-types

tests: lexer_test parser_test eval_test

all: repl tests

repl: $(FILES)
	$(CC) $(CFLAGS) -o bin/$(NAME) $(FILES)
lexer_test: tests/lexer.c 
	$(CC) $(CFLAGS) -o bin/tests/lexer_test -ggdb3 tests/lexer.c
parser_test: tests/parser.c
	$(CC) $(CFLAGS) -o bin/tests/parser_test -ggdb3 tests/parser.c
eval_test: tests/object.c
	$(CC) $(CFLAGS) -o bin/tests/eval_test -ggdb3 tests/object.c
