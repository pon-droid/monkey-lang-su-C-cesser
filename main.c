#include "object.h"

//TODO: add GNU readline
char *
read_input (FILE *stream)
{
  size_t len = 15;
  char *input;
  input = malloc(len * sizeof(*input));
  char ch = ' ';
  size_t size = 0;
  for (; (ch = fgetc(stream)) != EOF && ch != '\n'; size++)
    {
      if ((size + 1) >= len)
	input = realloc(input, (len *= 2) * sizeof(*input));
      input[size] = ch;
    }
  
  input[size++] = 0;
  input = realloc(input, size * sizeof(*input));
  return input;
}

int
main (void)
{
  while (1)
    {
      //const char *input = "if (!10) { 5 }";
      
      printf("\ninput>: ");
      char *input = read_input(stdin);
      struct stmt_list *program = parse_program(input);
      struct object *o = eval_stmt_list(program);
      if (o != NULL)
	{
	  char *eval = obj_str(o);
	  printf("%s", eval);
	  free(eval);
	  free_obj(o);
	}
      free_stmt_list(program);
      free(input);
      //free(input);
      
      /*
      struct lexer l = get_lexer(input);
      struct parser p = get_parser(&l);

      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);
      struct object *o = eval(s);
      char *eval = obj_str(o);
      printf("%s", eval);
      
      free_obj(o);
      free(eval);
      free_stmt(s);
      free_parser(&p);
      free(input);
      */
    }
  return 0;
}
