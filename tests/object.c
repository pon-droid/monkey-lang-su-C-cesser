#include "../object.h"
#include <assert.h>

struct test
{
  char* input;
  int output;
};

void
test_int (void)
{
  struct test tests [] =
    {
      {"5", 5},
      {"10", 10},
      {"4", 4},
      {"0", 0},
      {"2382938", 2382938},
      {"-5", -5},
      {"-10", -10},
      {"-9999", -9999},
      {"5 + 5 + 5 + 5 - 10", 10},
      {"2 * 2 * 2 * 2 * 2", 32},
      {"-50 + 100 + -50", 0},
      {"5 * 2 + 10", 20},
      {"5 + 2 * 10", 25},
      {"20 + 2 * -10", 0},
      {"50 / 2 * 2 + 10", 60},
      {"2 * (5 + 10)", 30},
      {"3 * 3 * 3 + 10", 37},
      {"3 * (3 * 3) + 10", 37},
      {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},     
    };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);

      //assert(s->expr->type == INT_EXPR);
      struct enviro *env = get_enviro();
      struct object *o = eval(s, env);
      free_enviro(env);
      assert(o->type == INT_OBJ);
      assert(o->integer == tests[i].output);
      
      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

void
test_bool (void)
{
  struct test tests [] =
    {
      {"true", 1},
      {"false", 0},
    };

  for (int i = 0; i < 2; i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);
      struct enviro *env = get_enviro();
      struct object *o = eval(s, env);
      free_enviro(env);
      assert(o->type == BOOL_OBJ);
      assert(o->bool == tests[i].output);

      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

void
test_prefix (void)
{
  struct test tests [] =
    {
      {"!true", 0},
      {"!false", 1},
      {"!5", 0},
      {"!!true", 1},
      {"!!false", 0},
      {"!!5", 1},
      {"true", 1},
      {"false", 0},
      {"1 < 2", 1},
      {"1 > 2", 0},
      {"1 < 1", 0},
      {"1 > 1", 0},
      {"1 == 1", 1},
      {"1 != 1", 0},
      {"1 == 2", 0},
      {"1 != 2", 1},
      {"true == true", 1},
      {"false == false", 1},
      {"true == false", 0},
      {"true != false", 1},
      {"false != true", 1},
      {"(1 < 2) == true", 1},
      {"(1 < 2) == false", 0},
      {"(1 > 2) == true", 0},
      {"(1 > 2) == false", 1},
    };
  
  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);
      //assert(s->expr->type == PREFIX_EXPR);
      struct enviro *env = get_enviro();
      struct object *o = eval(s, env);
      free_enviro(env);
      assert(o->type == BOOL_OBJ);
      assert(o->bool == tests[i].output);

      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

void
test_if_expr (void)
{
  struct test tests [] =
    {
      {"if (true) { 10 }", 10},
      {"if (1) { 10 }", 10},
      {"if (1 < 2) { 10 }", 10},
      {"if (1 > 2) { 10 } else { 20 }", 20},
      {"if (1 < 2) { 10 } else { 20 }", 10}, 
    };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i].input);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);
      struct enviro *env = get_enviro();
      struct object *o = eval(s, env);
      free_enviro(env);
      assert(o->integer == tests[i].output);

      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }
}

void
test_void_if (void)
{
  const char *tests [] =
    {
      "if (false) { 10 }"
      "if (1 > 2) { 10 }"
    };
  
  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct lexer l = get_lexer(tests[i]);
      struct parser p = get_parser(&l);
      struct stmt *s = get_stmt(&p);
      getfree_errors(&p.elist, 0);
      struct enviro *env = get_enviro();
      struct object *o = eval(s, env);
      free_enviro(env);
      assert(o->type == NULL_OBJ);
      assert(o == NULL_OBJECT);

      free_obj(o);
      free_stmt(s);
      free_parser(&p);
    }  
}

