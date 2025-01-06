#ifndef _HLE_CTF_H
#define _HLE_CTF_H

#include	"hle_gamemodes.h"
#include	"hle_utility.h"


enum CTF_INFO
{
	RED_SCORED = 0,
	RED_SAVED,
	RED_CAPTURED,
	BLUE_SCORED,
	BLUE_SAVED,
	BLUE_CAPTURED,
};


/*
class CBaseCtfBlueStart : public CPointEntity
{
public:
	void		KeyValue( KeyValueData *pkvd );
	BOOL		IsTriggered( CBaseEntity *pEntity );

private:
};

class CBaseCtfRedStart : public CPointEntity
{
public:
	void		KeyValue( KeyValueData *pkvd );
	BOOL		IsTriggered( CBaseEntity *pEntity );

private:
};
*/



class CGameModeCTF:  public CHalfLifeTeamplay
{

public:
	
//deathnotice
//IPointsForKill

	//virtual void PlayerSpawn(CBasePlayer *pPlayer);
	CGameModeCTF();
	virtual void ResetPlayerHud(CBasePlayer* pPlayer);
	virtual void SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget);
	virtual BOOL MatchInProgress(void);
	virtual void MatchStart(int nType);
	virtual BOOL CanStartObserver(CBasePlayer *pPlayer);
	virtual void Think();
	virtual BOOL IsTeamplay( void );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void CapturedFlag(CBasePlayer *pPlayer);
	virtual BOOL RestrictObserver( void );
	virtual const char *GetGameDescription( void ) { return "HLE CTF"; }  // this is the game name that gets seen in the server browser
	//virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual void SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever=NULL);

	//int m_nRedScore;
	//int m_nBlueScore;

protected:

	int m_nCaptures[7];

private:
//	int m_nNumber;
	//int m_nRedTeam;
	//int m_nBlueTeam;
//	bool m_DisableDeathMessages;
//	bool m_DisableDeathPenalty;
};


class CCtfTriggerKill: public CBaseEntity
{
	bool bActive;
public:
	CCtfTriggerKill();
	void Spawn();
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT TouchKill(CBaseEntity*);
	void KeyValue( KeyValueData *);
private:
	int  m_nZoneColor;

};


#endif

