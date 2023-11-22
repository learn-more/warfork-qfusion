#define DEBUGPIPE 1

#if DEBUGPIPE
#define dbgpipe printf
#else
static inline void dbgpipe(const char *fmt, ...) {}
#endif
