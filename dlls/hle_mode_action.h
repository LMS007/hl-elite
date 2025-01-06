#ifndef _MODE_ACTION_H
#define _MODE_ACTION_H

#include	"hle_gamemodes.h"

class CGameModeAction : public CHalfLifeMultiplay
{
public:
	//CGameModeAction( );

    CGameModeAction();
	virtual BOOL MatchMode() {return FALSE;}
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
    //virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
    virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
    virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );
//	void UpdateGameMode( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return "HLE Action"; }  // this is the game name that gets seen in the server browser
	virtual void InitHUD( CBasePlayer *pl );

private:

//    int m_nServerMaps;
//    char **m_ppMapList;
//    BOOL m_bMapListInIt;
    //hudtextparms_t m_hudtAction;
};

#endif