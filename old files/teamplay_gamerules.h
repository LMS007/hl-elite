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
/*
#include    "hle_utility.h"
#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS			32

#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH

class CHalfLifeTeamplay : public CHalfLifeMultiplay
{
public:
	CHalfLifeTeamplay();
	
	virtual void ClientDisconnected( edict_t *pClient );
	virtual ~CHalfLifeTeamplay();
	virtual void ResetPlayerHud(CBasePlayer* pPlayer);
	virtual void SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget);
	virtual void MatchStart(int nType );
	virtual BOOL MatchInProgress(void);
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer*);
	virtual BOOL CanStartObserver(CBasePlayer *pPlayer);
	virtual BOOL CanStopObserver(CBasePlayer *pPlayer);
	//virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );   
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "Team FFA"; }  // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( const char *pTeamName );
	virtual const char *GetIndexedTeamName( int teamIndex );
	virtual BOOL IsValidTeam( const char *pTeamName );
	const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	virtual void CapturedFlag(CBasePlayer *pPlayer);
	virtual BOOL RestrictObserver();
	void ChangePlayerModel( CBasePlayer *pPlayer, const char *pModelName, BOOL bKill );
	void SetDefaultModel(CBasePlayer *pPlayer);
	int CheckValidModel(char *pCurrentModel, char* pNewModel);
	void SetDefaultTeam(CBasePlayer* pPlayer);
	int	CheckValidTeam(char* pCurrentTeam, char* pNewTeam);
	BOOL CanStartAutoObserver(CBasePlayer* pPlayer);


private:

	void ResetTeamColors(CBasePlayer*, int);
	CMatchStart m_MatchStart;
	int		m_nNumber;
	CEntityList allowedPlayers;
	CEntityList disallowedPlayers;
	void RecountTeams( bool bResendInfo = FALSE );
	int TeamWithFewestPlayers( void );

	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;
	BOOL m_teamLimit;				// This means the server set only some teams as valid
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH];
};
*/