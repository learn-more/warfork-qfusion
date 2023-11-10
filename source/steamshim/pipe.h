#ifndef _INCL_STEAMSHIM_PIPE_H_
#define _INCL_STEAMSHIM_PIPE_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
typedef HANDLE PipeType;
#define NULLPIPE NULL
typedef unsigned __int8 uint8;
typedef __int32 int32;
typedef unsigned __int64 uint64;
#include <stdio.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>

#ifndef __cplusplus
typedef uint8_t uint8;
typedef int32_t int32;
typedef uint64_t uint64;
#endif
typedef int PipeType;

#define NULLPIPE -1
#endif

#if DEBUGPIPE
void Com_Printf( const char *format, ... );
#define dbgpipe Com_Printf
#else
static inline void dbgpipe(const char *fmt, ...) {}
#endif


static PipeType GPipeRead = NULLPIPE;
static PipeType GPipeWrite = NULLPIPE;
typedef enum ShimCmd
{
    SHIMCMD_BYE,
    SHIMCMD_PUMP,
    SHIMCMD_REQUESTSTATS,
    SHIMCMD_STORESTATS,
    SHIMCMD_SETACHIEVEMENT,
    SHIMCMD_GETACHIEVEMENT,
    SHIMCMD_RESETSTATS,
    SHIMCMD_SETSTATI,
    SHIMCMD_GETSTATI,
    SHIMCMD_SETSTATF,
    SHIMCMD_GETSTATF,
    SHIMCMD_REQUESTSTEAMID,
    SHIMCMD_REQUESTPERSONANAME,
    SHIMCMD_SETRICHPRESENCE,
} ShimCmd;


#ifdef __cplusplus
extern "C"{
#endif
int writePipe(PipeType fd, const void *buf, const unsigned int _len);
int readPipe(PipeType fd, void *buf, const unsigned int _len);
void closePipe(PipeType fd);
char *getEnvVar(const char *key, char *buf, const size_t buflen);
int pipeReady(PipeType fd);
int writeThing(PipeType fd, const uint8 ev, const void *val, const size_t vallen, const int okay);
#ifdef __cplusplus
};
#endif

#endif

