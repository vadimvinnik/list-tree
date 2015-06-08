#include "list_tree.h"
#include "list_tree_test_data_creator.h"

typedef struct _bound_t
{
  size_t length;
  size_t depth;
} bound_t;

static
int packed_int_generator(
    path_item_t const* path,
    void *raw_state,
    void **data)
{
  bound_t *state = (bound_t*) raw_state;

  if (path->index == state->length)
    return 0;

  size_t depth = 0;
  path_item_t const* current = path;
  while (NULL != current && depth != state->depth)
  {
    ++depth;
    current = current->prev;
  }

  if (depth == state->depth)
    return 0;

  long result = 0L;
  depth = 0;
  current = path;
  while (NULL != current)
  {
    result += (1 + current->index) << (4 * depth);
    ++depth;
    current = current->prev;
  }

  *data = (void*) result;
  return 1;
}

list_tree_node_t*
make_test_tree(
    size_t length,
    size_t depth)
{
  bound_t bound =
  {
    length,
    depth
  };

  return list_tree_generate(
      packed_int_generator,
      &bound);
}

