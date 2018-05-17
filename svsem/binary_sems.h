#include "tlpi_hdr.h"

#include <sys/types.h> /* For portability */
#include <sys/sem.h>

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && \
    !defined(__sgi) && !defined(__APPLE__)
/* Some implementations already declare this union */

union semun { /* Used in calls to semctl() */
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};

#endif

extern Boolean bsUseSemUndo;
extern Boolean bsRetryOnEintr;

int initSemAvailable(int semId, int semNum);
int initSemInUse(int semId, int semNum);
int reserveSem(int semId, int semNum);
int releaseSem(int semId, int semNum);
