/*
   Each node has a link to the next node that makes a single-linked list.
   Except this, the node has a link to the child node.  Since that child,
   in its turn, is a head of a list, this makes a tree.

   Vadim Vinnik, 2015, just for fun
*/

#ifndef _LIST_TREE_H_
#define _LIST_TREE_H_

#include <stdlib.h>

typedef
  struct _list_tree_node_t
  list_tree_node_t;

typedef struct _path_item_t
{
  size_t index;
  struct _path_item_t const* prev;
} path_item_t;

typedef
  int
  (*node_generator_t)(
      path_item_t const* path,
      void *state,
      void **data);

/* See list_tree_traverse_depth for more detail */
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

/* Callback before and after visiting a subtree */
typedef
  traverse_status_t
  (*list_tree_visitor_t)(
      list_tree_node_t *node,
      void *state);

/*
   Callback on entering or exiting a subtree */
typedef
  traverse_status_t
  (*list_tree_level_notifier_t)(
      void *state);

/* Callback to dispose data stored in a node */
typedef
  void
  (*data_disposer_t)(
      void*);

typedef
  int
  (*predicate_t)(
      void const* data,
      void const* param);

/* Getters */
void*
list_tree_get_data(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_get_next(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_get_first_child(
    list_tree_node_t *node);

/* Constructors */
list_tree_node_t*
list_tree_make_singleton(
    void *data);

list_tree_node_t*
list_tree_make(
    void *data,
    list_tree_node_t *next,
    list_tree_node_t *first_child);

list_tree_node_t*
list_tree_generate(
    node_generator_t generator,
    void *state);

/* Modifiers */
void
list_tree_prepend(
    list_tree_node_t **first,
    list_tree_node_t *tree);

void
list_tree_append(
    list_tree_node_t *last,
    list_tree_node_t *appendant);

list_tree_node_t*
list_tree_prepend_child(
    list_tree_node_t *parent,
    list_tree_node_t *new_child);

/* Destructor */
void
list_tree_dispose(
    list_tree_node_t *root,
    data_disposer_t data_disposer);

/*
  Traverse a list-tree in depth-first order invoking call-backs,
  normally in the following order:
  - pre_visitor;
  - if the node has a first child:
    - descent;
    - traverse from the first child;
    - ascent;
  - post_visitor;
  - traverse from the next node, if any.

  Each callback returns a value of type traverse_status_t
  indicating the further mode of operation:
  
  traverse_ok
    continue with the next callback in the normal order;
  traverse_skip_this
    skip all remaining callbacks for this node;
  traverse_skip_level
    skip this level, return to the parent node (if any);
  traverse_break
    break the entire traversal process.

  Returns:
  - traverse_ok if all nodes were visited;
  - traverse_break if one of callbacks signaled break;
*/
traverse_status_t
list_tree_traverse_depth(
    list_tree_node_t *root,
    list_tree_visitor_t pre_visitor,
    list_tree_level_notifier_t descent,
    list_tree_level_notifier_t ascent,
    list_tree_visitor_t post_visitor,
    void *state);

/* Various functions */
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

#endif
