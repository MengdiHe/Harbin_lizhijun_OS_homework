#include <stdio.h>

void producer(int task_fd)
{
    int i = 0;
    for (int i = 0; i < 500; i++) {
        P(empty);
        P(mutex);
        //
        V(mutex);
        V(full);
    }
}

void consumer(int task_fd, int out_fd)
{
    while 
}

