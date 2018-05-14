#include "fifo_seqnum.h"
#include <signal.h>

int main()
{
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response res;
    int seqNum = 0;
    umask(0);
    
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);

    serverFd = open(SERVER_FIFO, O_RDONLY);     // 将会被阻塞直至第一个客户端打开并写入数据
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    dummyFd = open(SERVER_FIFO, O_WRONLY);      // 确保所有客户端关闭写入端之后不会看到 EOF
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    // 忽略试图向一个没有读者的 FIFO 写入数据时产生的 SIGPIPE 信号，而是 write() 收到 EPIPE 错误
        errExit("signal");

    while(1)
    {
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) 
        {
            fprintf(stderr, "Error reading request, discard.\n");
            continue;
        }

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)req.pid);

        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1)
        {
            errExit("open %s", clientFifo);
            continue;
        }

        res.seqNum = seqNum;

        if (write(clientFd, &res, sizeof(struct response)) != sizeof(struct response))
            fprintf(stderr, "Error writing response. client: %s\n", clientFifo);

        if (close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen;
    }
}