#include "svmsg_file.h"

#include <sys/msg.h>        // for: msgget()、msgctl()、msgsnd()、IPC_PRIVATE...
#include <sys/stat.h>       // for: S_IWUSR
#include <signal.h>         // for: sigaction()、SIGCHLD...
#include <sys/wait.h>       // for: waitpid()
#include <fcntl.h>          // for: open(), O_RDONLY

// 避免僵死进程的信号处理函数
static void grimReaper(int sig)
{
    int savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

static void serveRequest(const struct requestMsg* req)
{
    int fd;
    ssize_t numRead;
    struct responseMsg resp;

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1){
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        exit(EXIT_FAILURE);
    }

    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0)
        if (msgsnd(req->clientId, &resp, numRead, 0) == -1)
            break;

    resp.mtype = RESP_MT_END;
    msgsnd(req->clientId, &resp, 0, 0);
}

int main(int argc, char** argv)
{
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    int serverId;
    struct sigaction sa;

    serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverId == -1)
        errExit("msgget");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;   // 自动重启由信号处理器中断的系统调用
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    while(1){
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);    // 可能会阻塞，此时若有 SIGCHLD 信号会被中断，所以在 EINTR 时重启一下
        if (msgLen == -1){
            if(errno == EINTR)
                continue;
            errMsg("msgrcv");
            break;
        }

        pid = fork();
        if (pid == -1){
            errMsg("fork");
            break;
        }
        if (pid == 0){
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }
    }

    if (msgctl(serverId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
    exit(EXIT_SUCCESS);
}