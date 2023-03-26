#include "../lexer.h"
#include "../parser.h"
#include <assert.h>

void
test_ident (void)
{
  const char *input = "foobar;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist);
  assert(s->type == EXPR_STMT);
  printf("%s\n", s->expr->ident);
  assert(s->expr->type == IDENT_EXPR);
  free_stmt(s);
  free_parser(&p);
}

void
test_int (void)
{
  const char *input = "5;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist);
  assert(s->type == EXPR_STMT);
  printf("%d\n", s->expr->integer);
  assert(s->expr->type == INT_EXPR);
  free_stmt(s);
  free_parser(&p);
}

void
test_prefix (void)
{
  const char *input = "!foobar;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist);
  assert(s->type == EXPR_STMT);
  printf("%s\n", s->expr->token.literal);
  //  printf("%d\n", s->expr->expr->integer);
  printf("%s\n", s->expr->expr->ident);
  assert(s->expr->type == PREFIX_EXPR);
  free_stmt(s);
  free_parser(&p);
}

int
main (void)
{
  test_ident();
  test_int();
  test_prefix();
  /*
  const char *input =
    "let  5;\n"
    "let  = 10;\n"
    "let foobar = 838383;\n"
    ;
  input =
     "return 5;\n"
     "return 10;\n"
     "return 993322;\n";
     
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  
  const char *tests[3] =
    {
      [0] = "x",
      [1] = "y",
      [2] = "foobar",
    };
    
  for (int i = 0; i < 3; i++)
    {
      struct stmt *s = get_stmt(&p);
      if (s) {
        assert(s->token.type == RETURN); free_stmt(s);  } else {
	cycle_token(&p); getfree_errors(&p.elist); assert(0);}
    }
  getfree_errors(&p.elist);
  free_parser(&p);
  */
  return 0;
}
