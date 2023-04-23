//
// Created by cartignh on 09/06/2022.
//

#ifndef PROJET_SYS_SCHEDULER_H
#define PROJET_SYS_SCHEDULER_H

#include "../shared/queue.h"
#include "process.h"

// The maximum priority a process can get
#define MAX_PRIORITY 256

extern Process *process_table[NB_PROC];
extern uint16_t next_free_pid;

extern struct list_link activable_processes;
extern Process *active_process;

int16_t get_free_pid();

void mark_current_process_as_waiting_for_children(int pid);

void wake_process(Process *p, int child_pid);

void mark_process_as_zombie(Process *p);

void mark_process_as_dead(Process *process);

void mark_current_process_as_sleeping(uint32_t clock);

void process_garbage_collector();

void free_process(Process *p);

void schedule();

void schedule_if_current_not_prioritary();

char* my_name();

uint32_t my_pid();

void context_switch();

void context_switch_to(Process *p);

void make_process_activable(Process *p);

// start
// exit
// kill
// waitpid
// getprio
// chprio
// getpid

#endif //PROJET_SYS_SCHEDULER_H
