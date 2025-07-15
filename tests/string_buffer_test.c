#include "../src/string_buffer.h"

#include <assert.h>
#include <string.h>

int main(void) {
  StringBuffer *buf = StringBuffer_init();
  assert(buf != NULL);

  StringBuffer_append(buf, "hello");
  assert(buf->size == 5);
  assert(strcmp(buf->data, "hello") == 0);

  StringBuffer_append(buf, " world");
  assert(buf->size == 11);
  assert(strcmp(buf->data, "hello world") == 0);

  assert(StringBuffer_index_of(buf, "hello", 0) == 0);
  assert(StringBuffer_index_of(buf, "world", 0) == 6);
  assert(StringBuffer_index_of(buf, "mars", 0) == -1);
  assert(StringBuffer_index_of(buf, "llo", 2) == 2);
  assert(StringBuffer_index_of(buf, "hello", 99) == -1);
  assert(StringBuffer_index_of(buf, "hello", buf->size) == -1);
  assert(StringBuffer_index_of(buf, "d", buf->size - 1) ==
         (int)(buf->size - 1));
  assert(StringBuffer_index_of(buf, "h", 1) == -1);

  MatchResult *mr = StringBuffer_match_all(buf, "l", 0);
  if (!mr) {
    return -1;
  }
  assert(mr->count == 3);
  assert(mr->positions[0] == 2);
  assert(mr->positions[1] == 3);
  assert(mr->positions[2] == 9);
  MatchResult_free(mr);

  SplitResult *sr = StringBuffer_split(buf, " ");
  assert(sr->count == 2);
  assert(strcmp(sr->parts[0], "hello") == 0);
  assert(strcmp(sr->parts[1], "world") == 0);
  SplitResult_free(sr);

  StringBuffer_prepend(buf, "hello ");
  assert(buf->size == 17);
  assert(strcmp("hello hello world", buf->data) == 0);

  StringBuffer_remove(buf, " ", 0);
  assert(buf->size == 15);
  assert(strcmp("hellohelloworld", buf->data) == 0);

  StringBuffer_replace(buf, "hellohello", "hello ", 0);
  assert(buf->size == 11);
  assert(strcmp("hello world", buf->data) == 0);

  StringBuffer_clear(buf);
  assert(buf->size == 0);
  assert(strlen(buf->data) == 0);

  StringBuffer_free(buf);
  return 0;
}
