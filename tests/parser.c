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
  getfree_errors(&p.elist, 0);
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
  getfree_errors(&p.elist, 0);
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
  getfree_errors(&p.elist, 0);
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
  getfree_errors(&p.elist, 0);
  printf("work\n");
  assert(s->type == EXPR_STMT);
  printf("%s\n", s->expr->token.literal);
  //  printf("%d\n", s->expr->expr->integer);
  printf("%d\n", s->expr->expr[RIGHT]->integer);
  assert(s->expr->type == PREFIX_EXPR);
  free_stmt(s);
  free_parser(&p);
}

void
test_infix (void)
{
  const char *input = "5 < 4 != 3 > 4;";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
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
  PREVERI("-abbbbbbbbbbbbbbbbbbbbbbbbb * b", "((-abbbbbbbbbbbbbbbbbbbbbbbbb) * b)");
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

void
test_bool (void)
{
  PREVERI("true", "true");
  PREVERI("false", "false");
  PREVERI("3 > 5 == false", "((3 > 5) == false)");
  PREVERI("3 < 5 == true", "((3 < 5) == true)");
}

void
test_bracket (void)
{
  PREVERI("1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)");
  PREVERI("(5 + 5) * 2", "((5 + 5) * 2)");
  PREVERI("2 / (5 + 5)", "(2 / (5 + 5))");
  PREVERI("-(5 + 5)", "(-(5 + 5))");
  PREVERI("!(true == true)", "(!(true == true))");
}

int
main (void)
{
  test_ident();
  test_int();
  test_infix();
  test_minix();
  test_prefix();
  operator_test();
  test_bool();
  test_bracket();
  return 0;
}
