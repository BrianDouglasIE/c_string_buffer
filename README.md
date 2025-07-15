# StrinBuffer

```c
// string_buffer.h
#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>

/// @brief A dynamic string buffer that supports common string operations.
typedef struct {
  size_t size;     ///< The current size (length) of the string.
  size_t capacity; ///< The total allocated capacity of the buffer.
  char *data;      ///< Pointer to the character data.
} StringBuffer;

/// @brief Represents all positions where a match was found.
typedef struct {
  size_t *positions; ///< Array of match positions.
  size_t count;      ///< Number of matches found.
} MatchResult;

/// @brief Represents the result of splitting a string.
typedef struct {
  char **parts; ///< Array of substrings resulting from the split.
  size_t count; ///< Number of parts.
} SplitResult;

// Lifecycle

/// @brief Initializes a new, empty StringBuffer.
/// @return A pointer to the newly allocated StringBuffer.
StringBuffer *StringBuffer_init();

/// @brief Frees memory used by the given StringBuffer.
/// @param buf Pointer to the StringBuffer to be freed.
void StringBuffer_free(StringBuffer *buf);

/// @brief Clears the contents of the StringBuffer without freeing the object.
/// @param buf Pointer to the StringBuffer to be cleared.
void StringBuffer_clear(StringBuffer *buf);

// Operations

/// @brief Prints the contents of the StringBuffer to stdout.
/// @param buf Pointer to the StringBuffer to be printed.
void StringBuffer_print(const StringBuffer *buf);

/// @brief Appends text to the end of the StringBuffer.
/// @param buf Pointer to the StringBuffer.
/// @param text Null-terminated string to append.
void StringBuffer_append(StringBuffer *buf, const char *text);

/// @brief Prepends text to the beginning of the StringBuffer.
/// @param buf Pointer to the StringBuffer.
/// @param text Null-terminated string to prepend.
void StringBuffer_prepend(StringBuffer *buf, const char *text);

/// @brief Removes the first occurrence of text from the buffer starting from a given index.
/// @param buf Pointer to the StringBuffer.
/// @param text Null-terminated string to remove.
/// @param from Index to start the search from.
void StringBuffer_remove(StringBuffer *buf, const char *text, size_t from);

/// @brief Replaces the first occurrence of a substring with another string, starting from a given index.
/// @param buf Pointer to the StringBuffer.
/// @param original Substring to be replaced.
/// @param update Replacement string.
/// @param from Index to start the search from.
void StringBuffer_replace(StringBuffer *buf, const char *original,
                          const char *update, size_t from);

// Search & Split

/// @brief Finds the index of the first occurrence of text starting from a given index.
/// @param buf Pointer to the StringBuffer.
/// @param text Substring to search for.
/// @param from Index to start the search from.
/// @return The index of the first occurrence, or -1 if not found.
int StringBuffer_index_of(const StringBuffer *buf, const char *text,
                          size_t from);

/// @brief Finds all occurrences of a substring starting from a given index.
/// @param buf Pointer to the StringBuffer.
/// @param text Substring to match.
/// @param from Index to start the search from.
/// @return A pointer to a MatchResult containing all match positions.
MatchResult *StringBuffer_match_all(const StringBuffer *buf, const char *text,
                                    size_t from);

/// @brief Splits the buffer into parts using the given delimiter.
/// @param buf Pointer to the StringBuffer.
/// @param delimiter String delimiter to split by.
/// @return A pointer to a SplitResult containing the parts.
SplitResult *StringBuffer_split(const StringBuffer *buf, const char *delimiter);

// Cleanup

/// @brief Frees the memory used by a MatchResult.
/// @param matches Pointer to the MatchResult to free.
void MatchResult_free(MatchResult *matches);

/// @brief Frees the memory used by a SplitResult.
/// @param split Pointer to the SplitResult to free.
void SplitResult_free(SplitResult *split);

#endif
```
