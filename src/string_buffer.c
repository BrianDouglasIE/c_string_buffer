#include "string_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

static int ensure_capacity(StringBuffer *buf, size_t required) {
  if (required <= buf->capacity)
    return 1;

  size_t new_capacity = buf->capacity ? buf->capacity : INITIAL_CAPACITY;
  while (new_capacity < required) {
    new_capacity *= 2;
  }

  char *new_data = realloc(buf->data, new_capacity);
  if (!new_data) {
    perror("realloc");
    return 0;
  }

  buf->data = new_data;
  buf->capacity = new_capacity;
  return 1;
}

StringBuffer *StringBuffer_init() {
  StringBuffer *buf = malloc(sizeof(StringBuffer));
  if (!buf) {
    perror("malloc");
    return NULL;
  }

  buf->size = 0;
  buf->capacity = 0;
  buf->data = NULL;
  return buf;
}

void StringBuffer_free(StringBuffer *buf) {
  if (!buf)
    return;
  free(buf->data);
  free(buf);
}

void StringBuffer_clear(StringBuffer *buf) {
  if (!buf || !buf->data)
    return;
  buf->data[0] = '\0';
  buf->size = 0;
}

void StringBuffer_print(const StringBuffer *buf) {
  if (!buf || !buf->data)
    return;
  printf("size: %zu\n", buf->size);
  printf("data: %s\n", buf->data);
}

void StringBuffer_append(StringBuffer *buf, const char *text) {
  if (!buf || !text)
    return;

  size_t text_len = strlen(text);
  if (!ensure_capacity(buf, buf->size + text_len + 1))
    return;

  memcpy(buf->data + buf->size, text, text_len + 1);
  buf->size += text_len;
}

void StringBuffer_prepend(StringBuffer *buf, const char *text) {
  if (!buf || !text)
    return;

  size_t text_len = strlen(text);
  if (!ensure_capacity(buf, buf->size + text_len + 1))
    return;

  memmove(buf->data + text_len, buf->data,
          buf->size + 1);
  memcpy(buf->data, text, text_len);
  buf->size += text_len;
}

int StringBuffer_index_of(const StringBuffer *buf, const char *text,
                          size_t from) {
  if (!buf || !text || !buf->data || from >= buf->size)
    return -1;

  char *match = strstr(buf->data + from, text);
  return match ? (int)(match - buf->data) : -1;
}

MatchResult *StringBuffer_match_all(const StringBuffer *buf, const char *text,
                                    size_t from) {
  if (!buf || !text || !buf->data || from >= buf->size)
    return NULL;

  MatchResult *matches = malloc(sizeof(MatchResult));
  if (!matches)
    return NULL;
  matches->positions = NULL;
  matches->count = 0;

  int index = StringBuffer_index_of(buf, text, from);
  while (index != -1) {
    size_t *new_positions =
        realloc(matches->positions, sizeof(size_t) * (matches->count + 1));
    if (!new_positions) {
      MatchResult_free(matches);
      return NULL;
    }

    matches->positions = new_positions;
    matches->positions[matches->count++] = index;
    index = StringBuffer_index_of(buf, text, index + 1);
  }

  return matches;
}

void StringBuffer_remove(StringBuffer *buf, const char *text, size_t from) {
  if (!buf || !text || !buf->data || from >= buf->size)
    return;

  size_t text_len = strlen(text);
  int index = StringBuffer_index_of(buf, text, from);

  while (index != -1) {
    size_t tail_len = buf->size - (index + text_len);
    memmove(buf->data + index, buf->data + index + text_len, tail_len + 1);
    buf->size -= text_len;
    index = StringBuffer_index_of(buf, text, index);
  }
}

void StringBuffer_replace(StringBuffer *buf, const char *original,
                          const char *update, size_t from) {
  if (!buf || !original || !update || !buf->data || from >= buf->size)
    return;

  size_t original_len = strlen(original);
  size_t update_len = strlen(update);
  if (original_len == 0 || update_len == (size_t)-1)
    return;

  int index = StringBuffer_index_of(buf, original, from);

  while (index != -1) {
    if (update_len > original_len) {
      if (!ensure_capacity(buf, buf->size + (update_len - original_len) + 1))
        return;
    }

    size_t tail_len = buf->size - (index + original_len);
    memmove(buf->data + index + update_len, buf->data + index + original_len,
            tail_len + 1);
    memcpy(buf->data + index, update, update_len);
    buf->size = buf->size - original_len + update_len;

    index = StringBuffer_index_of(buf, original, index + update_len);
  }
}

SplitResult *StringBuffer_split(const StringBuffer *buf,
                                const char *delimiter) {
  if (!buf || !delimiter || !buf->data)
    return NULL;

  char *copy = strdup(buf->data);
  if (!copy)
    return NULL;

  SplitResult *result = malloc(sizeof(SplitResult));
  if (!result) {
    free(copy);
    return NULL;
  }

  result->parts = NULL;
  result->count = 0;

  char *token = strtok(copy, delimiter);
  while (token) {
    char **tmp = realloc(result->parts, sizeof(char *) * (result->count + 1));
    if (!tmp) {
      SplitResult_free(result);
      free(copy);
      return NULL;
    }
    result->parts = tmp;
    result->parts[result->count] = strdup(token);
    if (!result->parts[result->count]) {
      SplitResult_free(result);
      free(copy);
      return NULL;
    }
    result->count++;
    token = strtok(NULL, delimiter);
  }

  free(copy);
  return result;
}

void MatchResult_free(MatchResult *matches) {
  if (!matches)
    return;
  free(matches->positions);
  free(matches);
}

void SplitResult_free(SplitResult *split) {
  if (!split)
    return;
  for (size_t i = 0; i < split->count; i++) {
    free(split->parts[i]);
  }
  free(split->parts);
  free(split);
}
