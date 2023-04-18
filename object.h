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
#define NULL_OBJECT (&CONSTANT_OBJECTS[2])

struct object *eval (const struct stmt *);
struct object *eval_expr (const struct expr *);

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
    case NULL_OBJ:
      return strdup("null");
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
    return NULL_OBJECT;
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
bool_obj_conv (int bool)
{
  return bool ? TRUE : FALSE;
}

int
obj_bool_conv (struct object *cond)
{
  if (cond->type == BOOL_OBJ)
    return cond->bool;
  if (cond->type == NULL_OBJ)
    return 0;
  return 1;
}

struct object *
eval_integer_infix (enum token_type op, struct object *left, struct object *right)
{
  int lval = left->integer;
  int rval = right->integer;

  free_obj(left);
  free_obj(right);

  int result;  
  switch (op)
    {
    case PLUS: result = lval + rval; break;
    case MINUS: result = lval - rval; break;
    case ASTERISK: result = lval * rval; break;
    case SLASH: result = lval / rval; break;
    case LT: return bool_obj_conv(lval < rval); break;
    case GT: return bool_obj_conv(lval > rval); break;
    case EQ: return bool_obj_conv(lval == rval); break;
    case NOT_EQ: return bool_obj_conv(lval != rval); break;
    default:
      return NULL;
    }
  return get_obj(INT_OBJ, &result);
}

struct object *
eval_infix_expr (enum token_type op, struct object *left, struct object *right)
{
  if (left->type == INT_OBJ && right->type == INT_OBJ)
    return eval_integer_infix(op, left, right);
  if (op == EQ)
    return bool_obj_conv(left == right);
  if (op == NOT_EQ)
    return bool_obj_conv(left != right);
  
  return NULL_OBJECT;
}

struct object *
eval_stmt_list (const struct stmt_list *stmt_list)
{
  struct object *obj;
  obj = eval(stmt_list->list[stmt_list->count - 1]);
  return obj;
}

struct object *
eval_if_expr (const struct expr *node)
{
  
  struct object *cond = eval_expr(node->cond);
  int then_eval = obj_bool_conv(cond);
  free_obj(cond);
  
  if (then_eval)
    return eval_stmt_list(node->stmt_lists[THEN]);
  if (node->stmt_lists[ALT])
    return eval_stmt_list(node->stmt_lists[ALT]);

  return NULL_OBJECT;
}

struct object *
eval_expr (const struct expr *node)
{
  switch (node->type)
    {
    case INT_EXPR: return get_obj(INT_OBJ, &node->integer); break;
    case BOOL_EXPR: return get_obj(BOOL_OBJ, &node->bool); break;
    case PREFIX_EXPR: return eval_prefix_expr (node->token.type, eval_expr(node->expr[RIGHT])); break;
    case INFIX_EXPR: return eval_infix_expr (node->token.type, eval_expr(node->expr[LEFT]), eval_expr(node->expr[RIGHT])); break;
    case IF_EXPR: return eval_if_expr(node); break;
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
