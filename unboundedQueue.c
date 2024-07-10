#include "unboundedQueue.h"

void initializeQueue(UnboundedQueue* queue) {
    queue->head = NULL;
    queue->tail = NULL;
    sem_init(&(queue->slotsOccupied), 0, 0);
    sem_init(&(queue->accessLock), 0, 1);
}

void unboundedEnqueue(UnboundedQueue* queue, Article* newArticle) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = newArticle;
    newNode->next = NULL;

    sem_wait(&(queue->accessLock));

    if (queue->head == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }

    sem_post(&(queue->slotsOccupied));
    sem_post(&(queue->accessLock));
}

Article* unboundedDequeue(UnboundedQueue* queue) {
    sem_wait(&(queue->slotsOccupied));
    sem_wait(&(queue->accessLock));

    Node* removedNode = queue->head;
    Article* removedData = removedNode->data;
    queue->head = queue->head->next;

    free(removedNode);
    sem_post(&(queue->accessLock));

    return removedData;
}

void cleanupQueue(UnboundedQueue* queue) {
    Node* current = queue->head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    sem_destroy(&(queue->slotsOccupied));
    sem_destroy(&(queue->accessLock));
    free(queue);
}

void displayQueue(UnboundedQueue* queue) {
    Node* current = queue->head;
    while (current->next != NULL) {
        printf("%d %s %d\n", current->data->creatorId, current->data->type, current->data->count);
        current = current->next;
    }
    printf("%d %s %d\n", current->data->creatorId, current->data->type, current->data->count);
}