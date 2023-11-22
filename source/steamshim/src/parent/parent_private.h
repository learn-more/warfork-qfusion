#define DEBUGPIPE 1

#if DEBUGPIPE
#define dbgpipe printf
#else
inline void dbgpipe(const char *fmt, ...) {}
#endif



extern int GArgc;
extern char **GArgv;
