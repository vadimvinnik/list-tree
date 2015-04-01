/*
   Vadim Vinnik, 2015, just for fun
*/

#include <assert.h>
#include <stdlib.h>
#include "list_tree.h"

struct _list_tree_node_t {
  void *data;
  list_tree_node_t *next;
  list_tree_node_t *first_child;
};

void*
list_tree_get_data(
    list_tree_node_t *node)
{
  return node->data;
}

list_tree_node_t*
list_tree_get_next(
    list_tree_node_t *node)
{
  return node->next;
}

list_tree_node_t*
list_tree_get_first_child(
    list_tree_node_t *node)
{
  return node->first_child;
}

list_tree_node_t*
list_tree_init(
    void *data)
{
  list_tree_node_t *node =
    (list_tree_node_t*) malloc(sizeof(list_tree_node_t));

  node->data = data;
  node->next = NULL;
  node->first_child = NULL;

  return node;
}

void
list_tree_prepend(
    list_tree_node_t **first,
    list_tree_node_t *singleton)
{
  assert(NULL != first);
  assert(NULL != singleton);
  assert(NULL == singleton->next);

  singleton->next = *first;
  *first = singleton;
}

void
list_tree_append(
    list_tree_node_t *last,
    list_tree_node_t *appendant)
{
  assert(NULL != last);
  assert(NULL == last->next);

  last->next = appendant;
}

list_tree_node_t*
list_tree_prepend_child(
    list_tree_node_t *parent,
    void *data)
{
  list_tree_node_t *new_child = list_tree_init(data);

  list_tree_prepend(
      &parent->first_child,
      new_child);

  return new_child;
}

traverse_status_t
list_tree_traverse_depth(
    list_tree_node_t *root,
    list_tree_visitor_t pre_visitor,
    list_tree_level_notifier_t descent,
    list_tree_level_notifier_t ascent,
    list_tree_visitor_t post_visitor,
    void *state)
{
  while (NULL != root)
  {
    traverse_status_t status;

    status = (NULL == pre_visitor)
      ? traverse_ok
      : pre_visitor(root, state);

    if (traverse_ok == status)
    {
      if (NULL != root->first_child)
      {
        status = (NULL == descent)
          ? traverse_ok
          : descent(state);

        if (traverse_ok == status)
        {
          status = list_tree_traverse_depth(
              root->first_child,
              pre_visitor,
              descent,
              ascent,
              post_visitor,
              state);

          if (traverse_ok == status)
          {
            status = (NULL == ascent)
              ? traverse_ok
              : ascent(state);
          }
        }
      }

      if (traverse_ok == status)
      {
        status = (NULL == post_visitor)
          ? traverse_ok
          : post_visitor(root, state);
      }
    }

    switch (status)
    {
      case traverse_ok:
      case traverse_skip_this:
        break;

      case traverse_skip_level:
        return traverse_ok;

      case traverse_break:
        return traverse_break;

      default:
        assert(0);
    }

    root = root->next;
  }

  return traverse_ok;
}

typedef struct _node_counter_state_t
{
  size_t count;
  traverse_status_t mode;
} node_counter_state_t;

static
traverse_status_t
list_tree_node_counter(
    list_tree_node_t *node,
    void *raw_state)
{
  assert(NULL != raw_state);

  node_counter_state_t *state =
    (node_counter_state_t*) raw_state;

  ++ state->count;

  return state->mode;
}

static
size_t
list_tree_count_nodes(
    list_tree_node_t *root,
    traverse_status_t mode)
{
  node_counter_state_t state = { 0, mode };

  traverse_status_t status = list_tree_traverse_depth(
      root,
      list_tree_node_counter,
      NULL,
      NULL,
      NULL,
      &state);

  assert(traverse_ok == status);

  return state.count;
}

size_t
list_tree_size(
    list_tree_node_t *root)
{
  return list_tree_count_nodes(root, traverse_ok);
}

size_t
list_tree_length(
    list_tree_node_t *root)
{
  return list_tree_count_nodes(root, traverse_skip_this);
}

typedef struct _depth_counter_t
{
  size_t current_depth;
  size_t max_depth;
} depth_counter_t;

static
traverse_status_t
list_tree_depth_tracker_on_enter(
    void *raw_state)
{
  assert(NULL != raw_state);

  depth_counter_t *state =
    (depth_counter_t*) raw_state;

  ++ state->current_depth;

  return traverse_ok;
}

static
traverse_status_t
list_tree_depth_tracker_on_leave(
    void *raw_state)
{
  assert(NULL != raw_state);

  depth_counter_t *state =
    (depth_counter_t*) raw_state;

  if (state->max_depth < state->current_depth)
    state->max_depth = state->current_depth;

  -- state->current_depth;

  return traverse_ok;
}

size_t
list_tree_depth(
    list_tree_node_t *root)
{
  depth_counter_t state = { 0, 0 };

  traverse_status_t status = list_tree_traverse_depth(
      root,
      NULL,
      list_tree_depth_tracker_on_enter,
      list_tree_depth_tracker_on_leave,
      NULL,
      &state);

  assert(traverse_ok == status);

  return state.max_depth;
}

void
flatten(
    list_tree_node_t *root)
{
}

