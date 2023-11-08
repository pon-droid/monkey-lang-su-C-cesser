#include "list.h"
#include <assert.h>

//the S stands for string
struct shmap
{
  struct list *shlist;
  uint size;
};

struct shmap *
get_shmap (uint shmap_size)
{
  assert(shmap_size % 2 == 0);
  struct shmap *s = malloc(sizeof(struct shmap));
  s->size = shmap_size;
  s->shlist = get_list(sizeof(int));
  shrink_to_size(s->shlist, shmap_size);
  return s;
}

// Shamelessly taken from Algorithims Third Edition.
static int
hash (char *c, uint size)
{
  int h = 0;
  for (; *c != '\0'; c++)
    h = (127 * h + *c) % size;
  return h;
}

void
append_shmap (struct shmap *sh, char *c, void *val)
{
  uint index = hash(c, sh->size);
  assert(sh->shlist->list[index]);
  sh->shlist->list[index] = val;
}
 
