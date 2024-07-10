#ifndef EX3_BOUNDEDBUFFER_H
#define EX3_BOUNDEDBUFFER_H
#include "article.h"
#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    int capacity, finished, head, tail;
    sem_t slotsAvailable, slotsOccupied;
    sem_t accessLock;
    Article** elements;
} BoundedBuffer;

int setupBuffer(BoundedBuffer* buffer, int capacity);
void addToBuffer(BoundedBuffer* buffer, Article* newElement);
Article* removeFromBuffer(BoundedBuffer* buffer);
void cleanupBuffer(BoundedBuffer* buffer);

#endif //EX3_BOUNDEDBUFFER_H
