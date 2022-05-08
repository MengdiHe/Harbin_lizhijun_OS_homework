#define __LIBRARY__
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t *, sem);
_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, const char *, name);
_syscall2(int, shmget, unsigned int, key, int, size);
_syscall1(int, shmat, int, shmid);

#define TASK_LENGTH 10
#define TASK_TOTAL 500
#define CONSUMER_NUM 5
#if 1
#define P(x) sem_wait(x)
#define V(x) sem_post(x)
#else
#define P(x)
#define V(x)
#endif

int main()
{
    sem_t *empty, *full, *mutex;
    int fbuffer = -1;
    int i = 0;
    int shmid = -1;
    int* array = NULL;
    int curPos = 0;

    empty = sem_open("empty", TASK_LENGTH);
    full = sem_open("full", 0);
    mutex = sem_open("mutex", 1);
    if (empty == -1 || full == -1 || mutex == -1) {
        printf("sem_open failed!\n");
        return -1;
    }

    if ((shmid = shmget(1234, 40)) == -1) {
        printf("shmid %d\n", shmid);
        return -1;
    }
    if ((array = (int*)shmat(shmid)) == -1) {
        printf("shmat failed!\n");
        return -1;
    }

    for (i = 0; i < TASK_TOTAL; i++) {
        P(full);
        P(mutex);
        printf("consumer consume %d\n", array[curPos]);
        fflush(stdout);
        curPos = (curPos + 1) % TASK_LENGTH;
        V(mutex);
        V(empty);
    }

    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");
    return 0;
}
