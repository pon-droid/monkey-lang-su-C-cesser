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
  return 0;
}
