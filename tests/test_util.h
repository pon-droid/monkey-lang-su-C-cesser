#pragma once

#include "../parser.h"
#include <assert.h>

struct debug_buffer
{
  char **buffer;
  int count;
  int size;
  char *str;
};

struct debug_buffer *
get_buffer (void)
{
  struct debug_buffer *b = malloc(sizeof(struct debug_buffer));
  b->count = 0;
  b->size = 8;
  b->buffer = malloc(sizeof(char *) * b->size);
  b->str = NULL;
  return b;
}

void
append_buffer (struct debug_buffer *b, char *c)
{
  if ((b->count + strlen(c) + 1) >= b->size)
    {
      b->size += b->count * 2 + strlen(c) + 1;
      b->buffer = realloc(b->buffer, sizeof(char *) * b->size);
    }
  b->buffer[b->count++] = c;
}

void
free_debug_buf (struct debug_buffer *b)
{
  free(b->buffer);
  free(b->str);
  free(b);
}

void
expr_string (struct debug_buffer *b, const struct expr *e)
{
  switch(e->type)
    {
    case PREFIX_EXPR:
      append_buffer(b, "(");
      append_buffer(b, e->token.literal);
      expr_string(b, e->expr[RIGHT]);
      append_buffer(b, ")");
      break;
    case INFIX_EXPR:
      append_buffer(b, "(");
      expr_string(b, e->expr[LEFT]);
      append_buffer(b, " ");
      append_buffer(b, e->token.literal);
      append_buffer(b, " ");
      expr_string(b, e->expr[RIGHT]);
      append_buffer(b, ")");
      break;
    default:
      append_buffer(b, e->token.literal);
    }
}

void
print_buffer (struct debug_buffer *b)
{
  for(int i = 0; i < b->count; i++)
    {
      printf("%s", b->buffer[i]);
    }
  printf("\n");
}

void
buffer_string (struct debug_buffer *b)
{
  char *str = malloc(sizeof(char) * b->size);
  int size = 0;

  for(int i = 0; i < b->count; i++)
    {
      int length = strlen(b->buffer[i]); //Without null delimiter

      memcpy(str + size, b->buffer[i], length);
      size += length;
    }
  str[size] = '\0';
  b->str = str;
}

int
test_prec (const char *input, const char *output)
{
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);
  struct stmt *s = get_stmt(&p);
  struct debug_buffer *b = get_buffer();
  assert(s->type == EXPR_STMT);
  expr_string(b, s->expr);
  buffer_string(b);
  
  int success = (strcmp(b->str, output) == 0);
  if (!success)
    {
      getfree_errors(&p.elist, 0);
    }
  free_stmt(s);
  getfree_errors(&p.elist, 1);
  free_parser(&p);
  free_debug_buf(b);
  return success;
}
