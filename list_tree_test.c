/*
   Vadim Vinnik, 2015, just for fun
*/

#include <stdio.h>

#include "list_tree.h"

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

static
list_tree_node_t*
make_test_tree(size_t length, size_t depth)
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

typedef
int (*data_writer_t)(
    FILE *output,
    void *data,
    int level);

typedef struct _write_state_t
{
  int level;
  data_writer_t writer;
  FILE *output;
  int indent;
} write_state_t;

static
traverse_status_t
write_pre_visitor(
    list_tree_node_t *node,
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  int result = state->writer(
      state->output,
      list_tree_get_data(node),
      state->level);

  return result ? traverse_ok : traverse_break;
}

static
traverse_status_t
write_descent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;
  ++ state->level;
  return traverse_ok;
}

static
traverse_status_t
write_ascent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;
  -- state->level;
  return traverse_ok;
}

int
list_tree_write(
    list_tree_node_t *root,
    data_writer_t writer,
    FILE *output,
    int indent)
{
  write_state_t state =
  {
    0,
    writer,
    output,
    indent
  };

  traverse_status_t result = list_tree_traverse_depth(
      root,
      write_pre_visitor,
      write_descent,
      write_ascent,
      NULL,
      &state);

  return result == traverse_ok;
}

static
int
wrapped_int_writer(
    FILE *output,
    void *data,
    int level)
{
  for (int i = 0; i < level; ++i)
  {
    fputc('\t', output);
  }

  fprintf(
      output,
      "%4lX\n",
      (long)data);

  return 1;
}

int main()
{
  list_tree_node_t *tree = make_test_tree(3, 4);

  list_tree_write(
      tree,
      wrapped_int_writer,
      stdout,
      2);

  return 0;
}

