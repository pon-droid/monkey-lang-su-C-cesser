#define MEM_DEBUG (1)
#include "../lexer.h"
#include "../parser.h"
#include <assert.h>


int
main (void)
{
  const char *input =
    "let x = 5;\n"
    "let y = 10;\n"
    "let foobar = 838383;\n"
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
      //printf("%s and %s\n", s->ident.literal, tests[i]);
      //printf("%d\n", MEM_COUNT);
      assert(s != NULL);

      assert(!strcmp(s->ident.literal, tests[i]));
      free_stmt(s);
    }
   free_parser(&p);
   printf("%d\n", MEM_COUNT);
   assert(MEM_COUNT == 0);
  return 0;
}
