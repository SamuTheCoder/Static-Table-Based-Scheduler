#ifndef FIFO_H
#define FIFO_H

#pragma once

#include <zephyr/kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define FIFO_SIZE 10


typedef struct {
    struct k_sem buffer[FIFO_SIZE];
    int front;
    int rear;
    int count;
} FIFO;

void fifo_init(FIFO *fifo);

bool fifo_is_empty(FIFO *fifo);

bool fifo_is_full(FIFO *fifo);

bool fifo_push(FIFO *fifo, struct k_sem* value);

void fifo_pop(FIFO *fifo, struct k_sem *value);

void fifo_print(FIFO *fifo);

#endif // FIFO_H