/*
   Vadim Vinnik, 2015, just for fun
   vadim.vinnik@gmail.com
*/

#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#include "list_tree.h"
#include "list_tree_test_data_creator.h"

static int const test_tree_length = 3;
static int const test_tree_depth = 4;

static
list_tree_node_t*
make_test_object()
{
  return make_wrapped_int_tree(
      test_tree_length,
      test_tree_depth);
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

static
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

static
void
test_print()
{
  list_tree_node_t *tree = make_test_object();

  list_tree_print(tree);

  list_tree_dispose(tree, NULL);
}

static
void
test_memory()
{
  int free_before_create = mallinfo().fordblks;

  list_tree_node_t *tree = make_test_object();

  int free_after_create = mallinfo().fordblks;

  assert(free_after_create < free_before_create);

  list_tree_dispose(tree, NULL);

  int free_after_dispose = mallinfo().fordblks;

  assert(free_before_create == free_after_dispose);
}

static
void
test_metrics()
{
  list_tree_node_t *tree = make_test_object();

  assert(list_tree_length(tree) == test_tree_length);
  assert(list_tree_depth(tree) == test_tree_depth);

  size_t expected_size = 0;
  size_t level_count = 1;
  for (int i = 0; i < test_tree_depth; ++i)
  {
    level_count *= test_tree_length;
    expected_size += level_count;
  }

  assert(list_tree_size(tree) == expected_size);

  list_tree_dispose(tree, NULL);
}

int main()
{
  test_print();
  test_memory();
  test_metrics();

  return 0;
}

