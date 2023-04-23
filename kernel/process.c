//
// Created by cartignh on 07/06/2022.
//

#include "process.h"
#include <stdint.h>
#include <string.h>
#include "mem.h"
#include "scheduler.h"
#include "debug.h"
#include <stdbool.h>
#include "queue.h"
#include "message_queue.h"
#include "user_stack_mem.h"
#include "start.h"
#include "segment.h"

// Remove the process from the queues in which it is;
// to be called before going zombie
void remove_from_queues(Process *p) {
    switch (p->state) {
        case SLEEPING:
            queue_del(p, next_sleeping_process);
            break;

        case ACTIVABLE:
            queue_del(p, next_process);
            break;

        case IO_WAIT:
            // Process inside a message queue
            if (p->message_queue_id != -1) {
                queue_del(p, next_message_queue_process);
            }
            break;

        case ACTIVE:
            // DO NOTHING
            break;

        default:
            assert(0);
    }
}

void sysinfo() {
    printf("System Information - gladOS 1.0\n");
    printf("--------------------------\n");
    printf("Process: (Max: %d)\n", NB_PROC);
    for(size_t i = 1; i < NB_PROC; ++i) {
        Process * process = process_table[i];
        if(process) {
            char * state_name;
            switch (process->state) {
                case ZOMBIE:
                    state_name = "Zombie";
                    break;
                case ACTIVE:
                    state_name = "Active";
                    break;
                case ACTIVABLE:
                    state_name = "Activable";
                    break;
                case CHILDREN_WAIT:
                    state_name = "Children_wait";
                    break;
                case IO_WAIT:
                    state_name = "IO_Wait";
                    break;
                case SLEEPING:
                    state_name = "Sleeping";
                    break;
                case SEMAPHORE_WAIT:
                    state_name = "Semaphore_wait";
                    break;
                default:
                    state_name = "Unknown";
                    break;
            }
            printf(" %s, pid = %d, prio = %d, state = %s\n", process->name, process->pid, process->priority, state_name);
        }
    }
    printf("--------------------------\n");
    printf("Queue: (Max: %d)\n", NBQUEUE);
    for(size_t i = 0; i < NBQUEUE; ++i) {
        MessageQueue * mq = message_queues[i];
        if(mq) {
            printf(" %d, max = %d, message = %d\n", mq->id, mq->count, mq->message_size);
        }
    }
    printf("--------------------------\n");


}

/*
 * pt_func: pointer on function to launch the process
 * ssize: available bytes for the stack
 * prio: process priority
 * name: the process name, displayed in ps for instance
 * arg: argument passed to pt_func
 *
 * Returns: pid of created process
 *
 * Errors:
 *  -1: process not created: NB_PROC (max number of concurrent processes) reached;
 *  -2: priority too high (superior to MAX_PRIORITY) or too low (less than 0)
 *
 *  Note: priority 0 is reserved for the kernel idle loop
 */
