#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"hle_gamemodes.h"
#include	"voice_gamemgr.h"

#include	"game.h"
#include	"hle_flag.h"
#include	"hle_vote.h"

#define BLUE_TEAM	1
#define RED_TEAM	2
#define GREEN_TEAM	4
#define YELLOW_TEAM	8	
#define PURPLE_TEAM	16
#define ORANGE_TEAM	32


#define UPKEEP_TIME 1.5
#define LIFE_DECLINE 1
#define ARMOR_DECLINE 1
#define LIFE_BONUS 10;
#define ARMOR_BONUS 10;

extern CVoiceGameMgr	g_VoiceGameMgr;

extern int gmsgGameMode;
extern int gmsgReset;
extern int gmsgHudStatus;
extern int gmsgCap;
extern int gmsgTimer;
extern int gmsgTeamScores;
extern int gmsgIconInfo;
extern int gmsgRadar;
extern int g_bPlayLongjumpSound;
extern int gmsgBonus;
extern int gmsgTeamInfo;
extern int gmsgAnounce;

extern int num_teams;
extern char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
extern int	color_order[MAX_TEAMS];


CGameModeTLMS::CGameModeTLMS()
{
	//m_bJustConnected = true;
	m_bInProgress = false;
	m_bStartSequence = false;
	m_nNumber = 0;
	//m_bDeux = false;
	m_bAllowRestrictedSpawning = false;	
	m_nLTS = 0;
	

	m_nBonus = DUEL_BONUS_NONE;


	m_hMessage.x            = 0.45f;
    m_hMessage.y            = 0.4f;
    m_hMessage.a1           = 0.0f;
    m_hMessage.a2           = 0.0f;

    m_hMessage.b1           = 255.0f;
    m_hMessage.g1           = 185.0f;
    m_hMessage.r1           = 120.0f;
	
	m_hMessage.b2           = 255.0f;
	m_hMessage.g2           = 255.0f;
    m_hMessage.r2           = 255.0f;
    
	m_hMessage.channel      = 4;
    m_hMessage.effect       = 2;
    m_hMessage.fxTime       = 0.5;
    m_hMessage.fadeinTime   = 0.02;
    m_hMessage.fadeoutTime  = 0.5;    
    m_hMessage.holdTime     = 4.0;

	m_hInfo.x            = 0.45;
    m_hInfo.y            = 0.2f;
    m_hInfo.a1           = 0.0f;
    m_hInfo.a2           = 0.0f;

    m_hInfo.b1           = 255.0f;
    m_hInfo.g1           = 255.0f;
    m_hInfo.r1           = 255.0f;
	
	m_hInfo.b2           = 0.0f;
	m_hInfo.g2           = 0.0f;
    m_hInfo.r2           = 0.0f;
    
	m_hInfo.channel      = 4;
    m_hInfo.effect       = 2;
    m_hInfo.fxTime       = 0.5;
    m_hInfo.fadeinTime   = 0.02;
    m_hInfo.fadeoutTime  = 0.5;    
    m_hInfo.holdTime     = 4.0;

	for(int i = 1; i < MAX_TEAMS+1; i++)
	{
		m_nTeamWins[i] = 0;
		m_nTeamLosses[i]=0;
	}
}

	

BOOL CGameModeTLMS :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if(g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
		return TRUE;

	else if(FStrEq(pcmd, "changeteam"))
	{
		int changed = 0;
		if(CMD_ARGC()==2)
		{
			changed = ChangePlayerTeam(pPlayer, CMD_ARGV(1), TRUE, TRUE);
		}
		else
		{
			changed = ChangePlayerTeam(pPlayer, "auto", TRUE, TRUE);
		}
		
		if(!pPlayer->m_isSpectator && changed)
		{
			pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, 3);
		}

		MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
			WRITE_BYTE( pPlayer->entindex() );
			WRITE_BYTE(	g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)+1 );
			WRITE_BYTE( pPlayer->m_isSpectator );
			WRITE_BYTE(	pPlayer->m_bIsRemoved );
			WRITE_BYTE(	0 );
		MESSAGE_END();


		UpdateRound();
		int nTeam = 0;
		const char* pTeamName = pPlayer->m_szTeamName;
		if ( pTeamName && *pTeamName != 0 )
		{
			for(int i = 0; i < num_teams; i++) 
			{
				if(!stricmp(pTeamName, team_names[i]))
				{
					nTeam = color_order[i]+1;
					break;
				}
			}
		}	
		if(nTeam)
			ResetTeamColors(pPlayer, nTeam);		

		char buf[128];
		*buf=NULL;
		sprintf(buf, "You will spawn next round");
		UTIL_HudMessage(pPlayer, m_hInfo, buf);
		return TRUE;
	}

	return FALSE;
}


