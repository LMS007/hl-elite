
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h" 
#include	"game.h"
#include	"items.h"
#include	"voice_gamemgr.h"
#include    "client.h"
#include    "hle_utility.h"
#include	"hle_gamemodes.h"
#include	"hle_maploader.h"
#include	"hle_mode_teamduel.h"
#include	"hle_flag.h"


/* 
extern Vote* g_pVote; // global vote class pointer
extern int gmsgDeathMsg;	// client dll messages

extern int gmsgMOTD;
*/
extern int gmsgScoreInfo;

extern int gmsgServerName;
extern int g_teamplay;
extern int gmsgGameMode;
extern int gmsgBonus;
extern int gmsgTimer;
extern int gmsgHudStatus;
extern int gmsgBonus;
//extern int gmsgHudColor;
extern int gmsgIconInfo;
extern int gmsgReset;
extern int	gmsgAnounce;
extern int gmsgTeamScores;

extern int g_bPlayLongjumpSound;
extern cvar_t winsleft;


extern COutput debug_file;

CGameModeTeamDuel::CGameModeTeamDuel()
{
	m_bInProgress = false;
	m_bStartSequence = false;
	m_nNumber = 0;
	m_bAllowRestrictedSpawning = false;	
	m_nWinner = 0;
	memset(m_nTeamList, 0, 6);
	m_bInitSpawns = false;
	//m_nBonus = DUEL_BONUS_NONE;
}




void CGameModeTeamDuel::Think()
{
	if(!m_bInitSpawns)
	{
		g_pGameRules->InitSpawns();
		m_bInitSpawns = true;
	}

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
	
}

void CGameModeTeamDuel::TimeIsUp()
{
	m_nWinner=0;
	m_nLoser = 0;
	m_nResult = RESULT_TIE;
	int m_nTeamOneAlive =0;
	int m_nTeamTwoAlive =0;

	CBaseEntity* pEnt;
	CBasePlayer* pPlayer;

	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			pPlayer = (CBasePlayer *)pEnt;
			if(pPlayer->IsAlive())
			{
				if(g_pGameRules->GetTeamIndex(pPlayer)==m_nTeamList[0])
					m_nTeamOneAlive++;
				else if(g_pGameRules->GetTeamIndex(pPlayer)==m_nTeamList[1])
					m_nTeamTwoAlive++;
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, 3);
				pPlayer->pev->takedamage = 0;
			}
		}
	}

	MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
		WRITE_BYTE(0);
		WRITE_BYTE(2);
		WRITE_BYTE(3);
	MESSAGE_END();

	if(m_nTeamOneAlive>m_nTeamTwoAlive)
	{
		m_nResult = RESULT_DEFEAT;
		m_nWinner = m_nTeamList[0];
		m_nLoser = m_nTeamList[1];

	}
	else if(m_nTeamOneAlive>m_nTeamTwoAlive)
	{
		m_nResult = RESULT_DEFEAT;
		m_nWinner = m_nTeamList[1];
		m_nLoser = m_nTeamList[0];
	}

	AnounceOutcome();
	m_dRoundTimer.ClearTime();
	m_dEndMessage.ClearTime();
	EndRound();
}

void CGameModeTeamDuel::StartRoundTime()
{
	m_nRoundTime = roundtime.value;
	
	if(m_nRoundTime >=1000)
		m_nRoundTime=999;
	else if(m_nRoundTime<=0)
		m_nRoundTime = 0;
	m_dRoundTimer.AddTime(m_nRoundTime);

	MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
		WRITE_SHORT (m_nRoundTime); // send time left, not time limit
	MESSAGE_END();
	
}


// add at bottom of list when a client connects
void CGameModeTeamDuel::AddTeam(int nTeam)
{
	for(int i = 0; i<6; i++)
	{
		if(m_nTeamList[i]==0)
		{
			m_nTeamList[i]=nTeam;
			break;
		}
	}
    UpdateRound();
}

int CGameModeTeamDuel::LookUpTeam(int nTeam)
{
	for(int i = 0; i<6; i++)
	{
		if(nTeam == m_nTeamList[i])
			return i;
	}
	return -1;
}

bool CGameModeTeamDuel::IsPlayerInRound(CBasePlayer *pPlayer)
{

	if(pPlayer->m_bIsRemoved || !pPlayer->IsAlive())
		return false;
	int team = g_pGameRules->GetTeamIndex(pPlayer);
	for(int i = 0; i<2; i++)
	{
		if(team == m_nTeamList[i])
			return true;
	}
	return false;
}

int CGameModeTeamDuel::PlayersOnTeam(int nTeam, bool AreAlive)
{

	int players =0;
	CBaseEntity* pEnt;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer* pPlayer = (CBasePlayer *)pEnt;
			if(!pPlayer->m_bIsRemoved)
			{
				if(AreAlive)
				{
					if(pPlayer->IsAlive())
						if(g_pGameRules->GetTeamIndex(pPlayer)==nTeam)
							players++;
				}
				else if(g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)==nTeam)
					players++;
			}
		}
	}
	return players;
}


