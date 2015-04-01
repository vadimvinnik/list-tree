CC = gcc
CPPFLAGS = -c -g -std=c99 -Wall -pedantic-errors
LDFLAGS =

EXECUTABLE = list_tree_test

SOURCES = \
	list_tree.c \
	list_tree_test.c \

DEPENDENCIES = $(SOURCES:.c=.d)
OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $< -o $@

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(DEPENDENCIES)

include $(DEPENDENCIES)

