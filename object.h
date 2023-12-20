#pragma once
#include "parser.h"
#include "enviro.h"
#include <stdarg.h>

enum obj_type
{
  INT_OBJ,
  BOOL_OBJ,
  NULL_OBJ,
  RET_OBJ,
  ERR_OBJ,
  FN_OBJ,
};

char const *obj_type_str[] =
{
  "INTEGER",
  "BOOLEAN",
  "NULL",
  "RETURN",
  "ERROR",
  "FUNCTION",
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
    struct // function object
    {
      struct list *params;
      struct stmt_list *body;
      struct enviro *env;
    };
  };
};

#define TRUE ((struct object *)(&CONSTANT_OBJECTS[1]))
#define FALSE ((struct object *)(&CONSTANT_OBJECTS[0]))
#define NULL_OBJECT ((struct object *)(&CONSTANT_OBJECTS[2]))

struct object *eval (const struct stmt *, struct enviro *);
struct object *eval_expr (const struct expr *, struct enviro *);

const struct object CONSTANT_OBJECTS [] =  
{
  {BOOL_OBJ, .bool = 0},
  {BOOL_OBJ, .bool = 1},
  {NULL_OBJ, .bool = 0} //Placeholder
};

struct object *
get_obj (enum obj_type type, const void *val)
{
  if (type == BOOL_OBJ)
    {
      if (*(int *)val == 0)
	return FALSE;
      else
	return TRUE;
    }
  
  struct object *obj = malloc(sizeof(struct object));
  obj->type = type;
  switch (obj->type)
    {
    case INT_OBJ:
      obj->integer = *(int *)val;
      break;
    case RET_OBJ:
      obj->return_val = (struct object *)val;
      break;
    default:;
      obj->type = ERR_OBJ;
      int len = snprintf(NULL, 0, "Couldn't create object of type %s", obj_type_str[type]);
      char *str = malloc(len + 1);
      snprintf(str, len + 1, "Couldn't create object of type %s", obj_type_str[type]);      
    }
  return obj;
}

struct object *
get_err_obj (const char *fmsg, ...)
{
  struct object *obj = malloc(sizeof(struct object)); 
  obj->type = ERR_OBJ;
  
  va_list args;
  va_list tmp_args;
  va_start(args, fmsg);
  va_copy(tmp_args, args);
  
  int len = vsnprintf(NULL, 0, fmsg, tmp_args);
  obj->err = malloc(len + 1);
  vsnprintf(obj->err, len + 1, fmsg, args);
  
  va_end(args);
  va_end(tmp_args);
  
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
eval_infix_expr (enum token_type op, struct object *left, struct object *right, struct enviro *env)
{
  if (left->type == INT_OBJ && right->type == INT_OBJ)
    return eval_integer_infix(op, left, right);
  else if (left->type != right->type)
    {
      struct object *err = get_err_obj("type mismatch: %s %s %s", obj_type_str[left->type], toktype_str[op], obj_type_str[right->type]);
      free_obj(left);
      free_obj(right);
      return err;
    }
  if (op == EQ)
    return bool_obj_conv(left == right);
  if (op == NOT_EQ)
    return bool_obj_conv(left != right);
  
  return get_err_obj("unknown operator: %s %s %s", obj_type_str[left->type], toktype_str[op], obj_type_str[right->type]);
}

struct object *
eval_stmt_list (const struct stmt_list *stmt_list, struct enviro *env)
{
  struct object *obj = NULL;
  for (int i = 0; i < stmt_list->count; i++)
    {
      if (obj)
	free_obj(obj);

      obj = eval(stmt_list->list[i], env);

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
eval_body (const struct stmt_list *stmt_list, struct enviro *env)
{
  struct object *obj = NULL;
  for (int i = 0; i < stmt_list->count; i++)
    {
      if (obj)
	free_obj(obj);
      obj = eval(stmt_list->list[i], env);

      if (obj && (obj->type == RET_OBJ || obj->type == ERR_OBJ))
	return obj;
    }
  
  return obj;
}
      
struct object *
eval_if_expr (const struct expr *node, struct enviro *env)
{
  struct object *cond = eval_expr(node->cond, env);
  int then_eval = obj_bool_conv(cond);
  free_obj(cond);
  
  if (then_eval)
    return eval_body(node->stmt_lists[THEN], env);
  if (node->stmt_lists[ALT])
    return eval_body(node->stmt_lists[ALT], env);

  return NULL_OBJECT;
}

struct object *
eval_expr (const struct expr *node, struct enviro *env)
{
  switch (node->type)
    {
    case INT_EXPR: return get_obj(INT_OBJ, &node->integer); break;
    case BOOL_EXPR: return get_obj(BOOL_OBJ, &node->bool); break;
    case PREFIX_EXPR:
      {      
	struct object *right = eval_expr(node->expr[RIGHT], env);
	if (right->type == ERR_OBJ)
	  return right;
	return eval_prefix_expr (node->token.type, right);
      }
      break;
    case INFIX_EXPR:
      {
	struct object *left = eval_expr(node->expr[LEFT], env);

	if (left->type == ERR_OBJ)
	  return left;	
	struct object *right = eval_expr(node->expr[RIGHT], env);

	if (right->type == ERR_OBJ)
	  return right;
	return eval_infix_expr (node->token.type, left, right, env);
      }
      break;
    case IF_EXPR: return eval_if_expr(node, env); break;
    case IDENT_EXPR:
      {
	if (!get_env(env, node->ident))
	  return get_err_obj("identifier not found: %s", node->ident);

	struct object *val = malloc(sizeof(struct object));

	memcpy(val, get_env(env, node->ident), sizeof(struct object));

	return val;
      }
      break;
    case FN_EXPR:
      {
	struct object *fn = malloc(sizeof(struct object));
	
	fn->type = FN_OBJ;
	fn->params = node->params;
	fn->body = node->body;
	
	fn->env = env;
	return fn;
      }
      break;
    case CALL_EXPR:
      {
	struct object *fn = eval_expr(node->fn, env);
	
	if (fn->type == ERR_OBJ)
	  return fn;

	struct list *args = get_list(sizeof(struct object));

	for (int i = 0; i < node->args->count; i++)
	  {
	    struct object *arg = eval_expr(node->args->list[i], env); //test eval
	    if (arg->type == ERR_OBJ)
	      {
		struct object *arg_err = NULL;
		memcpy(arg_err, arg, sizeof(struct object));
		free_list(args, free_obj);

		return arg_err;
	      }
	    append_list(args, arg);
	  }
	
	
      }
      break;
    default:
      return NULL_OBJECT;
    }
}

struct object *
eval (const struct stmt *node, struct enviro *env)
{
  switch (node->type)
    {
    case EXPR_STMT:
      return eval_expr(node->expr, env);
    case RET_STMT:
      return get_obj(RET_OBJ, eval_expr(node->expr, env));
    case LET_STMT:
      {
	struct object *e = eval_expr(node->expr, env);

	if (e->type == ERR_OBJ)
	  return e;


	if (!get_env(env, node->ident.literal))
	  set_env(env, node->ident.literal, malloc(sizeof(struct object)));
	
	//Overwrite previous variable if variable is redefined
        memcpy(get_env(env, node->ident.literal), e, sizeof(struct object));
	
	return e;
      }
    default:
      return NULL_OBJECT;
    }
}