BOOL CGameModeTLMS::CanStartObserver(CBasePlayer *pPlayer)
{
	if(!pPlayer->m_isSpectator)
	{
		bool inGame = IsInRound(pPlayer->edict());
		pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
		if(inGame)	
			UpdateRound();
		
		
	}
	return TRUE;
}

BOOL CGameModeTLMS::CanStopObserver(CBasePlayer *pPlayer)
{
	if(CheckValidTeam(pPlayer->m_szTeamName, pPlayer->m_szTeamName) == -1)
	{
		pPlayer->ShowVGUIMenu(2);
		*pPlayer->m_szTeamName=NULL; 
	}
	return FALSE;
}


void CGameModeTLMS::MatchStart(int nType)
{
	
}

BOOL CGameModeTLMS::MatchInProgress(void)
{
	return FALSE;
}


void CGameModeTLMS::Think()
{
	if(m_bStartSequence)
	{
		if(m_dStartMessage.TimeUp())
		{
			m_dStartMessage.ClearTime();
			VersesMessage();
		}

		if(m_dStartRound.TimeUp())
		{
			m_dStartRound.AddTime(1);// add 1 second for the next count

			{
				SendCountDown(MAX_SPRITE_NUMBER - m_nNumber);
				m_nNumber++;

				if(m_nNumber > MAX_SPRITE_NUMBER)
				{
					RemoveAllEntites();
					ToggleSpawningRestriction(); // turn on selected spawning
					SpawnValidPlayers(); // spawn the players up for this round
 					ToggleSpawningRestriction();  // turn off selected spawning, no one can spawn now
					RespawnAllEntites();
					m_bStartSequence = false;
					m_dStartRound.ClearTime();
					StartRoundTime();
				}
			}
		}
	}
	else 
	{
		if(m_dRoundTimer.TimeUp())			
		{
			TimeIsUp();
		}
		if(m_dEndMessage.TimeUp())
		{
 			AnounceOutcome();
			m_dEndMessage.ClearTime();
			EndRound();
		}

	}
	

	//if(m_dUpkeep.TimeUp())
	{
	//	UpKeep();	
	//	m_dUpkeep.AddTime(UPKEEP_TIME);
	}

	for(int i = 1; i < MAX_TEAMS+1; i++)
	{
		if(m_nTeamWins[i] >= winlimit.value && winlimit.value !=0)
		{
 			GoToIntermission();
		}
	}

	CHalfLifeMultiplay::Think();
}


