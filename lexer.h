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
  return t;
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

struct token *
next_tok (struct lexer *l)
{
  struct token *t;
  char ch = l->input[l->pos++];

  switch (ch)
    {
    case '=': t = new_token(ASSIGN, "="); break;
    case ';': t = new_token(SEMICOLON, ";"); break;
    case '(': t = new_token(LPAREN, "("); break;
    case ')': t = new_token(RPAREN, ")"); break;
    case ',': t = new_token(COMMA, ","); break;
    case '+': t = new_token(PLUS, "+"); break;
    case '{': t = new_token(LBRACE, "{"); break;
    case '}': t = new_token(RBRACE, "}"); break;
    default: t = new_token(EOF, "");
    }
  return t;
}
