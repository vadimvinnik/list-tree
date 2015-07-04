/*
   Vadim Vinnik, 2015, just for fun
   vadim.vinnik@gmail.com
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
void
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
      NULL,
      NULL,
      list_tree_dispose_post_visitor,
      &state);
}

static
void
list_tree_traverse_subtree(
    list_tree_node_t *node,
    list_tree_enter_notifier_t enter,
    list_tree_leave_notifier_t leave,
    list_tree_pre_visitor_t pre_visitor,
    list_tree_enter_notifier_t descent,
    list_tree_leave_notifier_t ascent,
    list_tree_enter_notifier_t forward,
    list_tree_leave_notifier_t backward,
    list_tree_post_visitor_t post_visitor,
    void *state)
{
  if (NULL == node)
    return;

  if ((NULL == enter) || enter(state))
  {
    list_tree_traverse_depth(
        node,
        pre_visitor,
        descent,
        ascent,
        forward,
        backward,
        post_visitor,
        state);

    if (NULL != leave)
      leave(state);
  }
}

void
list_tree_traverse_depth(
    list_tree_node_t *root,
    list_tree_pre_visitor_t pre_visitor,
    list_tree_enter_notifier_t descent,
    list_tree_leave_notifier_t ascent,
    list_tree_enter_notifier_t forward,
    list_tree_leave_notifier_t backward,
    list_tree_post_visitor_t post_visitor,
    void *state)
{
  if (NULL == root)
    return;

  if ((NULL == pre_visitor) || pre_visitor(root, state))
  {
    list_tree_traverse_subtree(
        root->first_child,
        descent,
        ascent,
        pre_visitor,
        descent,
        ascent,
        forward,
        backward,
        post_visitor,
        state);

    list_tree_traverse_subtree(
        root->next,
        forward,
        backward,
        pre_visitor,
        descent,
        ascent,
        forward,
        backward,
        post_visitor,
        state);

    if (NULL != post_visitor)
      post_visitor(root, state);
  }
}

int
enter_false(
    void *_)
{
  return 0;
}

static
int
list_tree_node_counter(
    list_tree_node_t *_,
    void *raw_state)
{
  assert(NULL != raw_state);

  size_t *state =
    (size_t*) raw_state;

  ++ *state;

  return 1;
}

static
size_t
list_tree_count_nodes(
    list_tree_node_t *root,
    list_tree_enter_notifier_t descent)
{
  size_t count = 0;

  list_tree_traverse_depth(
      root,
      list_tree_node_counter,
      descent,
      NULL,
      NULL,
      NULL,
      NULL,
      &count);

  return count;
}

size_t
list_tree_size(
    list_tree_node_t *root)
{
  return list_tree_count_nodes(root, NULL);
}

size_t
list_tree_length(
    list_tree_node_t *root)
{
  return list_tree_count_nodes(root, enter_false);
}

typedef struct _depth_counter_t
{
  size_t current_depth;
  size_t max_depth;
} depth_counter_t;

static
int
list_tree_depth_descent(
    void *raw_state)
{
  assert(NULL != raw_state);

  depth_counter_t *state =
    (depth_counter_t*) raw_state;

  ++ state->current_depth;

  return 1;
}

static
void
list_tree_depth_ascent(
    void *raw_state)
{
  assert(NULL != raw_state);

  depth_counter_t *state =
    (depth_counter_t*) raw_state;

  if (state->max_depth < state->current_depth)
    state->max_depth = state->current_depth;

  -- state->current_depth;
}

size_t
list_tree_depth(
    list_tree_node_t *root)
{
  if (NULL == root)
    return 0;

  depth_counter_t state = { 0, 0 };

  list_tree_traverse_depth(
      root,
      NULL,
      list_tree_depth_descent,
      list_tree_depth_ascent,
      NULL,
      NULL,
      NULL,
      &state);

  return state.max_depth + 1;
}

typedef struct _find_state_t
{
  predicate_t predicate;
  void *predicate_param;
  list_tree_node_t *result;
} find_state_t;

static
int
list_tree_node_find_pre_visitor(
    list_tree_node_t *node,
    void *raw_state)
{
  assert(NULL != raw_state);

  find_state_t *state = (find_state_t*) raw_state;

  assert(NULL != state->predicate);

  if (NULL != state->result)
    return 0;

  int is_matching = state->predicate(
      node->data,
      state->predicate_param);

  if (is_matching)
    state->result = node;

  return !is_matching;
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
      list_tree_node_find_pre_visitor,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      &state);

  return state.result;
}

typedef struct _locate_state_t
{
  size_t const* path;
  size_t path_length;
  size_t count;
  list_tree_node_t *result;
} locate_state_t;

static
int
list_tree_node_locate_pre_visitor(
    list_tree_node_t* node,
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;

  assert(0 <= state->path_length);
  assert(NULL == state->result);
  assert(state->count <= *state->path);
  
  if (state->count == *state->path && 1 == state->path_length)
  {
    state->result = node;
    return 0;
  }
  
  return 1;
}

static
int
list_tree_node_locate_descent(
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;

  if (NULL != state->result || state->count != *state->path)
    return 0;
  
  assert(1 < state->path_length);

  state->count = 0;
  ++ state->path;
  -- state->path_length;

  return 1;
}

static
void
list_tree_node_locate_ascent(
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;

  assert(0 == state->count);
  
  ++ state->path_length;
  -- state->path;
  state->count = *state->path;
}

static
int
list_tree_node_locate_forward(
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;

  if (NULL != state->result || state->count >= *state->path)
    return 0;
  
  ++ state->count;

  return 1;
}

static
void
list_tree_node_locate_backward(
    void *raw_state)
{
  assert(NULL != raw_state);

  locate_state_t *state = (locate_state_t*) raw_state;

  assert(0 < state->count);
  
  -- state->count;
}

list_tree_node_t*
list_tree_locate(
    list_tree_node_t *root,
    size_t const* path,
    size_t path_length)
{
  locate_state_t state =
  {
    path,
    path_length,
    0,
    NULL
  };

  list_tree_traverse_depth(
      root,
      list_tree_node_locate_pre_visitor,
      list_tree_node_locate_descent,
      list_tree_node_locate_ascent,
      list_tree_node_locate_forward,
      list_tree_node_locate_backward,
      NULL,
      &state);

  assert(state.path_length == path_length);
  assert(state.path == path);
  assert(state.count == 0);

  return state.result;
}

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
int
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

  state->writer(
      state->output,
      list_tree_get_data(node));

  return 1;
}

static
int
write_descent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  if (NULL != state->opening_tag)
    fputs(state->opening_tag, state->output);

  ++ state->level;
  return 1;
}

static
void
write_ascent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  if (NULL != state->closing_tag)
    fputs(state->closing_tag, state->output);

  -- state->level;
}

void
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

  list_tree_traverse_depth(
      root,
      write_pre_visitor,
      write_descent,
      write_ascent,
      NULL,
      NULL,
      NULL,
      &state);
}