void CGameModeTLMS::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	DeathNotice( pVictim, pKiller, pInflictor );

	pVictim->m_iDeaths += 1;

	FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );
	CBasePlayer *peKiller = NULL;
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );
	if ( ktmp && (ktmp->Classify() == CLASS_PLAYER) )
		peKiller = (CBasePlayer*)ktmp;

	if ( pVictim->pev == pKiller )
	{
		// killed self do nothign for now
	}

	else if ( ktmp && ktmp->IsPlayer() )
	{
		pKiller->frags += IPointsForKill( peKiller, pVictim );
		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
		peKiller->m_nStreak++;

		m_nBonus = -1;
		int klr_bonus = -1;  // we set the killer bonus sprite lower so it does not obstruct his vision because hes still playing

		if(peKiller->m_nStreak==m_nSpawnedPlayers-1 && m_nSpawnedPlayers > 3) // ANAINILATION
			m_nBonus = 5;

		else if((pKiller->health >= 100) && (pKiller->armorvalue >= 100)) // DUEL_BONUS_FLAWLESS
		{
			m_nBonus = 1;
			klr_bonus = 9;
		}

		else if(pKiller->armorvalue >= 100) // DUEL_BONUS_OWNED
		{
			m_nBonus = 0;
			klr_bonus = 8;
		}

		else if(pKiller->armorvalue >= 90) // DUEL_BONUS_PUNISH
		{
			m_nBonus = 3;
			klr_bonus = 10;
		}
		
		else if(pKiller->armorvalue <= 0 && pKiller->health < 10) // DUEL_BONUS_LUCK
		{
			m_nBonus = 4;
			klr_bonus =11;
		}
		
		if(m_nBonus>=0 && m_nBonus<=5 )
		{
			// send to killer
			MESSAGE_BEGIN( MSG_ONE, gmsgBonus, NULL,  peKiller->edict());
			WRITE_BYTE(klr_bonus);
			MESSAGE_END();
			
			// send to killer's observers
			CEntityNode* pClient = peKiller->m_lObservers.head();
			while(pClient)
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgBonus, NULL,  pClient->m_pEntity);
				WRITE_BYTE(klr_bonus);
				MESSAGE_END();
				pClient = pClient->m_pNext;
			}
				// send to victim
			MESSAGE_BEGIN( MSG_ONE, gmsgBonus, NULL,  pVictim->edict());
			WRITE_BYTE(m_nBonus);
			MESSAGE_END();
			
			// send to victim's observers
			pClient = pVictim->m_lObservers.head();
			while(pClient)
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgBonus, NULL,  pClient->m_pEntity);
				WRITE_BYTE(m_nBonus);
				MESSAGE_END();
				pClient = pClient->m_pNext;
			}
		}
		GivePlayerBonus(GetClassPtr((CBasePlayer *)pKiller));
	}

	MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pVictim->edict() );  
		WRITE_SHORT(0);
		WRITE_SHORT(0);
	MESSAGE_END();

	CEntityNode* pClient = pVictim->m_lObservers.head();
	while(pClient)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pClient->m_pEntity );
			WRITE_SHORT(0);
			WRITE_SHORT(0);
		MESSAGE_END();
		pClient = pClient->m_pNext;
	}

	SendScoreInfo(pVictim);

	// killers score, if it's a player
	CBaseEntity *ep = CBaseEntity::Instance( pKiller );
	if ( ep && ep->Classify() == CLASS_PLAYER )
	{
		CBasePlayer *PK = (CBasePlayer*)ep;
		SendScoreInfo(PK);

		// let the killer paint another decal as soon as he'd like.
		PK->m_flNextDecalTime = gpGlobals->time;
	}
	
#ifndef HLDEMO_BUILD
	if ( pVictim->HasNamedPlayerItem("weapon_satchel") )
	{
		DeactivateSatchels( pVictim );
	}
#endif

	UpdateRound();

	RecountTeams();

}



void CGameModeTLMS::ResumePlayer(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsRemoved = false;

	UpdateRound();

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( pPlayer->entindex() );
		WRITE_BYTE(	g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)+1 );
		WRITE_BYTE( pPlayer->m_isSpectator );
		WRITE_BYTE(	0 );
		WRITE_BYTE(	0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	WRITE_BYTE( 3 );
	MESSAGE_END();
}

void CGameModeTLMS::RemovePlayer(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsRemoved = true;

	CanStartObserver(pPlayer);

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( pPlayer->entindex() );
		WRITE_BYTE(	g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)+1 );
		WRITE_BYTE( pPlayer->m_isSpectator );
		WRITE_BYTE(	1 );
		WRITE_BYTE(	0 );
	MESSAGE_END();
	
	UpdateRound();

	MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	WRITE_BYTE( 1 );
	MESSAGE_END();
}

int CGameModeTLMS::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CGameModeTLMS::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

