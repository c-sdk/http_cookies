#include <stdio.h>
#include <string.h>

#include "http_cookies.h"

static void read_cookie_pair(arena_t* arena,
                             char* position, char* next,
                             char** key, char** value) {
  char* separator = strchr(next, '=');
  size_t key_length = separator - next + 1;
  size_t value_length = position - separator + 1;
  *key = arena_string_with_null(arena, next, key_length);
  *value = arena_string_with_null(arena, separator + 1, value_length);
}

// When parsing the cookies string,
// the user must trim left and right.
//
// CLIENT-SIZE:
// -----------
//
// cookie-header = "Cookie:" OWS cookie-string OWS
// cookie-string = cookie-pair *( ";" SP cookie-pair )
//
int http_parse_request_cookies(arena_t* arena,
                               http_request_cookie_t* map,
                               const char* const text) {
  char* position = (char*)text;
  char* next = (char*)text;
  char* key = NULL, * value = NULL;

  while ((position = strchr(next, ';')) != NULL) {
    read_cookie_pair(arena, position, next, &key, &value);
    string_map_add((struct string_map_t*)map, key, value);
    next = position + 1;
    // NOTE: it's expected an space after the `;`.
    if (*next++ != ' ') {
      goto malformatted;
    }
  }

  if (next != NULL && next[0] != 0) {
    read_cookie_pair(arena,
                     next + strlen(next) - 1,
                     next,
                     &key, &value);
    string_map_add((struct string_map_t*)map, key, value);
  }

  return map->count;

 malformatted:
  string_map_clean(map);

  return -1;
}

#define HTTP_COOKIE_STRING_FORMAT "%a, %d %b %Y %H:%M:%S GMT"

size_t http_cookie_to_string(char *buffer, size_t buffer_size, const struct http_cookie_t *cookie) {
  size_t wrote = 0;

  wrote += snprintf(buffer + wrote, buffer_size - wrote, "%s=%s", cookie->name, cookie->value);

  // TODO(dias): optmize later.
  char expires_str[40] = {0};
  strftime(expires_str, sizeof(expires_str), HTTP_COOKIE_STRING_FORMAT, &cookie->expires);
  wrote += snprintf(buffer + wrote, buffer_size - wrote, "; Expires=%s", expires_str);

  if (cookie->max_age >= 0) {
    wrote += snprintf(buffer + wrote, buffer_size - wrote, "; Max-Age=%d", cookie->max_age);
  }

  if (cookie->domain) {
    wrote += snprintf(buffer + wrote, buffer_size - wrote, "; Domain=%s", cookie->domain);
  }

  if (cookie->path) {
    wrote += snprintf(buffer + wrote, buffer_size - wrote, "; Path=%s", cookie->path);
  }

  if (cookie->secure) {
    wrote += snprintf(buffer + wrote, buffer_size - wrote, "; Secure");
  }

  if (cookie->http_only) {
    wrote += snprintf(buffer + wrote, buffer_size - wrote, "; HttpOnly");
  }

  if (cookie->same_site) {
    wrote += snprintf(buffer + wrote, buffer_size - wrote, "; SameSite=%s", cookie->same_site);
  }

  *(buffer + wrote++) = 0;
  
  return wrote;
}
