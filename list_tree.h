/*
   Each node has a link to the next node that makes a single-linked list.
   Except this, the node has a link to the child node.  Since that child,
   in its turn, is a head of a list, this makes a tree.

   Vadim Vinnik, 2015, just for fun
*/

#ifndef _LIST_TREE_H_
#define _LIST_TREE_H_

#include <stdlib.h>

/*
   The actual underlying data structure is hidden
*/
typedef
  struct _list_tree_node_t
  list_tree_node_t;

/*
   When returned from visiting callback functions, indicate the further
   mode of operation, see list_tree_traverse_depth for more detail
*/
enum _traverse_status_t
{
  traverse_ok,
  traverse_skip_this,
  traverse_skip_level,
  traverse_break
};

typedef
  enum _traverse_status_t
  traverse_status_t;

/*
   Type of the function to be called for each node
   before and after visiting a subtree
*/
typedef
  traverse_status_t
  (*list_tree_visitor_t)(
      list_tree_node_t *node,
      void *state);

/*
   Type of the function to be called when diving
   into or returning from a subtree
*/
typedef
  traverse_status_t
  (*list_tree_level_notifier_t)(
      void *state);

typedef
  int
  (*predicate_t)(
      void const* data,
      void const* param);

/*
   Abstraction from the underlying data structure
*/
void*
list_tree_get_data(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_get_next(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_get_first_child(
    list_tree_node_t *node);

/*
  Allocate a singleton list-tree holding the given data
*/
list_tree_node_t*
list_tree_init(
    void *data);

/*
  Prepend the singleton list-tree to the given list-tree
  and set the pointer to the new head
*/
void
list_tree_prepend(
    list_tree_node_t **first,
    list_tree_node_t *singleton);

/*
  Given the last node, append a list-tree to it
*/
void
list_tree_append(
    list_tree_node_t *last,
    list_tree_node_t *appendant);

/*
  Given a list-tree node, prepend a new child node
  holding the given data
*/
list_tree_node_t*
list_tree_prepend_child(
    list_tree_node_t *parent,
    void *data);

/*
  Traverse a list-tree in depth-first order applying call-backs
  for each node and on each level change.

  For each node, callbacks are applied normally in the following order:
  - pre_visitor;
  - if the node has children:
    - descent;
    - entire traversal starting from the first child;
    - ascent;
  - post_visitor;
  and then do the same with the next node.

  Each callback returns a value indicating the further mode of
  operation:
   - continue with the next callback in the normal order;
   - skip this node, i.e. skip all remaining callbacks for this node;
   - skip this level, i.e. return to the parent node (if any);
   - break the entire traversal process.
*/
traverse_status_t
list_tree_traverse_depth(
    list_tree_node_t *root,
    list_tree_visitor_t pre_visitor,
    list_tree_level_notifier_t descent,
    list_tree_level_notifier_t ascent,
    list_tree_visitor_t post_visitor,
    void *state);

size_t
list_tree_size(
    list_tree_node_t *root);

size_t
list_tree_length(
    list_tree_node_t *root);

size_t
list_tree_depth(
    list_tree_node_t *root);

list_tree_node_t*
list_tree_find(
    list_tree_node_t *root,
    predicate_t predicate,
    void *predicate_param);

list_tree_node_t*
list_tree_locate(
    list_tree_node_t *root,
    size_t const* path,
    size_t path_length);

void
list_tree_flatten(
    list_tree_node_t *root);

#endif
