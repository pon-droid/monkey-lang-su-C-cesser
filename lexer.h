#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum token_type
{
  ILLEGAL,
  IDENT,
  INT,
  ASSIGN,
  PLUS,
  MINUS,
  COMMA,
  SEMICOLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  FUNCTION,
  LET,
  END_FILE,
  SLASH,
  ASTERISK,
  BANG,
  LT,
  GT,
  TRUE,
  FALSE,
  IF,
  ELSE,
  RETURN,
  EQ,
  NOT_EQ
};

struct token
{
  enum token_type type;
  char *literal;
};

static struct token *
new_token (enum token_type type, char *input)
{
  struct token *t = malloc(sizeof(struct token));
  
  if (!t)
    printf("Couldn't allocate memory for token %s\n", input);
  
  t->type = type;

  t->literal = input;

  if (!t->literal)
    printf("Couldn't allocate memory for token %s\n", input);

  
  return t;
}

void
free_token (struct token *t)
{
  free(t->literal);
  free(t);
}

struct lexer
{
  const char *input;
  int pos;
};

struct lexer 
get_lexer (const char *input)
{
  return (struct lexer)
    {
      .input = input,
      .pos = 0,
    };
}

static int
is_abc(const char *ch)
{
  return ('a' <= *ch && *ch <= 'z') || ( 'A' <= *ch && *ch <= 'Z') || (*ch == '_');
}

static int
is_123(const char *ch)
{
  return ('0' <= *ch && *ch <= '9');
}

static void
set_ident (struct token *t)
{
  if (is_123(t->literal))
    {
      t->type = INT;
      return;
    }
  // TODO: Get us a hashtable up in here
  if (!strcmp(t->literal, "let"))
    {
      t->type = LET;
      return;
    }
  if (!strcmp(t->literal, "fn"))
    {
      t->type = FUNCTION;
      return;
    }
  if (!strcmp(t->literal, "true"))
    {
      t->type = TRUE;
      return;
    }
  if (!strcmp(t->literal, "false"))
    {
      t->type = FALSE;
      return;
    }
  if (!strcmp(t->literal, "if"))
    {
      t->type = IF;
      return;
    }
  if (!strcmp(t->literal, "else"))
    {
      t->type = ELSE;
      return;
    }
  if (!strcmp(t->literal, "return"))
    {
      t->type = RETURN;
      return;
    }    
  t->type = IDENT;
}

static struct token *
get_ident (struct lexer *l, int (*check)(const char *))
{
  int i, j;
  for (i = (l->pos - 1), j = 0; (*check)(&l->input[i]); i++, j++);

  struct token *t = malloc(sizeof(struct token));

  if (!t)
    printf("Couldn't allocate memory for USERDEF token!\n");
  
  t->literal = malloc(j * sizeof(char));


  if (!t->literal)
    printf("Couldn't allocate memory for USERDEF token literal!\n");
  
  strncpy(t->literal, l->input + l->pos - 1, j);
  
  t->literal[j] = '\0';
  l->pos = i;
  set_ident(t);
  return t;
}

struct token *
next_tok (struct lexer *l)
{
  struct token *t;
  char ch = l->input[l->pos++];

  while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
    ch = l->input[l->pos++];
  
  switch (ch)
    {
      //    case '=': t = new_token(ASSIGN, &ch); break;
    case '=': 
      if (l->input[l->pos] == '=')
	{
	  t = new_token(EQ, strdup("=="));
	  l->pos++;
	}
      else
	t = new_token(ASSIGN, strdup("="));
      break;
    case ';': t = new_token(SEMICOLON, strdup(";")); break;
    case '(': t = new_token(LPAREN, strdup("(")); break;
    case ')': t = new_token(RPAREN, strdup(")")); break;
    case ',': t = new_token(COMMA, strdup(",")); break;
    case '+': t = new_token(PLUS, strdup("+")); break;
    case '{': t = new_token(LBRACE, strdup("{")); break;
    case '}': t = new_token(RBRACE, strdup("}")); break;
    case '-': t = new_token(MINUS, strdup("-")); break;
    case '!':
      if (l->input[l->pos] == '=')
	{
	  t = new_token(NOT_EQ, strdup("!="));
	  l->pos++;
	}
      else
	t = new_token(BANG, strdup("!"));
      break;
    case '/': t = new_token(SLASH, strdup("/")); break;
    case '*': t = new_token(ASTERISK, strdup("*")); break;
    case '<': t = new_token(LT, strdup("<")); break;
    case '>': t = new_token(GT, strdup(">")); break;
    case 0:   t = new_token(EOF, strdup("")); break;
    default:
      if (is_abc(&ch))
	t = get_ident(l, &is_abc);
      else if (is_123(&ch))
	t = get_ident(l, &is_123);
      else
	t = new_token(ILLEGAL, &ch);
    }
  return t;
}
