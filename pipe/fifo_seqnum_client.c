#include "fifo_seqnum.h"

char clientFifo[CLIENT_FIFO_NAME_LEN];

void removeFifo()
{
    unlink(clientFifo);
}

int main(int argc, char **argv)
{
    int serverFd, clientFd;
    struct request req;
    struct response res;

    if(argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len...]\n", argv[0]);

    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());

    umask(0);

    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        errExit("mkfifo %s", clientFifo);

    if (atexit(removeFifo) != 0)        // 在程序正常退出时执行 removeFifo()
        errExit("atexit");

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    req.pid = getpid();
    req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    if(write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
        errExit("Error writing request. server: %s", SERVER_FIFO);

    clientFd = open(clientFifo, O_RDONLY);      // 阻塞至写入端写入数据
    if (clientFd == -1)
        errExit("open %s", clientFifo);

    if(read(clientFd, &res, sizeof(struct response)) != sizeof(struct response))
        errExit("Error reading response. client: %s", clientFifo);

    fprintf(stdout, "response: %d\n", res.seqNum);

    if(close(clientFd) == -1)
        errMsg("close %s", clientFifo);

    if (close(serverFd) == -1)
        errMsg("close %s", SERVER_FIFO);

}