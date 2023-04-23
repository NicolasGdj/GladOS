#include "hardware_time.h"
#include "cpu.h"
#include "stdio.h"
#include <stdint.h>
#include "../shared/console.h"

// France is UTC+2
#define UTC_OFFSET 2

// CMOS ports :
// - 0x70 (command)
unsigned short CMOS_RTC_COMMAND_PORT = 0x70;
// - 0x71 (data)
unsigned short CMOS_RTC_DATA_PORT = 0x71;

uint8_t read_CMOS(uint8_t reg) {
    // Send command
    outb(0x80 | reg, CMOS_RTC_COMMAND_PORT);
    // Read data
    uint8_t bcd_data = inb(CMOS_RTC_DATA_PORT);

    // Decode BCD
    // The port result is on 8 bits, so we decode the bits 4 by 4
    return (bcd_data >> 4) * 10 + (bcd_data & 15);
}

// Get interesting data from CMOS and display them
void show_date(void) {
    char str[255];
    sprintf(
            str, "%i:%i:%i (date: %i/%i/%i)",
            read_CMOS(4) + UTC_OFFSET, read_CMOS(2), read_CMOS(0),
            read_CMOS(7), read_CMOS(8), read_CMOS(9)
    );

    display_top_right(str);
}