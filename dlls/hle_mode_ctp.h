#ifndef _HLE_CTP_H
#define _HLE_CTP_H


#include	"hle_gamemodes.h"


enum CTP_INFO
{
	RED_TAGGED = 0,
	BLUE_TAGGED,
	YELLOW_TAGGED,
	GREEN_TAGGED,
//	PURPLE_TAGGED,
//	ORANGE_TAGGED,
};


class CGameModeCTP: public CGameModeCTF
{

	bool m_bHasCtpFile;
	static ofstream g_OutCtpFile;
	ifstream m_InCtpFile;
	hudtextparms_t m_hudtInfo;
	CDelayTime m_dScore;
	bool m_bInitFlags;
	void UpKeep(CBasePlayer* pPlayer);
	void CleanUpTeam(int team);

public:

	CGameModeCTP();
	void SummonFlags();
	friend static void CreateCTPFile();
	friend static void CompleteCTPFile();
	void PlaceFlag(CBasePlayer*);
	void PlayerSpawn(CBasePlayer *pPlayer);
	BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	BOOL CanStartObserver(CBasePlayer *pPlayer);
	
	//void ResetPlayerHud(CBasePlayer* pPlayer);
	void SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget);
	void SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever=NULL);
	void Think();
	void PlayerThink(CBasePlayer* pPlayer);
	//int CountAllTeams();
//	edict_t *GetPlayerSpawnSpot( CBasePlayer *pPlayer );
	int ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	//int TeamWithFewestPlayers( void );
	void CapturedFlag(CBasePlayer *pPlayer);
	virtual const char *GetGameDescription( void ) { return "HLE CTP"; }  // this is the game name that gets seen in the server browser
//	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	void InitHUD( CBasePlayer *pPlayer );
	void ClientDisconnected( edict_t *pClient );

	
	int m_nScores[7];

	static int g_nCtpFlags[7];


private:

	//void ResetTeamColors(CBasePlayer*, int);
	int m_nNumber;
	int m_nTeamCount[7];

	bool m_DisableDeathMessages;
	bool m_DisableDeathPenalty;
};


#endif

