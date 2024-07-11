#include "boundedBuffer.h"
#include "article.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Function to initialize semaphores
int initializeSemaphores(BoundedBuffer* buffer, int capacity) {
    if (sem_init(&buffer->slotsAvailable, 0, capacity) == -1) {
        perror("Failed to initialize semaphore");
        return 1;
    }
    if (sem_init(&buffer->slotsOccupied, 0, 0) == -1) {
        perror("Failed to initialize semaphore");
        return 1;
    }
    if (sem_init(&buffer->accessLock, 0, 1) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }
    return 0;
}

// Function to clean up the elements in the buffer
void freeBufferElements(BoundedBuffer* buffer) {
    int temp = buffer->head;
    while (temp != buffer->tail) {
        free(buffer->elements[temp]);
        temp = (temp + 1) % buffer->capacity;
    }
}

int setupBuffer(BoundedBuffer* buffer, int capacity) {
    buffer->elements = (Article**) malloc(sizeof(Article*) * capacity);
    if (buffer->elements == NULL) {
        printf("Buffer allocation failed\n");
        return 1;
    }
    buffer->capacity = capacity;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->finished = 0;

    // Initialize semaphores and locks
    if (initializeSemaphores(buffer, capacity) != 0) {
        free(buffer->elements);
        return 1;
    }
    return 0;
}

void addToBuffer(BoundedBuffer* buffer, Article* newElement) {
    // Wait for an available slot in the buffer
    sem_wait(&buffer->slotsAvailable);

    // Lock the access lock to ensure exclusive access to the buffer
    sem_wait(&buffer->accessLock);

    buffer->elements[buffer->tail] = newElement;
    buffer->tail = (buffer->tail + 1) % buffer->capacity;

    // Release the access lock to allow other threads to access the buffer
    sem_post(&buffer->accessLock);

    // Signal that a slot has been filled in the buffer
    sem_post(&buffer->slotsOccupied);
}

Article* removeFromBuffer(BoundedBuffer* buffer) {
    // Wait for a filled slot in the buffer
    sem_wait(&buffer->slotsOccupied);

    // Lock the access lock to ensure exclusive access to the buffer
    sem_wait(&buffer->accessLock);

    // Critical section
    Article* retrievedElement = buffer->elements[buffer->head];
    buffer->head = (buffer->head + 1) % buffer->capacity;

    // Release the access lock to allow other threads to access the buffer
    sem_post(&buffer->accessLock);

    // Signal that a slot has been emptied in the buffer
    sem_post(&buffer->slotsAvailable);

    return retrievedElement;
}

void cleanupBuffer(BoundedBuffer* buffer) {
    // Free buffer elements
    freeBufferElements(buffer);

    // Destroy semaphores and locks
    sem_destroy(&buffer->slotsAvailable);
    sem_destroy(&buffer->slotsOccupied);
    sem_destroy(&buffer->accessLock);

    // Free buffer memory
    free(buffer->elements);
    free(buffer);
}
