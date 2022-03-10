#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * Define THREAD_POOL_PARAM_T such that we can avoid a compiler
 * warning when we use the dispatch_*() functions below
 */
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/dispatch.h>

static resmgr_connect_funcs_t connect_funcs;
static resmgr_io_funcs_t io_funcs;
static iofunc_attr_t attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_read2(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
int getspeedometer(char *pspeed, size_t size);

static char *buffer = "powertrain\n";
char recvbuf[200];

int main(int argc, char **argv)
{
    /* declare variables we'll be using */
    thread_pool_attr_t pool_attr;
    resmgr_attr_t resmgr_attr;
    dispatch_t *dpp;
    thread_pool_t *tpp;
    int id;

    /* initialize dispatch interface */
    if ((dpp = dispatch_create()) == NULL)
    {
        fprintf(stderr, "%s: Unable to allocate dispatch handle.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize resource manager attributes */
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* initialize functions for handling messages */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.read = io_read;
    io_funcs.write = io_write;

    /* initialize attribute structure used by the device */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
    attr.nbytes = sizeof recvbuf; // strlen(buffer) + 1;

    /* attach our device name */
    id = resmgr_attach(dpp,               /* dispatch handle */
                       &resmgr_attr,      /* resource manager attrs */
                       "/dev/powertrain", /* device name */
                       _FTYPE_ANY,        /* open type */
                       0,                 /* flags */
                       &connect_funcs,    /* connect routines */
                       &io_funcs,         /* I/O routines */
                       &attr);            /* handle */
    if (id == -1)
    {
        fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize thread pool attributes */
    memset(&pool_attr, 0, sizeof pool_attr);
    pool_attr.handle = dpp;
    pool_attr.context_alloc = dispatch_context_alloc;
    pool_attr.block_func = dispatch_block;
    pool_attr.unblock_func = dispatch_unblock;
    pool_attr.handler_func = dispatch_handler;
    pool_attr.context_free = dispatch_context_free;
    pool_attr.lo_water = 2;
    pool_attr.hi_water = 4;
    pool_attr.increment = 1;
    pool_attr.maximum = 50;

    /* allocate a thread pool handle */
    if ((tpp = thread_pool_create(&pool_attr,
                                  POOL_FLAG_EXIT_SELF)) == NULL)
    {
        fprintf(stderr, "%s: Unable to initialize thread pool.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* Start the threads. This function doesn't return. */
    thread_pool_start(tpp);
    return EXIT_SUCCESS;
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    size_t nleft;
    size_t nbytes;
    int nparts;
    int status;

    if ((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return (ENOSYS);

    /*
     * On all reads (first and subsequent), calculate
     * how many bytes we can return to the client,
     * based upon the number of bytes available (nleft)
     * and the client's buffer size
     */

    nleft = ocb->attr->nbytes - ocb->offset;
    // nbytes = min(_IO_READ_GET_NBYTES(msg), nleft);
    nbytes = min(_IO_READ_GET_NBYTES(msg), nleft);

    printf(" %d, %d, %d\n", msg->i.nbytes, ocb->attr->nbytes, ocb->offset);
    printf("nbytes = %d, nleft = %d\n", nbytes, nleft);
    if (nbytes > 0)
    {
        // /* set up the return data IOV */
        // SETIOV(ctp->iov, buffer + ocb->offset, nbytes);

        // /* set up the number of bytes (returned by client's read()) */
        // _IO_SET_READ_NBYTES(ctp, nbytes);

        // /*
        //  * advance the offset by the number of bytes
        //  * returned to the client.
        //  */
        // ocb->offset += nbytes;

        // nparts = 1;

        int r = getspeedometer(recvbuf, sizeof recvbuf);
        if (r > 0)
        {
            printf("recvbuf = %s\n", recvbuf);
            SETIOV(ctp->iov, recvbuf + ocb->offset, nbytes);
            _IO_SET_READ_NBYTES(ctp, nbytes);
            ocb->offset += nbytes;
            nparts = 1;
        }
        else
        {
            _IO_SET_READ_NBYTES(ctp, 0);
            nparts = 0;
        }
    }
    else
    {
        printf("%d\n", nbytes);

        /*
         * they've asked for zero bytes or they've already previously
         * read everything
         */
        _IO_SET_READ_NBYTES(ctp, 0);
        nparts = 0;
    }

    /* mark the access time as invalid (we just accessed it) */

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_ATIME;

    return (_RESMGR_NPARTS(nparts));
}

int io_read2(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    int status;
    int nbytes;
    int nleft;

    if ((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return (ENOSYS);

    nleft = ocb->attr->nbytes - ocb->offset;

    nbytes = min(nleft, msg->i.nbytes);

    if (nbytes)
    {
        int r = getspeedometer(recvbuf, sizeof recvbuf);
        if (r > 0)
        {
            printf("recvbuf = %s, %d, %d, %d, 0x%x\n", recvbuf, r, nbytes, strlen(recvbuf), recvbuf);
            MsgReply(ctp->rcvid, nbytes, recvbuf + ocb->offset, nbytes);
            ocb->attr->flags |= IOFUNC_ATTR_ATIME | IOFUNC_ATTR_DIRTY_TIME;
            ocb->offset += nbytes;
        }
        else
        {
            printf("%d\n", r);
            MsgReply(ctp->rcvid, EOK, NULL, 0);
        }
    }
    else
    {
        MsgReply(ctp->rcvid, EOK, NULL, 0);
    }
    return (_RESMGR_NOREPLY);
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
    int status;
    char *buf;
    size_t nbytes;

    if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return (ENOSYS);

    /* Extract the length of the client's message. */
    nbytes = _IO_WRITE_GET_NBYTES(msg);

    /* Filter out malicious write requests that attempt to write more
    data than they provide in the message. */
    if (nbytes > (size_t)ctp->info.srcmsglen - (size_t)ctp->offset - sizeof(io_write_t))
    {
        return EBADMSG;
    }

    /* set up the number of bytes (returned by client's write()) */
    _IO_SET_WRITE_NBYTES(ctp, nbytes);

    buf = (char *)malloc(nbytes + 1);
    if (buf == NULL)
        return (ENOMEM);

    /*
     * Reread the data from the sender's message buffer.
     * We're not assuming that all of the data fit into the
     * resource manager library's receive buffer.
     */

    resmgr_msgread(ctp, buf, nbytes, sizeof(msg->i));
    buf[nbytes] = '\0'; /* just in case the text is not NULL terminated */
    printf("Received %zu bytes = '%s'\n", nbytes, buf);
    free(buf);

    if (nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS(0));
}

int getspeedometer(char *pspeed, size_t size)
{
    int ret = 0;
    if (pspeed == NULL)
    {
        ret = -9;
        goto END_OF_GETSPEEDOMETER;
    }

    // char recvbuf[200];
    memset(pspeed, 0, size);

    int sockfd = 0;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        // return -1;
        ret = -1;
        goto END_OF_GETSPEEDOMETER;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(3490);
    server.sin_addr.s_addr = inet_addr("192.168.179.129");
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect");
        // return 1;
        ret = -2;
        goto CLOSE_SOCKET;
    }

    if ((ret = recv(sockfd, pspeed, 200, 0)) <= 0)
    {
        perror("no result");
        ret = 0;
        goto CLOSE_SOCKET;
    }

    printf("recvbuf = %s, %d, %d, 0x%x\n", pspeed, ret, sizeof pspeed, pspeed);

CLOSE_SOCKET:
    close(sockfd);

END_OF_GETSPEEDOMETER:
    return ret;
}

int getodometer(char *podo)
{
    return 0;
}