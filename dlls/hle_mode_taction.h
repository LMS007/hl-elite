
#ifndef _TEAMACTION_H
#define _TEAMACTION_H

#include    "hle_utility.h"
#include	"hle_gamemodes.h"

#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS			6

class CHalfLifeTAction : public CHalfLifeTeamplay
{
public:


	virtual void MatchStart(int nType ) {};
	virtual BOOL MatchInProgress(void) {return FALSE;}
	virtual BOOL MatchMode() {return FALSE;}
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
    virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
    virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );
//	void UpdateGameMode( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return "HLE Team Action"; }  // this is the game name that gets seen in the server browser

};


#endif