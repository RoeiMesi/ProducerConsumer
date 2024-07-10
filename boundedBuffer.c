#include "boundedBuffer.h"
#include "article.h"

int setupBuffer(BoundedBuffer* buffer, int capacity) {
    buffer->elements = (Article**) malloc(sizeof(Article) * capacity);
    buffer->capacity = capacity;
    buffer->finished = 0;
    if(buffer->elements == NULL){
        printf("Buffer allocation failed\n");
        return 1;
    }
    buffer->head = 0;
    buffer->tail = 0;
    // Initialize the semaphores
    if (sem_init(&buffer->slotsAvailable, 0, buffer->capacity) == -1) {
        perror("Failed to initialize semaphore");
        return 1;
    }
    if (sem_init(&buffer->slotsOccupied, 0, 0) == -1) {
        perror("Failed to initialize semaphore");
        return 1;
    }
    // Initialize the access lock
    if (sem_init(&buffer->accessLock, 0, 1) != 0) {
        printf("Mutex initialization failed\n");
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
    Article* retrievedElement;

    // Wait for a filled slot in the buffer
    sem_wait(&(buffer->slotsOccupied));

    // Lock the access lock to ensure exclusive access to the buffer
    sem_wait(&(buffer->accessLock));

    // critical section start
    retrievedElement = buffer->elements[buffer->head];
    buffer->head = (buffer->head + 1) % buffer->capacity;
    // critical section end

    // Release the access lock to allow other threads to access the buffer
    sem_post(&(buffer->accessLock));

    // Signal that a slot has been emptied in the buffer
    sem_post(&(buffer->slotsAvailable));

    return retrievedElement;
}

void cleanupBuffer(BoundedBuffer* buffer) {
    int temp = buffer->head;
    while(temp != buffer->tail){
        free(buffer->elements[temp]);
        temp = (temp + 1) % buffer->capacity;
    }
    sem_destroy(&(buffer->slotsAvailable));
    sem_destroy(&(buffer->slotsOccupied));
    sem_destroy(&(buffer->accessLock));
    free(buffer->elements);
    free(buffer);
}
