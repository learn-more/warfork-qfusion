#include "pipe.h"
#include <string.h>
#include <assert.h>

#ifdef _WIN32

int pipeReady(PipeType fd)
{
    DWORD avail = 0;
    return (PeekNamedPipe(fd, NULL, 0, NULL, &avail, NULL) && (avail > 0));
} /* pipeReady */

int writePipe(PipeType fd, const void *buf, const unsigned int _len)
{
    const DWORD len = (DWORD) _len;
    DWORD bw = 0;
    return ((WriteFile(fd, buf, len, &bw, NULL) != 0) && (bw == len));
} /* writePipe */

int readPipe(PipeType fd, void *buf, const unsigned int _len)
{
    const DWORD len = (DWORD) _len;
    DWORD br = 0;
    return ReadFile(fd, buf, len, &br, NULL) ? (int) br : -1;
} /* readPipe */

void closePipe(PipeType fd)
{
    CloseHandle(fd);
} /* closePipe */

char *getEnvVar(const char *key, char *buf, const size_t _buflen)
{
    const DWORD buflen = (DWORD) _buflen;
    const DWORD rc = GetEnvironmentVariableA(key, buf, buflen);
    /* rc doesn't count null char, hence "<". */
    return ((rc > 0) && (rc < buflen)) ? buf : NULL;
} /* getEnvVar */

#else

int pipeReady(PipeType fd)
{
    int rc;
    struct pollfd pfd = { fd, POLLIN | POLLERR | POLLHUP, 0 };
    while (((rc = poll(&pfd, 1, 0)) == -1) && (errno == EINTR)) { /*spin*/ }
    return (rc == 1);
} /* pipeReady */

int writePipe(PipeType fd, const void *buf, const unsigned int _len)
{
    const ssize_t len = (ssize_t) _len;
    ssize_t bw;
    while (((bw = write(fd, buf, len)) == -1) && (errno == EINTR)) { /*spin*/ }
    return (bw == len);
} /* writePipe */

int readPipe(PipeType fd, void *buf, const unsigned int _len)
{
    const ssize_t len = (ssize_t) _len;
    ssize_t br;
    while (((br = read(fd, buf, len)) == -1) && (errno == EINTR)) { /*spin*/ }
    return (int) br;
} /* readPipe */

void closePipe(PipeType fd)
{
    close(fd);
} /* closePipe */

char *getEnvVar(const char *key, char *buf, const size_t buflen)
{
    const char *envr = getenv(key);
    if (!envr || (strlen(envr) >= buflen))
        return NULL;
    strcpy(buf, envr);
    return buf;
} /* getEnvVar */

#endif


int write1ByteCmd(const uint8 b1)
{
    uint8 buf[sizeof(int)+1];
    *(int*)buf = 1;
    buf[sizeof(int)] = b1;
    return writePipe(GPipeWrite, buf, sizeof (buf));
} /* write1ByteCmd */

int write2ByteCmd(const uint8 b1, const uint8 b2)
{
    const uint8 buf[] = { 2, b1, b2 };
    return writePipe(GPipeWrite, buf, sizeof (buf));
} /* write2ByteCmd */

int writeBye(void)
{
    dbgpipe("Child sending SHIMCMD_BYE().\n");
    return write1ByteCmd(SHIMCMD_BYE);
} // writeBye

int writeThing(PipeType fd, const uint8 ev, const void *val, const size_t vallen, const int okay){
    uint8 buf[MAX_BUFFSIZE+sizeof(ev)+sizeof(okay)+sizeof(vallen)];
    uint8 *ptr = buf+sizeof(int);
    *(ptr++) = (uint8) ev;
    *(ptr++) = okay ? 1 : 0;
    memcpy(ptr, val, vallen);
    ptr += vallen;
    *(int*)buf = (int) ((ptr-1) - buf);

    return writePipe(fd, buf, *(int*)buf + sizeof(int));
}


// pipe buffer manipulation stuff
static pipebuff_t pipeSendBuffer;
void PIPE_Init()
{
    pipeSendBuffer.cursize = 0;
}

void PIPE_WriteData(void *val, size_t vallen)
{
    assert(pipeSendBuffer.cursize + vallen < MAX_BUFFSIZE);
    memcpy(pipeSendBuffer.data+pipeSendBuffer.cursize, val, vallen);
    pipeSendBuffer.cursize += vallen;
}

void PIPE_WriteInt(int val){
    PIPE_WriteData(&val,sizeof(val));
}

void PIPE_WriteFloat(float val){
    PIPE_WriteData(&val,sizeof(val));
}

void PIPE_WriteLong(long long val){
    PIPE_WriteData(&val,sizeof(val));
}

void PIPE_SendEvt(PipeType fd, const uint8 ev, const int okay){
    printf("AAAAAAAAAAAAAAAAAAAAA%llu\n",pipeSendBuffer.cursize);
    writeThing(fd, ev, pipeSendBuffer.data, pipeSendBuffer.cursize, okay);
}

int PIPE_SendCmd(const uint8 cmd){
    uint8 buf[MAX_BUFFSIZE+sizeof(cmd)];
    uint8 *ptr = buf+1;
    *(ptr++) = (uint8) cmd;
    memcpy(ptr, pipeSendBuffer.data, pipeSendBuffer.cursize+1);

    buf[0] = (uint8) pipeSendBuffer.cursize+1;
    return writePipe(GPipeWrite, buf, buf[0] + 1);
}


static pipebuff_t *pipeReadBuffer;
void PIPE_Read(pipebuff_t *buf){
    pipeReadBuffer = buf;
}

void* PIPE_ReadData(size_t vallen){
    assert(pipeReadBuffer->cursize + vallen < MAX_BUFFSIZE);
    void* val = pipeReadBuffer->data+pipeReadBuffer->cursize;
    pipeReadBuffer->cursize+=vallen;
    return val;
}

int PIPE_ReadInt(){
    return *(int*)PIPE_ReadData(sizeof(int));
}

float PIPE_ReadFloat(){
    return *(float*)PIPE_ReadData(sizeof(float));
}

long long PIPE_ReadLong(){
    return *(long long*)PIPE_ReadData(sizeof(long long));
}
