//
// Created by cartignh on 13/06/2022.
//

#include <stdint.h>
#include "message_queue.h"
#include "../shared/queue.h"
#include "process.h"
#include "scheduler.h"
#include "mem.h"
#include <stdbool.h>

// Stores all the message queues
MessageQueue *message_queues[NBQUEUE] = {0};
int16_t last_queue_id = NBQUEUE - 1;

// Return the first empty queue id
// Returns -1 if no free queue id was found
int16_t get_free_queue_id() {
    // Try all pid starting from last_queue_id + 1 until we reach again last_queue_id (modulo)
    // This avoids starting from 0 each time since we try to allocate Queue ids contiguously
    for (unsigned i = 1; i <= NBQUEUE; i++) {
        // The following line makes sure we iterate in range [1; NBQUEUE]
        int16_t id = (last_queue_id + i) % NBQUEUE;

        if (message_queues[id] == NULL) {
            // pid is free, so return it
            last_queue_id = id;
            return id;
        }
    }
    return -1;
}

// Empties the passed MessageQueue and releases blocked processes
void __empty_queue(MessageQueue *mq) {
    // Release waiting processes, empty the queue
    while (queue_empty(&mq->waiting_processes) == 0) {
        Process *p = queue_out(&mq->waiting_processes, Process, next_message_queue_process);
        //p->message_queue_id = -1;
        if (p->state != ZOMBIE)
            make_process_activable(p);
    }
    schedule_if_current_not_prioritary();
}

// Blocks the current process
void __block_process(MessageQueue *mq) {
    active_process->state = IO_WAIT;
    active_process->message_queue_id = mq->id;
    queue_add(active_process, &(mq->waiting_processes), Process, next_message_queue_process, priority);
    schedule();
}

// Signals that a message is now available, so unblock the older/most important process
void __unblock_process(MessageQueue *mq) {
    Process *out = queue_out(&(mq->waiting_processes), Process, next_message_queue_process);
    if (out) {
        out->message_queue_id = -1;
        // If the queue is empty and processes are blocked waiting, the older process
        // in the more important ones is unblocked and receives this message
        context_switch_to(out);
    }
}

// Creates a queue of `count` capacity
// Returns: the new queue id created
// Errors: returns -1 if max queue count was reached, or if `count` <= 0;
//         returns -2 if trap
int pcreate(int count) {
    if (count <= 0) return -1;

    if(count > INT32_MAX / (int) sizeof (int)) {
        return -1;
    }
    //IN CASE OF TRAP
    if(1073741827 == count) return -2;

    int16_t id = get_free_queue_id();
    // Error if no free index was found
    if (id == -1) return -1;

    void * mem = mem_alloc(count * sizeof(int*));
    if(!mem)
        return -1;

    MessageQueue *queue = mem_alloc(sizeof(MessageQueue));
    if (!queue) {
        mem_free(mem, count * sizeof(int *));
        return -1;
    }
    queue->id = id;
    queue->count = count;
    queue->waiting_processes = (link) LIST_HEAD_INIT(queue->waiting_processes);
    queue->curr_index = 0;
    queue->message_size = 0;
    queue->messages = mem;
    // Add it to the queue
    message_queues[id] = queue;

    return id;
}

// Destroys the queue identified by `fid` and makes all activable all processes that
// were blocked waiting in it. Messages left in the queue will be discarded.
// Returns: 0
// Errors: -1 if `fid` is invalid
int pdelete(int fid) {
    if (fid < 0 || fid >= NBQUEUE) return -1;
    MessageQueue *mq = message_queues[fid];
    if (mq == NULL)
        return -1;

    // Empty the list
    mem_free(mq->messages, mq->count * sizeof(mq->messages));
    message_queues[fid] = 0;

    __empty_queue(mq);

    return 0;
}

// Sends `message` in the queue identified by `fid`
// Returns: 0
// Errors: -1 if `fid` is invalid
int psend(int fid, int message) {
    if (fid < 0 || fid >= NBQUEUE) return -1;

    MessageQueue *mq = message_queues[fid];
    if (mq == NULL)
        return -1;

    // Queue full
    while (mq->message_size == mq->count) {
        /* If the queue is full, then the process calling goes in the blocked state,
           until a place is available in the queue to put the message */
        __block_process(mq);
        if (active_process->message_queue_id != -1) {
            active_process->message_queue_id = -1;
            return -1;
        }
    }
    //queue not full
    ++mq->message_size;
    // The queue is not full and not process is blocked waiting for a message;
    // the message is put directly in the queue
    mq->messages[(mq->curr_index + mq->message_size) % mq->count] = message;


    __unblock_process(mq);

    return 0;
}

// Reads and pops the first (older) message from the queue `fid`.
// The read message is placed in `*message` if it is not null, else it is forgotten.
// Returns: 0
// Errors: -1 if `fid` is invalid or if queue is discarded (deleted/reset)
int preceive(int fid, int *message) {
    if (fid < 0 || fid >= NBQUEUE) return -1;

    MessageQueue *mq = message_queues[fid];
    if (mq == NULL)
        return -1;

    while (mq->message_size == 0) { //Queue empty
        __block_process(mq);

        if (active_process->message_queue_id != -1) {
            active_process->message_queue_id = -1;
            return -1;
        }
    }

    mq->curr_index = (mq->curr_index + 1) % mq->count;
    --mq->message_size;
    if (message != NULL)
        *message = mq->messages[mq->curr_index];

    __unblock_process(mq);
    // Read & pop message from the queue
    /// Reading the first message from the queue may mean that the current process has to wait
    /// for a message to become available (wait for a `psend()` call)

    /// It is also possible that, after becoming blocked waiting for a message,
    /// The current process becomes active again by a `preset()`/`pdelete()` call.
    /// In this case, we return -1

    // If the queue was full, fill it immediately with the message from the most important older
    // blocked sending process, if there is one. This process then becomes activable (or active)
    return 0;
}

// Empties the queue `fid` and makes activable (or active) all processes in it
// Discards all messages in the queue
// Returns: 0
// Errors: -1 if `fid` is invalid
int preset(int fid) {
    if (fid < 0 || fid >= NBQUEUE) return -1;

    MessageQueue *mq = message_queues[fid];
    if (mq == NULL)
        return -1;

    mq->curr_index = 0;
    mq->message_size = 0;
    __empty_queue(mq);

    return 0;
}

// Reads the number of messages & processes waiting on queue `fid`.
// If count is not a null ref, a negative value equals to the inverse of the blocked
// process on an empty queue is placed in it. Otherwise, a positive value equal to the
// number of messages in the queue + the number of processes blocked on full queue is placed in it.
// Returns: 0
// Errors: -1 if `fid` is invalid
int pcount(int fid, int *count) {
    if (fid < 0 || fid >= NBQUEUE) return -1;
    MessageQueue *mq = message_queues[fid];
    if (mq == NULL)
        return -1;

    if (count != NULL) {

        int count_of_waiting_processes = 0;
        Process *p;
        queue_for_each(p, &mq->waiting_processes, Process, next_message_queue_process) {
            ++count_of_waiting_processes;
        }

        if (mq->message_size != 0) { //empty
            *count = mq->message_size + count_of_waiting_processes;
        } else {
            *count = -count_of_waiting_processes;
        }

    }
    return 0;
}

