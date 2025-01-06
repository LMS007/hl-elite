#ifndef _MODE_PRAC_H
#define _MODE_PRAC_H

#include	"hle_gamemodes.h"

class CGameModePractice : public CHalfLifeMultiplay
{
public:

	virtual BOOL MatchMode() {return FALSE;}
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
    virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
    virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );
//	void UpdateGameMode( CBasePlayer *pPlayer );
	virtual void InitHUD( CBasePlayer *pl );
	virtual const char *GetGameDescription( void ) { return "HLE Practice"; }  // this is the game name that gets seen in the server browser
};
#endif
