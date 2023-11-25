#ifndef _INCL_STEAMSHIM_TYPES_H_
#define _INCL_STEAMSHIM_TYPES_H_


#define PIPEMESSAGE_MAX 1050
#define AUTH_TICKET_MAXSIZE 1024
typedef struct {
  char pTicket[AUTH_TICKET_MAXSIZE];
  long long pcbTicket;
} SteamAuthTicket_t;

#endif
