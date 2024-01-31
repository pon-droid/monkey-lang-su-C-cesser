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
  struct enviro *env = get_enviro();
  while (1)
    {  
      printf("\ninput>: ");
      char *input = read_input(stdin);
      struct stmt_list *program = parse_program(input);
      struct object *o = eval_stmt_list(program, env);
      if (o)
	{
	  char *eval = obj_str(o);
	  printf("%s", eval);
	  free(eval);
	  free_obj(o);
	}
      free_stmt_list(program);

      free(input);
    }
  free_enviro(env);
  return 0;
}