int start(int (*pt_func)(void *), unsigned long ssize, int prio, const char *name, void *arg) {
    // Check that the priority value is valid
    if (prio < 0 || prio > MAX_PRIORITY) return -2;

    // Check if stack can be allocated
    if (ssize > UINT32_MAX - (32 * sizeof (uint32_t))) {
        return -1;
    }

    int16_t pid = get_free_pid();
    // Exit with error if no pid is available
    if (pid == -1) {
        return -1;
    }

    Process *process = (Process *) mem_alloc(sizeof(Process));
    strcpy(process->name, name);

    // Assign it a unique PID
    process->pid = pid;
    // Add ref to parent (current) process
    process->parent = active_process;

    uint32_t kernel_stack_base_size = 512; // 32 bits slots
    uint32_t kernel_stack_size = kernel_stack_base_size;
    uint32_t *kernel_stack_alloc = mem_alloc(kernel_stack_size * sizeof(uint32_t));
    if (kernel_stack_alloc == 0) {
        mem_free(process, sizeof(Process));
        return -1;
    }

    if(pid == 0) { //idle

        /* KERNEL STACK */
        /*
         |          | MAX
         +----------+  <- internal kernel_stack_size
         |   arg    |
         +----------+  <- ebp + 8
         |   exit   |
         +----------+  <- ebp + 4
         | pt_func  |
         +----------+  <- ssize, ebp
         | local va |
         +----------|
         | local va |
         +----------+
         |  ret val |
         +----------+
         |    ...   |
         +----------|  <- p->stack
         |          | 0
        */
        // Assign the allocated memory zone
        process->kernel_stack = (uint32_t *) kernel_stack_alloc;

        process->kernel_stack_size = kernel_stack_size;
        process->kernel_stack[kernel_stack_size - 1] = (uint32_t) arg;
        process->kernel_stack[kernel_stack_size - 2] = (uint32_t) terminate;
        process->kernel_stack[kernel_stack_size - 3] = (uint32_t) pt_func;

        // Make %esp (stack pointer) point just before pt_func
        // Reference the assembly userland launcher function
        process->registers_save_zone[0] = 0;
        process->registers_save_zone[1] = (uint32_t) &process->kernel_stack[kernel_stack_size - 3];
        process->registers_save_zone[2] = 0;
        process->registers_save_zone[3] = 0;
        process->registers_save_zone[4] = 0;
        // Store pt_func in TSS
        process->registers_save_zone[5] = (uint32_t) &process->kernel_stack[kernel_stack_size - 3];

        process->user_stack = 0;
        process->user_stack_size = 0;

    } else {

        uint32_t usr_stack_base_size = ssize;
        // On garde un peu de place pour les params supplémentaires de la stack
        uint32_t usr_stack_size = usr_stack_base_size + 32 * sizeof(uint32_t);
        uint32_t *usr_stack_alloc = user_stack_alloc(usr_stack_size);
        if(usr_stack_alloc == 0) {
            mem_free(kernel_stack_alloc, kernel_stack_size * sizeof(uint32_t));
            mem_free(process, sizeof(Process));
            return -1;
        }


        /* USER STACK */
        /*
         |          | MAX
         +----------+  <- internal user_stack_size
         |   arg    |
         +----------+  <- ebp + 4
         |   exit   |
         +----------+  <- usr_stack_base_size/4, ebp
         | local va |
         +----------|
         | local va |
         +----------+
         |  ret val |
         +----------+
         |    ...   |
         +----------|  <- p->stack
         |          | 0
        */

        uint32_t top = usr_stack_size / sizeof (uint32_t);
        process->user_stack = (uint32_t *) usr_stack_alloc;
        process->user_stack_size = usr_stack_size;
        process->user_stack[top - 2] = (uint32_t) _exit; //exit on user mode (write on user/crt0.S just after user_start)
        process->user_stack[top - 1] = (uint32_t) arg; //args

        /* KERNEL STACK */
        /*
         |          | MAX
         +----------+  <- internal kernel_stack_size
         |  start   | fonction assembleur permettant de démarrer votre processus en mode user par retour d'interruption
         +----------+   <- ebp + 20
         |   func   | pointeur de la fonction à lancer
         +----------+  <- ebp + 16
         |    CS    | 0x43 (user_mode)
         +----------+  <- ebp + 12
         |  EFLAGS  | 0x202 (user_mode)
         +----------|  <- ebp + 8
         | USR_STCK | adresse du haut de la pile utilisateur
         +----------+  <- ebp + 4
         |    DS    | 0x4b (user_mode)
         +----------+ <- p->kernel_stack_size_without_parameters
         |    ...   |1
         +----------| <- p->kernel_stack
         |          | 0
        */

        // Set %esp to the address of the top of the stack
        // Reserve 3 extra slots at the top of the stack for pt_func, exit and arg
        // (see https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_sp%C3%A9cification#start_-_D.C3.A9marrage_d.27un_nouveau_processus)
        process->kernel_stack = (uint32_t *) kernel_stack_alloc;
        // Store the stack size for future free
        process->kernel_stack_size = kernel_stack_size;
        process->kernel_stack[kernel_stack_size - 6] = (uint32_t) switch_to_userland;
        process->kernel_stack[kernel_stack_size - 5] = (uint32_t) pt_func;
        process->kernel_stack[kernel_stack_size - 4] = USER_CS;
        process->kernel_stack[kernel_stack_size - 3] = USER_EFLAGS;
        process->kernel_stack[kernel_stack_size - 2] = (uint32_t) &(process->user_stack[top - 2]);
        process->kernel_stack[kernel_stack_size - 1] = USER_DS;

        // Make %esp (stack pointer) point just before pt_func
        // Reference the assembly userland launcher function
        process->registers_save_zone[0] = 0;
        process->registers_save_zone[1] = (uint32_t) &(process->kernel_stack[kernel_stack_size - 6]);
        process->registers_save_zone[2] = 0;
        process->registers_save_zone[3] = 0;
        process->registers_save_zone[4] = 0;
        // Store pt_func in TSS
        process->registers_save_zone[5] = (uint32_t) &(process->kernel_stack[kernel_stack_size - 5]);
    }

    process->priority = prio;
    process->alarm = 0;
    process->message_queue_id = -1;

    // And to the process table
    process_table[pid] = process;

    // Init children processes list
    process->children = (link) LIST_HEAD_INIT(process->children);

    if(pid == 0) {
        process->state = ACTIVE;
        active_process = process;
        uint32_t r[6];
        ctx_sw(r, process->registers_save_zone);
    } else {
        // Add new process to parent (current process)'s children
        // The priority doesn't matter, we just use it because it is a chained list
        queue_add(process, &active_process->children, Process, next_process_child, priority);

        // Finally, add process to queue
        make_process_activable(process);

        //if prio is > to the one of the current process, make it ACTIVE
        if(prio > active_process->priority)
            schedule();
    }

    return pid;
}

