/*
   Each node has a link to the next node that makes a linked list.
   The node has a link to a child node.  Since that child, in its
   turn, is a head of a list, this makes a tree.

   This data structure can also be described as a binary tree
   rotated over 45 degrees counterclockwise:

   root
   |
   *-----------*-----------*----...----*
   |           |           |           |          
   *---*---*   *---*---*   *---*...    *
   |   |   |   |   |   |   |   |
   *   *   *   *   *   *   *   *...

   The library provides API to create, destroy, get&set values
   from such data structures, as well as a high-level generic
   traversal algorithm and a number of service functions that
   are, by the way, specializations of the generic traversal.

   Vadim Vinnik, 2015, just for fun
   vadim.vinnik@gmail.com
*/

#ifndef _LIST_TREE_H_
#define _LIST_TREE_H_

#include <stdio.h>
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

/* Callback before visiting a subtree */
typedef
  int
  (*list_tree_pre_visitor_t)(
      list_tree_node_t *node,
      void *state);

/* Callback after visiting a subtree */
typedef
  void
  (*list_tree_post_visitor_t)(
      list_tree_node_t *node,
      void *state);

/* Callback on moving to a child or next item */
typedef
  int
  (*list_tree_enter_notifier_t)(
      void *state);

/* Callback on returning from a child or next item */
typedef
  void
  (*list_tree_leave_notifier_t)(
      void *state);

/* Callback to dispose data stored in a node */
typedef
  void
  (*data_disposer_t)(
      void*);

/* Callback to check whether node data meets a condition */
typedef
  int
  (*predicate_t)(
      void const* data,
      void const* param);

/* Callback to write node data to a file */
typedef
int (*data_writer_t)(
    FILE *output,
    void *data);

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
  Traverse a list-tree in depth-first order invoking user-
  specified call-backs, normally in the following order:
  - pre_visitor;
  - if the node has a first child:
    - descent;
    - traverse from the first child;
    - ascent;
  - if the node has a next node:
    - forward;
    - traverse from the next node;
    - backward;
  - post_visitor.

  Pre_visitor, descent and forward callbacks return a value
  indicating the further mode of operation:
    - true (non-0): continue with other callbacks;
    - false (0): skip all remaining callbacks for, respectively:
      - the current node including its child and next,
      - the child node,
      - the next node.

  Each of the 6 callbacks can be NULL. It is equivalent to
  passing a function that has no side effects and, in case
  of pre_visitor, descent and forward, always returns 1.

  Each callback has a state parameter that is a buffer with
  application-specific data. Callbacks are responsible for
  interpreting, processing and modifying the state correctly.
*/
void
list_tree_traverse_depth(
    list_tree_node_t *root,
    list_tree_pre_visitor_t pre_visitor,
    list_tree_enter_notifier_t descent,
    list_tree_leave_notifier_t ascent,
    list_tree_enter_notifier_t forward,
    list_tree_leave_notifier_t backward,
    list_tree_post_visitor_t post_visitor,
    void *state);

int
enter_false(
    void *);

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

/* Output */
void
list_tree_write(
    list_tree_node_t *root,
    data_writer_t writer,
    FILE *output,
    char const* indent,
    char const* opening_tag,
    char const* closing_tag);

#endif
