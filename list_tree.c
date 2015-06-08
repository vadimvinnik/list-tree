/*
   Vadim Vinnik, 2015, just for fun
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
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
list_tree_make_singleton(
    void *data)
{
  return list_tree_make(data, NULL, NULL);
}

list_tree_node_t*
list_tree_make(
    void *data,
    list_tree_node_t *next,
    list_tree_node_t *first_child)
{
  list_tree_node_t *node =
    (list_tree_node_t*) malloc(sizeof(list_tree_node_t));

  node->data = data;
  node->next = next;
  node->first_child = first_child;

  return node;
}

static
list_tree_node_t*
list_tree_generate_helper(
    node_generator_t generator,
    path_item_t const* path,
    void *state)
{
  assert(NULL != path);
  void *data;

  int is_existing = generator(path, state, &data);

  if (!is_existing)
    return NULL;

  path_item_t child_path =
  {
    0,
    path
  };

  list_tree_node_t *child = list_tree_generate_helper(
      generator,
      &child_path,
      state);

  path_item_t next_path =
  {
    path->index + 1,
    path->prev
  };

  list_tree_node_t *next = list_tree_generate_helper(
      generator,
      &next_path,
      state);

  return list_tree_make(
      data,
      next,
      child);
}

list_tree_node_t*
list_tree_generate(
    node_generator_t generator,
    void *state)
{
  path_item_t root =
  {
    0,
    NULL
  };

  return list_tree_generate_helper(
      generator,
      &root,
      state);
}

void
list_tree_prepend(
    list_tree_node_t **first,
    list_tree_node_t *tree)
{
  assert(NULL != first);
  assert(NULL != tree);
  assert(NULL == tree->next);

  tree->next = *first;
  *first = tree;
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
    list_tree_node_t *new_child)
{
  list_tree_prepend(
      &parent->first_child,
      new_child);

  return new_child;
}

typedef struct _dispose_state_t
{
  data_disposer_t disposer;
} dispose_state_t;

static
traverse_status_t
list_tree_dispose_post_visitor(
      list_tree_node_t *node,
      void *raw_state)
{
  assert(NULL != raw_state);

  dispose_state_t *state = (dispose_state_t*) raw_state;
  data_disposer_t disposer = state->disposer;
  
  if (NULL != disposer)
    disposer(node->data);

  free(node);

  return traverse_ok;
}
    
void
list_tree_dispose(
    list_tree_node_t *root,
    data_disposer_t data_disposer)
{
  dispose_state_t state = {data_disposer};

  list_tree_traverse_depth(
      root,
      NULL,
      NULL,
      NULL,
      list_tree_dispose_post_visitor,
      &state);
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

    list_tree_node_t * const next = root->next;

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

    root = next;
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

typedef struct _find_state_t
{
  predicate_t predicate;
  void *predicate_param;
  list_tree_node_t *result;
} find_state_t;

static
traverse_status_t
list_tree_node_finder(
    list_tree_node_t *node,
    void *raw_state)
{
  assert(NULL != raw_state);

  find_state_t *state = (find_state_t*) raw_state;

  assert(NULL != state->predicate);
  assert(NULL == state->result);

  int is_matching = state->predicate(
      node->data,
      state->predicate_param);

  if (is_matching)
    state->result = node;

  return is_matching
    ? traverse_break
    : traverse_ok;
}

list_tree_node_t*
list_tree_find(
    list_tree_node_t *root,
    predicate_t predicate,
    void *predicate_param)
{
  find_state_t state =
  {
    predicate,
    predicate_param,
    NULL
  };
  
  list_tree_traverse_depth(
      root,
      list_tree_node_finder,
      NULL,
      NULL,
      NULL,
      &state);

  return state.result;
}

typedef struct _locate_state_t
{
  size_t *path;
  size_t path_length;
  list_tree_node_t *result;
} locate_state_t;

static
traverse_status_t
list_tree_node_locate_pre_visitor(
    list_tree_node_t *node,
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;

  assert(0 < state->path_length);
  assert(NULL == state->result);
  
  if (0 == *state->path)
  {
    if (1 == state->path_length)
    {
      state->result = node;
      return traverse_break;
    }

    return traverse_ok;
  }
  
  -- *state->path;
  return traverse_skip_this;
}

static
traverse_status_t
list_tree_node_locate_descent(
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;
  
  assert(0 == *state->path);
  assert(1 < state->path_length);
  assert(NULL == state->result);

  -- state->path_length;
  ++ state->path;

  return traverse_ok;
}

static
traverse_status_t
list_tree_node_locate_ascent(
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;
  assert(NULL == state->result);

  return traverse_break;
}

static
traverse_status_t
list_tree_node_locate_post_visitor(
    list_tree_node_t *node,
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;
  assert(NULL == state->result);

  return traverse_break;
}

list_tree_node_t*
list_tree_locate(
    list_tree_node_t *root,
    size_t const* path,
    size_t path_length)
{
  size_t const path_size = sizeof(*path) * path_length;
  size_t *path_copy = (size_t*) malloc(path_size);

  locate_state_t state =
  {
    path_copy,
    path_length,
    NULL
  };

  memcpy(path_copy, path, path_size);

  list_tree_traverse_depth(
      root,
      list_tree_node_locate_pre_visitor,
      list_tree_node_locate_descent,
      list_tree_node_locate_ascent,
      list_tree_node_locate_post_visitor,
      &state);

#ifdef NDEBUG
  if (NULL != state.result)
  {
    size_t *current = path_copy;
    size_t * const end = path_copy + path_length;

    while (end != current)
    {
      assert(0 == *current);
      ++current;
    }
  }
#endif

  free(path_copy);

  return state.result;
}

void
list_tree_flatten(
    list_tree_node_t *root)
{
  // TODO
}

