#include "debugger.h"
#include "cpu.h"
#include "../shared/console.h"
#include <stdio.h>
#include <mem.h>
#include <process.h>
#include <scheduler.h>
#include <string.h>
#include <clock.h>
#include <interruptions.h>
#include "time_driver.h"
#include <stdbool.h>
#include "queue.h"
#include "start.h"

void start_idle_process() {
    Process *p_idle = (Process*) mem_alloc(sizeof(Process));
    strcpy(p_idle->name, "* kernel init *");
    p_idle->pid = 0;
    p_idle->state = ACTIVE;
    // Lowest priority possible since the kernel does nothing
    // (it works with interruptions)
    p_idle->priority = 0;
    p_idle->alarm = 0;
    p_idle->children = (link) LIST_HEAD_INIT(p_idle->children);

    active_process = p_idle;

    start(idle, 2048, 1, "idle", 0);
}

int idle() {
    start(user_start, 2048, 2, "user_start", 0);

    // Do nothing and let the scheduler handle the rest!
    while(true) {
        sti();
        hlt();
        cli();
    }
}

void kernel_start(void) {
    // Clock initialization
    printf("Initializing Clock...");
    clock_init();
    printf(" completed!\n");

    // Time driver initialization
    printf("Initializing Time driver...");
    update_time_display();
    printf(" completed!\n");

    // Initialize Interruption Service Routines (ISRs)
    printf("Initializing Interruptions...");
    interruptions_init();
    printf(" completed!\n");

    // Initialize input buffer
    printf("Initializing input buffer...");
    init_input_buffer();
    printf(" completed!\n");


    start_idle_process();
}