void CGameModeTLMS::PlayerSpawn( CBasePlayer *pPlayer )
{
	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	pPlayer->m_iCaptures = 0;// for LMS and LTS we will use this value as the number of frags per round.
	//if(!m_bInProgress)
		//UpdateRound();
	///////////////////////
	///START SPAWN CHECK///
	///////////////////////
	pPlayer->m_bUnSpawn = true;
	if(!m_bAllowRestrictedSpawning)	
		return;
	else
		pPlayer->m_bUnSpawn = false;

	/////////////////////
	///END SPAWN CHECK///
	/////////////////////

	if(radar.value)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, pPlayer->edict() );   // try to show the radar
			WRITE_SHORT( pPlayer->entindex() );
			WRITE_BYTE( 1 );
		MESSAGE_END();
	}

	MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pPlayer->edict() );  
		WRITE_SHORT(0);
		WRITE_SHORT(0);
	MESSAGE_END();

	CEntityNode* pClient = pPlayer->m_lObservers.head();
	while(pClient)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pClient->m_pEntity );
			WRITE_SHORT(0);
			WRITE_SHORT(0);
		MESSAGE_END();
		pClient = pClient->m_pNext;
	}

	if(spawnform.value <=0) 
	{
		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveAmmo( 68, "9mm", _9MM_MAX_CARRY );
	}
	else
	{
		g_bPlayLongjumpSound =false;
		ArmPlayer(pPlayer);
		LoadPlayer(pPlayer);
		FillPlayer(pPlayer);
		g_bPlayLongjumpSound =true;	
	}

	CTeamIndicator::Create(pPlayer);
}

void CGameModeTLMS::ClientDisconnected(edict_t* pEntity)
{
	CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&pEntity->v);

	pPlayer->m_bIsRemoved = true;
    UpdateRound();

	CHalfLifeTeamplay::ClientDisconnected(pEntity);
}


void CGameModeTLMS::TimeIsUp()
{
	m_nLTS = NULL;
	//m_nResult = RESULT_DRAW;

	CBaseEntity* pEnt;
	CBasePlayer* pPlayer;

	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			pPlayer = (CBasePlayer *)pEnt;
			if(pPlayer->IsAlive())
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, 3);
		}
	}

	AnounceOutcome();
	m_dRoundTimer.ClearTime();
	m_dEndMessage.ClearTime();
	EndRound();
}

void CGameModeTLMS::AnounceOutcome()  // UNDONE
{
	m_dEndMessage.ClearTime();
	char buf[64];
	char ValidTeams;
	ValidTeams=0;

	*buf=NULL;


	if(!m_nLTS) // no team won
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
			WRITE_BYTE(2);
			WRITE_BYTE(1);
			WRITE_BYTE(0);
		MESSAGE_END();
		//strcpy(buf, "Draw");
	}
	else
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
			if ( pEnt && pEnt->IsPlayer( ) )
			{
				CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
				pPlayer->pev->takedamage = DAMAGE_NO; // no more shooting at this point	
			}
		}
		
		MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
			WRITE_BYTE(1);
			WRITE_BYTE(m_nLTS);
			WRITE_BYTE(0);
		MESSAGE_END();

		/*switch(m_nLTS)
		{
		case BLUE:
			
			m_hMessage.r1           = 0.0f;
			m_hMessage.g1           = 100.0f;
			m_hMessage.b1           = 255.0f;
			
			strcpy(buf, "Blue is the winner\n");
			break;
		case RED:
			m_hMessage.r1           = 255.0f;
			m_hMessage.g1           = 0.0f;
			m_hMessage.b1           = 0.0f;
			strcpy(buf, "Red is the winner\n");
			break;
		case GREEN:
			m_hMessage.r1           = 0.0f;
			m_hMessage.g1           = 255.0f;
			m_hMessage.b1           = 0.0f;
			strcpy(buf, "Green is the winner\n");
			break;
		case YELLOW:
			m_hMessage.r1           = 255.0f;
			m_hMessage.g1           = 255.0f;
			m_hMessage.b1           = 0.0f;
			strcpy(buf, "Yellow is the winner\n");
			break;
		case PURPLE:
			m_hMessage.r1           = 200.0f;
			m_hMessage.g1           = 0.0f;
			m_hMessage.b1           = 255.0f;
			strcpy(buf, "Purple is the winner\n");
			break;
		case ORANGE:
			m_hMessage.r1           = 255.0f;
			m_hMessage.g1           = 100.0f;
			m_hMessage.b1           = 0.0f;
			strcpy(buf, "Orange is the winner\n");
			break;
		}
		
		m_nTeamWins[m_nLTS]++;*/

		MESSAGE_BEGIN( MSG_ALL, gmsgTeamScores );
			WRITE_BYTE( m_nLTS );
			WRITE_BYTE( m_nTeamWins[m_nLTS] );
			WRITE_BYTE( 0 );
		MESSAGE_END();
		
	}

	float fLength = (float)strlen(buf);
	float fOffset = fLength*=0.0035;
	m_hMessage.x = 0.5-fOffset;
	UTIL_HudMessageAll(m_hMessage, buf);
}