void CGameModeTeamDuel::MoveTeam(int nTeam)
{
	
	for(int i = 0; i<6; i++)
	{
		if(nTeam == m_nTeamList[i])
		{
			for(int j = i; j<i+1; j++)
			{
			
				m_nTeamList[j]=m_nTeamList[j+1];
				
			}
			m_nTeamList[6]=0;
			AddTeam(nTeam);
			return;
			
		}
	}
}
// remove from list when client disconnects
void CGameModeTeamDuel::DropTeam(int nTeam)
{
	bool inRound = false;
	
	int order = LookUpTeam(nTeam);
	for(int i = order; i<5;i++)
	{
		m_nTeamList[i] = m_nTeamList[i+1];
	}
	m_nTeamList[6] = 0;

	if(order<2 && order>=0)
		EndRound();
}

bool CGameModeTeamDuel::MinimumTeamsConnected()
{
	if(m_nTeamList[1])
		return true;
	return false;

}

int CGameModeTeamDuel::MinimumTeams()
{
	return 2;
}


void CGameModeTeamDuel::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
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

		//if(peKiller->m_nStreak==m_nSpawnedPlayers-1 && m_nSpawnedPlayers > 3) // ANAINILATION
		//	m_nBonus = 5;

		if((pKiller->health >= 100) && (pKiller->armorvalue >= 100)) // DUEL_BONUS_FLAWLESS
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


void CGameModeTeamDuel::ClientDisconnected(edict_t* pEntity)
{
    
	CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&pEntity->v);
	pPlayer->m_bIsRemoved = true;
    UpdateRound();
	CHalfLifeTeamplay::ClientDisconnected(pEntity);
}


void CGameModeTeamDuel::StartRound()
{
	m_dStartMessage.AddTime(ROUND_DELAY);
	m_dStartRound.AddTime(ROUND_DELAY);
	m_bAllowRestrictedSpawning	= false;
	m_bStartSequence			= true;
	m_bInProgress				= true;
	m_nNumber					= 0;
	m_nWinner					= 0;
	m_nLoser					= 0;
	//m_nBonus					= DUEL_BONUS_NONE;
	m_nResult					= RESULT_NONE;
}

void CGameModeTeamDuel::EndRound()
{
	m_bInProgress		= false;
	m_nNumber			= 0;
	m_bStartSequence	= false;
	m_nRoundTime		= 0;
	UpdateRound();
}
void CGameModeTeamDuel::UpdateRound()
{
	if(m_bInProgress)
	{
		if(IsOver())
			EndRound();
	}
	else if(MinimumTeamsConnected())
	{
		if(m_dEndMessage.GetTimeLeft()!=0)
			return;
		StartRound();
	}
}



void CGameModeTeamDuel::PlayerSpawn( CBasePlayer *pPlayer )
{
	//BOOL		addDefault;
	//CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	

	///////////////////////
	///NEW PLAYER CHECK ///
	///////////////////////
	int team = g_pGameRules->GetTeamIndex(pPlayer);
	if(!LookUpTeam(team))
	{
		AddTeam(team);
		return;
	}

	///////////////////////
	///START SPAWN CHECK///
	///////////////////////
	
	bool UnSpawn = true;

	if(m_bAllowRestrictedSpawning)
	{	
		if(MinimumTeamsConnected())
		{				
			if(IsPlayerInRound(pPlayer))
				UnSpawn = false;
		}
	}
	if(UnSpawn)
	{
		pPlayer->m_bUnSpawn = true;
		return;
	}

	/*addDefault = TRUE;
	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}*/

	//if(addDefault)
	{
		if(spawnform.value <=0) 
		{
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveNamedItem( "weapon_crowbar" );
			pPlayer->GiveAmmo( 68, "9mm", _9MM_MAX_CARRY );
		}
		else
		{
			ArmPlayer(pPlayer);
			LoadPlayer(pPlayer);
			FillPlayer(pPlayer);	
		}
	}
}


void CGameModeTeamDuel::VersesMessage()
{
	
	MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
	WRITE_BYTE(0);
	WRITE_BYTE(m_nTeamList[0]);
	WRITE_BYTE(m_nTeamList[1]);
	MESSAGE_END();	
}


