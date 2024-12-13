#include "../lib/fifo.h"

void fifo_init(FIFO *fifo) {
    fifo->front = 0;
    fifo->rear = 0;
    fifo->count = 0;
}

bool fifo_is_empty(FIFO *fifo) {
    return fifo->count == 0;
}

bool fifo_is_full(FIFO *fifo) {
    return fifo->count == FIFO_SIZE;
}

bool fifo_push(FIFO *fifo, struct k_sem* value) {
    if (fifo_is_full(fifo)) {
        printf("FIFO is full\n");
        return false;
    }
    //Point to the same semaphore
    fifo->buffer[fifo->rear] = *value;
    fifo->rear = (fifo->rear + 1) % FIFO_SIZE;
    fifo->count++;
    return true;
}

void fifo_pop(FIFO *fifo, struct k_sem *value) {
    if (fifo_is_empty(fifo)) {
        printf("FIFO is empty\n");
        return NULL; // Indicate error (e.g., no element to dequeue)
    }
    value = &(fifo->buffer[fifo->front]);
    fifo->front = (fifo->front + 1) % FIFO_SIZE;
    fifo->count--;
}

void fifo_print(FIFO *fifo) {
    printf("FIFO contents: ");
    for (int i = 0, idx = fifo->front; i < fifo->count; i++) {
        printf("%d ", fifo->buffer[idx]);
        idx = (idx + 1) % FIFO_SIZE;
    }
    printf("\n");
}