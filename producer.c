#include "producer.h"

void* producerRoutine(void* producerPtr) {
    /*
     * 1. Initialize buffer
     * 2. Loop that generates items
     * 3. Push items to buffer
     */
    Producer* producer = (Producer*) producerPtr;
    // Add items to buffer
    int typeCounters[TYPES_COUNT] = {0};
    for (int i = 0; i < producer->itemCount; i++) {
        Article* newItem = createArticle(producer->producerId, typeCounters);
        addToBuffer(producer->producerBuffer, newItem);
    }
    Article* sentinel = (Article*) malloc(sizeof(Article));
    if (sentinel == NULL) {
        printf("Failed to allocate sentinel\n");
        free(sentinel);
    } else {
        sentinel->creatorId = producer->producerId;
        sentinel->type = "DONE";
        sentinel->count = 0;
        addToBuffer(producer->producerBuffer, sentinel);
    }
    return NULL;
}
