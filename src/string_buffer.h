#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>

typedef struct {
  size_t size;
  size_t capacity;
  char *data;
} StringBuffer;

typedef struct {
  size_t *positions;
  size_t count;
} MatchResult;

typedef struct {
  char **parts;
  size_t count;
} SplitResult;

// Lifecycle
StringBuffer *StringBuffer_init();
void StringBuffer_free(StringBuffer *buf);
void StringBuffer_clear(StringBuffer *buf);

// Operations
void StringBuffer_print(const StringBuffer *buf);
void StringBuffer_append(StringBuffer *buf, const char *text);
void StringBuffer_prepend(StringBuffer *buf, const char *text);
void StringBuffer_remove(StringBuffer *buf, const char *text, size_t from);
void StringBuffer_replace(StringBuffer *buf, const char *original,
                          const char *update, size_t from);

// Search & Split
int StringBuffer_index_of(const StringBuffer *buf, const char *text,
                          size_t from);
MatchResult *StringBuffer_match_all(const StringBuffer *buf, const char *text,
                                    size_t from);
SplitResult *StringBuffer_split(const StringBuffer *buf, const char *delimiter);

// Cleanup
void MatchResult_free(MatchResult *matches);
void SplitResult_free(SplitResult *split);

#endif
