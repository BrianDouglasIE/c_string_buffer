#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_buffer.h"

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

void StringBuffer_append(StringBuffer *buf, char *text, size_t text_len) {
	if (!buf || !text || !text_len)
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

void StringBuffer_prepend(StringBuffer *buf, char *text, size_t text_len) {
	if(!buf || !text || !text_len) return;

	size_t new_size = buf->size + text_len + 1;
	buf->data = realloc(buf->data, new_size);
	if(!buf->data) {
		perror("realloc");
		return;
	}

	memcpy(buf->data + text_len, buf->data, buf->size);
	memcpy(buf->data, text, text_len);

	buf->size += text_len;
	buf->data[buf->size] = '\0';
}

int StringBuffer_match(StringBuffer *buf, char *text, size_t from) {
	if(!buf || !text || !buf->data) return -1;
	if(from >= buf->size) return -1;

	char *match = strstr(buf->data + from, text);
	if(match == NULL) {
		return -1;
	}

	return match - buf->data;
}

MatchResult *StringBuffer_match_all(StringBuffer *buf, char *text, size_t from) {
	if(!buf || !text || !buf->data) return NULL;
	if(from >= buf->size) return NULL;

	MatchResult *matches = malloc(sizeof(*matches));
	if(!matches) {
		perror("malloc");
		return NULL;
	}

	matches->count = 0;
	matches->positions = NULL;

	size_t current_index = from;
	while(current_index != -1) {
		current_index = StringBuffer_match(buf, text, current_index + 1);
		if(current_index == -1) break;

		matches->positions = realloc(matches->positions, sizeof(size_t) * matches->count + 1);
		if(!matches->positions) {
			perror("realloc");
			break;
		}
		matches->positions[matches->count] = current_index;
		matches->count++;
	}

	return matches;
}

void MatchResult_free(MatchResult *matches) {
	if(!matches) return;

	free(matches->positions);
	free(matches);
}

void SplitResult_free(SplitResult *split) {
	if(!split) return;
	for(size_t i = 0; i < split->count; i++) {
		free(split->parts[i]);
	}
	free(split->parts);
	free(split);
}

SplitResult *StringBuffer_split(StringBuffer *buf, char *delimiter) {
	if(!buf || !delimiter || !buf->data) return NULL;

	char *str_copy = strdup(buf->data);
	if(!str_copy) return NULL;

	SplitResult *result = malloc(sizeof(*result));
	if(!result) {
		free(str_copy);
		return NULL;
	}
	result->parts = NULL;
	result->count = 0;

	char *save_ptr;
	char *part;

	part = strtok_r(str_copy, delimiter, &save_ptr);
	while(part != NULL) {
		char **tmp = realloc(result->parts, (result->count + 1) * sizeof(char *));
		if(!tmp) {
			SplitResult_free(result);
			free(str_copy);
			return NULL;
		}

		result->parts = tmp;
		result->parts[result->count] = strdup(part);
		if(!result->parts[result->count]) {
			SplitResult_free(result);
			free(str_copy);
			return NULL;
		}
		
		result->count++;
		part = strtok_r(NULL, delimiter, &save_ptr);
	}

	char **tmp = realloc(result->parts, (result->count + 1) * sizeof(char *));
	if(!tmp) {
		SplitResult_free(result);
		free(str_copy);
		return NULL;
	}

	result->parts = tmp;
	result->parts[result->count] = NULL;

	free(str_copy);

	return result;
}

int main(void) {
	StringBuffer *buf = StringBuffer_init();
	assert(buf != NULL);

	StringBuffer_append(buf, "hello", 5);
	assert(buf->size == 5);
	assert(strcmp(buf->data, "hello") == 0);

	StringBuffer_append(buf, " world", 6);
	assert(buf->size == 11);
	assert(strcmp(buf->data, "hello world") == 0);

	assert(StringBuffer_match(buf, "hello", 0) == 0);
	assert(StringBuffer_match(buf, "world", 0) == 6);
	assert(StringBuffer_match(buf, "mars", 0) == -1);
	assert(StringBuffer_match(buf, "llo", 2) == 2);
	assert(StringBuffer_match(buf, "hello", 99) == -1);
	assert(StringBuffer_match(buf, "hello", buf->size) == -1);
	assert(StringBuffer_match(buf, "d", buf->size - 1) == buf->size - 1);
	assert(StringBuffer_match(buf, "h", 1) == -1);

	MatchResult *mr = StringBuffer_match_all(buf, "l", 0);
	if(!mr) {
		return -1;
	}
	assert(mr->count == 3);
	assert(mr->positions[0] == 2);
	assert(mr->positions[1] == 3);
	assert(mr->positions[2] == 9);

	SplitResult *sr = StringBuffer_split(buf, " ");
	assert(sr->count == 2);
	assert(strcmp(sr->parts[0], "hello") == 0);
	assert(strcmp(sr->parts[1], "world") == 0);

	StringBuffer_prepend(buf, "hello ", 6);
	assert(buf->size == 17);
	assert(strcmp("hello hello world", buf->data) == 0);

	SplitResult_free(sr);
	MatchResult_free(mr);
	StringBuffer_free(buf);
	return 0;
}
