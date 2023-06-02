#ifndef MAIN_H
#define MAIN_H

#define UNUSED __attribute__((unused))
#define true 1
#define false 0
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int _getline(char **lineptr, size_t *n, int fd);

/* memory utilities */
void _memcpy(void *dest, void *src, size_t n);
void *_realloc(void *ptr, unsigned int old_size, unsigned int new_size);
void *_calloc(size_t count, size_t size);
void *_memchr(char *s, char c, int n);
void *_recalloc(void *ptr, unsigned int old_size, unsigned int new_size);


#endif
