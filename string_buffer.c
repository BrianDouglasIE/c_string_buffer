#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *value;
	size_t size;
} StringBuffer;

StringBuffer *string_buffer_init() {
	StringBuffer *buf = malloc(sizeof(*buf));
	if (!buf) {
		perror("malloc");
		return NULL;
	}
	buf->value = NULL;
	buf->size = 0;
	return buf;
}

void string_buffer_append(StringBuffer *buf, char *text, size_t text_len) {
	if (!buf || !text || !text_len)
		return;

	size_t new_size = buf->size + text_len + 1;
	buf->value = realloc(buf->value, new_size);
	if (!buf->value) {
		perror("realloc");
		return;
	}
	memcpy(buf->value + buf->size, text, text_len);
	buf->size += text_len;
	buf->value[buf->size] = '\0';
}

int string_buffer_index_of(StringBuffer *buf, char *text, int from) {
	if(!buf || !text || !buf->value) return -1;
	if(from >= buf->size) return -1;

	char *match = strstr(buf->value + from, text);
	if(match == NULL) {
		return -1;
	}

	return match - buf->value;
}

typedef struct {
	int count;
	int *indices;
} StringMatches;

StringMatches *string_buffer_matches(StringBuffer *buf, char *text, int from) {
	if(!buf || !text || !buf->value) return NULL;
	if(from >= buf->size) return NULL;

	StringMatches *matches = malloc(sizeof(*matches));
	if(!matches) {
		perror("malloc");
		return NULL;
	}

	matches->count = 0;
	matches->indices = NULL;

	int current_index = from;
	while(current_index != -1) {
		current_index = string_buffer_index_of(buf, text, current_index + 1);
		if(current_index == -1) break;

		matches->indices = realloc(matches->indices, sizeof(int) * matches->count + 1);
		if(!matches->indices) {
			perror("realloc");
			break;
		}
		matches->indices[matches->count] = current_index;
		matches->count++;
	}

	return matches;
}

void string_matches_free(StringMatches *matches) {
	if(!matches) return;

	free(matches->indices);
	free(matches);
}

typedef struct {
	int count;
	char **parts;
} StringSplit;

void string_split_free(StringSplit *split) {
	if(!split) return;
	for(int i = 0; i < split->count; i++) {
		free(split->parts[i]);
	}
	free(split->parts);
	free(split);
}

StringSplit *string_buffer_split(StringBuffer *buf, char *delimiter) {
	if(!buf || !delimiter || !buf->value) return NULL;

	char *str_copy = strdup(buf->value);
	if(!str_copy) return NULL;

	StringSplit *result = malloc(sizeof(*result));
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
			string_split_free(result);
			free(str_copy);
			return NULL;
		}

		result->parts = tmp;
		result->parts[result->count] = strdup(part);
		if(!result->parts[result->count]) {
			string_split_free(result);
			free(str_copy);
			return NULL;
		}
		
		result->count++;
		part = strtok_r(NULL, delimiter, &save_ptr);
	}

	char **tmp = realloc(result->parts, (result->count + 1) * sizeof(char *));
	if(!tmp) {
		string_split_free(result);
		free(str_copy);
		return NULL;
	}

	result->parts = tmp;
	result->parts[result->count] = NULL;

	free(str_copy);

	return result;
}

void string_buffer_free(StringBuffer *buf) {
	if (!buf)
		return;

	free(buf->value);
	free(buf);
}

int main(void) {
	StringBuffer *buf = string_buffer_init();
	assert(buf != NULL);

	string_buffer_append(buf, "hello", 5);
	assert(buf->size == 5);
	assert(strcmp(buf->value, "hello") == 0);

	string_buffer_append(buf, " world", 6);
	assert(buf->size == 11);
	assert(strcmp(buf->value, "hello world") == 0);

	assert(string_buffer_index_of(buf, "hello", 0) == 0);
	assert(string_buffer_index_of(buf, "world", 0) == 6);
	assert(string_buffer_index_of(buf, "mars", 0) == -1);
	assert(string_buffer_index_of(buf, "llo", 2) == 2);
	assert(string_buffer_index_of(buf, "hello", 99) == -1);
	assert(string_buffer_index_of(buf, "hello", buf->size) == -1);
	assert(string_buffer_index_of(buf, "d", buf->size - 1) == buf->size - 1);
	assert(string_buffer_index_of(buf, "h", 1) == -1);

	StringMatches *m = string_buffer_matches(buf, "l", 0);
	if(!m) {
		return -1;
	}
	assert(m->count == 3);
	assert(m->indices[0] == 2);
	assert(m->indices[1] == 3);
	assert(m->indices[2] == 9);

	StringSplit *words = string_buffer_split(buf, " ");
	assert(words->count == 2);
	assert(strcmp(words->parts[0], "hello") == 0);
	assert(strcmp(words->parts[1], "world") == 0);

	string_split_free(words);
	string_matches_free(m);
	string_buffer_free(buf);
	return 0;
}
