#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"

int
main (void)
{
  const char* input = "=+(){},;";
  struct lexer l = get_lexer(input);
  struct token *t = new_token(EQ, " ");
  while(t->type != EOF){
    t = next_tok(&l);
    printf("%c\n", *t->literal);
  }
  free(t->literal);
  free(t);

  return 0;
}
