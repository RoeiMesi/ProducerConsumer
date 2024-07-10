#ifndef EX3_PRODUCER_H
#define EX3_PRODUCER_H
#include <stdio.h>
#include "boundedBuffer.h"
#include "article.h"
#include <stdlib.h>
#define TYPES_COUNT 3

typedef struct {
    int producerId;
    int itemCount;
    int bufferCapacity;
    BoundedBuffer* producerBuffer;
} Producer;

void* producerRoutine(void* args);

#endif //EX3_PRODUCER_H