void CGameModeTLMS::StartRoundTime()
{
	m_nRoundTime = roundtime.value;
	if(m_nRoundTime >999)
		m_nRoundTime=999;
	else if(m_nRoundTime<0)
		m_nRoundTime = 0;
	m_dRoundTimer.AddTime(m_nRoundTime);
	
	//m_dUpkeep.AddTime(UPKEEP_TIME);
	MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
		WRITE_SHORT (m_nRoundTime); // send time left, not time limit
	MESSAGE_END();
}


void CGameModeTLMS::VersesMessage()
{
	/*m_hMessage.r1           = 255.0f;
	m_hMessage.g1           = 255.0f;
	m_hMessage.b1           = 255.0f;
	char buf[128];
	*buf=NULL;
	sprintf(buf, "Last Team Standing Wins");
	UTIL_HudMessageAll(m_hMessage, buf);*/

	MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
		WRITE_BYTE(2);
		WRITE_BYTE(0);
		WRITE_BYTE(0);
	MESSAGE_END();


}


bool CGameModeTLMS::MinimumTeamsConnected()
{
	int CurrentTeam= 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer* pPlayer = (CBasePlayer*)pEnt;
			if(!pPlayer->m_bIsRemoved)
			{
				/*if(CurrentTeam == 0)
					CurrentTeam =  GetTeamIndex3(pPlayer) +1; // get the first team, it does not matter what it is
				else
				{
					if(CurrentTeam !=  GetTeamIndex3(pPlayer) +1) // compare this team with all other teams, its we have 1 other team, we can start
						return true;
				}*/
				int index = GetTeamIndex2(pPlayer->m_szTeamName)+1;
				if(index == 0)
					continue;
				if(CurrentTeam == 0)
					CurrentTeam =  index;
				else if(CurrentTeam !=  index)
					return true;
			}
		}
	}
	return false;
}
bool CGameModeTLMS::IsInRound(edict_t* pEntity)
{
	CBasePlayer *pPlayer;
	entvars_t *pev = &pEntity->v;
	pPlayer = GetClassPtr((CBasePlayer *)pev);
	return pPlayer->IsAlive()==0 ? false : true;
}
void CGameModeTLMS::UpdateRound()
{
	int	nPlayers = 0;
	if(m_bInProgress)
	{
		if(IsOver()) 
		{
			m_dRoundTimer.ClearTime();
			m_bStartSequence = false;
			MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
			WRITE_SHORT (-1); // -1 freezes the current timer 
			MESSAGE_END();
			m_dEndMessage.AddTime(END_DELAY); // congrads
		}				
	} 
	else
	{
		if(MinimumTeamsConnected())
			StartRound();
	}
}

bool CGameModeTLMS::IsOver() // udone
{
	int  CurrentTeam = 0;
	m_nLTS = 0;
	bool LastTeam = false;

	int nPlayersLeft = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			if(m_bStartSequence)
			{
				if(!pPlayer->m_bIsRemoved /*&& !FStrEq(STRING(pPlayer->pev->netname), "")*/)
				{
					int index = GetTeamIndex2(pPlayer->m_szTeamName)+1;
					if(index == 0)
						continue;

					if(CurrentTeam == 0)
						CurrentTeam =  index;
					else if(CurrentTeam !=  index)
							return false;
				}
			}
			else if(pPlayer->IsAlive() && !pPlayer->m_bIsRemoved /*&& !FStrEq(STRING(pPlayer->pev->netname), "")*/)
			{
					int index = GetTeamIndex(pPlayer)+1;
					if(index == 0)
						continue;

					if(CurrentTeam == 0)
						CurrentTeam =  index;
					else if(CurrentTeam !=  index)
							return false;
			}
		}
	}
	// last team stands
	m_nLTS = 0;
	if(CurrentTeam)
	{		
		if(m_bStartSequence)			
		{
			m_dRoundTimer.ClearTime();
			EndRound();
			return false;
		}
		m_nLTS = CurrentTeam;
		return true;
	}
	return false;
}

