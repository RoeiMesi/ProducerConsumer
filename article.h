#ifndef EX3_ARTICLE_H
#define EX3_ARTICLE_H
#include <stdlib.h>

typedef struct {
    int creatorId;
    int count;
    char* type;
} Article;

Article* createArticle(int creatorId, int* typeCounters);

#endif //EX3_ARTICLE_H
