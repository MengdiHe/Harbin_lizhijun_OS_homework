#include <unistd.h>
#include <errno.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/sched.h>

#define SEM_LIST_LENGTH 20

sem_t sem_list[SEM_LIST_LENGTH];

/*
sem_open()的功能是创建一个信号量，或打开一个已经存在的信号量。
*/

sem_t *sys_sem_open(const char * name,unsigned int value)
{
	if (name == NULL)
    {
        printk("name == NULL\n");
        return NULL;
    }
    /* 首先将信号量的名称赋值到新建的缓冲区中 */
    char nbuf[20];
    int i = 0;
    for(; i< 20; i++)
    {
    	nbuf[i] = get_fs_byte(name+i);
    }

    /* 然后开始遍历已有的信号量数组，如果有该名字的信号量，直接返回信号量的地址 */
    sem_t * result = NULL;
    i = 0;
    for(; i < SEM_LIST_LENGTH; i++)
    {
    	if(sem_list[i].name[0] == '\0')
    		break;
        if(!strcmp(sem_list[i].name, nbuf))
        {
            result = &sem_list[i];
            printk("sem %s is found\n",result->name);
            return result;
        }
    }
    /* 如果找不到信号量，就开始新建一个名字为name的信号量，值=value，队列指针=NULL，然后返回信号量的地址 */
    strcpy(sem_list[i].name, nbuf);
    sem_list[i].value = value;
    sem_list[i].queue = NULL;
    result = &sem_list[i];
    return result;
}

/*
 sem_wait()就是信号量的P原子操作。
 如果继续运行的条件不满足，则令调用进程等待在信号量sem上。
 返回0表示成功，返回-1表示失败。
 */
int sys_sem_wait(sem_t * sem)
{
    /* 判断:如果传入的信号量是无效信号量，P操作失败，返回-1 */
    if(sem == NULL || sem < sem_list || sem > sem_list + SEM_LIST_LENGTH)
    {
        printk("P(sem) error\n");
        return -1;
    }
    /* 关中断 */
    cli();
    while(sem->value == 0)
    {
        sleep_on(&(sem->queue));
    }
    sem->value--; 
    /* 开中断 */
    sti();
    return 0;
}

/*
sem_post()就是信号量的V原子操作。
如果有等待sem的进程，它会唤醒其中的一个。
返回0表示成功，返回-1表示失败。
*/
int sys_sem_post(sem_t * sem)
{
    /* 判断:如果传入的信号量是无效信号量，V操作失败，返回-1 */
    if(sem == NULL || sem < sem_list || sem > sem_list + SEM_LIST_LENGTH)
    {
        printk("V(sem) error\n");
        return -1;
    }
    /* 关中断 */
    cli();
    sem->value++;
    /* 如果有等待sem的进程，它会唤醒其中的一个。 */
    wake_up(&(sem->queue));
    /* 开中断 */
    sti();
    return 0;
}

/*
sem_unlink()的功能是删除名为name的信号量。
返回0表示成功，返回-1表示失败。
*/
int sys_sem_unlink(const char *name)
{
    if (name == NULL)
        return -1;
    /* 首先将信号量的名称赋值到新建的缓冲区中 */
    char nbuf[20];
    int i = 0;
    for (; i < 20; i++)
    {
        nbuf[i] = get_fs_byte(name + i);
        if (nbuf[i] == '\0')
            break;
    }
    i = 0;
    for (; i < SEM_LIST_LENGTH; i++)
    {
        if (strcmp(sem_list[i].name, nbuf))
        {
            sem_list[i].name[0] = '\0';
            sem_list[i].value = 0;
            sem_list[i].queue = NULL;
        }
    }
    /* 没找到该名字的信号量，删除失败，返回-1 */
    if (i == SEM_LIST_LENGTH)
        return -1;

    return 0;
}

