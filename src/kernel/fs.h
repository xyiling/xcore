#ifndef FS_H
#define FS_H

#include "../include/type.h"

void init_filesystem(void);
int32_t fs_open(const char *path, uint32_t flags);
int32_t fs_close(int32_t fd);
int32_t fs_read(int32_t fd, void *buf, uint32_t count);
int32_t fs_write(int32_t fd, const void *buf, uint32_t count);
int32_t fs_listdir(const char *path, char *buffer, uint32_t size);

#endif