void CGameModeTeamDuel::AnounceOutcome()
{
	char buf[128];
	*buf=NULL;
//	CBasePlayer* pWinner;
//	CBasePlayer* pLoser;

	//if(!MinimumTeamsConnected())
	//{
	//	UpdateRound();
	//	return;
	//}

	switch(m_nResult)
	{

	case RESULT_NONE:

		break;

	case RESULT_TIE:
		
		MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
		WRITE_BYTE(2);
		WRITE_BYTE(2);
		WRITE_BYTE(0);
		MESSAGE_END();	
		break;

	case RESULT_DEFEAT://////////////////////
		{

		if(MinimumTeamsConnected())
		{
			if(m_nWinner == m_nTeamList[0])
			{
				m_nLoser = m_nTeamList[1];
				MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
				WRITE_BYTE(1);
				WRITE_BYTE(m_nTeamList[0]);
				WRITE_BYTE(0);
				MESSAGE_END();	

				m_nTeamWins[m_nTeamList[0]]++;
				m_nTeamLosses[m_nTeamList[1]]++;
			}
			else if(m_nWinner == m_nTeamList[1])
			{
				m_nLoser = m_nTeamList[0];

				MESSAGE_BEGIN( MSG_ALL, gmsgAnounce, NULL);
				WRITE_BYTE(1);
				WRITE_BYTE(m_nTeamList[1]);
				WRITE_BYTE(0);
				MESSAGE_END();	

				m_nTeamWins[m_nTeamList[1]]++;
				m_nTeamLosses[m_nTeamList[0]]++;
			}
			else
				break;

			MESSAGE_BEGIN( MSG_ALL, gmsgTeamScores );
				WRITE_BYTE( m_nWinner );
				WRITE_BYTE( m_nTeamWins[m_nWinner] );
				WRITE_BYTE( m_nTeamLosses[m_nWinner] );
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_ALL, gmsgTeamScores );
				WRITE_BYTE( m_nLoser );
				WRITE_BYTE( m_nTeamWins[m_nLoser] );
				WRITE_BYTE( m_nTeamLosses[m_nLoser] );
			MESSAGE_END();

			

		}
		break;
		}
	default:
		break;
	}

}

void CGameModeTeamDuel::ToggleSpawningRestriction()
{
	g_bPlayLongjumpSound =! g_bPlayLongjumpSound;
	m_bAllowRestrictedSpawning =! m_bAllowRestrictedSpawning;
}

void CGameModeTeamDuel::SpawnValidPlayers()
{
				
	CBasePlayer* pPlayer=NULL;
	if(!MinimumTeamsConnected())
	{
		UpdateRound();
		return;
	}
	g_bPlayLongjumpSound = false;

	CBaseEntity* pEnt;


	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			int team = GetTeamIndex(pPlayer);
			if(LookUpTeam(team)<2)
			{
				pPlayer = (CBasePlayer *)pEnt;
				pPlayer->RemoveAllItems( TRUE );
				pPlayer->pev->takedamage = DAMAGE_YES;

				if(pPlayer->m_isSpectator)
				{
					pPlayer->StopObserver(); // this calls gmsgScoreInfo
				}
				else
				{
					SendScoreInfo(pPlayer);
					pPlayer->Spawn();
				}
			}
			// not dead but no longer in round, not the first 2 players
			else if(!pPlayer->m_isSpectator) 
			{
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
			}
			else // they are dead and we need to update thier score (tourney mode only) 
			{
				SendScoreInfo(pPlayer);

				MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
					WRITE_BYTE( pPlayer->entindex() );
					WRITE_BYTE( 0 );
					WRITE_BYTE( 1);
					WRITE_BYTE(	pPlayer->m_bIsRemoved);
					WRITE_BYTE(	0 );
				MESSAGE_END();
			}
		}
		
	}
	g_bPlayLongjumpSound = true;
}


BOOL CGameModeTeamDuel::CanStartObserver(CBasePlayer *pPlayer)
{

	if(!pPlayer->m_isSpectator)
	{
		if(IsPlayerInRound(pPlayer))
			UpdateRound();
		
		
	}
	return TRUE;
	
}

BOOL CGameModeTeamDuel::CanStopObserver(CBasePlayer *pPlayer)
{
	return FALSE;
}

int CGameModeTeamDuel::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CGameModeTeamDuel::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

bool CGameModeTeamDuel::IsOver()
{
	bool TeamOnePlayers=0;
	bool TeamTwoPlayers=0;

	if(MinimumTeamsConnected())
	{
		CBasePlayer* pPlayer;
		CBaseEntity* pEnt;
		int nAlive=0;
		
		for (int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			pEnt = UTIL_PlayerByIndex( i );

			if ( pEnt && pEnt->IsPlayer( ) )
			{
				pPlayer = (CBasePlayer *)pEnt;
				if(pPlayer->IsAlive())
				{
					int team = GetTeamIndex(pPlayer);
					if(LookUpTeam(team)==0)
					{
						TeamOnePlayers=true;
					}
					else if(LookUpTeam(team)==1)
					{
						TeamTwoPlayers=true;
					}
				}
			}
		}
		if(!TeamOnePlayers || !TeamTwoPlayers)
		{
			if(!m_bStartSequence)
				return true;
		}
	}
	return false;
}



void CGameModeTeamDuel::ResumePlayer(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsRemoved = false;
	
	int team = GetTeamIndex(pPlayer);
	if(!LookUpTeam(team))
	{
		AddTeam(team);
	}


	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
			WRITE_BYTE( pPlayer->entindex() );
			WRITE_BYTE( 0 );
			WRITE_BYTE( pPlayer->m_isSpectator );
			WRITE_BYTE(	0 );
			WRITE_BYTE(	0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	WRITE_BYTE( 3 );
	MESSAGE_END();
}


void CGameModeTeamDuel::SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever)
{
	if(pReciever==NULL)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iWins );
			WRITE_SHORT( 0 );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pReciever->edict() );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
 			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iWins );
			WRITE_SHORT( 0 );
		MESSAGE_END();
	}
}


