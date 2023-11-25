#ifndef STEAMSHIM_CHILD_H
#define STEAMSHIM_CHILD_H

#include <stdint.h>
#include "../steamshim.h"
#include "../steamshim_types.h"
#ifdef __cplusplus
extern "C" {
#endif

int STEAMSHIM_init(void);  /* non-zero on success, zero on failure. */
void STEAMSHIM_deinit(void);
int STEAMSHIM_alive(void);
const STEAMSHIM_Event *STEAMSHIM_pump(void);
void STEAMSHIM_requestStats(void);
void STEAMSHIM_storeStats(void);
void STEAMSHIM_setAchievement(const char *name, const int enable);
void STEAMSHIM_getAchievement(const char *name);
void STEAMSHIM_resetStats(const int bAlsoAchievements);
void STEAMSHIM_setStatI(const char *name, const int _val);
void STEAMSHIM_getStatI(const char *name);
void STEAMSHIM_setStatF(const char *name, const float val);
void STEAMSHIM_getStatF(const char *name);
void STEAMSHIM_getSteamID();
void STEAMSHIM_getPersonaName();
void STEAMSHIM_setRichPresence(const char* key, const char* val);
void STEAMSHIM_getAuthSessionTicket();
void STEAMSHIM_beginAuthSession(uint64_t steamid, SteamAuthTicket_t* ticket);

#ifdef __cplusplus
}
#endif

#endif

