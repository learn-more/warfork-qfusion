#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
typedef PROCESS_INFORMATION ProcessType;
typedef HANDLE PipeType;
#define NULLPIPE NULL
#include <malloc.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>
typedef pid_t ProcessType;
typedef int PipeType;
#define NULLPIPE -1
#endif

/* Windows and Unix implementations of this stuff below. */
void fail(const char *err);
bool createPipes(PipeType *pPipeParentRead, PipeType *pPipeParentWrite,
                        PipeType *pPipeChildRead, PipeType *pPipeChildWrite);
bool setEnvVar(const char *key, const char *val);
bool launchChild(ProcessType *pid);
int closeProcess(ProcessType *pid);

int writePipe(PipeType fd, const void *buf, const unsigned int _len);
int readPipe(PipeType fd, void *buf, const unsigned int _len);
void closePipe(PipeType fd);
char *getEnvVar(const char *key, char *buf, const size_t buflen);
int pipeReady(PipeType fd);
