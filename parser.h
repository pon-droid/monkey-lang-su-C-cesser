#pragma once

#include "lexer.h"

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
  
struct parser
{
  struct lexer *l;
  struct token *cur_tok;
  struct token *peek_tok;
};

struct parser
get_parser (struct lexer *l)
{
  return (struct parser)
    {
      .l = l,
      .cur_tok = next_tok(l),
      .peek_tok = next_tok(l),
    };
}

void
next_token (struct parser *p)
{
  //TODO: Fix memory leak
  //free_token(p->cur_tok);
  p->cur_tok = p->peek_tok;
  //free_token(p->peek_tok);
  p->peek_tok = next_tok(p->l);
}

int
expect_peek (struct parser *p, enum token_type t)
{
  if (p->peek_tok->type == t)
    return 1;
  else
    return 0;
}

struct stmt *
get_let_stmt (struct parser *p)
{
  struct stmt *s = malloc(sizeof(struct stmt));

  if(!s)
    return NULL;
  
  s->token = *p->cur_tok;

  if (!expect_peek(p, IDENT))
    return NULL;
  
  next_token(p);
  s->ident = *p->cur_tok;

  if (!expect_peek(p, ASSIGN))
    return NULL;
  
  //TODO: Skipping over expressions
  for (;p->cur_tok->type != SEMICOLON; next_token(p));
  next_token(p);

  return s;
}

struct stmt *
get_stmt (struct parser *p)
{
  switch (p->cur_tok->type)
    {
    case LET:
      return get_let_stmt(p);
      break;
    default:
      return NULL;
    }
}