void CGameModeTLMS::StartRound()
{
	m_dStartMessage.AddTime(ROUND_DELAY);
	m_dStartRound.AddTime(ROUND_DELAY);
	m_bAllowRestrictedSpawning	= false;
	m_bStartSequence			= true;
	m_bInProgress				= true;
	m_nNumber					= 0;
//	m_nWinner					= NULL;
	m_nResult					= RESULT_NONE;
}

void CGameModeTLMS::EndRound()
{
	m_bInProgress		= false;
	m_nNumber			= 0;
	m_bStartSequence	= false;
	m_nRoundTime		= 0;
	UpdateRound();
}


void CGameModeTLMS::ToggleSpawningRestriction()
{
	g_bPlayLongjumpSound =! g_bPlayLongjumpSound;
	m_bAllowRestrictedSpawning =! m_bAllowRestrictedSpawning;
}


void CGameModeTLMS::SpawnValidPlayers()
{
	m_nSpawnedPlayers = 0; 
	g_bPlayLongjumpSound = false;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			if(CHalfLifeTeamplay::CanStopObserver(pPlayer))
			{
				if(!pPlayer->m_bIsRemoved)
				{
					pPlayer->m_nStreak = 0; // we will use this value for counting round frags in lms
					pPlayer->RemoveAllItems( FALSE );
					if(pPlayer->m_isSpectator)
						pPlayer->StopObserver();
					else 
					{
						pPlayer->Spawn();
						SendScoreInfo(pPlayer);
					}
					m_nSpawnedPlayers++;

				}
			}
			pPlayer->pev->takedamage = DAMAGE_YES; // fair game
	
		}
	}
	g_bPlayLongjumpSound = true;
}


void CGameModeTLMS::UpKeep()
{
	CBasePlayer *pPlayer;
	CBaseEntity *pEnt;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			pPlayer = (CBasePlayer *)pEnt;
			if(pPlayer->IsAlive())
			{
				if(pPlayer->pev->armorvalue>0)
				{
				pPlayer->pev->armorvalue -= ARMOR_DECLINE;
					if(0>=(int)pPlayer->pev->armorvalue) // trick because hev could be 0.8937 but we want it to be 0
						pPlayer->pev->armorvalue = 0;
				}
				
				else if(pPlayer->pev->health>LIFE_DECLINE)
					pPlayer->pev->health-=LIFE_DECLINE;		
			}
		}
	}
}


void CGameModeTLMS::GivePlayerBonus(CBasePlayer* pPlayer)
{
	pPlayer->m_iCaptures++;
	LoadPlayer(pPlayer);

	int addlife = 15 + ((pPlayer->m_iCaptures-1)*10);  // 15, 25, 35, 45.. etc
	int addhev  = addlife;
		
	if(addlife+pPlayer->pev->health > pPlayer->pev->max_health)
	{
		addlife = pPlayer->pev->max_health-pPlayer->pev->health;
	}
	if(addhev+pPlayer->pev->armorvalue >pPlayer->max_hev)
	{
		addhev = pPlayer->max_hev-pPlayer->pev->armorvalue;
	}

	pPlayer->pev->armorvalue += addhev;
	pPlayer->pev->health	 += addlife;
				
	MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pPlayer->edict() );  
	WRITE_SHORT(addlife);
	WRITE_SHORT(addhev);
	MESSAGE_END();

	
	CEntityNode* pClient = pPlayer->m_lObservers.head();
	while(pClient)
	{
	
		MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pClient->m_pEntity );
		WRITE_SHORT(addlife);
		WRITE_SHORT(addhev);
		MESSAGE_END();
		pClient = pClient->m_pNext;
	}
}

void CGameModeTLMS::StartObserver(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
		WRITE_BYTE( g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)+1 );
		WRITE_BYTE( 1 );
		WRITE_BYTE(	(int)pPlayer->m_bIsRemoved);
		WRITE_BYTE(	0 );
	MESSAGE_END();
}

