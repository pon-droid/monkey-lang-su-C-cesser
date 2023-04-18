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

#define TRUE (&CONSTANT_OBJECTS[1])
#define FALSE (&CONSTANT_OBJECTS[0])

const struct object CONSTANT_OBJECTS [] =  
{
  {BOOL_OBJ, .bool = 0},
  {BOOL_OBJ, .bool = 1},
  {NULL_OBJ, .bool = 0} //Placeholder
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
    case BOOL_OBJ:
      free(obj);
      if (*(int *)val == 0)
	return FALSE;
      else
	return TRUE;
      break;
    }
  return obj;
}

void
free_obj (struct object *obj)
{
  if (obj->type != BOOL_OBJ && obj->type != NULL_OBJ)
    {
      free(obj);
    }
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
    case BOOL_OBJ:
      if (obj->bool)
	return strdup("true");
      else
	return strdup("false");
      break;
    }
  return str;
}

struct object *
eval_bang_op (struct object *obj)
{
  enum obj_type t = obj->type;
  int bool = obj->bool;
  free_obj(obj);
  if (t == BOOL_OBJ)
    return bool ? FALSE : TRUE;
  else if (t == NULL_OBJ)
    return TRUE;
  return FALSE;
}

struct object *
eval_minus_op (struct object *obj)
{
  if (obj->type != INT_OBJ)
    return NULL;
  obj->integer *= -1;
  return obj;
}

struct object *
eval_prefix_expr (enum token_type op, struct object *obj)
{
  switch (op)
    {
    case BANG:
      return eval_bang_op(obj);
    case MINUS:
      return eval_minus_op(obj);
    default:
      return NULL;
    }
}
struct object *
eval_expr (const struct expr *node)
{
  switch (node->type)
    {
    case INT_EXPR: return get_obj(INT_OBJ, &node->integer); break;
    case BOOL_EXPR: return get_obj(BOOL_OBJ, &node->bool); break;
    case PREFIX_EXPR: return eval_prefix_expr (node->token.type, eval_expr(node->expr[RIGHT])); break;      
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
