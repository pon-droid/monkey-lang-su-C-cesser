#include "object.h"

//TODO: add GNU readline
char *
read_input (FILE *stream)
{
  char *input = malloc(5 * sizeof(char *));
  char ch;
  size_t len = 5;
  size_t size;
  for (size = 0; (ch = fgetc(stream)) != EOF && ch != '\n'; size++)
    {
      if (size >= len)
	  input = realloc(input, len *= 2);
      input[size] = ch;
    }
  input[size] = '\0';
  //input = realloc(input, size);

  return input;
}

int
main (void)
{
  while (1)
    {
      printf("\ninput>: ");
      char *input = read_input(stdin);
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
    }
  return 0;
}
