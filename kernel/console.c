//
// Created by cartignh on 07/06/2022.
//

#include "../shared/console.h"
#include <stdint.h>
#include <stdbool.h>
#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include "time_driver.h"
#include "message_queue.h"
#include "mem.h"

int SCREEN_WIDTH = 80;
int SCREEN_HEIGHT = 25;

// Echo specifies whether we want to print letters when a key is pressed or not
bool echo = true;
uint32_t coul_texte = 15;
uint32_t coul_background_texte = 0;

// Number of characters from the beginning of the current line that cannot be erased
int prompt_lock = 0;

uint32_t font_color(uint32_t new) {
    uint32_t tmp = coul_texte;
    coul_texte = new;
    return tmp;
}

uint32_t font_background_color(uint32_t new) {
    uint32_t tmp = coul_background_texte;
    coul_background_texte = new;
    return tmp;
}

uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
    return (uint16_t*) (0xb8000 + 2 * (lig * SCREEN_WIDTH + col));
}

void write_char(uint32_t lig, uint32_t col, char c, uint32_t coul_texte, uint32_t coul_fond) {
     // Format : 7[0] | 6-4[bg_color] | 3-0[text_color]
     uint8_t char_format = 0 << 7 | (coul_fond & 0x7) << 4 | (coul_texte & 0xf);
     uint16_t char_data = char_format << 8 | c;

     (*ptr_mem(lig, col)) = char_data;
}

void clear_screen(void) {
    for (int y = 0; y < SCREEN_WIDTH; y++) {
        for (int x = 0; x < SCREEN_HEIGHT; x++)
            write_char(x, y, ' ', coul_texte, coul_background_texte);
    }
    move_cursor_to(1, 0);
    update_time_display();
}

unsigned short cur_port = 0x3d4;
unsigned short data_port = 0x3d5;

void move_cursor_to(uint32_t lig, uint32_t col) {
     int16_t cur_pos = col + lig * SCREEN_WIDTH;

     // 1) envoi de 0x0f pour indiquer que l'on va envoyer
     // la partie basse de la position du curseur
     outb(0x0f, cur_port);
     // 2) envoi de la partie basse
     outb(cur_pos & 0xff, data_port);
     // 3) envoie de 0x0e pour indiquer qu'on envoie maintenant
     // la partie haute
     outb(0x0e, cur_port);
     // 4) envoi de la partie haute sur le port de données
     outb(cur_pos >> 8, data_port);
 }

int16_t get_cursor_pos() {
    outb(0x0f, cur_port);
    uint8_t lower_byte = inb(data_port);
    outb(0x0e, cur_port);
    uint8_t higher_byte = inb(data_port);

    return higher_byte << 8 | lower_byte;
}

void scroll_screen(void) {
    // Set prompt_lock to 0
    prompt_lock = 0;

    // Move the lines [1-24] to [0-23]
    memmove((void*) 0xb8000, (void*) 0xb80a0, 0xf00);
    // Clear the line 24
    for (int x = 0; x < SCREEN_WIDTH; x++)
        write_char(24, x, ' ', 15, 0);

    // Update time display here since we scrolled everything
    update_time_display();
}

bool process_char(char c) {
    uint16_t cur_pos = get_cursor_pos();
    uint8_t lig = cur_pos / ((float) SCREEN_WIDTH);
    uint8_t col = cur_pos % SCREEN_WIDTH;
    bool wasCharPrinted = false;

    switch (c) {
        case 8: // \b
            if (col != 0)
                move_cursor_to(lig, col - 1);
            break;
        case 9: // \t
            if (col < SCREEN_WIDTH - 1) {
                col = col + 8 - (col % 8);
                if (col >= SCREEN_WIDTH) col = SCREEN_WIDTH - 1;
                move_cursor_to(lig, col);
            }
            break;
        case 10: // \n
            if (lig == 24) {
                scroll_screen();
                move_cursor_to(lig, 0);
            } else
                move_cursor_to(lig + 1, 0);
            break;
        // Non géré
        /*case 12: // \f
            efface_ecran();
            place_curseur(0, 0);
            break;*/
        case 13: // \r
            move_cursor_to(lig, 0);
            break;

        case 127: // backspace
            // Do not erase if we are at the beginning of the line
            // or prompt_lock has been reached
            if (col == 0 || col == prompt_lock) break;

            move_cursor_to(lig, --col);
            write_char(lig, col, ' ', 15, 0);

            break;

        default:
            // Only display a certain range of chars
            if (c >= 32 && c <= 126) {
                write_char(lig, col, c, coul_texte, coul_background_texte);
                wasCharPrinted = true;
            }
            break;
    }

    return wasCharPrinted;
}

