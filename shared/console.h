#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/*
 * This is the function called by printf to send its output to the screen. You
 * have to implement it in the kernel and in the user program.
 */

#include <stdint.h>
#include <stdbool.h>

#define INPUT_BUFFER_SIZE 999

extern bool echo;

uint16_t *ptr_mem(uint32_t lig, uint32_t col);

void write_char(uint32_t lig, uint32_t col, char c, uint32_t coul_texte, uint32_t coul_fond);

void clear_screen(void);

void move_cursor_to(uint32_t lig, uint32_t col);

int16_t get_cursor_pos();

void scroll_screen(void);

bool process_char(char c);

void display_top_left(char *str);

void display_top_right(char *str);

void clear_first_line();

extern void console_putbytes(const char *s, int len);

extern int input_buffer;

void init_input_buffer();

extern int cons_read(char *string, unsigned long length);

extern void cons_write(const char *str, long size);

void cons_echo(int on);

void print_prompt(char *prompt);

uint32_t font_color(uint32_t new);
uint32_t font_background_color(uint32_t new);

#endif
