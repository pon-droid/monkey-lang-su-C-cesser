#define MEM_DEBUG 1
#include "../lexer.h"
#include "../parser.h"
#include <assert.h>


int
main (void)
{
  const char *input =
    "let  5;\n"
    "let  = 10;\n"
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

  
  while(p.cur_tok->type != END_FILE)
    {
      struct stmt *s = get_stmt(&p);
      if (s) {
	printf("%s of type %s\n", s->ident.literal, toktype_str[s->ident.type]); free_stmt(s);  } else {
	cycle_token(&p); }
    }
  getfree_errors(p.elist);
  free(p.elist);
  free_parser(&p);
  printf("%d\n", MEM_COUNT);
  return 0;
}
