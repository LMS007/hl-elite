/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// teamplay_gamerules.h
//

#ifndef _TEAMMODE_H
#define _TEAMMODE_H

#include    "hle_utility.h"
#include	"hle_gamemodes.h"

#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS			6

#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH

class CHalfLifeTeamplay : public CHalfLifeMultiplay
{
public:
	CHalfLifeTeamplay();
	~CHalfLifeTeamplay();
	//virtual void ClientDisconnected( edict_t *pClient );
	virtual float DeathTime(){return 20;}
	virtual void ResetPlayerHud(CBasePlayer* pPlayer);
	virtual void SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget);
	virtual void SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever=NULL);
	//virtual void MatchStart(int nType );
	//virtual BOOL MatchInProgress(void);
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	//virtual void PlayerThink(CBasePlayer*);
	//virtual BOOL CanStartObserver(CBasePlayer *pPlayer);
	
	virtual BOOL CanStopObserver(CBasePlayer *pPlayer);
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );   
	virtual void ClientDisconnected( edict_t *pClient );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "HLE Teamplay"; }  // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( CBasePlayer* pPlayer );
	virtual int GetTeamIndex2( const char *pTeamName );
	virtual edict_t* GetRandomSpawn( CBasePlayer *pPlayer );
	void PlayerThink( CBasePlayer *pPlayer );
	virtual void StartObserver(CBasePlayer *pPlayer);


	virtual const char *GetIndexedTeamName( int teamIndex );
	virtual BOOL IsValidTeam( const char *pTeamName );
	virtual const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual int ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	virtual void CapturedFlag(CBasePlayer *pPlayer);
	virtual BOOL RestrictObserver();
	virtual void ChangePlayerModel( CBasePlayer *pPlayer, const char *pModelName, BOOL bKill );
	virtual void SetDefaultModel(CBasePlayer *pPlayer);
	virtual int CheckValidModel(char *pCurrentModel, char* pNewModel);
	virtual void SetDefaultTeam(CBasePlayer* pPlayer);
	virtual int	CheckValidTeam(char* pCurrentTeam, char* pNewTeam);
	//virtual BOOL CanStartAutoObserver(CBasePlayer* pPlayer);
	//virtual edict_t* GetPlayerSpawnSpot( CBasePlayer *pPlayer );
	virtual void  InitSpawns();
protected:

	SpawnPointsTeam m_pTeamSpawnPoints;
	virtual void ResetTeamColors(CBasePlayer*, int);
	virtual int TeamWithFewestPlayers( void );
	virtual void RecountTeams( bool bResendInfo = FALSE );

	
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH];

	int m_nMaxTeams;
	int		m_nNumber;
	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;
	BOOL m_teamLimit;				// This means the server set only some teams as valid
	
};


#endif