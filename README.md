# Static-Table-Based-Scheduler

Brief: A static table based scheduler implementation for Nordic nRF52840 Dev Kit

## What is this?

This repository contains an implementation of a static table based scheduler developped on top of Zephyr RTOS's kernel. The scheduler is designed to run on the Nordic nRF52840 Development Kit, but it most likely runs on every microcontroller that supports Zephyr. This scheduler is offline and static, meaning that the tasks are scheduled before running and the scheduler does not support dynamic task creation or deletion. The scheduler is designed to be simple and efficient, and is intended to be used in applications where the task set is known at compile time and the tasks are periodic. 

## Implementation

The algorithm for this scheduler follows what was written by Giorgio Buttazzo in his book "Hard Real-Time Computing Systems". It is a timeline scheduler that uses rate monotonic for assigning priorities to tasks based on their periods (smaller period = higher priority).

Throughout the creation of the schedule, the algorithm goes as follows:

1. Sort the tasks by period in ascending order (rate monotonic)
2. Calculate the hyperperiod/major cycle of the tasks (the least common multiple of all the periods)
3. Calculate the tick size/minor cycle of the tasks (the greatest common divisor of all the periods)
4. Create the schedule table

## Showcase Scenario

For showcasing the scheduler's efficiency, a simple scenario was created in which 3 tasks are scheduled: 

- LED task, writes stored values to the nRF52840's LEDs
- Button task, reads the state of the nRF52840's buttons
- Print task, prints the Real Time Database (RTDB) to the console

## How to run

Ensure that you have the necessary Zephyr SDK and toolchain installed. If you are completely unaware of it, you can find how to install it [on their course's first lesson](https://academy.nordicsemi.com/courses/nrf-connect-sdk-fundamentals/lessons/lesson-1-nrf-connect-sdk-introduction/topic/exercise-1-1/).

After that, you need to create a build configuration for the nRF52840 Development Kit in the nRF Connect extension of VS Code. You can find help on the course previously mentioned.

Finally, build the project and flash it to the nRF52840 Development Kit. Use a serial monitor like PuTTY or minicom to see the results and make sure that the baudrate is set to 115200.
