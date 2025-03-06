#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "arena.h"
#include "string_map.h"
#include "http_cookies.h"


void test_parse_empty_string(void) {
  arena_t arena = {0};
  arena_create(&arena, getpagesize());

  http_request_cookie_t map = {0};
  string_map_init_with_arena(&arena, &map, 2);

  const char* const string = "";
  int res = http_parse_request_cookies(NULL, &map, string);
  assert(res == 0);

  arena_free(&arena);
}

void test_parse_string_with_single_entry(void) {
  arena_t arena = {0};
  arena_create(&arena, getpagesize());

  http_request_cookie_t map = {0};
  string_map_init_with_arena(&arena, &map, 2);

  const char* const string = "key=value";
  int res = http_parse_request_cookies(&arena, &map, string);
  assert(res == 1);
  assert(strncmp(map.data[0].key, "key", 3) == 0);
  assert(strncmp(map.data[0].value, "value", 3) == 0);

  arena_free(&arena);
}

void test_parse_string_with_single_entry_with_no_value(void) {
  arena_t arena = {0};
  arena_create(&arena, getpagesize());

  http_request_cookie_t map = {0};
  string_map_init_with_arena(&arena, &map, 2);

  const char* const string = "key=";
  int res = http_parse_request_cookies(&arena, &map, string);
  assert(res == 1);
  assert(strncmp(map.data[0].key, "key", 3) == 0);
  assert(strncmp(map.data[0].value, "", 0) == 0);

  arena_free(&arena);
}

void test_parse_string_with_multiple_entries(void) {
  arena_t arena = {0};
  arena_create(&arena, getpagesize());

  http_request_cookie_t map = {0};
  string_map_init_with_arena(&arena, &map, 2);

  const char* const string = "key=value; key2=value2";
  int res = http_parse_request_cookies(&arena, &map, string);
  assert(res == 2);
  assert(strncmp(map.data[0].key, "key", 3) == 0);
  assert(strncmp(map.data[0].value, "value", 5) == 0);
  assert(strncmp(map.data[1].key, "key2", 4) == 0);
  assert(strncmp(map.data[1].value, "value2", 6) == 0);

  arena_free(&arena);
}

void test_parse_malformatted_string_missing_space(void) {
  arena_t arena = {0};
  arena_create(&arena, getpagesize());

  http_request_cookie_t map = {0};
  string_map_init_with_arena(&arena, &map, 2);

  const char* const string = "key=value;key2=value2";
  int res = http_parse_request_cookies(&arena, &map, string);
  assert(res == -1);
  assert(map.data[0].key == NULL);
  assert(map.data[0].value == NULL);

  arena_free(&arena);
}

int main(void) {
  printf("test http cookies\n");
  test_parse_empty_string();
  test_parse_string_with_single_entry();
  test_parse_string_with_single_entry_with_no_value();
  test_parse_string_with_multiple_entries();
  test_parse_malformatted_string_missing_space();
  printf("done.\n");
  return 0;
}
