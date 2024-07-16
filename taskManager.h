#ifndef EX3_TASKMANAGER_H
#define EX3_TASKMANAGER_H
#include "boundedBuffer.h"
#include "unboundedQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int numProducers;
    BoundedBuffer** primaryBuffers;
    UnboundedQueue** secondaryQueues;
} TaskManager;

void processTaskManagerArticle(TaskManager* manager, Article* item);
void enqueueSentinels(TaskManager* manager);
void runTaskManager(TaskManager* manager);
void initTaskManager(TaskManager* manager, UnboundedQueue** uq, BoundedBuffer** lb, int numProducers);
void* taskManagerThread(void* params);

#endif //EX3_TASKMANAGER_H
