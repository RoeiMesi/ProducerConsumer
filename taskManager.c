#include "taskManager.h"
#include <string.h>
#include <stdlib.h>

// Function to process each article and enqueue it to the corresponding secondary queue
void processTaskManagerArticle(TaskManager* manager, Article* item) {
    if (strcmp(item->type, "NEWS") == 0) {
        unboundedEnqueue(manager->secondaryQueues[0], item);
    } else if (strcmp(item->type, "WEATHER") == 0) {
        unboundedEnqueue(manager->secondaryQueues[1], item);
    } else {
        unboundedEnqueue(manager->secondaryQueues[2], item);
    }
}

// Function to handle sentinel values
void enqueueSentinels(TaskManager* manager) {
    for (int i = 0; i < 3; i++) {
        Article* sentinel = (Article*) malloc(sizeof(Article));
        sentinel->type = "DONE";
        sentinel->creatorId = i;
        sentinel->count = 0;
        unboundedEnqueue(manager->secondaryQueues[i], sentinel);
    }
}

// Function to handle the task manager's main loop
void runTaskManager(TaskManager* manager) {
    int completedBuffers = 0;
    while (1) {
        for (int i = 0; i < manager->numProducers; i++) {
            if (manager->primaryBuffers[i]->finished) {
                continue;
            }
            Article* item = removeFromBuffer(manager->primaryBuffers[i]);
            if (strcmp(item->type, "DONE") == 0) {
                free(item);
                if (!manager->primaryBuffers[i]->finished) {
                    manager->primaryBuffers[i]->finished = 1;
                    completedBuffers++;
                }
                continue;
            } else {
                processTaskManagerArticle(manager, item);
            }
        }
        if (completedBuffers == manager->numProducers) {
            enqueueSentinels(manager);
            return;
        }
    }
}

void initTaskManager(TaskManager* manager, UnboundedQueue** uq, BoundedBuffer** lb, int numProducers) {
    manager->numProducers = numProducers;
    manager->primaryBuffers = lb;
    manager->secondaryQueues = uq;
}

void* taskManagerThread(void* params) {
    TaskManager* manager = (TaskManager*) params;
    runTaskManager(manager);
    return NULL;
}
