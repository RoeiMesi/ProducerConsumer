#ifndef EX3_UNBOUNDEDQUEUE_H
#define EX3_UNBOUNDEDQUEUE_H

#include <semaphore.h>
#include "article.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    Article* data;
    struct node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    sem_t slotsOccupied;
    sem_t accessLock;
} UnboundedQueue;

Node* allocateNode(Article* newArticle);
void cleanupQueue(UnboundedQueue* queue);
void initializeQueue(UnboundedQueue* queue);
void unboundedEnqueue(UnboundedQueue* queue, Article* newArticle);
Article* unboundedDequeue(UnboundedQueue* queue);
void displayQueue(UnboundedQueue* queue);

#endif //EX3_UNBOUNDEDQUEUE_H
