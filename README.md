# Producer-Consumer System

**Producer-Consumer System** is a multithreaded program simulating a producer-consumer architecture. It demonstrates inter-thread communication and synchronization using bounded and unbounded buffers, semaphores, and mutexes. The project simulates a news production pipeline, where producers create articles, co-editors process them, and a screen manager displays the results.

---

## Features

1. **Producers**  
   - Create articles of different types (NEWS, WEATHER, SPORTS).
   - Use bounded buffers to communicate with a dispatcher.

2. **Dispatcher**  
   - Distributes articles to category-specific unbounded queues.
   - Signals completion using sentinel articles.

3. **Co-Editors**  
   - Process articles from unbounded queues.
   - Forward processed articles to a primary bounded buffer.

4. **Screen Manager**  
   - Consumes articles from the primary buffer.
   - Displays article details and manages termination signals.

## Setup

### Prerequisites

- **GCC**: GNU Compiler Collection for compiling the code.
- **Make**: A build automation tool.
- **Linux/Unix-based system**: Required for proper execution of the multithreaded program.

### Build Instructions

1. Clone the Repository  
   Clone the project repository using the following command:
   ```bash
   git clone https://github.com/<username>/Producer-Consumer.git
   cd Producer-Consumer

2. Compile the Program
Build the executable using make:

```
make
```
This will generate the ex3.out executable file.

Clean Build Artifacts
To remove all object files and the executable, run:
```
make clean
```

### Configuration
The program reads its behavior from a configuration file. An example configuration.txt is shown below:

```plaintext
Producer 1
10
queue size = 5

Producer 2
8
queue size = 4

Producer 3
12
queue size = 6

Co-Editor queue size = 3
```
* Each producer is assigned an ID, an article count, and a queue size.
* The co-editor queue size sets the capacity of category-specific unbounded queues.

### Usage
1. Ensure the configuration file is properly set up.
2. Run the program with the configuration file as an argument:

```
./ex3.out configuration.txt
```

### How It Works
* Producers
Read the configuration file to determine their behavior.
Produce articles and add them to their respective bounded buffers.

* Dispatcher
Reads articles from producer buffers.
Distributes articles to category-specific unbounded queues.

* Co-Editors
Consume articles from unbounded queues.
Process articles and forward them to the primary bounded buffer.

* Screen Manager
Reads articles from the primary buffer.
Displays article details until sentinel articles indicate the end.

## Project Structure

```plaintext
Producer-Consumer/
├── Makefile                  # Build configuration
├── article.c/.h              # Article management utilities
├── boundedBuffer.c/.h        # Implementation of bounded buffer
├── unboundedQueue.c/.h       # Implementation of unbounded queue
├── producer.c/.h             # Producer thread functionality
├── coEditors.c/.h            # Co-Editor thread functionality
├── screenManager.c/.h        # Screen manager functionality
├── taskManager.c/.h          # Dispatcher thread functionality
├── main.c                    # Main entry point for the program
├── configuration.txt         # Example configuration file
└── README.md                 # Documentation
