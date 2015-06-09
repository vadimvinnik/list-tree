/*
   Vadim Vinnik, 2015, just for fun
*/

#include <stdio.h>

#include "list_tree.h"
#include "list_tree_test_data_creator.h"

typedef
int (*data_writer_t)(
    FILE *output,
    void *data);

typedef struct _write_state_t
{
  int level;
  data_writer_t writer;
  FILE *output;
  char const* indent;
  char const* opening_tag;
  char const* closing_tag;
} write_state_t;

static
traverse_status_t
write_pre_visitor(
    list_tree_node_t *node,
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  if (NULL != state->indent)
  {
    for (int i = 0; i < state->level; ++i)
    {
      fputs(state->indent, state->output);
    }
  }

  int result = state->writer(
      state->output,
      list_tree_get_data(node));

  return result ? traverse_ok : traverse_break;
}

static
traverse_status_t
write_descent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  if (NULL != state->opening_tag)
    fputs(state->opening_tag, state->output);

  ++ state->level;
  return traverse_ok;
}

static
traverse_status_t
write_ascent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  if (NULL != state->closing_tag)
    fputs(state->closing_tag, state->output);

  -- state->level;
  return traverse_ok;
}

int
list_tree_write(
    list_tree_node_t *root,
    data_writer_t writer,
    FILE *output,
    char const* indent,
    char const* opening_tag,
    char const* closing_tag)
{
  write_state_t state =
  {
    0,
    writer,
    output,
    indent,
    opening_tag,
    closing_tag
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
    void *data)
{
  fprintf(
      output,
      "%4lX\n",
      (long)data);

  return 1;
}

void
list_tree_print(
    list_tree_node_t *root)
{
  list_tree_write(
      root,
      wrapped_int_writer,
      stdout,
      "\t",
      NULL,
      NULL);

}

int main()
{
  list_tree_node_t *tree = make_wrapped_int_tree(3, 4);

  list_tree_print(tree);

  return 0;
}

