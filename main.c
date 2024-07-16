// Roei Mesilaty, 315253336
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

int checkAllocation(void* allocation, char* type) {
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

// Function to parse configuration and create producers
Producer** configHandler(char* path, int* ptrProdCount) {
    Producer** producers = (Producer**) malloc(sizeof(Producer) * BUFFER_SIZE);
    if (checkAllocation((void*)producers, "producers")) {
        return NULL;
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        free(producers);
        return NULL;
    }

    char line[100];
    int producerId, itemCount, bufferCapacity;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "Producer", 8) == 0) {
            Producer* producer = (Producer*) malloc(sizeof(Producer));
            if (checkAllocation((void*)producer, "producer")) {
                fclose(file);
                free(producers);
                return NULL;
            }

            sscanf(line, "Producer %d", &producerId);
            fgets(line, sizeof(line), file);
            sscanf(line, "%d", &itemCount);
            fgets(line, sizeof(line), file);
            sscanf(line, "queue size = %d", &bufferCapacity);

            producer->producerId = producerId;
            producer->itemCount = itemCount;
            producer->bufferCapacity = bufferCapacity;

            if (*ptrProdCount >= BUFFER_SIZE) {
                Producer** tempProducers = realloc(producers, sizeof(Producer) * (*ptrProdCount + 1));
                if (checkAllocation((void*)tempProducers, "producers")) {
                    fclose(file);
                    free(producers);
                    return NULL;
                }
                producers = tempProducers;
            }

            producers[*ptrProdCount] = producer;
            (*ptrProdCount)++;
        }
    }

    fclose(file);
    return producers;
}

void initializeProducers(Producer** producers, BoundedBuffer** boundedBuffers, pthread_t* t_gid, int producerCount) {
    for (int i = 0; i < producerCount; i++) {
        BoundedBuffer* newBuffer = (BoundedBuffer*) malloc(sizeof(BoundedBuffer));
        if (checkAllocation((void*)newBuffer, "bounded buffer")) {
            exit(1);
        }

        setupBuffer(newBuffer, producers[i]->bufferCapacity);
        producers[i]->producerBuffer = newBuffer;
        boundedBuffers[i] = newBuffer;

        pthread_t thread_id;
        int result = pthread_create(&thread_id, NULL, runProducer, (void*)producers[i]);
        t_gid[i] = thread_id;
        if (result != 0) {
            printf("Failed to create producer thread: %d\n", result);
        }
    }
}

UnboundedQueue** initializeUnboundedQueues() {
    UnboundedQueue** unboundedQueues = (UnboundedQueue**) malloc(sizeof(UnboundedQueue) * CATEGORIES_NUM);
    if (checkAllocation((void*)unboundedQueues, "unbounded queues")) {
        return NULL;
    }

    for (int i = 0; i < CATEGORIES_NUM; i++) {
        UnboundedQueue* newQueue = (UnboundedQueue*) malloc(sizeof(UnboundedQueue));
        if (checkAllocation((void*)newQueue, "unbounded queue")) {
            return NULL;
        }
        initializeQueue(newQueue);
        unboundedQueues[i] = newQueue;
    }

    return unboundedQueues;
}

void initializeCoEditors(coEditor** coEditors, BoundedBuffer* mainBuffer, UnboundedQueue** unboundedQueues, pthread_t* t_gid2) {
    for (int i = 0; i < CATEGORIES_NUM; i++) {
        coEditor* co_editor = (coEditor*) malloc(sizeof(coEditor));
        if (checkAllocation((void*)co_editor, "co-editor")) {
            exit(1);
        }
        initializeEditor(co_editor, mainBuffer, unboundedQueues[i]);
        coEditors[i] = co_editor;

        pthread_t thread_id;
        int result = pthread_create(&thread_id, NULL, editorThread, (void*)coEditors[i]);
        t_gid2[i] = thread_id;
        if (result != 0) {
            printf("Failed to create co-editor thread: %d\n", result);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    int producerCount = 0;
    int* ptrProdSize = &producerCount;
    Producer** producers = configHandler(argv[1], ptrProdSize);
    if (producers == NULL) {
        return 1;
    }

    BoundedBuffer** boundedBuffers = (BoundedBuffer**) malloc(sizeof(BoundedBuffer) * producerCount);
    if (checkAllocation((void*)boundedBuffers, "bounded buffers")) {
        return 1;
    }

    pthread_t* t_gid = (pthread_t*) malloc(sizeof(pthread_t) * producerCount);
    if (t_gid == NULL) {
        printf("Failed to allocate memory for producer threads\n");
        return 1;
    }

    initializeProducers(producers, boundedBuffers, t_gid, producerCount);

    UnboundedQueue** unboundedQueues = initializeUnboundedQueues();
    if (unboundedQueues == NULL) {
        return 1;
    }

    TaskManager* dp = (TaskManager*) malloc(sizeof(TaskManager));
    if (checkAllocation((void*)dp, "task manager")) {
        return 1;
    }

    initTaskManager(dp, unboundedQueues, boundedBuffers, producerCount);
    pthread_t dispatcher_thread;
    int res = pthread_create(&dispatcher_thread, NULL, taskManagerThread, (void*)dp);
    if (res != 0) {
        printf("Failed to create dispatcher thread: %d\n", res);
    }

    int mainBufferSize = getLastIntegerFromFile(argv[1]);
    BoundedBuffer* mainBuffer = (BoundedBuffer*) malloc(sizeof(BoundedBuffer));
    if (checkAllocation((void*)mainBuffer, "main buffer")) {
        return 1;
    }

    setupBuffer(mainBuffer, mainBufferSize);

    pthread_t* t_gid2 = (pthread_t*) malloc(sizeof(pthread_t) * CATEGORIES_NUM);
    if (t_gid2 == NULL) {
        printf("Failed to allocate memory for co-editor threads\n");
        return 1;
    }

    coEditor** coEditors = (coEditor**) malloc(sizeof(coEditor) * CATEGORIES_NUM);
    if (checkAllocation((void*)coEditors, "co-editors")) {
        return 1;
    }

    initializeCoEditors(coEditors, mainBuffer, unboundedQueues, t_gid2);

    pthread_t screenThread;
    res = pthread_create(&screenThread, NULL, screenRoutine, (void*)mainBuffer);
    if (res != 0) {
        printf("Failed to create screen thread: %d\n", res);
    }

    for (int i = 0; i < producerCount; i++) {
        pthread_join(t_gid[i], NULL);
    }
    pthread_join(dispatcher_thread, NULL);
    for (int i = 0; i < CATEGORIES_NUM; i++) {
        pthread_join(t_gid2[i], NULL);
    }
    pthread_join(screenThread, NULL);

    deallocStructs(producers, boundedBuffers, unboundedQueues, coEditors, producerCount);
    deallocThreads(t_gid, t_gid2);
    free(dp);
    cleanupBuffer(mainBuffer);

    return 0;
}
