#ifndef _MODE_TOURNEY_H
#define _MODE_TOURNEY_H

#include    "hle_utility.h"
#include	"hle_gamemodes.h"

class CGameModeTourney : public CGameModeDuel
{
private:

	int m_nFragsLeft;
	int m_nSpawnLimit;
	int m_nOldFragsLeft;
	bool m_bFirstSpawn;
	edict_t* m_pLeader;
	edict_t* m_pTrailer;

public:

    CGameModeTourney();
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
//	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	virtual const char* GetGameDescription( void ) { return "HLE Tournament"; }  // this is the game name that gets seen in the server browser
	virtual BOOL CanStartAutoObserver(CBasePlayer* pPlayer);
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
	virtual void SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever=NULL);
	virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual void TimeIsUp();

	
private:

	void UpdatePositions();
	void SendFragsLeft();

protected:

	virtual void StartRoundTime();
	virtual void SpawnValidPlayers();
	virtual void StartRound();
	virtual void EndRound();
	virtual bool IsOver();
};

#endif