void exit(int retval) {

    remove_from_queues(active_process);

    // Store the return value
    active_process->retval = retval;

    // The active process is forced to have a father so make it a zombie
    // so that the father can use `waitpid` on its child
    active_process->state = ZOMBIE;

    // The Process struct nor the stack cannot be freed here,
    // so we will delegate this job to the process garbage collector
    // If the parent process is the kernel (pid 0) or has no parent (also 0), then we can free it immediately
    // Else, we keep it as a zombie so that the parent can read its return value with `waitpid`
    if (active_process->parent == NULL || active_process->parent->pid == 0) {
        mark_process_as_dead(active_process);
    }
    if (active_process->parent != NULL && active_process->parent->state == CHILDREN_WAIT &&
        (active_process->parent->waiting_for_child_pid == -1 || active_process->parent->waiting_for_child_pid == getpid())) {
        wake_process(active_process->parent, getpid());
    }

    // Now context_switch away while everything burns! ;)
    schedule();

    // The `exit` function should not return (GCC requires it)
    while (1) assert(0);
}

// Returns 0 is process killed successfully
// Returns -1 if pid does not exist
int kill(int pid) {
    if (pid <= 0 || pid >= NB_PROC) return -1;

    //Check if we try to kill the current process (exit 0)
    if (pid == getpid()) {
        exit(0);
        assert(0);
    }

    Process *p = process_table[pid];
    if (p == 0 || p->state == ZOMBIE) return -1;

    remove_from_queues(p);

    // Set the return value to NULL
    p->retval = 0;

    mark_process_as_zombie(p);

    // If the parent process is waiting for this children to terminate, then we wake it!
    if (p->parent != NULL && p->parent->state == CHILDREN_WAIT &&
        (p->parent->waiting_for_child_pid == -1 || p->parent->waiting_for_child_pid == pid)) {
        wake_process(p->parent, pid);
    }

    // If the parent process doesn't exist or is a Zombie or has no parent (Kernel), the we can free it immediately
    if (p->parent == NULL || p->parent->pid == 0 || p->parent->state == ZOMBIE) {
        free_process(p);
    }

    return 0;
}

int __waitpid_child(Process* child, int *retvalp) {
    //Set the return value
    if (retvalp != NULL) {
        *retvalp = child->retval;
    }
    uint32_t pid = child->pid;

    //Free the child process
    free_process(child);

    //return the child pid
    return pid;
}

/*
 * Passing a pid < 0
 * Returns the pid that was waited for
 * Errors: returns -1 if process has no child
 */
