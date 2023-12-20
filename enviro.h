#include "hashmap.h"

#define ENVIRO_SIZE (256)

//#define shmap_k(X, Y) X->store->shlist->list[hash(Y, ENVIRO_SIZE)]

struct enviro
{
  struct shmap *store;
  struct enviro *outer;
};

struct enviro *
get_enviro (void)
{
  struct enviro *e = malloc(sizeof(struct enviro));
  e->store = get_shmap(ENVIRO_SIZE);
  e->outer = NULL;
  return e;
}

void
free_enviro (struct enviro *e)
{
  free_list(e->store->shlist, free_obj);
  free(e->store);
  free(e);
}

void *
get_env (struct enviro *e, const char *ident)
{
  int index = hash(ident, ENVIRO_SIZE);
  if (!e->outer)
    return e->store->shlist->list[index];
  else
    return e->outer;
}

void
set_env (struct enviro *e, const char *ident, struct object *o)
{
  int index = hash(ident, ENVIRO_SIZE);
  e->store->shlist->list[index] = o;
}
