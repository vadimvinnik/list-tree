#ifndef _LIST_TREE_H_
#define _LIST_TREE_H_

#include <stdlib.h>

typedef
  struct _list_tree_node_t
  list_tree_node_t;

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

typedef
  traverse_status_t
  (*list_tree_visitor_t)(
      list_tree_node_t *node,
      void *state);

typedef
  traverse_status_t
  (*list_tree_level_notifier_t)(
      void *state);

void*
list_tree_get_data(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_get_next(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_get_first_child(
    list_tree_node_t *node);

list_tree_node_t*
list_tree_init(
    void *data);

void
list_tree_prepend(
    list_tree_node_t **first,
    list_tree_node_t *singleton);

void
list_tree_append(
    list_tree_node_t *last,
    list_tree_node_t *appendant);

list_tree_node_t*
list_tree_prepend_child(
    list_tree_node_t *parent,
    void *data);

traverse_status_t
list_tree_traverse_depth(
    list_tree_node_t *root,
    list_tree_visitor_t pre_visitor,
    list_tree_visitor_t post_visitor,
    list_tree_level_notifier_t descent,
    list_tree_level_notifier_t ascent,
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

void
flatten(
    list_tree_node_t *root);

#endif
