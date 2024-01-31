#pragma once
#include <stdlib.h>

struct list
{
  void **list;
  int count;
  int size;
  int typesize;
};

struct list *
get_list (int typesize)
{
  struct list *l = malloc(sizeof(struct list));
  l->count = 0;
  l->size = 1;
  l->typesize = typesize;
  l->list = malloc(typesize * l->size);
  return l;
}

void
shrink_to_size (struct list *l, int size)
{
  l->list = realloc(l->list, l->typesize * size);
  l->size = size;
}

void
append_list (struct list *l, void *x)
{
  if ((l->count + 1) >= l->size)
    {
      l->size *= 2;
      l->list = realloc(l->list, l->typesize * l->size);
    }
  l->list[l->count++] = x;
}

void
free_list (struct list *l, void (*free_obj)(void *))
{
  for (int i = 0; i < l->count; i++)
    {
      if (l->list[i])
	free_obj(l->list[i]);
    }
  free(l->list);
  free(l);
}
