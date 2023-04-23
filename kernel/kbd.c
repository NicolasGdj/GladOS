#include "kbd.h"
#include "../shared/console.h"
#include "cpu.h"
#include "stdio.h"
#include "message_queue.h"

void keyboard_handler() {
    // The char we need is on port 0x60
    unsigned char c = inb(0x60);

    do_scancode(c);
}

void keyboard_data(char *str) {
    // Guess length based on number of chars we can read before finding NULL
    for (int i = 0;; i++) {
        char c = str[i];
        if (c == 0) break;

        // Only print if echo is on
        if (echo) {
            if(c == '\r')
                console_putbytes("\n", 1);
            // Print char before sending it to the input buffer
                console_putbytes(str, 1);
        }

        int chars_in_buffer = 0;
        pcount(input_buffer, &chars_in_buffer);

        // Add char to input buffer if buffer is not full
        if (chars_in_buffer < INPUT_BUFFER_SIZE)
            psend(input_buffer, (int) c);
    }
}

/* * * *   Fancy stuff (and not useful yet) below   * * * */

/* KEYBOARD LED
void print_char_as_bits(unsigned char c) {
    for (int i = 0; i < 8; i++) {
        printf("%d", !!((c << i) & 0x80));
    }
    printf("\n");
}

void kbd_leds(unsigned char leds) {
    //printf("Got a new led state: ");
    print_char_as_bits(leds);


    */
/*while (inb(0x64) & 0x2);
    outb(0xed, 0x60);
    while (inb(0x64) & 0x2);
    outb(0x07, 0x60);*//*

    outb(0x03, 0x60);
}*/
