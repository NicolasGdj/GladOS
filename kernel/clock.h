//
// Created by cartignh on 08/06/2022.
//

#include <stdint.h>

#ifndef PROJET_SYS_CLOCK_H
#define PROJET_SYS_CLOCK_H

// Clock freqency, in hertz
// Value range: from 100 to 1000 hz
#define CLOCK_FREQ 100

// The i8253 controller emulates the quartz
#define QUARTZ_FREQ 0x1234DD

#define SCHED_FREQ 1

extern const uint32_t CLOCK_INTERVAL;

extern uint32_t interruption_count;

void clock_init();

void clock_interruption_handler();

void clock_settings(unsigned long *quartz, unsigned long *ticks);

unsigned long current_clock();

void wait_clock(unsigned long clock);

void sleep(unsigned long time);

#endif //PROJET_SYS_CLOCK_H
