/*
Copyright (C) 2014 Victor Luchits

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "../qcommon/qcommon.h"
#include "../steamshim/steamshim_child.h"

#define UNIMPLEMENTED_DBGBREAK()                                         \
	do {                                                                 \
		Com_Printf( S_COLOR_RED "%s is UNIMPLEMENTED\n", __FUNCTION__ ); \
		assert( 0 );                                                     \
	} while( 0 )

static void printEvent( const STEAMSHIM_Event *e )
{
	if( !e )
		return;

	Com_Printf( "CHILD EVENT: " );
	switch( e->type ) {
#define PRINTGOTEVENT( x )     \
	case SHIMEVENT_##x:        \
		Com_Printf( "%s(", #x ); \
		break
		PRINTGOTEVENT( BYE );
		PRINTGOTEVENT( STATSRECEIVED );
		PRINTGOTEVENT( STATSSTORED );
		PRINTGOTEVENT( SETACHIEVEMENT );
		PRINTGOTEVENT( GETACHIEVEMENT );
		PRINTGOTEVENT( STEAMIDRECIEVED );
		PRINTGOTEVENT( PERSONANAMERECIEVED );
		PRINTGOTEVENT( RESETSTATS );
		PRINTGOTEVENT( SETSTATI );
		PRINTGOTEVENT( GETSTATI );
		PRINTGOTEVENT( SETSTATF );
		PRINTGOTEVENT( GETSTATF );
#undef PRINTGOTEVENT
		default:
			Com_Printf( "UNKNOWN(" );
			break;
	} /* switch */

	Com_Printf( "%sokay, ival=%d, fval=%f, time=%llu, name='%s').\n", e->okay ? "" : "!", e->ivalue, e->fvalue, e->epochsecs, e->name );
} /* printEvent */

/*
* Steam_Init
*/
void Steam_Init( void )
{
	int r = STEAMSHIM_init();
	if( !r ) {
		Com_Printf( "Steam initialization failed.\n" );
		return;
	}

	STEAMSHIM_requestStats();
}

/*
* Steam_RunFrame
*/
void Steam_RunFrame( void )
{
	const STEAMSHIM_Event *evt = STEAMSHIM_pump();
	if( evt ) {
		printEvent( evt );
	}
}

/*
* Steam_Shutdown
*/
void Steam_Shutdown( void )
{
	STEAMSHIM_deinit();
}

/*
* Steam_GetSteamID
*/
uint64_t Steam_GetSteamID( void )
{
	STEAMSHIM_getSteamID();
	const STEAMSHIM_Event *evt;
	while( !( evt = STEAMSHIM_pump() ) ) {
	}

	return evt->epochsecs;
}

/*
* Steam_GetAuthSessionTicket
*/
int Steam_GetAuthSessionTicket( void ( *callback )( void *, size_t ) )
{
	// UNIMPLEMENTED_DBGBREAK();
	return 0;
}

/*
* Steam_AdvertiseGame
*/
void Steam_AdvertiseGame( const uint8_t *ip, unsigned short port )
{
	UNIMPLEMENTED_DBGBREAK();
}

/*
* Steam_GetPersonaName
*/
void Steam_GetPersonaName( char *name, size_t namesize )
{
	if( !namesize ) {
		return;
	}
	STEAMSHIM_getPersonaName();
	const STEAMSHIM_Event *evt;
	while( !( evt = STEAMSHIM_pump() ) ) {
	}
	strncpy(name, evt->name,namesize);
}

/*
* Steam_SetRichPresence
*/
void Steam_SetRichPresence( const char *key, const char *val )
{
	STEAMSHIM_setRichPresence(key, val);
}
