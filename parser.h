#pragma once

#include "lexer.h"
#include "list.h"

//infix expressions
#define LEFT (0)
#define RIGHT (1)
//if statement conditions
#define THEN (0)
#define ALT (1)

enum stmt_type
{
  LET_STMT,
  RET_STMT,
  EXPR_STMT,
};

enum expr_type
{
  INT_EXPR,
  IDENT_EXPR,
  PREFIX_EXPR,
  INFIX_EXPR,
  BOOL_EXPR,
  IF_EXPR,
  FN_EXPR,
  CALL_EXPR,
};

enum op_prec
{
  LOWEST,
  EQUALS,
  LESSGREATER,
  SUM,
  PRODUCT,
  PREFIX,
  CALL,
};

struct stmt_list;

struct expr
{
  enum expr_type type;
  struct token token;
  union
  {
    int integer;
    char *ident;
    struct expr *expr[2];
    int bool;
    struct //if statement stuff
    {
      struct expr *cond;
      struct stmt_list *stmt_lists[2];
    };
    struct //fn literal
    {
      struct list *params;
      struct stmt_list *body;
    };
    struct
    {
      struct expr *fn; //function or identifier
      struct list *args;
    };
  };
};

struct stmt
{
  enum stmt_type type;
  struct token token;
  struct token ident;
  struct expr *expr;
};

struct enode
{
  enum token_type expect_tok, got_tok;
  struct enode *node;
};

struct stmt_list
{
  struct stmt **list;
  int count;
  int size;
  struct token token; // {
};

struct parser
{
  struct lexer *l;
  struct token *cur_tok;
  struct token *peek_tok;
  struct enode *elist;
};

// Forward declarations
//TODO: Get rid of this mess
struct expr *prefix_fns (struct parser *);
struct expr * parse_expr (struct parser *, enum op_prec);
int expect_peek (struct parser *, enum token_type);
struct stmt * get_stmt (struct parser *);
struct expr * parse_if_expr (struct parser *);
void free_expr (struct expr *);
void free_stmt (struct stmt *);
struct expr * parse_fn_lit (struct parser *);

enum op_prec
get_prec (enum token_type t)
{
  switch(t)
    {
    case EQ: case NOT_EQ: return EQUALS; break;
    case LT: case GT: return LESSGREATER; break;
    case PLUS: case MINUS: return SUM; break;
    case SLASH: case ASTERISK: return PRODUCT; break;
    case LPAREN: return CALL; break;
    default:
      return LOWEST;
    }
}

void
cycle_token (struct parser *p)
{
  if (p->cur_tok)
    free_token(p->cur_tok);
  
  p->cur_tok = p->peek_tok;
  p->peek_tok = next_tok(p->l);
}

struct expr *
parse_ident (const struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = IDENT_EXPR;
  e->ident = strdup(p->cur_tok->literal);
  cpy_token(&e->token, p->cur_tok);
  return e;
}

struct expr *
parse_int (struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = INT_EXPR;
  e->integer = atoi(p->cur_tok->literal);
  cpy_token(&e->token, p->cur_tok);
  return e;
}

struct expr *
parse_prefix (struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = PREFIX_EXPR;
  cpy_token(&e->token, p->cur_tok);
  cycle_token(p);
  e->expr[RIGHT] = parse_expr(p, PREFIX);
  return e;
}

struct expr *
parse_bool (const struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = BOOL_EXPR;
  cpy_token(&e->token, p->cur_tok);
  e->bool = e->token.type == TRUE ? 1 : 0;
  return e;
}

struct expr *
parse_group (struct parser *p)
{
  cycle_token(p);
  struct expr *e = parse_expr(p, LOWEST);
  expect_peek(p, RPAREN);
  return e;
}

struct expr *
prefix_fns (struct parser *p)
{
  switch (p->cur_tok->type)
    {
    case IDENT:
      return parse_ident(p);
    case INT:
      return parse_int(p);
    case BANG: case MINUS:
      return parse_prefix(p);
    case TRUE: case FALSE:
      return parse_bool(p);
    case LPAREN:
      return parse_group(p);
    case IF:
      return parse_if_expr(p);
    case FUNCTION:
      return parse_fn_lit(p);
    default:
      return NULL;
    }
}

int is_infix (enum token_type t)
{
  if (t == PLUS || t == MINUS || t == SLASH || t == ASTERISK || t == EQ || t == NOT_EQ || t == LT || t == GT || t == LPAREN)
    return 1;
  return 0;
}

void
append_stmt_list (struct stmt_list *slist, struct stmt *s)
{
  if (((slist->count + 1)) >= slist->size)
    {
      slist->size *= 2;
      slist->list = realloc(slist->list, sizeof(struct stmt *) * slist->size);
    }
  slist->list[slist->count++] = s;
}

