#include "screenManager.h"
#include <string.h>
#include <stdio.h>

// Function to process each article
void processArticle(Article* item) {
    printf("Producer %d %s %d\n", item->creatorId, item->type, item->count);
    free(item);
}

// Function to handle the screen routine's main loop
void screenMainLoop(BoundedBuffer* primaryBuffer) {
    int doneCount = 0;
    while (1) {
        Article* item = removeFromBuffer(primaryBuffer);
        if (strcmp(item->type, "DONE") == 0) {
            free(item);
            doneCount++;
            if (doneCount == 3) {
                printf("DONE\n");
                return;
            }
        } else {
            processArticle(item);
        }
    }
}

void* screenRoutine(void* params) {
    BoundedBuffer* primaryBuffer = (BoundedBuffer*) params;
    screenMainLoop(primaryBuffer);
    return NULL;
}
