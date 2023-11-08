#include "hashmap.h"

#define ENVIRO_SIZE (256)

#define shmap_k(X, Y) X->shlist[hash(Y, X->size)]

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
//TODO: Stop ignoring the existence of circular dependencies
struct object;

void free_obj (struct object *);

void
free_enviro (struct enviro *e)
{
  free_list(e->store->shlist, free_obj);
  free(e->store);
  free(e);
}