struct list *
get_lit_list (struct parser *p)
{

  if (p->peek_tok->type == RPAREN)
    {
      cycle_token(p);
      return NULL;
    }
  
  cycle_token(p);

  struct list *l = malloc(sizeof(struct list));
  l->count = 0;
  l->size = 8;
  l->typesize = sizeof(struct expr *);
  l->list = malloc(l->size * l->typesize);
		   
  append_list(l, parse_ident(p));

  while (p->peek_tok->type == COMMA)
    {
      cycle_token(p);
      cycle_token(p);
      append_list(l, parse_ident(p));
    }

  if (!expect_peek(p, RPAREN))
    return NULL;

  return l;
}

struct stmt_list *
get_stmt_list (struct parser *p)
{
  struct stmt_list *s = malloc(sizeof(struct stmt_list));
  s->count = 0;
  s->size = 2;
  s->list = malloc(sizeof(struct stmt *) * s->size);

  s->token = *p->cur_tok;
  cycle_token(p);

  while (p->cur_tok->type != RBRACE && p->cur_tok->type != END_FILE)
    {
      struct stmt *state = get_stmt(p);
      if (state)
	append_stmt_list(s, state);
      cycle_token(p);
    }
  return s;
}

struct expr *
parse_fn_lit (struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = FN_EXPR;
  cpy_token(&e->token, p->cur_tok);
  
  if (!expect_peek(p, LPAREN))
    return NULL;

  e->params = get_lit_list(p);

  if (!expect_peek(p, LBRACE))
    return NULL;

  e->body = get_stmt_list(p);

  return e;
}

struct expr *
parse_if_expr (struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = IF_EXPR;
  cpy_token(&e->token, p->cur_tok);

  if (!expect_peek(p, LPAREN))
    return NULL;

  cycle_token(p);
  e->cond = parse_expr(p, LOWEST);

  if (!expect_peek(p, RPAREN))
    return NULL;

  if (!expect_peek(p, LBRACE))
    return NULL;

  e->stmt_lists[THEN] = get_stmt_list(p);
  e->stmt_lists[ALT] = NULL;

  if (p->peek_tok->type == ELSE)
    {
      cycle_token(p);

      if (!expect_peek(p, LBRACE))
	return NULL;

      e->stmt_lists[ALT] = get_stmt_list(p);
    }
  return e;
}

struct list *
parse_call_args (struct parser *p)
{
  struct list *args = get_list(sizeof(struct expr *));

  if (p->peek_tok->type == RPAREN)
    {
      cycle_token(p);
      return args;
    }

  cycle_token(p);
  append_list(args, parse_expr(p, LOWEST));

  while (p->peek_tok->type == COMMA)
    {
      cycle_token(p);
      cycle_token(p);
      append_list(args, parse_expr(p, LOWEST));
    }

  if (!expect_peek(p,RPAREN))
    return NULL;

  return args;
}

struct expr *
parse_infix_expr (struct parser *p, struct expr *left)
{
  struct expr *e = malloc(sizeof(struct expr));
  cpy_token(&e->token, p->cur_tok);

  if (p->cur_tok->type != LPAREN)
    {
      e->type = INFIX_EXPR;
      enum op_prec prec = get_prec(p->cur_tok->type);
      cycle_token(p);
      e->expr[LEFT] = left;
      e->expr[RIGHT] = parse_expr(p, prec);
    }
  else
    {
      e->type = CALL_EXPR;
      e->fn = left;
      e->args = parse_call_args(p);
    }
  return e;
}

struct expr *
parse_expr (struct parser *p, enum op_prec prec)
{
  struct expr *left = prefix_fns(p);
  if (!left)
    return NULL;

  while (p->peek_tok->type != SEMICOLON && prec < get_prec(p->peek_tok->type))
    {
      if (!is_infix(p->peek_tok->type))
	return left;

      cycle_token(p);
      left = parse_infix_expr(p, left);
    }
	  

  return left;
}
  
struct enode *
get_node (enum token_type expt, enum token_type got)
{
  struct enode *e = malloc(sizeof(struct enode));
  if (!e)
    return NULL;
  e->expect_tok = expt;
  e->got_tok = got;
  e->node = NULL;
  return e;
}

struct parser
get_parser (struct lexer *l)
{
  return (struct parser)
    {
      .l = l,
      .cur_tok = next_tok(l),
      .peek_tok = next_tok(l),
      .elist = NULL,
    };
}

void
new_error (struct enode **elist, enum token_type expt, enum token_type got)
{
  if (!*elist)
    *elist = get_node(expt, got);
  else
    {
      struct enode *cur = *elist;
      while (cur->node)
	cur = cur->node;
      cur->node = get_node(expt, got);
    }
}

int
expect_peek (struct parser *p, enum token_type t)
{
  if (p->peek_tok->type == t)
    {
      cycle_token(p);
      return 1;
    }
  new_error(&p->elist, t, p->peek_tok->type);
  return 0;
}

void
free_stmt_list (struct stmt_list *sl)
{
  for(int i = 0; i < sl->count; i++)
      free_stmt(sl->list[i]);
  free(sl->list);
  free(sl);
}

