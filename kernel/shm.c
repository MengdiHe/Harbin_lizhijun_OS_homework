#define __LIBRARY__
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <errno.h>

typedef struct shm_ds {
    unsigned int key;
    unsigned int size;
    unsigned long page;
} shm_ds;

#define SHM_SIZE 64

static shm_ds shm_list[SHM_SIZE];

int sys_shmget(unsigned int key, size_t size)
{
    int i = 0;
    unsigned long page;

    if (size > PAGE_SIZE) {
        printk("size %u cannot be greater than page size %d\n", size, PAGE_SIZE);
        return -ENOMEM;
    }

    if (!key) {
        printk("key cannot be 0\n");
        return -EINVAL;
    }

    for (i = 0; i < SHM_SIZE; i++) {
        if (shm_list[i].key == key)
            return i;
    }

    page = get_free_page();
    if (!page) {
        printk("get_free_page failed!\n");
        return -ENOMEM;
    }

    for (i = 0; i < SHM_SIZE; i++) {
        if (shm_list[i].key == 0) {
            shm_list[i].key = key;
            shm_list[i].size = size;
            shm_list[i].page = page;
            return i;
        }
    }

    printk("shm list full!\n");
    return -1;
}

void* sys_shmat(int shmid)
{
    unsigned long data_base, brk;
    if (shmid < 0 || shmid >= SHM_SIZE || shm_list[shmid].page == 0 || shm_list[shmid].key <= 0)
        return (void *)-EINVAL;

    data_base = get_base(current->ldt[2]);
    brk = current->brk + data_base;
    current->brk += PAGE_SIZE;
    if (put_page(shm_list[shmid].page, brk) == 0)
        return (void*)-ENOMEM;
    
    return (void*)(current->brk - PAGE_SIZE);
}
