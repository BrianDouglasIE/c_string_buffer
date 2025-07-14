#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// StringBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  size_t size;
  char *data;
} StringBuffer;

StringBuffer *StringBuffer_init();
void StringBuffer_free(StringBuffer *buf);
void StringBuffer_append(StringBuffer *buf, char *text);
void StringBuffer_prepend(StringBuffer *buf, char *text);
void StringBuffer_remove(StringBuffer *buf, char *text, size_t from);
void StringBuffer_replace(StringBuffer *buf, char *original, char *update,
                          size_t from);
int StringBuffer_index_of(StringBuffer *buf, char *text, size_t from);
void StringBuffer_print(StringBuffer *buf);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MatchResult
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  size_t count;
  size_t *positions;
} MatchResult;

MatchResult *StringBuffer_match(StringBuffer *buf, char *text, size_t from);
void MatchResult_free(MatchResult *matches);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SplitResult
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  size_t count;
  char **parts;
} SplitResult;

SplitResult *StringBuffer_split(StringBuffer *buf, char *delimiter);
void SplitResult_free(SplitResult *split);

#endif
