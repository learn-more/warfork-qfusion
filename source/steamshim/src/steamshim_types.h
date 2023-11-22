#ifndef STEAMSHIM_TYPES_H_
#define STEAMSHIM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AUTH_TICKET_MAXSIZE 1024
typedef struct {
  char pTicket[AUTH_TICKET_MAXSIZE];
  long long pcbTicket;
} SteamAuthTicket_t;

#endif
#ifdef __cplusplus
}
#endif
