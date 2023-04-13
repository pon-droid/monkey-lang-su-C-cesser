#include "../lexer.h"
#include "../parser.h"
#include <assert.h>
#include "test_util.h"

void
test_let (void)
{
   const char *input =
    "let x = 5;"
    "let y = 10;"
    "let foobar = 838383;"
    ;
     
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
      assert(s);
      assert(s->type == LET_STMT);
      assert(s->token.type == LET);
      assert(strcmp(s->ident.literal, tests[i]) == 0);
      free_stmt(s);
    }
  getfree_errors(&p.elist, 0);
  free_parser(&p);
}

void
test_return (void)
{
  const char *input =
    "return 5;\n"
    "return 10;\n"
    "return 993322;\n";

  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  for (int i = 0; i < 3; i++)
    {
      struct stmt *s = get_stmt(&p);
      assert(s);
      assert(s->type == RET_STMT);
      assert(s->token.type == RETURN);
      free_stmt(s);
    }
  getfree_errors(&p.elist, 0);
  free_parser(&p);
}
 
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
//PREVERI("3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"); This is fine
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

void
test_if (void)
{
  const char *input = "if (x < y) { x }";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == EXPR_STMT);
  assert(s->expr->type == IF_EXPR);
  assert(s->expr->cond->type == INFIX_EXPR);
  assert(strcmp(s->expr->stmt_lists[THEN]->list[0]->expr->ident, "x") == 0);
  free_stmt(s);
  free_parser(&p);
}

void
test_if_else (void)
{
  const char *input = "if (x < y) { x } else { y }";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == EXPR_STMT);
  assert(s->expr->type == IF_EXPR);
  assert(s->expr->cond->type == INFIX_EXPR);
  assert(strcmp(s->expr->stmt_lists[THEN]->list[0]->expr->ident, "x") == 0);
  assert(strcmp(s->expr->stmt_lists[ALT]->list[0]->expr->ident, "y") == 0);
  free_stmt(s);
  free_parser(&p);
}

void
test_fn_literal(void)
{
  const char *input = "fn(x, y) { x + y; };";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == EXPR_STMT);
  assert(s->expr->type == FN_EXPR);
  assert(strcmp(((struct expr *)s->expr->params->list[0])->ident, "x") == 0);
  assert(strcmp(((struct expr *)s->expr->params->list[1])->ident, "y") == 0);
  free_stmt(s);
  free_parser(&p);
}

void
test_fn_literal2(void)
{
  const char *input = "fn() {};";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == EXPR_STMT);
  assert(s->expr->type == FN_EXPR);
  assert(s->expr->params == NULL);
  free_stmt(s);
  free_parser(&p);
}

void
test_fn_literal3(void)
{
  const char *input = "fn(x, y, z) {};";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == EXPR_STMT);
  assert(s->expr->type == FN_EXPR);
  assert(strcmp(((struct expr *)s->expr->params->list[0])->ident, "x") == 0);
  assert(strcmp(((struct expr *)s->expr->params->list[1])->ident, "y") == 0);
  assert(strcmp(((struct expr *)s->expr->params->list[2])->ident, "z") == 0);
  free_stmt(s);
  free_parser(&p);
}

void
test_call_expr (void)
{
  const char *input = "add(1, 2 * 3, 4 + 5);";
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == EXPR_STMT);
  assert(((struct expr *)s->expr->args->list[0])->integer == 1);
  
  assert(((struct expr *)s->expr->args->list[1])->expr[LEFT]->integer == 2);
  assert(((struct expr *)s->expr->args->list[1])->token.type == ASTERISK);
  assert(((struct expr *)s->expr->args->list[1])->expr[RIGHT]->integer == 3);

  assert(((struct expr *)s->expr->args->list[2])->expr[LEFT]->integer == 4);
  assert(((struct expr *)s->expr->args->list[2])->token.type == PLUS);
  assert(((struct expr *)s->expr->args->list[2])->expr[RIGHT]->integer == 5);

  free_stmt(s);
  free_parser(&p);
}

void
test_let_real (void)
{
  const char *input =
    "let x = 5;"
    "let y = true;"
    "let foobar = y;";

  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == LET_STMT);
  assert(s->expr->integer == 5);
  assert(strcmp(s->ident.literal, "x") == 0);
  
  free_stmt(s);
  s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s->type == LET_STMT);
  assert(s->expr->bool == 1);
  assert(strcmp(s->ident.literal, "y") == 0);
  
  free_stmt(s);
  s = get_stmt(&p);
  getfree_errors(&p.elist, 0);

  assert(s->type == LET_STMT);
  assert(strcmp(s->expr->ident, "y") == 0);
  assert(strcmp(s->ident.literal, "foobar") == 0);
  
  free_stmt(s);
  free_parser(&p);
}

void
test_ret_real (void)
{
  const char *input =
    "return 5;"
    "return true;"
    "return y;";

  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  struct stmt *s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s);
  assert(s->type == RET_STMT);
  assert(s->expr->integer == 5);
  
  free_stmt(s);
  s = get_stmt(&p);
  getfree_errors(&p.elist, 0);
  
  assert(s->type == RET_STMT);
  assert(s->expr->bool == 1);
  
  free_stmt(s);
  s = get_stmt(&p);
  getfree_errors(&p.elist, 0);

  assert(s->type == RET_STMT);
  assert(strcmp(s->expr->ident, "y") == 0);
  
  free_stmt(s);
  free_parser(&p);
}

int
main (void)
{
  test_let();
  test_return();
  test_ident();
  test_int();
  test_infix();
  test_minix();
  test_prefix();
  operator_test();
  test_bool();
  test_bracket();
  test_if();
  test_if_else();
  test_fn_literal();
  test_fn_literal2();
  test_fn_literal3();
  test_call_expr();
  test_let_real();
  test_ret_real();
  return 0;
}
