#pragma once

#include "lexer.h"
#include <assert.h>

enum stmt_type
{
  LET_STMT,
};

enum expr_type
{
    INT_EXPR,
};

struct expr
{
  enum expr_type type;
  union
  {
    int integer;
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

struct elist
{
  struct enode *head;
};
  
struct parser
{
  struct lexer *l;
  struct token *cur_tok;
  struct token *peek_tok;
  //struct enode *elist;
  struct elist *elist;
};

struct enode *
get_node (enum token_type expt, enum token_type got)
{
#ifdef MEM_DEBUG
  MEM_COUNT++;
#endif
  struct enode *e = malloc(sizeof(struct enode));
  if (!e)
    return NULL;
  e->expect_tok = expt;
  e->got_tok = got;
  e->node = NULL;
  return e;
}

struct elist *
get_elist (void)
{
  struct elist *e = malloc(sizeof(struct elist));
  if (!e)
    return NULL;
  
  e->head = NULL;
  return e;
}

struct parser
get_parser (struct lexer *l)
{
  #ifdef MEM_DEBUG
  MEM_COUNT++; 
  #endif
  
  return (struct parser)
    {
      .l = l,
      .cur_tok = next_tok(l),
      .peek_tok = next_tok(l),
      .elist = get_elist(),
    };
}

void
cycle_token (struct parser *p)
{
  if (p->cur_tok)
    free_token(p->cur_tok);
  
  p->cur_tok = p->peek_tok;
  p->peek_tok = next_tok(p->l);
}

void
new_error (struct elist *elist, enum token_type expt, enum token_type got)
{
  printf("new eror\n");
  if (!elist->head)
    elist->head = get_node(expt, got);
  else
    {
      struct enode *cur = elist->head;
      while (cur->node)
	cur = cur->node;
      cur->node = get_node(expt, got);
    }
}

int
expect_peek (struct parser *p, enum token_type t)
{
  if (p->peek_tok->type == t)
    return 1;
  new_error(p->elist, t, p->peek_tok->type);
  return 0;
}

void
free_stmt (struct stmt *s)
{
  if (s->expr)
    free(s->expr);
  
  free(s->token.literal);
  free(s->ident.literal);
  free(s);
  #ifdef MEM_DEBUG
  MEM_COUNT--;
  #endif
}

struct stmt *
get_let_stmt (struct parser *p)
{
  struct stmt *s = malloc(sizeof(struct stmt));
  s->expr = NULL; // Have not implemented this yet
  
  if(!s)
    return NULL;

  cpy_token(&s->token, p->cur_tok);
  
  if (!expect_peek(p, IDENT))
    {
      cycle_token(p);
      free_stmt(s);
      return NULL;
    }

  cycle_token(p);
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

#ifdef MEM_DEBUG
  MEM_COUNT++;
#endif

  return s;
}

struct stmt *
get_stmt (struct parser *p)
{
  switch (p->cur_tok->type)
    {
    case LET: return get_let_stmt(p); break;
    default:
      return NULL;
    }
}

void
free_parser (struct parser *p)
{
  if (p->cur_tok)
    free_token(p->cur_tok);

  if (p->peek_tok)
    free_token(p->peek_tok);
  //free(p->elist);
  #ifdef MEM_DEBUG
  MEM_COUNT--;
  #endif
}

void
getfree_errors (struct elist *elist)
{
  struct enode *cur = elist->head;
  struct enode *node = cur;
  while (node)
    {
      node = cur->node;
      printf("parser error: expected token type %s got %s instead!\n", toktype_str[cur->expect_tok], toktype_str[cur->got_tok]);
      free(cur);
#ifdef MEM_DEBUG
      MEM_COUNT--;
#endif
      cur = node;
    }
  //  free(elist);
}
  
