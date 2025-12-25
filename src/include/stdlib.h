#ifndef STDLIB_H
#define STDLIB_H

#include "type.h"

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);

void halt(void);
void reboot(void);
void shutdown(void);

#endif

