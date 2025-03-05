#ifndef __AIL_HTTP_COOKIES_H_
#define __AIL_HTTP_COOKIES_H_ 1

#include <stdbool.h>
#include <time.h>

#include "arena.h"
#include "string_map.h"

typedef struct string_map_t http_request_cookie_t;

struct http_cookie_t {
  char *name;
  char *value;
  char *domain;
  char *path;
  char *same_site;
  struct tm expires;
  int max_age;
  bool secure;
  bool http_only;
};

int http_parse_request_cookies(arena_t* arena,
                               http_request_cookie_t* map,
                               const char* const text);

size_t http_cookie_to_string(char *buffer, size_t buffer_size, const struct http_cookie_t *cookie);

#endif // __AIL_HTTP_COOKIES_H_
