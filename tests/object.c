#include "../object.h"
#include <assert.h>

struct test
{
  char* input;
  int output;
};

int
main (void)
{
  struct test tests [] =
    {
      {"5", 5},
      {"10", 10},
      {"4", 4},
      {"0", 0},
      {"2382938", 2382938},
    };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);

      assert(s->expr->type == INT_EXPR);
      
      struct object *o = eval(s);
      assert(o->type == INT_OBJ);
      assert(o->integer == tests[i].output);
      
      free(o);
      free_stmt(s);
      free_parser(&p);
    }

  return 0;
}