void
test_return (void)
{
    const struct test tests [] =
    {
      {"return 10;", 10},
      {"return 10; 9;", 10},
      {"return 2 * 5; 9;", 10},
      {"9; return 2 * 5; 9;", 10},
    };
  
  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct stmt_list *program = parse_program(tests[i].input);
      struct enviro *env = get_enviro();
      struct object *o = eval_stmt_list(program, env);
      free_enviro(env);
      assert(o->integer == 10);
      free_obj(o);
      free_stmt_list(program);
    }
}

void
test_return_if (void)
{
  const char *input =
    "if (10 > 1) {"
    "if (10 > 1) {"
    "return 10;"
    "}"
    "return 1;"
    "}";

  struct stmt_list *program = parse_program(input);
  struct enviro *env = get_enviro();
  struct object *o = eval_stmt_list(program, env);
  free_enviro(env);
  assert(o->integer == 10);
  free_obj(o);
  free_stmt_list(program);
}

struct str_test
{
  const char *input;
  const char *output;
};

void
test_err (void)
{
  const struct str_test tests [] =
    {
      {
	"5 + true;",
	"type mismatch: INTEGER + BOOLEAN",
      },
      {
	"5 + true; 5;",
	"type mismatch: INTEGER + BOOLEAN",
      },
      {
	"-true",
	"unknown operator: -BOOLEAN",
      },
      {
	"true + false;",
	"unknown operator: BOOLEAN + BOOLEAN",
      },
      {
	"5; true + false; 5",
	"unknown operator: BOOLEAN + BOOLEAN",
      },
      {
	"if (10 > 1) { true + false; }",
	"unknown operator: BOOLEAN + BOOLEAN",
      },
      {
	"if (10 > 1) {"
	"if (10 > 1) {"
	"return true + false;"
	"}"
	"return 1;"
	"}",
	"unknown operator: BOOLEAN + BOOLEAN",
      },
      {
	"foobar",
	"identifier not found: foobar",
      }
    };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct stmt_list *program = parse_program(tests[i].input);
      struct enviro *env = get_enviro();
      struct object *o = eval_stmt_list(program, env);
      free_enviro(env);
      assert(o->type == ERR_OBJ);
      assert(strcmp(o->err, tests[i].output) == 0);
      free_obj(o);
      free_stmt_list(program);
    }
}

void
test_bindings (void)
{
  struct str_int_cmp
  {
    const char *input;
    int output;
  };

  struct str_int_cmp tests [] =
    {
      //{"let a = 5; a;", 5},
      // {"let a = 5 * 5; a;", 25},
      //      {"let a = 5; let a = 5;", 5},
      {"let a = 5; let b = a; b;", 5},
      // {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };
  

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      struct enviro *env = get_enviro();  
      struct stmt_list *program = parse_program(tests[i].input);
      printf("work\n");
      struct object *o = eval_stmt_list(program, env);
      printf("%s\n", obj_str(o));
      assert(o->type == INT_OBJ);
      assert(o->integer == tests[i].output);

      free_enviro(env);
      free_obj(o);
      free_stmt_list(program);
      printf("index: %d\n", i);

    }


}

void
test_parser_q (void)
{
  struct str_cmp
  {
    const char *input;
  };

  const char *input =
    "let a = 5; let b = a; b;";

  struct stmt_list *program = parse_program(input);

  struct enviro *env = get_enviro();
  struct object *o = eval_stmt_list(program, env);
  //  printf("%s\n", obj_str(o));
  assert(o->type == INT_OBJ);
  assert(o->integer == 5);
  
  free_enviro(env);
  free_obj(o);
  free_stmt_list(program);
  
  
}

int
main (void)
{/*
  test_int();
  test_bool();
  test_prefix();
  test_if_expr();
  test_void_if();
  test_return();
  test_return_if();
  test_err();
 */
  //  test_bindings();
  test_parser_q();
  return 0;
}
