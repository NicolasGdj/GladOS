//
// Created by cartignh on 08/06/2022.
//

#include "clock.h"
#include <stdint.h>
#include "cpu.h"
#include "time_driver.h"
#include <stdio.h>
#include "process.h"
#include "scheduler.h"

uint32_t interruption_count = 0;

const uint32_t CLOCK_INTERVAL = QUARTZ_FREQ / CLOCK_FREQ;

void clock_init() {
    // 1. Tell the clock we are going to send the frequency setting
    // with 2 successive uint8 values on the 0x43 data port
    outb(0x34, 0x43);

    // 2. Send the 8 lower bits of the frequency setting on the 0x40 data port
    outb(CLOCK_INTERVAL % 256, 0x40);
    // 3. Send the 8 higher bits of the frequency setting on the 0x40 data port
    outb(CLOCK_INTERVAL / 256, 0x40);
}

void clock_interruption_handler() {
    interruption_count++;

    // Update time display on the screen each TIME_DRIVER_UPDATE_FREQ
    if (interruption_count % TIME_DRIVER_UPDATE_FREQ == 0)
        update_time_display();

    // Run the process garbage collector
    process_garbage_collector();

    // Run scheduler each SCHED_FREQ ticks
    if (interruption_count % SCHED_FREQ == 0)
        schedule();
}

// Returns the system quartz frequency in *quartz and the number
// of oscillations in *ticks between each interruption
void clock_settings(unsigned long *quartz, unsigned long *ticks) {
    *quartz = QUARTZ_FREQ;
    *ticks = CLOCK_INTERVAL;
}

// Returns the number of interruptions of the clock since the kernel start
unsigned long current_clock() {
    return interruption_count;
}

// Sleeps until the number of interruptions passed in parameter have been reached
void wait_clock(unsigned long clock) {
    // Check if we need to wait
    if (interruption_count < clock) {
        mark_current_process_as_sleeping(clock);
        schedule();
    }
}
// Sleeps in second
void sleep(unsigned long time) {
    wait_clock(current_clock() + time * CLOCK_FREQ);
}