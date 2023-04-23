//
// Created by cartignh/guerroun on 09/02/2022.
//


#include "chainedlist.h"
#include "../mem.h"
#include "../../shared/debug.h"

struct JobList * createJobList() {
    struct JobList * list = mem_alloc(sizeof(struct JobList));
    list->head = NULL;
    list->size = 0;
    return list;
}

void addJobToJobList(struct JobList * list, int pid, Command *cmd) {
    struct JobNode * node = mem_alloc(sizeof (struct JobNode));
    node->next = list->head;
    node->pid = pid;
    node->cmd = cmd;
    list->head = node;
    ++list->size;
}

void removeNodeJobFromJobList(struct JobList * list, struct JobNode * last, struct JobNode * node) {
    if(last == NULL) {
        list->head = node->next;
    } else {
        last->next = node->next;
    }
    mem_free(node, sizeof (struct JobNode));
    --list->size;
}


bool removeJobFromJobList(struct JobList * list, int pid) {
    struct JobNode * node = list->head;
    struct JobNode * last = NULL;
    while(node) {

        if(node->pid == pid) {
            removeNodeJobFromJobList(list, last, node);
            return true;
        }

        last = node;
        node = node->next;
    }
    return false;
}

void removeJobList(struct JobList * list) {
    struct JobNode * node = list->head;
    while(node) {
        struct JobNode * next = node->next;
        mem_free(node, sizeof(struct JobNode));
        node = next;
    }
    mem_free(list, sizeof(struct JobList));
}
