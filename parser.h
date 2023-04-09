#pragma once

#include "lexer.h"

#define LEFT (0)
#define COND (0)
#define THEN (0)
#define ALT (1)
#define RIGHT (1)

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
    struct stmt_list *stmt_lists[2];
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
  
enum op_prec
get_prec (enum token_type t)
{
  switch(t)
    {
    case EQ: case NOT_EQ: return EQUALS; break;
    case LT: case GT: return LESSGREATER; break;
    case PLUS: case MINUS: return SUM; break;
    case SLASH: case ASTERISK: return PRODUCT; break;
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
parse_int (const struct parser *p)
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
    default:
      return NULL;
    }
}

int is_infix (enum token_type t)
{
  if (t == PLUS || t == MINUS || t == SLASH || t == ASTERISK || t == EQ || t == NOT_EQ || t == LT || t == GT)
    return 1;
  return 0;
}

void
append_stmt_list (struct stmt_list *slist, struct stmt *s)
{
  if (((slist->count + 1) * sizeof(struct stmt)) >= slist->size)
    {
      slist->size *= 2;
      slist->list = realloc(slist->list, sizeof(struct stmt *) * slist->size);
    }
  slist->list[slist->count++] = s;
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
parse_if_expr (struct parser *p)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = IF_EXPR;

  if (!expect_peek(p, LPAREN))
    return NULL;

  cycle_token(p);
  e->expr[COND] = parse_expr(p, LOWEST);

  if (!expect_peek(p, RPAREN))
    return NULL;

  if (!expect_peek(p, LBRACE))
    return NULL;

  e->stmt_lists[THEN] = get_stmt_list(p);

  return e;
}
  

struct expr *
parse_infix_expr (struct parser *p, struct expr *left)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->type = INFIX_EXPR;
  cpy_token(&e->token, p->cur_tok);
  enum op_prec prec = get_prec(p->cur_tok->type);
  cycle_token(p);
  e->expr[LEFT] = left;
  e->expr[RIGHT] = parse_expr(p, prec);
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
      cycle_token(p);
      free_stmt(s);
      return NULL;
    }

  //  cycle_token(p);
  cpy_token(&s->ident, p->cur_tok);

  if (!expect_peek(p, ASSIGN))
    {
      cycle_token(p);
      free_stmt(s);
      return NULL;
    }
  //TODO: Skipping over expressions
  for (;p->cur_tok->type != SEMICOLON; cycle_token(p));
  cycle_token(p);

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

  //TODO: Skipping over expressions
  for (;p->cur_tok->type != SEMICOLON; cycle_token(p));
  cycle_token(p);
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
  
