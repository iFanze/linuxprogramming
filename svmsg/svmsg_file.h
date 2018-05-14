#include "tlpi_hdr.h"
#include <stddef.h>     // for: offsetof()
#include <limits.h>     // for: PATH_MAX

#define SERVER_KEY 0x1aaaaaa1

struct requestMsg
{
    long mtype;
    int clientId;
    char pathname[PATH_MAX];
};

#define REQ_MSG_SIZE (offsetof(struct requestMsg, pathname) - offsetof(struct requestMsg, clientId) + PATH_MAX)

#define RESP_MSG_SIZE 8192

struct responseMsg
{
    long mtype;
    char data[RESP_MSG_SIZE];
};

#define RESP_MT_FAILURE 1
#define RESP_MT_DATA 2
#define RESP_MT_END 3
