#include "../lexer.h"
#include "../parser.h"
#include <assert.h>
#include "test_util.h"
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
  printf("%s\n", s->expr->expr[RIGHT]->ident);
  assert(s->expr->type == PREFIX_EXPR);
  free_stmt(s);
  free_parser(&p);
}

void
test_minix (void)
{
  const char *input = "-1;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist);
  printf("work\n");
  assert(s->type == EXPR_STMT);
  printf("%s\n", s->expr->token.literal);
  //  printf("%d\n", s->expr->expr->integer);
  printf("%d\n", s->expr->expr[RIGHT]->integer);
  assert(s->expr->type == PREFIX_EXPR);
  free_stmt(s);
  free_parser(&p);
}
/*
void
expr_string (struct expr *e)
{
  switch(e->type)
    {
    case INT_EXPR:
      printf("%s",e->token.literal);
      break;
    case IDENT_EXPR:
      printf("%s",e->ident);
      break;
    case PREFIX_EXPR:
      printf("( %s", e->token.literal);
      expr_string(e->expr[RIGHT]);
      printf(" )");
      break;
    case INFIX_EXPR:
      printf("( ");
      expr_string(e->expr[LEFT]);
      printf(" %s ", e->token.literal);
      expr_string(e->expr[RIGHT]);
      printf(")");
    }
}

void
printf_infix (struct expr *e)
{
  if (e->type == INFIX_EXPR && e->expr[LEFT]->type == INFIX_EXPR)
    {
      printf("(");
      printf_infix(e->expr[LEFT]);
      printf(")");
      return;
    }
  

  // else if (e->type == INFIX_EXPR) {
    int i = LEFT;
    while (i <= RIGHT)
      {
	if (e->expr[i]->type == INT_EXPR)
	  printf("%d", e->expr[i]->integer);
	if (e->expr[i]->type == IDENT_EXPR)
	  printf("%s", e->expr[i]->ident);
	if (i == LEFT)
	  printf(" %s ", e->token.literal);
	
	i++;
      }
    //    printf("\n");
    //  }
}
*/
void
test_infix (void)
{
  const char *input = "5 < 4 != 3 > 4;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist);
  //assert(s->type == EXPR_STMT);
  //assert(s->expr->type == INFIX_EXPR);
  //  assert(s->expr->expr[LEFT]->integer == 5);
  //  assert(s->expr->expr[RIGHT]->integer == 5);
  //  printf("%d\n", s->expr->expr[RIGHT]->integer);
  //printf("%s\n", s->expr->expr[LEFT]->expr[RIGHT]->ident);
  //printf("%s\n", s->expr->expr[RIGHT]->ident);
  //expr_string(s->expr);
  printf("\n");
  //printf_infix(s->expr);
  //printf("\n");
  /*
  printf("%s\n", s->expr->token.literal);
  printf("%d\n", s->expr->expr[LEFT]->expr[LEFT]->integer);  
  printf("%s\n", s->expr->expr[LEFT]->expr[RIGHT]->ident);
  */

  free_stmt(s);
  free_parser(&p);
}
#define PREVERI(input, expect) assert(test_prec(input, expect))
void
operator_test (void)
{
  PREVERI("-a * b", "((-a) * b)");
  PREVERI("!-a", "(!(-a))");
  PREVERI("a + b + c", "((a + b) + c)");
  PREVERI("a + b - c", "((a + b) - c)");
  PREVERI("a * b * c", "((a * b) * c)");
  PREVERI("a * b / c", "((a * b) / c)");
  PREVERI("a + b / c", "(a + (b / c))");
  PREVERI("a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)");
  //  PREVERI("3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"); This is fine
  PREVERI("3 + 4;", "(3 + 4)");
  PREVERI("-5 * 5", "((-5) * 5)");
  PREVERI("5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))");
  PREVERI("5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))");
  PREVERI("3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))");
  PREVERI("3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))");
}

int
main (void)
{
  //test_ident();
  //test_int();
  //test_prefix();
  //printf("minus\n");
  //test_minix();
  //test_infix();
  operator_test();
  /*
  struct debug_buffer *buff = get_buffer();
    const char *input = "5 < 4 != 3 > 4;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist);
  expr_string(buff, s->expr);
  //  print_buffer(buff);
  //  buffer_string(buff);
    printf("%s\n", buffer_string(buff));
  free_debug_buf(buff);
  free_stmt(s);
  free_parser(&p);

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
