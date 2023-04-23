//
// Created by cartignh on 08/06/2022.
//

#include "time_driver.h"
#include "clock.h"
#include <stdint.h>
#include "console.h"
#include <stdio.h>
#include <string.h>
#include "hardware_time.h"

const int HOUR_MS = 1000 * 60 * 60;
const int MINUTE_MS = 1000 * 60;
const int SECOND_MS = 1000;

void update_time_display() {
    // First, we clear the first line of the screen
    clear_first_line();

    uint32_t time_ms = (interruption_count / CLOCK_FREQ) * 1000;

    // Format time
    char time_str[255];
    uint32_t hours = time_ms / HOUR_MS;
    uint32_t minutes = (time_ms % HOUR_MS) / MINUTE_MS;
    uint32_t seconds = (time_ms % MINUTE_MS) / SECOND_MS;
    sprintf(
        time_str, "uptime: %.2lu:%.2lu:%.2lu",
        (unsigned long) hours, (unsigned long) minutes, (unsigned long) seconds
    );
    // Display time at the top right
    display_top_left(time_str);

    show_date();
}