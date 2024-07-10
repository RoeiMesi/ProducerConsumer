#include "taskManager.h"

void initTaskManager(TaskManager* manager, UnboundedQueue** uq, BoundedBuffer** lb, int numProducers) {
    manager->numProducers = numProducers;
    manager->primaryBuffers = lb;
    manager->secondaryQueues = uq;
}

void* taskManagerThread(void* params) {
    /*
     * Iterate through primary buffers in a round-robin fashion,
     * dequeue elements from each and enqueue them to the corresponding secondary queues based on categories.
     */
    TaskManager* manager = (TaskManager*) params;
    int completedBuffers = 0;
    while (1) {
        for (int i = 0; i < manager->numProducers; i++) {
            if (manager->primaryBuffers[i]->finished) {
                continue;
            }
            Article* item = removeFromBuffer(manager->primaryBuffers[i]);
            if (strcmp(item->type, "DONE") == 0) {
                free(item);
                if (manager->primaryBuffers[i]->finished) {
                    continue;
                } else {
                    manager->primaryBuffers[i]->finished = 1;
                    completedBuffers++;
                    continue;
                }
            } else if (strcmp(item->type, "NEWS") == 0) {
                unboundedEnqueue(manager->secondaryQueues[0], item);
            } else if (strcmp(item->type, "WEATHER") == 0) {
                unboundedEnqueue(manager->secondaryQueues[1], item);
            } else {
                unboundedEnqueue(manager->secondaryQueues[2], item);
            }
        }
        if (completedBuffers == manager->numProducers) {
            // Enqueue sentinel values to secondary queues
            for (int i = 0; i < 3; i++) {
                Article* sentinel = (Article*) malloc(sizeof(Article));
                sentinel->type = "DONE";
                sentinel->creatorId = i;
                sentinel->count = 0;
                unboundedEnqueue(manager->secondaryQueues[i], sentinel);
            }
            return NULL;
        }
    }
}
