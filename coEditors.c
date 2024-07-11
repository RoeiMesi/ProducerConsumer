#include "coEditors.h"
#include <string.h>
#include <unistd.h>

// Function to process an article and add it to the primary buffer
void processAndAddToBuffer(coEditor* editor, Article* item) {
    // Process the item if needed (processing code can be added here)
    addToBuffer(editor->primaryBuffer, item);
}

// Function to handle the editor's main loop
void editorMainLoop(coEditor* editor) {
    while (1) {
        Article* item = unboundedDequeue(editor->secondaryQueue);
        if (strcmp(item->type, "DONE") == 0) {
            addToBuffer(editor->primaryBuffer, item);
            return;
        } else {
            processAndAddToBuffer(editor, item);
        }
        usleep(100000);
    }
}

void initializeEditor(coEditor* editor, BoundedBuffer* primaryBuffer, UnboundedQueue* secondaryQueue) {
    editor->secondaryQueue = secondaryQueue;
    editor->primaryBuffer = primaryBuffer;
}

void* editorThread(void* params) {
    coEditor* editor = (coEditor*) params;
    editorMainLoop(editor);
    return NULL;
}