void
free_if_expr (struct expr *e)
{
  free_expr(e->cond);
  free_stmt_list(e->stmt_lists[THEN]);
  if (e->stmt_lists[ALT])
    free_stmt_list(e->stmt_lists[ALT]);
}

void
free_expr (struct expr *e)
{
  if (e->type == IDENT_EXPR)
    free(e->ident);
  if (e->token.literal)
    free(e->token.literal);
  if (e->type == PREFIX_EXPR)    
    free_expr(e->expr[RIGHT]);
  if (e->type == INFIX_EXPR)
    {
      free_expr(e->expr[LEFT]);
      free_expr(e->expr[RIGHT]);
    }
  if (e->type == IF_EXPR)
    free_if_expr(e);
  if (e->type == FN_EXPR)
    {
      free_stmt_list(e->body);
      if (e->params)
	free_list(e->params, free_expr);
    }
  if (e->type == CALL_EXPR)
    {
      free_expr(e->fn);
      if (e->args)
	free_list(e->args, free_expr);
    }
      
  free(e);
}

void
free_stmt (struct stmt *s)
{
  if (s->expr)
    free_expr(s->expr);
  if (s->token.literal)
    free(s->token.literal);
  if (s->ident.literal)
    free(s->ident.literal);
  
  free(s);
}

struct stmt *
get_let_stmt (struct parser *p)
{
  struct stmt *s = malloc(sizeof(struct stmt));
  memset(s, 0, sizeof(struct stmt));
  s->type = LET_STMT;
  if(!s)
    return NULL;

  cpy_token(&s->token, p->cur_tok);
  
  if (!expect_peek(p, IDENT))
    {
      free_stmt(s);
      return NULL;
    }

  cpy_token(&s->ident, p->cur_tok);

  if (!expect_peek(p, ASSIGN))
    {
      free_stmt(s);
      return NULL;
    }

  cycle_token(p);

  s->expr = parse_expr(p, LOWEST);
  /*
  printf("Current token: %s\n", toktype_str[p->cur_tok->type]);
  if (p->peek_tok->type == SEMICOLON)
    {
      cycle_token(p);
      //      cycle_token(p);
    }
  printf("Current token: %s\n", toktype_str[p->cur_tok->type]);
  */
  while (p->peek_tok->type == SEMICOLON) {
    //printf("peek_tok: %s cur_tok: %s \n", p->peek_tok->literal, p->cur_tok->literal);
    cycle_token(p);
  }
  //printf("peek_tok: %s cur_tok: %s \n", p->peek_tok->literal, p->cur_tok->literal);
  return s;
}

struct stmt *
get_ret_stmt (struct parser *p)
{
  struct stmt *s = malloc(sizeof(struct stmt));
  memset(s, 0, sizeof(struct stmt));
  s->type = RET_STMT;

  if (!s)
    return NULL;

  cpy_token(&s->token, p->cur_tok);

  cycle_token(p);

  s->expr = parse_expr(p, LOWEST);
  
  if (p->peek_tok->type == SEMICOLON)
    {
      cycle_token(p);
      cycle_token(p);
    }
  
  return s;
}

struct stmt *
get_expr_stmt (struct parser *p)
{
  struct stmt *s = malloc(sizeof(struct stmt));
  memset(s, 0, sizeof(struct stmt));
  if (!s)
    return NULL;
  
  s->type = EXPR_STMT;
  cpy_token(&s->token, p->cur_tok);
  s->expr = parse_expr(p, LOWEST);

  if (p->peek_tok->type == SEMICOLON)
    cycle_token(p);
  
  return s;
}

struct stmt *
get_stmt (struct parser *p)
{
  switch (p->cur_tok->type)
    {
    case LET: return get_let_stmt(p); break;
    case RETURN: return get_ret_stmt(p); break;
    default:
      return get_expr_stmt(p);
    }
}

void
free_parser (struct parser *p)
{
  if (p->cur_tok)
    free_token(p->cur_tok);

  if (p->peek_tok)
    free_token(p->peek_tok);
}

void
getfree_errors (struct enode **elist, int ignore)
{
  struct enode *cur = *elist;
  struct enode *node = cur;
  while (node)
    {
      node = cur->node;
      if (!ignore)
	printf("parser error: expected token type %s got %s instead!\n", toktype_str[cur->expect_tok], toktype_str[cur->got_tok]);
      free(cur);
      cur = node;
    }
}
  
struct stmt_list *
parse_program (const char *input)
{
  struct lexer l = get_lexer(input);
  struct parser p = get_parser(&l);

  
  struct stmt_list *program = malloc(sizeof(struct stmt_list));
  program->count = 0;
  program->size = 2;
  program->list = malloc(program->size * sizeof(struct stmt*));
  program->token = *p.cur_tok;

  while (p.cur_tok->type != END_FILE)
    {
      struct stmt *s = get_stmt(&p);
      
      if (s)
	append_stmt_list(program, s);
      
      cycle_token(&p);
    }
  getfree_errors(&p.elist, 0);
  free_parser(&p);

  return program;
}
