#include "../lexer.h"
#include <assert.h>


struct test {
  enum token_type type;
  char *ch;
};


int
main(void)
{
  /*
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
  */
  struct test tests[] =
    {
      {LET, "let"},
      {IDENT, "five"},
      {ASSIGN, "="},
      {INT, "5"},
      {SEMICOLON, ";"},
      {LET, "let"},
      {IDENT, "ten"},
      {ASSIGN, "="},
      {INT, "10"},
      {SEMICOLON, ";"},
      {LET, "let"},
      {IDENT, "add"},
      {ASSIGN, "="},
      {FUNCTION, "fn"},
      {LPAREN, "("},
      {IDENT, "x"},
      {COMMA, ","},
      {IDENT, "y"},
      {RPAREN, ")"},
      {LBRACE, "{"},
      {IDENT, "x"},
      {PLUS, "+"},
      {IDENT, "y"},
      {SEMICOLON, ";"},
      {RBRACE, "}"},
      {SEMICOLON, ";"},
      {LET, "let"},
      {IDENT, "result"},
      {ASSIGN, "="},
      {IDENT, "add"},
      {LPAREN, "("},
      {IDENT, "five"},
      {COMMA, ","},
      {IDENT, "ten"},
      {RPAREN, ")"},
      {SEMICOLON, ";"},
      {EOF, ""}
  };


  //const char* input = "=+(){},;";
  const char* input =
    "let five = 5;\n"
    "let ten = 10;\n"
    "let add = fn(x, y) {\n"
    " x + y;\n"
    "};\n"
    "\n"
    "let result = add(five, ten);";
  
  struct lexer l = get_lexer(input);
  for(int i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++)
    {
      struct token *t = next_tok(&l);
      printf("%s||%d\n", t->literal, t->type);
      assert(!strcmp(t->literal, tests[i].ch));
      assert(t->type == tests[i].type);
      free_token(t);
    }


  return 0;
}
