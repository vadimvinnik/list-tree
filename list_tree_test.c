/*
   Vadim Vinnik, 2015, just for fun
   vadim.vinnik@gmail.com
*/

#include <stdio.h>

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

int main()
{
  test_print();
  
  return 0;
}

