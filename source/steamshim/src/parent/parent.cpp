#include <cstddef>

#include "parent_private.h"
#include "../steamshim_private.h"
#include "../os.h"
#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

int GArgc = 0;
char **GArgv = NULL;


static void processCommands(PipeType pipeParentRead, PipeType pipeParentWrite)
{

  pipebuff_t buf;

  while (1){

    if (!buf.Recieve())
      return;

    // write(91,buf.buffer,10);
  }

} 

static bool initSteamworks(PipeType fd)
{
    // if (GServerType == STEAMGAMESERVER) {
    //     if (!SteamGameServer_Init(0, 27015, 0,eServerModeNoAuthentication,"0.0.0.0"))
    //         return 0;
    //     GSteamGameServer = SteamGameServer();
    //     if (!GSteamGameServer)
    //         return 0;
    //     
    // }else{
    //     // this can fail for many reasons:
    //     //  - you forgot a steam_appid.txt in the current working directory.
    //     //  - you don't have Steam running
    //     //  - you don't own the game listed in steam_appid.txt
    //     if (!SteamAPI_Init())
    //         return 0;
    //
    //     GSteamStats = SteamUserStats();
    //     GSteamUtils = SteamUtils();
    //     GSteamUser = SteamUser();
    //
    //     GAppID = GSteamUtils ? GSteamUtils->GetAppID() : 0;
	   //  GUserID = GSteamUser ? GSteamUser->GetSteamID().ConvertToUint64() : 0;
    // }
    //
    // GSteamBridge = new SteamBridge(fd);
    return 1;
} 

static bool setEnvironmentVars(PipeType pipeChildRead, PipeType pipeChildWrite)
{
    char buf[64];
    snprintf(buf, sizeof (buf), "%llu", (unsigned long long) pipeChildRead);
    if (!setEnvVar("STEAMSHIM_READHANDLE", buf))
        return false;

    snprintf(buf, sizeof (buf), "%llu", (unsigned long long) pipeChildWrite);
    if (!setEnvVar("STEAMSHIM_WRITEHANDLE", buf))
        return false;

    return true;
} // setEnvironmentVars


int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    GArgc = argc;
    GArgv = argv;


    PipeType pipeParentRead = NULLPIPE;
    PipeType pipeParentWrite = NULLPIPE;
    PipeType pipeChildRead = NULLPIPE;
    PipeType pipeChildWrite = NULLPIPE;
    ProcessType childPid;

    dbgpipe("Parent starting mainline.\n");

    // // temporary hack, make this better
    // if (strstr(*GArgv,"warfork_steam"))
    //     GServerType = STEAMGAMECLIENT;
    // else
    //     GServerType = STEAMGAMESERVER;

    if (!createPipes(&pipeParentRead, &pipeParentWrite, &pipeChildRead, &pipeChildWrite))
        fail("Failed to create application pipes");
    else if (!initSteamworks(pipeParentWrite))
        fail("Failed to initialize Steamworks");
    else if (!setEnvironmentVars(pipeChildRead, pipeChildWrite))
        fail("Failed to set environment variables");
    else if (!launchChild(&childPid))
        fail("Failed to launch application");

    // Close the ends of the pipes that the child will use; we don't need them.
    closePipe(pipeChildRead);
    closePipe(pipeChildWrite);
    pipeChildRead = pipeChildWrite = NULLPIPE;

    dbgpipe("Parent in command processing loop.\n");

    // Now, we block for instructions until the pipe fails (child closed it or
    //  terminated/crashed).
    processCommands(pipeParentRead, pipeParentWrite);

    dbgpipe("Parent shutting down.\n");

    // Close our ends of the pipes.
    // writeBye(pipeParentWrite);
    closePipe(pipeParentRead);
    closePipe(pipeParentWrite);

    // deinitSteamworks();

    dbgpipe("Parent waiting on child process.\n");

    // Wait for the child to terminate, close the child process handles.
    const int retval = closeProcess(&childPid);

    dbgpipe("Parent exiting mainline (child exit code %d).\n", retval);

    return retval;

}

