CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = ex3.out
SOURCES = article.c boundedBuffer.c coEditors.c taskManager.c main.c producer.c screenManager.c unboundedQueue.c
HEADERS = article.h boundedBuffer.h coEditors.h taskManager.h producer.h screenManager.h unboundedQueue.h
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