void display_top_left(char *str) {
    // Save cursor pos
    uint16_t cur_pos = get_cursor_pos();
    uint8_t lig = cur_pos / 80;
    uint8_t col = cur_pos % 80;

    move_cursor_to(0, 0);
    printf("%s", str);

    // Then restore it
    move_cursor_to(lig, col);
}

void display_top_right(char *str) {
    // Save cursor pos
    uint16_t cur_pos = get_cursor_pos();
    uint8_t lig = cur_pos / 80;
    uint8_t col = cur_pos % 80;

    move_cursor_to(0, 80 - strlen(str));
    printf("%s", str);

    // Then restore it
    move_cursor_to(lig, col);
}

void clear_first_line() {
    // Save cursor pos
    uint16_t cur_pos = get_cursor_pos();
    uint8_t lig = cur_pos / 80;
    uint8_t col = cur_pos % 80;

    for (int i = 0; i < SCREEN_WIDTH; i++)
        write_char(0, i, ' ', coul_texte, coul_background_texte);

    // Then restore it
    move_cursor_to(lig, col);
}

// Displays the string str at the current cursor position
void console_putbytes(const char *str, int len) {
    for (int i = 0; i < len; i++) {
        bool wasCharPrinted = process_char(str[i]);
        uint16_t cur_pos = get_cursor_pos();
        uint8_t lig = cur_pos / ((float) SCREEN_WIDTH);
        uint8_t col = cur_pos % SCREEN_WIDTH;

        if (wasCharPrinted) {
            if (col == 79) {
                if (lig == 24)
                    scroll_screen();
                else
                    lig++;
                col = 0;
            } else
                col++;
        }

        move_cursor_to(lig, col);
    }
}

// input string buffer (contains strings separated by `\n`)
int input_buffer;

// Init the input buffer when kernel starts as it is a non-constant declaration
void init_input_buffer() {
    input_buffer = pcreate(INPUT_BUFFER_SIZE);
}

// Writes the `length` first characters from stdin in `string`
// Returns the effective number of read characters, in case chr 13 (enter) is sent before `length` has been reached
// Waits for chr 13 (enter) to be sent before returning
// The characters that were not read (if length is too small) will be read at next call
// Returns 0 is `length` is null
int cons_read(char *string, unsigned long length) {
    for (unsigned long i = 0; i < length;) {
        int input;
        preceive(input_buffer, &input);

        // `input` (int) is 4 bytes while char is only 1,
        // so keep the rightmost byte
        char c = (char) input;

        switch (c) {
            // Backspace
            case 127:
                if (i > 0)
                    string[--i] = 0;
                break;

            // If current char is 13 (enter), we stop here and return
            // the effective number of transmitted characters
            case 13:
                return i + 1;

            default:
                *(string + i++) = c;
        }
    }

    return length; 
}

// This is literally `console_putbytes()`
void cons_write(const char *str, long size) {
    console_putbytes(str, (int) size);
}

void cons_echo(int on) {
    echo = on != 0;
}

// Prints prompt and avoids erasing it later when printing backspaces
// The prompt should NOT contain any backspace
void print_prompt(char *prompt) {
    int prompt_size = (int) strlen(prompt);

    console_putbytes(prompt, prompt_size);

    prompt_lock = prompt_size;
}