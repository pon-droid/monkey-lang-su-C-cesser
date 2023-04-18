#include "../object.h"
#include <assert.h>

struct test
{
  char* input;
  int output;
};

void
test_int (void)
{
    struct test tests [] =
    {
      {"5", 5},
      {"10", 10},
      {"4", 4},
      {"0", 0},
      {"2382938", 2382938},
      {"-5", -5},
      {"-10", -10},
      {"-9999", -9999},
      {"5 + 5 + 5 + 5 - 10", 10},
      {"2 * 2 * 2 * 2 * 2", 32},
      {"-50 + 100 + -50", 0},
      {"5 * 2 + 10", 20},
      {"5 + 2 * 10", 25},
      {"20 + 2 * -10", 0},
      {"50 / 2 * 2 + 10", 60},
      {"2 * (5 + 10)", 30},
      {"3 * 3 * 3 + 10", 37},
      {"3 * (3 * 3) + 10", 37},
      {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},     
    };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);

      //assert(s->expr->type == INT_EXPR);
      
      struct object *o = eval(s);
      assert(o->type == INT_OBJ);
      assert(o->integer == tests[i].output);
      
      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

void
test_bool (void)
{
  struct test tests [] =
    {
      {"true", 1},
      {"false", 0},
    };

  for (int i = 0; i < 2; i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);

      struct object *o = eval(s);
      assert(o->type == BOOL_OBJ);
      assert(o->bool == tests[i].output);

      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

void
test_prefix (void)
{
  struct test tests [] =
    {
      {"!true", 0},
      {"!false", 1},
      {"!5", 0},
      {"!!true", 1},
      {"!!false", 0},
      {"!!5", 1},
      {"true", 1},
      {"false", 0},
      {"1 < 2", 1},
      {"1 > 2", 0},
      {"1 < 1", 0},
      {"1 > 1", 0},
      {"1 == 1", 1},
      {"1 != 1", 0},
      {"1 == 2", 0},
      {"1 != 2", 1},    
    };
  
  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);
      //assert(s->expr->type == PREFIX_EXPR);
      
      struct object *o = eval(s);
      assert(o->type == BOOL_OBJ);
      assert(o->bool == tests[i].output);

      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

int
main (void)
{
  test_int();
  test_bool();
  test_prefix();
  return 0;
}
