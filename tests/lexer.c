#include "../lexer.h"
#include <assert.h>


struct test {
  enum token_type type;
  char *ch;
};


int
main(void)
{
  
  struct test tests[] = {
    {ASSIGN, "="},
    {PLUS, "+"},
    {LPAREN, "("},
    {RPAREN, ")"},
    {LBRACE, "{"},
    {RBRACE, "}"},
    {COMMA, ","},
    {SEMICOLON, ";"},
    {EOF, ""},
  };
  


  const char* input = "=+(){},;";
  struct lexer l = get_lexer(input);
  for(int i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++)
    {
      struct token *t = next_tok(&l);
      assert(!strcmp(t->literal, tests[i].ch));
      assert(t->type == tests[i].type);
    }


  return 0;
}
