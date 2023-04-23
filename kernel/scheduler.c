//
// Created by cartignh on 09/06/2022.
//

#include "scheduler.h"
#include "process.h"
#include "clock.h"
#include <stdint.h>
#include "../shared/queue.h"
#include "user_stack_mem.h"
#include "mem.h"
#include <stdbool.h>

Process *process_table[NB_PROC] = { 0 };
uint16_t last_pid = NB_PROC-1;

LIST_HEAD(activable_processes);
Process *active_process;

//List of all sleeping processes waiting for an alarm
LIST_HEAD(sleeping_processes);

// List of the processes to clean at next processes garbage collection execution
LIST_HEAD(dead_processes);

// Return the first available pid
// Returns -1 if no free pid was found
int16_t get_free_pid() {
    // Try all pid starting from last_pid + 1 until we reach again last_pid (modulo)
    // This avoids starting from 0 each time since we try to allocate PIDs contiguously
    for (int i = 1; i <= NB_PROC; i++) {
        // The following line makes sure we iterate in range [1; NB_PROC],
        // avoiding 0 as it is reserved for the kernel
        uint16_t pid = (last_pid + i) % (NB_PROC);

        if (process_table[pid] == NULL) {
            // pid is free, so return it
            last_pid = pid;
            return pid;
        }
    }

    return -1;
}

void mark_current_process_as_sleeping(uint32_t clock) {
    active_process->state = SLEEPING;
    active_process->alarm = clock;

    queue_add(active_process, &sleeping_processes, Process, next_sleeping_process, priority);
}

// If pid < 0, means that any children terminating will resume execution
void mark_current_process_as_waiting_for_children(int pid) {
    active_process->state = CHILDREN_WAIT;
    active_process->waiting_for_child_pid = pid;

    // Add to queue to wake later
    //queue_add(active_process, &waiting_for_children_processes, Process, next_children_waiting_process, priority);
}

// Wakes the process `p`, telling it that children pid `child_pid` woke it
void wake_process(Process *p, int child_pid) {
    p->waiting_for_child_pid = child_pid;
    make_process_activable(p);
}

// Register the passed pid as dead in order for it to be cleaned later by
// the process garbage collector
void mark_process_as_dead(Process *process) {
    queue_add(process, &dead_processes, Process, next_dead_process, priority);
    /*process->pid += 0;
    return;*/
}

void mark_process_as_zombie(Process *p) {
    p->state = ZOMBIE;
}

// Run the process garbage collector
void process_garbage_collector() {
    if (queue_empty(&dead_processes)) return;

    while (!queue_empty(&dead_processes)) {
        Process *p = queue_out(&dead_processes, Process, next_dead_process);
        free_process(p);
    }
}

// Free immediately a process so cannot be called directly from a running process,
// use the process garbage collector for that
void free_process(Process *p) {
    // Remove process from process table
    process_table[p->pid] = 0;

    // Remove the process from its father children list
    queue_del(p, next_process_child);

    // Set process' children parent ref to null
    Process *children;
    queue_for_each(children, &p->children, Process, next_process_child) {
        children->parent = NULL;
        if(children->state == ZOMBIE)
            mark_process_as_dead(children);
    }

    process_garbage_collector();

    p->pid = -1;
    // Free its stack
    if (p->kernel_stack && p->kernel_stack_size)
        mem_free(p->kernel_stack, p->kernel_stack_size * sizeof(uint32_t));
    if (p->user_stack && p->user_stack_size)
        user_stack_free(p->user_stack, p->user_stack_size);

    // Then free the Process object
    mem_free(p, sizeof(Process));
}


void __check_sleeping_processes() {
    Process *process;
    while (true) {
        if (queue_empty(&sleeping_processes)) break;

        bool finished = true;

        queue_for_each(process, &sleeping_processes, Process, next_sleeping_process) {
            // Check alarm time
            if (process->alarm <= current_clock()) {
                //It is necessary to wake up the process (SLEEPING -> ACTIVABLE)
                queue_del(process, next_sleeping_process);
                make_process_activable(process);
                finished = false;
                break;
            }
        }

        if (finished) break;
    }
}

void schedule() {

    __check_sleeping_processes();

    // Choose the next process to activate
    Process *old_active = active_process;
    //  The last active become ACTIVABLE IF HE WAS ACTIVE
    if (old_active->state == ACTIVE) {
        make_process_activable(old_active);
    }

    Process *next_process = queue_out(&activable_processes, Process, next_process);
    if (next_process == NULL)
        return;
    active_process = next_process;
    //So the new active process become ACTIVE
    active_process->state = ACTIVE;

    if(old_active != next_process)
        ctx_sw(old_active->registers_save_zone, active_process->registers_save_zone);
}

void schedule_if_current_not_prioritary() {
    if ((queue_top(&activable_processes, Process, next_process))->priority > active_process->priority)
        schedule();
}

void context_switch_to(Process *p) {
    p->state = ACTIVE;

    make_process_activable(active_process);
    Process *old_active = active_process;
    active_process = p;

    ctx_sw(old_active->registers_save_zone, active_process->registers_save_zone);
}

void make_process_activable(Process *p) {
    p->state = ACTIVABLE;
    queue_add(p, &activable_processes, Process, next_process, priority);
}