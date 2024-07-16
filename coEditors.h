#ifndef EX3_COEDITORS_H
#define EX3_COEDITORS_H
#include "boundedBuffer.h"
#include "unboundedQueue.h"
#include <string.h>
#include <unistd.h>

typedef struct {
    UnboundedQueue* secondaryQueue;
    BoundedBuffer* primaryBuffer;
} coEditor;

void processAndAddToBuffer(coEditor* editor, Article* item);
void runCoEditor(coEditor* editor);
void* editorThread(void* params);
void initializeEditor(coEditor* editor, BoundedBuffer* primaryBuffer, UnboundedQueue* secondaryQueue);

#endif //EX3_COEDITORS_H
