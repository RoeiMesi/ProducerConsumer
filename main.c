#include <stdlib.h>
#include <stdio.h>
#include "pthread.h"
#include "semaphore.h"
#include <string.h>
#include "producer.h"
#include "boundedBuffer.h"
#include "unboundedQueue.h"
#include "taskManager.h"
#include "coEditors.h"
#include "screenManager.h"

#define CATEGORIES_NUM 3
#define BUFFER_SIZE 10

void deallocStructs(Producer** producers, BoundedBuffer** boundedBuffers, UnboundedQueue** unboundedQueues,
                    coEditor** coEditors, int prodCount) {
    for (int i = 0; i < prodCount; i++) {
        cleanupBuffer(boundedBuffers[i]);
        free(producers[i]);
    }
    for (int i = 0; i < CATEGORIES_NUM; i++) {
        cleanupQueue(unboundedQueues[i]);
        free(coEditors[i]);
    }
    free(producers);
    free(boundedBuffers);
    free(unboundedQueues);
    free(coEditors);
}

void deallocThreads(pthread_t* t_gid, pthread_t* t_gid2) {
    free(t_gid);
    free(t_gid2);
}

int allocationCheck(void* allocation, char* type) {
    if (allocation == NULL) {
        printf("Failed to allocate %s\n", type);
        free(allocation);
        return 1;
    } else {
        return 0;
    }
}

int getLastIntegerFromFile(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return -1; // Return -1 to indicate an error
    }

    int lastInteger = -1;
    int currentValue;

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d", &currentValue) == 1) {
            lastInteger = currentValue;
        }
    }

    fclose(file);

    return lastInteger;
}

// Return producer array
Producer** configHandler(char* path, int* ptrProdCount) {
    Producer** producers = (Producer**) malloc(sizeof(Producer) * BUFFER_SIZE);
    if (allocationCheck((void*)producers, "producers")) {
        return NULL;
    }
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        // Failed to open the file
        printf("Failed to open file\n");
        return NULL;
    }
    int args[3];
    char line[100];
    int counter = 0;
    int bufferSize = BUFFER_SIZE;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n') {
            Producer* producer1 = (Producer*) malloc(sizeof(Producer));
            producer1->producerId = args[0];
            producer1->itemCount = args[1];
            producer1->bufferCapacity = args[2];
            // Add the producer to the producers array
            if (*ptrProdCount > bufferSize) {
                bufferSize *= 2;
                Producer** tempProducers = realloc(producers, sizeof(Producer) * bufferSize);
                if (allocationCheck((void*)tempProducers, "producers")) {
                    return NULL;
                }
                producers = tempProducers;
            }
            producers[*ptrProdCount] = producer1;
            (*ptrProdCount)++;
            continue;  // Skip empty lines
        }
        // Parse the block of text and extract the three integers
        sscanf(line, "%d", &args[counter]);
        counter = (counter + 1) % 3;
    }
    fclose(file);
    return producers;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return 1;
    } else {
        int producerCount = 0;
        int* ptrProdSize = &producerCount;
        Producer** producers = configHandler(argv[1], ptrProdSize);
        // Locked buffer array to use later
        BoundedBuffer** boundedBuffers = (BoundedBuffer**) malloc(sizeof(BoundedBuffer) * (*ptrProdSize));
        if (allocationCheck((void*)boundedBuffers, "locked buffers")) {
            return 1;
        }
        // Thread group ID for producers
        pthread_t* t_gid = (pthread_t*) malloc(sizeof(pthread_t) * (*ptrProdSize));
        for (int i = 0; i < *ptrProdSize; i++) {
            // Initialize buffer for producer
            BoundedBuffer* newBuffer = (BoundedBuffer*) malloc(sizeof(BoundedBuffer));
            if (allocationCheck((void*)newBuffer, "locked buffer")) {
                return 1;
            }
            setupBuffer(newBuffer, producers[i]->bufferCapacity);
            producers[i]->producerBuffer = newBuffer;
            // Assign to locked buffer
            boundedBuffers[i] = newBuffer;
            pthread_t thread_id;
            int result = pthread_create(&thread_id, NULL,
                                        producerRoutine, (void*)producers[i]);
            t_gid[i] = thread_id;
            if (result != 0) {
                printf("Failed to create thread: %d\n", result);
            }
        }
        // Initialize unbounded queues
        UnboundedQueue** unboundedQueues = (UnboundedQueue**) malloc(sizeof(UnboundedQueue) * CATEGORIES_NUM);
        if (allocationCheck((void*)unboundedQueues, "unbounded queues")) {
            return 1;
        }
        for (int i = 0; i < CATEGORIES_NUM; i++) {
            UnboundedQueue* newQueue = (UnboundedQueue*) malloc(sizeof(UnboundedQueue));
            if (allocationCheck((void*)newQueue, "unbounded queue")) {
                return 1;
            }
            initializeQueue(newQueue);
            unboundedQueues[i] = newQueue;
        }
        // Initialize dispatcher and create a thread
        TaskManager* dp = (TaskManager*) malloc(sizeof(TaskManager));
        initTaskManager(dp, unboundedQueues, boundedBuffers, (*ptrProdSize));
        pthread_t dispatcher_thread;
        int res = pthread_create(&dispatcher_thread, NULL,
                                 taskManagerThread, (void*)dp);
        if (res != 0) {
            printf("Failed to create dispatcher thread: %d\n", res);
        }
        // Initialize main buffer and create co-editor threads
        int mainBufferSize = getLastIntegerFromFile(argv[1]);
        BoundedBuffer* mainBuffer = (BoundedBuffer*) malloc(sizeof(BoundedBuffer));
        if (allocationCheck((void*)mainBuffer, "main buffer")) {
            return 1;
        }
        setupBuffer(mainBuffer, mainBufferSize);
        pthread_t* t_gid2 = (pthread_t*) malloc(sizeof(pthread_t) * CATEGORIES_NUM);
        coEditor** coEditors = (coEditor**) malloc(sizeof(coEditor) * CATEGORIES_NUM);
        for (int i = 0; i < CATEGORIES_NUM; i++) {
            coEditor* co_editor = (coEditor*) malloc(sizeof(coEditor));
            if (allocationCheck((void*)co_editor, "co-editor")) {
                return 1;
            }
            // Initialize co-editor
            initializeEditor(co_editor, mainBuffer, unboundedQueues[i]);
            coEditors[i] = co_editor;
            // Create co-editor threads and pass co-editor struct to them
            pthread_t thread_id;
            int result = pthread_create(&thread_id, NULL,
                                        editorThread, (void*)coEditors[i]);
            t_gid2[i] = thread_id;
            if (result != 0) {
                printf("Failed to create co-editor thread: %d\n", res);
            }
        }
        // Create screen manager thread
        pthread_t screenThread;
        res = pthread_create(&screenThread, NULL, screenRoutine, (void*)mainBuffer);
        if (res != 0) {
            printf("Failed to create screen thread: %d\n", res);
        }
        // Join producer threads, dispatcher and co-editors
        for (int i = 0; i < *ptrProdSize; i++) {
            pthread_join(t_gid[i], NULL);
        }
        pthread_join(dispatcher_thread, NULL);
        for (int i = 0; i < CATEGORIES_NUM; i++) {
            pthread_join(t_gid2[i], NULL);
        }
        pthread_join(screenThread, NULL);
        // Free memory
        deallocStructs(producers, boundedBuffers, unboundedQueues, coEditors, *ptrProdSize);
        deallocThreads(t_gid, t_gid2);
        free(dp);
        cleanupBuffer(mainBuffer);
        return 0;
    }
}
