#include "coEditors.h"

void initializeEditor(coEditor* editor, BoundedBuffer* primaryBuffer, UnboundedQueue* secondaryQueue) {
    editor->secondaryQueue = secondaryQueue;
    editor->primaryBuffer = primaryBuffer;
}

void* editorThread(void* params) {
    /*
     * Iterate through secondary queue until reaching sentinel value
     * For each element in queue 'process' it and pass it to primary buffer.
     */
    coEditor* editor = (coEditor*) params;
    while(1) {
        Article* item = unboundedDequeue(editor->secondaryQueue);
        if(strcmp(item->type, "DONE") == 0) {
            addToBuffer(editor->primaryBuffer, item);
            return NULL;
        } else {
            // process and insert in primary buffer
            addToBuffer(editor->primaryBuffer, item);
        }
        usleep(100000);
    }
}
