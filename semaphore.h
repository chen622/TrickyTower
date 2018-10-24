/*
 * \File
 * semaphore.h
 * \Brief
 * semaphore operation
 */
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__


union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int set_semvalue(int sem_id, int value);
void del_semvalue(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);

#endif
