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
  /*
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
      {BANG, "!"},
      {MINUS, "-"},
      {SLASH, "/"},
      {ASTERISK, "*"},
      {INT, "5"},
      {SEMICOLON, ";"},
      {INT, "5"},
      {LT, "<"},
      {INT, "10"},
      {GT, ">"},
      {INT, "5"},
      {SEMICOLON, ";"},
      // If statments
      {IF, "if"},
      {LPAREN, "("},
      {INT, "5"},
      {LT, "<"},
      {INT, "10"},
      {RPAREN, ")"},
      {LBRACE, "{"},
      {RETURN, "return"},
      {TRUE, "true"},
      {SEMICOLON, ";"},
      {RBRACE, "}"},
      {ELSE, "else"},
      {LBRACE, "{"},
      {RETURN, "return"},
      {FALSE, "false"},
      {SEMICOLON, ";"},
      {RBRACE, "}"},
      //Equality
      {INT, "10"},
      {EQ, "=="},
      {INT, "10"},
      {SEMICOLON, ";"},
      {INT, "10"},
      {NOT_EQ, "!="},
      {INT, "9"},
      {SEMICOLON, ";"},
      {EOF, ""}
  };

  //const char* input = "=+(){},;";
  /*
  const char* input =
    "let five = 5;\n"
    "let ten = 10;\n"
    "let add = fn(x, y) {\n"
    " x + y;\n"
    "};\n"
    "\n"
    "let result = add(five, ten);";
  */
  const char *input =
    "let five = 5;\n"
    "let ten = 10;\n"
    "let add = fn(x, y) {\n"
    " x + y;\n"
    "};\n"
    "\n"
    "let result = add(five, ten);\n"
    "!-/*5;\n"
    "5 < 10 > 5;\n"
    "if (5 < 10) {\n"
    "return true;\n"
    "} else {\n"
    "return false;\n"
    "}\n"
    "10 == 10;\n"
    "10 != 9;"
    ;
  struct lexer l = get_lexer(input);
  for(int i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++)
    {
      struct token *t = next_tok(&l);
      //printf("tok: %s test: %s\n", t->literal, tests[i].ch);
      assert(!strcmp(t->literal, tests[i].ch));
      assert(t->type == tests[i].type);
      free_token(t);
    }


  return 0;
}
