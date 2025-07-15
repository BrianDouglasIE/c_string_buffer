#include "string_buffer.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StringBuffer *StringBuffer_init() {
  StringBuffer *buf = malloc(sizeof(*buf));
  if (!buf) {
    perror("malloc");
    return NULL;
  }
  buf->data = NULL;
  buf->size = 0;
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

void StringBuffer_print(StringBuffer *buf) {
  printf("size: %zu\ndata: %s\n", buf->size, buf->data);
}

void StringBuffer_append(StringBuffer *buf, char *text) {
  if (!buf || !text)
    return;

  size_t text_len = strlen(text);
  if (!text_len)
    return;

  size_t new_size = buf->size + text_len + 1;
  buf->data = realloc(buf->data, new_size);
  if (!buf->data) {
    perror("realloc");
    return;
  }
  memcpy(buf->data + buf->size, text, text_len);
  buf->size += text_len;
  buf->data[buf->size] = '\0';
}

void StringBuffer_prepend(StringBuffer *buf, char *text) {
  if (!buf || !text)
    return;

  size_t text_len = strlen(text);
  if (!text_len)
    return;

  size_t new_size = buf->size + text_len + 1;
  buf->data = realloc(buf->data, new_size);
  if (!buf->data) {
    perror("realloc");
    return;
  }

  memcpy(buf->data + text_len, buf->data, buf->size);
  memcpy(buf->data, text, text_len);

  buf->size += text_len;
  buf->data[buf->size] = '\0';
}

int StringBuffer_index_of(StringBuffer *buf, char *text, size_t from) {
  if (!buf || !text || !buf->data)
    return -1;
  if (from >= buf->size)
    return -1;

  char *match = strstr(buf->data + from, text);
  if (match == NULL) {
    return -1;
  }

  return match - buf->data;
}

MatchResult *StringBuffer_match_all(StringBuffer *buf, char *text,
                                    size_t from) {
  if (!buf || !text || !buf->data || from >= buf->size)
    return NULL;

  MatchResult *matches = malloc(sizeof(*matches));
  if (!matches) {
    perror("malloc");
    return NULL;
  }

  matches->count = 0;
  matches->positions = NULL;

  int current_index = StringBuffer_index_of(buf, text, from);
  while (current_index != -1) {
    matches->positions =
        realloc(matches->positions, sizeof(size_t) * (matches->count + 1));
    if (!matches->positions) {
      perror("realloc");
      break;
    }
    matches->positions[matches->count] = current_index;
    matches->count++;

    current_index = StringBuffer_index_of(buf, text, current_index + 1);
    if (current_index == -1)
      break;
  }

  return matches;
}

void StringBuffer_remove(StringBuffer *buf, char *text, size_t from) {
  if (!buf || !text || !buf->data || from >= buf->size)
    return;

  size_t text_len = strlen(text);
  if (!text_len)
    return;

  MatchResult *match = StringBuffer_match_all(buf, text, from);
  if (!match || match->count == 0) {
    MatchResult_free(match);
    return;
  }

  for (size_t i = match->count; i > 0; --i) {
    size_t pos = match->positions[i - 1];
    size_t remaining_bytes = buf->size - (pos + text_len);
    memmove(buf->data + pos, buf->data + pos + text_len, remaining_bytes + 1);
    buf->size -= text_len;
  }

  MatchResult_free(match);
}

void StringBuffer_replace(StringBuffer *buf, char *original, char *update,
                          size_t from) {
  if (!buf || !original || !update || !buf->data || from >= buf->size)
    return;

  size_t original_len = strlen(original);
  size_t update_len = strlen(update);
  if (!original_len || original_len > buf->size)
    return;

  MatchResult *match = StringBuffer_match_all(buf, original, from);
  if (!match || match->count == 0) {
    MatchResult_free(match);
    return;
  }

  size_t new_size = buf->size + (update_len - original_len) * match->count + 1;
  char *new_data = malloc(new_size);
  if (!new_data) {
    perror("malloc");
    MatchResult_free(match);
    return;
  }

  size_t src_i = 0;
  size_t dst_i = 0;
  size_t match_i = 0;

  while (src_i < buf->size) {
    if (match_i < match->count && src_i == match->positions[match_i]) {
      memcpy(new_data + dst_i, update, update_len);
      dst_i += update_len;
      src_i += original_len;
      match_i++;
    } else {
      new_data[dst_i++] = buf->data[src_i++];
    }
  }

  new_data[dst_i] = '\0';

  free(buf->data);
  buf->data = new_data;
  buf->size = dst_i;

  MatchResult_free(match);
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

SplitResult *StringBuffer_split(StringBuffer *buf, char *delimiter) {
  if (!buf || !delimiter || !buf->data)
    return NULL;

  char *str_copy = strdup(buf->data);
  if (!str_copy)
    return NULL;

  SplitResult *result = malloc(sizeof(*result));
  if (!result) {
    free(str_copy);
    return NULL;
  }
  result->parts = NULL;
  result->count = 0;

  char *part;

  part = strtok(str_copy, delimiter);
  while (part != NULL) {
    char **tmp = realloc(result->parts, (result->count + 1) * sizeof(char *));
    if (!tmp) {
      SplitResult_free(result);
      free(str_copy);
      return NULL;
    }

    result->parts = tmp;
    result->parts[result->count] = strdup(part);
    if (!result->parts[result->count]) {
      SplitResult_free(result);
      free(str_copy);
      return NULL;
    }

    result->count++;
    part = strtok(NULL, delimiter);
  }

  char **tmp = realloc(result->parts, (result->count + 1) * sizeof(char *));
  if (!tmp) {
    SplitResult_free(result);
    free(str_copy);
    return NULL;
  }

  result->parts = tmp;
  result->parts[result->count] = NULL;

  free(str_copy);

  return result;
}
