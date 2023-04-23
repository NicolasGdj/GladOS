//
// Created by cartignh on 07/06/2022.
//

#ifndef PROJET_SYS_PROCESS_H
#define PROJET_SYS_PROCESS_H

#include <stdint.h>
#include "../shared/queue.h"

// Maximum number of processes that can coexist at a given time
// Includes the kernel process (pid 0)
#define NB_PROC 999
// Stack (pile) size
#define MAX_NAME_SIZE 64

// More details at https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_sp%C3%A9cification#.C3.89tats_d.27un_processus
enum PROCESS_STATE {
    // Le processus est celui qui possède le processeur.
    ACTIVE,
    // Le processus n'attend que la possession du processeur pour s'exécuter.
    ACTIVABLE,
    // Le processus a exécuté une opération sur un sémaphore qui demande d'attendre pour progresser (par exemple wait).
    SEMAPHORE_WAIT,
    // Le processus attend qu'une entrée/sortie soit réalisée.
    IO_WAIT,
    // Le processus attend qu'un de ses processus fils soit terminé.
    CHILDREN_WAIT,
    // Le processus a appelé wait_clock, la primitive de mise en sommeil jusqu'à une heure donnée.
    SLEEPING,
    // Le processus a terminé son exécution ou a été terminé par l'appel système kill et son père est toujours vivant et n'a pas encore fait de waitpid sur lui.
    ZOMBIE
};

typedef struct process {

    uint32_t pid;
    char name[MAX_NAME_SIZE];
    enum PROCESS_STATE state;
    uint16_t priority;
    uint32_t registers_save_zone[6]; // Only 6 registers to save on x86 arch

    unsigned long kernel_stack_size; // in bytes
    uint32_t *kernel_stack;

    unsigned long user_stack_size; // in bytes
    uint32_t *user_stack;

    struct process *parent;

    link next_process; // Used by the scheduler queue
    link children;
    link next_process_child; // Used by the children list/queue
    link next_message_queue_process; // Used by a message queue to store the next process in the queue
    link next_sleeping_process;
    link next_dead_process; // Used by the dead process garbage collector

    int retval; // Used to store the process return value if needed on exit
    int waiting_for_child_pid; // Used to indicate which child the process wait (-1 to any child)
    uint32_t alarm; //Used by the waitclock function to be wake up at the right time
    int16_t message_queue_id;

} Process;

extern link activable_processes;
extern Process *active_process;

//List of all sleeping processes waiting for an alarm
extern link sleeping_processes;

// List of the processes to clean at next processes garbage collection execution
extern link dead_processes;

//extern uint16_t active_process;

// Assembly functions
void ctx_sw(uint32_t *old_ctx, uint32_t *new_ctx);
void switch_to_userland();

//void register_new_process(Process *p);

int start(int (*pt_func) (void *), unsigned long ssize, int prio, const char *name, void *arg);

void exit(int retval);

int kill(int pid);

void terminate();

int waitpid(int pid, int *retvalp);

int waitpid_nohang(int pid, int *retvalp);

int getprio(int pid);

int chprio(int pid, int newprio);

char* getname();

int getpid();

void sysinfo();

#endif //PROJET_SYS_PROCESS_H
