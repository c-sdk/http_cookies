.DEFAULT_GOAL := run

CC?=clang

CFLAGS  = -Wall -Werror -std=c17
CFLAGS += -I.
CFLAGS += $(foreach X,$(shell ls ./deps), -I./deps/$(X))

SOURCES  = $(wildcard *.c)
SOURCES += $(foreach X,$(shell ls ./deps), $(wildcard deps/$(X)/*.c))
OBJECTS  = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJECTS) tests

tests: ./t/tests.c $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJECTS)

run: clean $(OBJECTS) tests
	./tests
