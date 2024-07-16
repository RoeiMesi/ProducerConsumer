// Roei Mesilaty, 315253336
#include "article.h"

Article* createArticle(int creatorId, int* typeCounters){
    char* types[] = {"NEWS", "WEATHER", "SPORTS"};
    Article* newArticle = (Article*) malloc(sizeof(Article));

    // Set the producer ID
    newArticle->creatorId = creatorId;

    // Generate a random type index
    int typeIndex = rand() % 3;
    newArticle->type = types[typeIndex];

    // Increment the counter for the chosen type
    typeCounters[typeIndex]++;

    // Set the count value for the article
    newArticle->count = typeCounters[typeIndex];

    return newArticle;
}
