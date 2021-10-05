#include "ipc.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "IOMisc.h"

extern local_id curProcessId;

int send(void *self, local_id dst, const Message *msg)
{
    struct IOInfo *ioInfo = (struct IOInfo *) self;
    struct ProcessInfo currentProcess = ioInfo->process[curProcessId];

    //Checks if we send message to ourselves
    if (dst == curProcessId)
    {
        Log(Debug, "Process %d trying to send message to himself!\n", 1, getpid());
        return -1;
    }

    ssize_t writeAmount = write(currentProcess.pipe[dst][1], &msg->s_header, sizeof(msg->s_header));
    if (writeAmount != sizeof(msg->s_header))
    {
        Log(Debug, "Process %d didn't send message header to process with local id: %d!\n", 2, getpid(), dst);
        if (writeAmount == -1)
        {
            Log(Debug, "Process %d didn't send message header to process with local id: %d!\n Error occured: %s\n", 3, getpid(), dst, strerror(errno));
        }
        return -1;
    }

    writeAmount = write(currentProcess.pipe[dst][1], msg->s_payload, msg->s_header.s_payload_len);

    if (writeAmount != msg->s_header.s_payload_len)
    {
        Log(Debug, "Process %d didn't send message payload to process with local id: %d!\n", 2, getpid(), dst);
        if (writeAmount == -1)
        {
            Log(Debug, "Process %d didn't send message payload to process with local id: %d!\n Error occured: %s\n", 3, getpid(), dst, strerror(errno));
        }
        return -1;
    }

    return 0;
}

int receive(void * self, local_id from, Message * msg)
{
    struct IOInfo *ioInfo = (struct IOInfo *) self;
    struct ProcessInfo* processInfo = &ioInfo->process[from];

    //Checks if we are trying to receive message from ourselves
    if (from == curProcessId)
    {
        Log(Debug, "Process %d trying to receive message from himself!\n", 1, getpid());
        return -1;
    }

    ssize_t readAmount = read(processInfo->pipe[curProcessId][0], &msg->s_header, sizeof(msg->s_header));

    if (readAmount != sizeof(msg->s_header))
    {
        Log(Debug, "Process %d didn't receive message header from process with local id: %d!\n", 2, getpid(), from);
        if (readAmount == -1)
        {
            Log(Debug, "Process %d didn't receive message header from process with local id: %d! Error occured: %s\n", 3, getpid(), from, strerror(errno));
        }
        return -1;
    }

    readAmount = read(processInfo->pipe[curProcessId][0], msg->s_payload, msg->s_header.s_payload_len);

    if (readAmount != msg->s_header.s_payload_len)
    {
        Log(Debug, "Process %d didn't receive message payload from process with local id: %d!\n", 2, getpid(), from);
        if (readAmount == -1)
        {
            Log(Debug, "Process %d didn't receive message payload from process with local id: %d!\n Error occured: %s\n", 3, getpid(), from, strerror(errno));
        }
        return -1;
    }

    return 0;
}

