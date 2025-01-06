/* example program, creates an internet "server" that is visible in ASE */


extern "C"
{
#include "ASEQuerySDK.h"
}


#ifdef _WIN32
#include <winsock.h>
#endif

int ASEmain(void)
{
#ifdef _WIN32 /* WinSock initialization */
        WSADATA WSAData;
        WSAStartup(MAKEWORD(1,1), &WSAData);
#endif

        ASEQuery_initialize(5000, 1, 0); /* joinport 5000, internet server, any network interface */

        while (1)
        {
#ifdef _WIN32
           Sleep(100);
#else
           usleep(100 * 1000);
#endif
           ASEQuery_check();
        }
        return 0;
}

void ASEQuery_wantstatus(void)
{
        ASEQuery_status("my server", "ffa", "dm6", "1.04", 0, 4, 16);
		//const char *hostname, const char *gametype, const char *mapname, const char *gamever, int password, int numplayers, int maxplayers
}

void ASEQuery_wantrules(void)
{
	//const char *key, const char *value
        ASEQuery_addrule("sv_gravity", "500");
        ASEQuery_addrule("team_red_score", "51");
        ASEQuery_addrule("fraglimit", "30");
}

void ASEQuery_wantplayers(void)
{
	//const char *name, const char *team, const char *model, const char *score, const char *ping, const char *time
        ASEQuery_addplayer("Killer", "red", "base", "6/66", "14", "34minutes");
        ASEQuery_addplayer("Terminator", "blue", 0, "4/2", "34", "3hours");
}
