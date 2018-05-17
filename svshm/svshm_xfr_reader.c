#include "svshm_xfr.h"

int main(int argc, char **argv)
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    // union semun dummy;

    // semid = semget(SEM_KEY, 2, IPC_CREAT | IPC_PERMS);
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1)
        errExit("semget");
    // if (initSemAvailable(semid, WRITE_SEM) == -1)
    //     errExit("initSemAvailable");
    // if (initSemInUse(semid, READ_SEM) == -1)
    //     errExit("initSemInUse");

    // shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | IPC_PERMS);
    shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1)
        errExit("shmget");

    // shmp = shmat(shmid, NULL, 0);
    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *)-1)
        errExit("shmat");

    for (xfrs = 0, bytes = 0;; xfrs++)
    {
        if (reserveSem(semid, READ_SEM) == -1)
            errExit("reserveSem");

        if (shmp->cnt == 0)
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            fatal("partial/failed write");

        if (releaseSem(semid, WRITE_SEM) == -1)
            errExit("releaseSem");
    }


    if (shmdt(shmp) == -1)
        errExit("shmdt");
    if (releaseSem(semid, WRITE_SEM) == -1)
        errExit("releaseSem");
    
    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}