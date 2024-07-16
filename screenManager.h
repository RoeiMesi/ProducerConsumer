#ifndef EX3_SCREENMANAGER_H
#define EX3_SCREENMANAGER_H
#include "boundedBuffer.h"
#include "article.h"
#include <string.h>

void processArticle(Article* item);
void runScreenManager(BoundedBuffer* primaryBuffer);
void* screenRoutine(void* params);

#endif //EX3_SCREENMANAGER_H
