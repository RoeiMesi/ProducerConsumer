#include "producer.h"

// Function to allocate and initialize a sentinel article
Article* createSentinel(int producerId) {
    Article* sentinel = (Article*) malloc(sizeof(Article));
    if (sentinel == NULL) {
        printf("Failed to allocate sentinel\n");
        return NULL;
    }
    sentinel->creatorId = producerId;
    sentinel->type = "DONE";
    sentinel->count = 0;
    return sentinel;
}

void* producerRoutine(void* producerPtr) {
    Producer* producer = (Producer*) producerPtr;
    
    // Initialize type counters
    int typeCounters[TYPES_COUNT] = {0};
    
    // Generate items and add them to the buffer
    for (int i = 0; i < producer->itemCount; i++) {
        Article* newItem = createArticle(producer->producerId, typeCounters);
        addToBuffer(producer->producerBuffer, newItem);
    }
    
    // Create and add sentinel to the buffer
    Article* sentinel = createSentinel(producer->producerId);
    if (sentinel != NULL) {
        addToBuffer(producer->producerBuffer, sentinel);
    }
    
    return NULL;
}
