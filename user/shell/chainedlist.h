//
// Created by guerroun/cartignh on 09/02/2022.
//
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "command.h"

#ifndef ENSISHELL_CHAINEDLIST_H
#define ENSISHELL_CHAINEDLIST_H


struct JobNode {
    struct JobNode * next;
    int pid;
    Command* cmd;
};

struct JobList {
    struct JobNode * head;
    unsigned long size;
};

struct JobList * createJobList();

void addJobToJobList(struct JobList * list, int pid, Command *cmd) ;

void removeNodeJobFromJobList(struct JobList * list, struct JobNode * last, struct JobNode * node);

bool removeJobFromJobList(struct JobList * list, int pid) ;

void removeJobList(struct JobList * list);

#endif //ENSISHELL_CHAINEDLIST_H
