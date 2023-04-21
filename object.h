#pragma once
#include "parser.h"
#include <stdarg.h>

enum obj_type
{
  INT_OBJ,
  BOOL_OBJ,
  NULL_OBJ,
  RET_OBJ,
  ERR_OBJ,
};

char const *obj_type_str[] =
{
  "INTEGER",
  "BOOLEAN",
  "NULL",
  "RETURN",
  "ERROR",
};

struct object
{
  enum obj_type type;
  union
  {
    int integer;
    int bool;
    struct object *return_val;
    char* err;
  };
};

#define TRUE ((struct object *)(&CONSTANT_OBJECTS[1]))
#define FALSE ((struct object *)(&CONSTANT_OBJECTS[0]))
#define NULL_OBJECT ((struct object *)(&CONSTANT_OBJECTS[2]))

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
    case RET_OBJ:
      obj->return_val = (struct object *)val;
      break;
    }
  return obj;
}

struct object *
get_err_obj (const char *fmsg, ...)
{
  struct object *obj = malloc(sizeof(struct object)); 
  obj->type = ERR_OBJ;
  obj->err = malloc(50 * sizeof(*fmsg)); 

  va_list args;
  va_start(args, fmsg);
  vsnprintf(obj->err, 50 * sizeof(*fmsg), fmsg, args);//TODO: copy INT_OBJ conversion method for getting length of string
  va_end(args);

  obj->err = realloc(obj->err, (strlen(obj->err) + 1) * sizeof(obj->err));
  return obj;
}

void
free_obj (struct object *obj)
{ 
  if (obj->type == BOOL_OBJ || obj == NULL_OBJECT)
    return;

  if (obj->type == RET_OBJ)
    free_obj(obj->return_val);

  if (obj->type == ERR_OBJ)
    free(obj->err);
  
  free(obj);
}

char *
obj_str (const struct object *obj)
{
  char *str = NULL;
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
      break;
    case RET_OBJ:
      return obj_str(obj->return_val);
      break;
    case ERR_OBJ:
      return strdup(obj->err);
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
    return get_err_obj("unknown operator: -%s", obj_type_str[obj->type]);
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
      return get_err_obj("unknown operator: %s%s", toktype_str[op], obj_type_str[obj->type]);
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
	return get_err_obj("unknown operator: %s %s %s", obj_type_str[left->type], toktype_str[op], obj_type_str[right->type]);
    }
  return get_obj(INT_OBJ, &result);
}

struct object *
eval_infix_expr (enum token_type op, struct object *left, struct object *right)
{
  if (left->type == INT_OBJ && right->type == INT_OBJ)
    return eval_integer_infix(op, left, right);
  else if (left->type != right->type)
    {
      enum obj_type left_t = left->type;
      enum obj_type right_t = right->type;
      free_obj(left);
      free_obj(right);
      return get_err_obj("type mismatch: %s %s %s", obj_type_str[left_t], toktype_str[op], obj_type_str[right_t]);
    }
  if (op == EQ)
    return bool_obj_conv(left == right);
  if (op == NOT_EQ)
    return bool_obj_conv(left != right);
  
  return get_err_obj("unknown operator: %s %s %s", obj_type_str[left->type], toktype_str[op], obj_type_str[right->type]);
}

struct object *
eval_stmt_list (const struct stmt_list *stmt_list)
{
  struct object *obj = NULL;
  for (int i = 0; i < stmt_list->count; i++)
    {
      if (obj)
	free_obj(obj);
      obj = eval(stmt_list->list[i]);

      if (obj->type == RET_OBJ)
	{
	  struct object *val = obj->return_val;
	  free(obj);
	  return val;
	}
      else if (obj->type == ERR_OBJ)
	return obj;
    }
  return obj;
}

struct object *
eval_body (const struct stmt_list *stmt_list)
{
  struct object *obj = NULL;
  for (int i = 0; i < stmt_list->count; i++)
    {
      if (obj)
	free_obj(obj);
      obj = eval(stmt_list->list[i]);

      if (obj && (obj->type == RET_OBJ || obj->type == ERR_OBJ))
	return obj;
    }
  
  return obj;
}
      
struct object *
eval_if_expr (const struct expr *node)
{
  struct object *cond = eval_expr(node->cond);
  int then_eval = obj_bool_conv(cond);
  free_obj(cond);
  
  if (then_eval)
    return eval_body(node->stmt_lists[THEN]);
  if (node->stmt_lists[ALT])
    return eval_body(node->stmt_lists[ALT]);

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
  return NULL_OBJECT;
}

struct object *
eval (const struct stmt *node)
{
  switch (node->type)
    {
    case EXPR_STMT:
      return eval_expr(node->expr);
    case RET_STMT:
      return get_obj(RET_OBJ, eval_expr(node->expr));
    }
  return NULL_OBJECT;
}
