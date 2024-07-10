#include "screenManager.h"

void* screenRoutine(void* params) {
    BoundedBuffer* primaryBuffer = (BoundedBuffer*) params;
    int doneCount = 0;
    while (1) {
        Article* item = removeFromBuffer(primaryBuffer);
        if (strcmp(item->type, "DONE") == 0) {
            free(item);
            doneCount++;
            if (doneCount == 3) {
                printf("DONE\n");
                return NULL;
            }
            continue;
        }
        printf("Producer %d %s %d\n", item->creatorId, item->type, item->count);
        free(item);
    }
}
