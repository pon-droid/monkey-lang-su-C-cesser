#pragma once
#include "parser.h"

enum obj_type
{
  INT_OBJ,
  BOOL_OBJ,
  NULL_OBJ,
};

struct object
{
  enum obj_type type;
  union
  {
    int integer;
    int bool;
  };
};

struct object *
get_obj (enum obj_type type, const void *val)
{
  struct object *obj = malloc(sizeof(struct object));
  obj->type = type;
  switch (type)
    {
    case INT_OBJ:
      obj->integer = *(int *)val;
      break;
    }
  return obj;
}

char *
obj_str (const struct object *obj)
{
  char *str;
  switch (obj->type)
    {
    case INT_OBJ:;
      int len = snprintf(NULL, 0, "%d", obj->integer);
      str = malloc(len + 1);
      snprintf(str, len + 1, "%d", obj->integer);
      break;
    }
  return str;
}

struct object *
eval_expr (const struct expr *node)
{
  switch (node->type)
    {
    case INT_EXPR: return get_obj(INT_OBJ, &node->integer); break;
    }
}

struct object *
eval (const struct stmt *node)
{
  switch (node->type)
    {
    case EXPR_STMT:
      return eval_expr(node->expr);
    }
}
