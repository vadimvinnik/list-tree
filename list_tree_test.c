/*
   Vadim Vinnik, 2015, just for fun
   vadim.vinnik@gmail.com
*/

#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#include "list_tree.h"
#include "list_tree_test_data_creator.h"

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
  list_tree_node_t *tree = make_wrapped_int_tree(3, 4);

  list_tree_print(tree);

  list_tree_dispose(tree, NULL);
}

static
void
test_memory()
{
  int free_before_create = mallinfo().fordblks;

  list_tree_node_t *tree = make_wrapped_int_tree(3, 4);

  int free_after_create = mallinfo().fordblks;

  assert(free_after_create < free_before_create);

  list_tree_dispose(tree, NULL);

  int free_after_dispose = mallinfo().fordblks;

  assert(free_before_create == free_after_dispose);
}

int main()
{
  test_print();
  test_memory();

  return 0;
}

