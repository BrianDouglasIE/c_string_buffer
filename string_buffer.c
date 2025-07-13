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
	if(!buf || !text) return -1;
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
	if(!buf || !text) return NULL;
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
	printf("matches = %d, %d, %d \n", m->indices[0], m->indices[1], m->indices[2]);
	printf("match_count = %d \n", m->count);
	assert(m->count == 3);
	assert(m->indices[0] == 2);
	assert(m->indices[1] == 3);
	assert(m->indices[2] == 9);

	string_matches_free(m);
	string_buffer_free(buf);
	return 0;
}
