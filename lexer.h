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

struct token *
new_token (enum token_type ttype, char *input)
{
  struct token *t = malloc(sizeof(struct token));
  
  if (!t)
    printf("Couldn't allocate memory for token %s", input);
  
  t->type = ttype;
  t->literal = malloc((sizeof (*input) / sizeof (input[0])) * sizeof (char));

  if(!t->literal)
    printf("Couldn't allocate memory for token %s", input);
  
  strcpy(t->literal, input);
  t->literal[sizeof(*input) / sizeof(input[0])] = '\0';
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
  int read_pos;
  int pos;
};

struct lexer 
get_lexer (const char *input)
{
  return (struct lexer)
    {
      .input = input,
      .read_pos = 1,
      .pos = 0,
    };
}

void
set_ident (struct token *t)
{
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

  t->type = IDENT;
}

struct token *
bad_token (char *ch)
{
    struct token *t = malloc(sizeof(struct token));
  
    if (!t)
      printf("Couldn't allocate memory for bad token\n");

    t->type = ILLEGAL;
    char *string = malloc(sizeof(char) * 2);
    t->literal = strcat(string, ch);
    return t;
}

int
is_abc(const char *ch)
{
  return ('a' <= *ch && *ch <= 'z') || ( 'A' <= *ch && *ch <= 'Z') || (*ch == '_');
}

int
is_123(const char *ch)
{
  return '0' <= *ch && *ch <= '9';
}

struct token *
get_ident (struct lexer *l)
{
  int i, j;
  for (i = (l->pos - 1), j = 0; is_abc(&l->input[i]); i++, j++);

  struct token *t = malloc(sizeof(struct token));

  
  if(!t)
    printf("Couldn't allocate memory for identifier token!\n");
  
  t->literal = malloc(j * sizeof(char));


  if(!t->literal)
    printf("Couldn't allocate memory for identifier token literal!\n");
  
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
    case '=': t = new_token(ASSIGN, &ch); break;
    case ';': t = new_token(SEMICOLON, &ch); break;
    case '(': t = new_token(LPAREN, &ch); break;
    case ')': t = new_token(RPAREN, &ch); break;
    case ',': t = new_token(COMMA, &ch); break;
    case '+': t = new_token(PLUS, &ch); break;
    case '{': t = new_token(LBRACE, &ch); break;
    case '}': t = new_token(RBRACE, &ch); break;
    default:
      if (is_abc(&ch))
	t = get_ident(l);
      else if (is_123(&ch))
	t = new_token(INT, &ch);
      else
	t = new_token(ILLEGAL, &ch);
    }
  return t;
}
