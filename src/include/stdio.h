#ifndef STDIO_H
#define STDIO_H

#include "type.h"

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void putchar(char c);
void puts(const char *str);
void printf(const char *format, ...);
void clear_screen(void);
void set_cursor(int row, int col);
int get_cursor_row(void);
int get_cursor_col(void);
char getchar(void);
void keyboard_putchar(char c);

#endif

