/*
qfusion
Copyright (c) 2014, Victor Luchits, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
*/

#include "../gameshared/q_shared.h"
#include "../qcommon/qcommon.h"
#include "steamlib_local.h"
#include <steam/steam_api.h>

namespace WSWSTEAM
{
typedef void ( *auth_callback_t )( void *, size_t );

struct CSteamCallbacks {
	auth_callback_t m_callback;
	uint8 m_AuthTicket[1024];
	uint32 m_AuthTicketLength;
	HAuthTicket m_AuthTicketResult;

	CSteamCallbacks() : m_callback( NULL ), m_AuthTicketLength( 0 ), m_AuthTicketResult( k_HAuthTicketInvalid ) {}

	int GetAuthSessionTicket( void ( *callback )( void *, size_t ) )
	{
		m_callback = callback;
		m_AuthTicketResult = SteamUser()->GetAuthSessionTicket( m_AuthTicket, sizeof( m_AuthTicket ), &m_AuthTicketLength );
		return m_AuthTicketResult != k_HAuthTicketInvalid ? 1 : 0;
	}

	STEAM_CALLBACK( CSteamCallbacks, OnGetAuthSessionTicketResponse, GetAuthSessionTicketResponse_t );
	STEAM_CALLBACK( CSteamCallbacks, OnWorkshopItemInstalled, ItemInstalled_t );

	STEAM_CALLBACK( CSteamCallbacks, OnGameServerChangeRequested, GameServerChangeRequested_t);
};
static CSteamCallbacks *g_pCallbacks = nullptr;

void CSteamCallbacks::OnGetAuthSessionTicketResponse( GetAuthSessionTicketResponse_t *pParam )
{
	if( pParam->m_eResult != k_EResultOK )
		m_AuthTicketLength = 0;

	if (m_callback)
	{
		m_callback(m_AuthTicket, m_AuthTicketLength);
	}
}

void CSteamCallbacks::OnWorkshopItemInstalled( ItemInstalled_t *pParam )
{
	if( pParam->m_unAppID == SteamUtils()->GetAppID() ) {
		Com_Printf( "New workshop item installed, please restart!\n" );
	}
}

void CSteamCallbacks::OnGameServerChangeRequested( GameServerChangeRequested_t *pParam )
{
	char connectCommand[1024];
	if( pParam->m_rgchPassword[0] ) {
		char password[65] = { 0 };
		Info_CleanValue( pParam->m_rgchPassword, password, sizeof( password ) );
		Q_snprintfz( connectCommand, sizeof( connectCommand ), "connect \"%s@%s\"\n", password, pParam->m_rgchServer );
	} else {
		Q_snprintfz( connectCommand, sizeof( connectCommand ), "connect \"%s\"\n", pParam->m_rgchServer );
	}

	WSWSTEAM::GetSteamImport()->Cbuf_ExecuteText( EXEC_APPEND, connectCommand );
}

void AddUGCItem( PublishedFileId_t workshopItemID )
{
	uint32 unItemState = SteamUGC()->GetItemState( workshopItemID );
	if( !( unItemState & k_EItemStateInstalled ) )
		return;

	uint32 unTimeStamp = 0;
	uint64 unSizeOnDisk = 0;
	char szItemFolder[1024] = { 0 };

	if( !SteamUGC()->GetItemInstallInfo( workshopItemID, &unSizeOnDisk, szItemFolder, sizeof( szItemFolder ), &unTimeStamp ) ) {
		Com_Printf( S_COLOR_RED "Unable to retrieve info for %uLL\n", workshopItemID );
		return;
	}

	if( unItemState & k_EItemStateLegacyItem ) {
		// szItemFolder just points directly to the item for legacy items that were published with the RemoteStorage API.
		// figure out how to handle this later!
		Com_Printf( S_COLOR_RED "Skipping legacy item '%s'\n", szItemFolder );
		return;
	}

	Com_Printf( "Found workshop folder '%s'\n", szItemFolder );
	WSWSTEAM::GetSteamImport()->FS_AddExtraPK3Directory( szItemFolder );
}

static void AddWorkshopDirectories(void)
{
	// Enumerate installed UGC (User Generated Content)
	uint32 count = SteamUGC()->GetNumSubscribedItems();
	if (count > 0) {
		PublishedFileId_t* vecSubscribedItems = new PublishedFileId_t[count];

		uint32 num = SteamUGC()->GetSubscribedItems(vecSubscribedItems, count);
		if (num > count)
			num = count;

		for (uint32 iSubscribedItem = 0; iSubscribedItem < count; iSubscribedItem++) {
			AddUGCItem(vecSubscribedItems[iSubscribedItem]);
		}

		delete[] vecSubscribedItems;
	}
}

/*
 * SteamLib_API
 */
int SteamLib_API( void )
{
	return STEAMLIB_API_VERSION;
}

/*
 * SteamLib_SteamLib_InitAPI
 */
int SteamLib_Init( void )
{
	if( SteamAPI_Init() ) {

		// Register our steam callbacks
		g_pCallbacks = new CSteamCallbacks();

		// Register workshop items
		AddWorkshopDirectories();

		return 1;
	}

	return 0;
}

/*
 * SteamLib_RunFrame
 */
void SteamLib_RunFrame( void )
{
	SteamAPI_RunCallbacks();
}

/*
 * SteamLib_Shutdown
 */
void SteamLib_Shutdown( void )
{
	delete g_pCallbacks;
	g_pCallbacks = nullptr;
	SteamAPI_Shutdown();
}

/*
 * SteamLib_GetSteamID
 */
uint64_t SteamLib_GetSteamID( void )
{
	return SteamUser()->GetSteamID().ConvertToUint64();
}

/*
 * SteamLib_GetAuthSessionTicket
 */
int SteamLib_GetAuthSessionTicket( void ( *callback )( void *, size_t ) )
{
	return g_pCallbacks->GetAuthSessionTicket( callback );
}

/*
 * SteamLib_AdvertiseGame
 */
void SteamLib_AdvertiseGame( const uint8_t *ip, unsigned short port )
{
	uint32 unIPServer = 0;
	CSteamID steamIDGameServer;
	if( ip ) {
		unIPServer = ( uint32( ip[0] ) << 24 ) | ( uint32( ip[1] ) << 16 ) | ( uint32( ip[2] ) << 8 ) | ( uint32( ip[3] ) << 0 );
		steamIDGameServer = k_steamIDNonSteamGS;
	} else {
		port = 0;
		steamIDGameServer = k_steamIDNil;
	}

	SteamUser()->AdvertiseGame( steamIDGameServer, unIPServer, port );
}

/*
 * SteamLib_GetPersonaName
 */
void SteamLib_GetPersonaName( char *name, size_t namesize )
{
	if( namesize ) {
		name[0] = '\0';
		Q_strncpyz( name, SteamFriends()->GetPersonaName(), namesize );
	}
}

} // namespace WSWSTEAM
