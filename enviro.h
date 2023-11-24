#include "hashmap.h"

#define ENVIRO_SIZE (256)

#define shmap_k(X, Y) X->store->shlist->list[hash(Y, ENVIRO_SIZE)]

struct enviro
{
  struct shmap *store;
};

struct enviro *
get_enviro (void)
{
  struct enviro *e = malloc(sizeof(struct enviro));
  e->store = get_shmap(ENVIRO_SIZE);
  return e;
}

void
free_enviro (struct enviro *e)
{
  free_list(e->store->shlist, free_obj);
  free(e->store);
  free(e);
}
