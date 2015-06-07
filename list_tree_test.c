/*
   Vadim Vinnik, 2015, just for fun
*/

#include <stdio.h>

#include "list_tree.h"

static
void*
wrap_int(int x)
{
  return (void*) (long) x;
}

static
int
unwrap_int(void* p)
{
  return (int) (long) p;
}

static
list_tree_node_t*
make_test_tree()
{
  return
    list_tree_make(
      wrap_int(0x1000),
      list_tree_make(
        wrap_int(0x2000),
        list_tree_make(
          wrap_int(0x3000),
          list_tree_make(
            wrap_int(0x4000),
            NULL,
            list_tree_make(
              wrap_int(0x4100),
              list_tree_make(
                wrap_int(0x4200),
                list_tree_make(
                  wrap_int(0x4300),
                  NULL,
                  list_tree_make(
                    wrap_int(0x4310),
                    list_tree_make(
                      wrap_int(0x4320),
                      list_tree_make(
                        wrap_int(0x4330),
                        NULL,
                        NULL
                      ),
                      NULL
                    ),
                    NULL
                  )
                ),
                list_tree_make(
                  wrap_int(0x4210),
                  list_tree_make(
                    wrap_int(0x4220),
                    list_tree_make(
                      wrap_int(0x4230),
                      NULL,
                      NULL
                    ),
                    NULL
                  ),
                  NULL
                )
              ),
              list_tree_make(
                wrap_int(0x4110),
                list_tree_make(
                  wrap_int(0x4120),
                  list_tree_make(
                    wrap_int(0x4130),
                    NULL,
                    NULL
                  ),
                  NULL
                ),
                NULL
              )
            )
          ),
          list_tree_make(
            wrap_int(0x3100),
            list_tree_make(
              wrap_int(0x3200),
              list_tree_make(
                wrap_int(0x3300),
                NULL,
                list_tree_make(
                  wrap_int(0x3310),
                  list_tree_make(
                    wrap_int(0x3320),
                    list_tree_make(
                      wrap_int(0x3330),
                      NULL,
                      NULL
                    ),
                    NULL
                  ),
                  NULL
                )
              ),
              list_tree_make(
                wrap_int(0x3210),
                list_tree_make(
                  wrap_int(0x3220),
                  list_tree_make(
                    wrap_int(0x3230),
                    NULL,
                    NULL
                  ),
                  NULL
                ),
                NULL
              )
            ),
            list_tree_make(
              wrap_int(0x3110),
              list_tree_make(
                wrap_int(0x3120),
                list_tree_make(
                  wrap_int(0x3130),
                  NULL,
                  NULL
                ),
                NULL
              ),
              NULL
            )
          )
        ),
        list_tree_make(
          wrap_int(0x2100),
          list_tree_make(
            wrap_int(0x2200),
            list_tree_make(
              wrap_int(0x2300),
              NULL,
              NULL
            ),
            NULL
          ),
          NULL
        )
      ),
      list_tree_make(
        wrap_int(0x1100),
        list_tree_make(
          wrap_int(0x1200),
          list_tree_make(
            wrap_int(0x1300),
            NULL,
            NULL
          ),
          NULL
        ),
        NULL
      )
    );
}

typedef
int (*data_writer_t)(
    FILE *output,
    void *data,
    int level);

typedef struct _write_state_t
{
  int level;
  data_writer_t writer;
  FILE *output;
  int indent;
} write_state_t;

static
traverse_status_t
write_pre_visitor(
    list_tree_node_t *node,
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;

  int result = state->writer(
      state->output,
      list_tree_get_data(node),
      state->level);

  return result ? traverse_ok : traverse_break;
}

static
traverse_status_t
write_descent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;
  ++ state->level;
  return traverse_ok;
}

static
traverse_status_t
write_ascent(
    void *raw_state)
{
  write_state_t *state = (write_state_t*) raw_state;
  -- state->level;
  return traverse_ok;
}

int
list_tree_write(
    list_tree_node_t *root,
    data_writer_t writer,
    FILE *output,
    int indent)
{
  write_state_t state =
  {
    0,
    writer,
    output,
    indent
  };

  traverse_status_t result = list_tree_traverse_depth(
      root,
      write_pre_visitor,
      write_descent,
      write_ascent,
      NULL,
      &state);

  return result == traverse_ok;
}

static
int
wrapped_int_writer(
    FILE *output,
    void *data,
    int level)
{
  for (int i = 0; i < level; ++i)
  {
    fputc('\t', output);
  }

  fprintf(
      output,
      "%04X\n",
      unwrap_int(data));

  return 1;
}

int main()
{
  list_tree_node_t *tree = make_test_tree();

  list_tree_write(
      tree,
      wrapped_int_writer,
      stdout,
      2);

  return 0;
}

