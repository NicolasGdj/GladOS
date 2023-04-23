//
// Created by cartignh on 13/06/2022.
//

#ifndef PROJET_SYS_MESSAGE_QUEUE_H
#define PROJET_SYS_MESSAGE_QUEUE_H

#include "../shared/queue.h"

// Maximum number of queues that can be created (alive) at the same time
#define NBQUEUE 50

typedef struct message_queue {
    int16_t id;
    link waiting_processes;
    int count;
    int curr_index;
    int message_size;
    int *messages;
} MessageQueue;
extern MessageQueue * message_queues[];

int pcreate(int count);

int pdelete(int fid);

int psend(int fid, int message);

int preceive(int fid, int *message);

int preset(int fid);

int pcount(int fid, int *count);

#endif //PROJET_SYS_MESSAGE_QUEUE_H