int waitpid(int pid, int *retvalp) {
    //Check if the active process have at least one child
    if (queue_empty(&active_process->children))
        return -1;

    if (pid < 0) { // Wait until any child end
        Process *child;
        //Check if a child is already dead
        queue_for_each(child, &active_process->children, Process, next_process_child) {
            if (child->state == ZOMBIE) {
                return __waitpid_child(child, retvalp);
            }
        }
        mark_current_process_as_waiting_for_children(-1);
    } else { // Wait until child with pid end
        if (pid >= NB_PROC) return -1;

        Process *p = process_table[pid];
        if (p == 0) return -1;

        //Check if the pid is a child of the active process
        bool found = false;
        Process *child;
        queue_for_each(child, &active_process->children, Process, next_process_child) {
            if (child->pid == (uint32_t) pid) {
                found = true;
                break;
            }
        }
        if (!found) //The pid is not a child
            return -1;

        //Check if the child's process is already a Zombie (Kill or terminated)
        if (child->state == ZOMBIE) {
            return __waitpid_child(child, retvalp);
        }

        //Wait for a specific pid
        mark_current_process_as_waiting_for_children(pid);
    }
    schedule();

    Process *child = process_table[active_process->waiting_for_child_pid];
    return __waitpid_child(child, retvalp);
}

/*
 * Passing a pid < 0
 * Returns the pid that was waited for
 * Returns 0 immediately if the process is not terminate
 * Errors: returns -1 if process has no child
 */
int waitpid_nohang(int pid, int *retvalp) {
    //Check if the active process have at least one child
    if (queue_empty(&active_process->children))
        return -1;

    if (pid < 0) { // Wait until any child end
        Process *child;
        //Check if a child is already dead
        queue_for_each(child, &active_process->children, Process, next_process_child) {
            if (child->state == ZOMBIE) {
                return __waitpid_child(child, retvalp);
            }
        }
        return 0;
    }
    // Wait until child with pid end
    if (pid >= NB_PROC) return -1;

    Process *p = process_table[pid];
    if (p == 0) return -1;

    //Check if the pid is a child of the active process
    bool found = false;
    Process *child;
    queue_for_each(child, &active_process->children, Process, next_process_child) {
        if (child->pid == (uint32_t) pid) {
            found = true;
            break;
        }
    }
    if (!found) //The pid is not a child
        return -1;

    //Check if the child's process is already a Zombie (Kill or terminated)
    if (child->state == ZOMBIE) {
        return __waitpid_child(child, retvalp);
    }

    //Wait for a specific pid
    return 0;
}


// Returns priority if pid is valid,
// or -1 if pid is invalid
int getprio(int pid) {
    if (pid < 0 || pid >= NB_PROC) return -1;

    Process *p = process_table[pid];

    // Error if the pid is still free
    if (p == 0) return -1;

    return p->priority;
}

// Returns: the old priority of the process
// Error: returns -1 if pid is invalid or if newprio is not is the allowed range
int chprio(int pid, int newprio) {
    if (newprio <= 0 || newprio > MAX_PRIORITY)
        return -1;

    Process *p = process_table[pid];

    // Error if the pid is still free
    if (p == 0 || p->state==ZOMBIE) return -1;

    int oldprio = p->priority;
    p->priority = newprio;

    // Reorder the queue holding the process:
    // remove from queue and then add it again because we changed its priority
    switch (p->state) {
        case SLEEPING:
            queue_del(p, next_sleeping_process);
            queue_add(p, &sleeping_processes, Process, next_sleeping_process, priority);
            break;

        case ACTIVABLE:
            queue_del(p, next_process);
            queue_add(p, &activable_processes, Process, next_process, priority);
            break;

        case IO_WAIT:
            // Process inside a message queue
            if (p->message_queue_id != -1) {
                MessageQueue * queue = message_queues[p->message_queue_id];
                queue_del(p, next_message_queue_process);
                queue_add(p, &(queue->waiting_processes), Process, next_message_queue_process, priority);
            }
            break;

        case ACTIVE:
            // DO NOTHING
            break;

        default:
            assert(0);
    }

    // Context switch to another more prioritary process if the current one
    // is not the most important one anymore
    schedule_if_current_not_prioritary();

    return oldprio;
}

char *getname() {
    return active_process->name;
}

int getpid() {
    return (int) active_process->pid;
}

