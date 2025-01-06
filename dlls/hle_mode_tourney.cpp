
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"hle_gamemodes.h"
#include	"game.h"
#include	"items.h"
#include	"voice_gamemgr.h"
#include    "hle_vote.h"
#include    "client.h"
#include    "hle_utility.h"


extern int gmsgGameMode;
extern int gmsgScoreInfo;
extern int gmsgTimer;
extern int gmsgFragsLeft;
//extern int gmsgHudColor;
extern int gmsgReset;

extern CVoiceGameMgr	g_VoiceGameMgr;
extern float g_fTimeOffset;
extern DLL_GLOBAL CGameRules	*g_pGameRules;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern float g_flIntermissionStartTime;
extern int g_bPlayLongjumpSound ;

extern cvar_t timeleft, fragsleft, winsleft;
extern cvar_t mp_chattime;


CGameModeTourney::CGameModeTourney() : CGameModeDuel()
{
	m_pLeader = NULL;
	m_pTrailer = NULL;
}

void CGameModeTourney::StartRound()
{
	m_nOldFragsLeft = 0;
	CGameModeDuel::StartRound();
}
void CGameModeTourney::EndRound()
{
	CGameModeDuel::EndRound();
}


void CGameModeTourney::StartRoundTime()
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

void CGameModeTourney::SpawnValidPlayers()
{
 	m_bFirstSpawn = true;
	
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{

			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			pPlayer->m_iDeaths  = 0;
			pPlayer->pev->frags = 0;
			pPlayer->pev->takedamage = DAMAGE_YES;
		}
	}
	CGameModeDuel::SpawnValidPlayers();
	m_bFirstSpawn = false;
	m_nSpawnLimit = spawnlimit.value;
	m_nFragsLeft = m_nSpawnLimit;
	SendFragsLeft();
}

void CGameModeTourney::PlayerSpawn( CBasePlayer *pPlayer )
{

	//BOOL		addDefault;
	//CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	
	if(spawnform.value ==0) 
	{
		int temp = g_bPlayLongjumpSound;
		g_bPlayLongjumpSound = false;
		CBaseEntity*pWeaponEntity=NULL;
		while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
		{
			pWeaponEntity->Touch( pPlayer );
		}
		g_bPlayLongjumpSound = temp;

		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveAmmo( 68, "9mm", _9MM_MAX_CARRY );// 4 full reloads
	}
	else // start loded, spawn normal
	{
		if(m_bFirstSpawn)
		{
			ArmPlayerRestricted(pPlayer);
			LoadPlayer(pPlayer);
			FillPlayer(pPlayer);	
		}
		else
		{
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveNamedItem( "weapon_crowbar" );
			pPlayer->GiveAmmo( 68, "9mm", _9MM_MAX_CARRY );// 4 full reloads
		}
	}		
	


	///////////////////////
	///NEW PLAYER CHECK ///
	///////////////////////

	if(!LookUpPlayer(pPlayer->edict()))
	{
		AddPlayer(pPlayer->edict());
		pPlayer->m_bUnSpawn = true;
		return;
	}
	
	///////////////////////
	///START SPAWN CHECK///
	///////////////////////
	
	if(!IsInRound(pPlayer->edict()) || !m_bInProgress || m_bStartSequence)
	{
		bool UnSpawn = true;

		if(m_bAllowRestrictedSpawning)
		{	
			if(MinimumClientsConnected())
			{				
				if(IsInRound(pPlayer->edict()))
					UnSpawn = false;
			}
		}
		if(UnSpawn)
			pPlayer->m_bUnSpawn = true;
	}
}

void CGameModeTourney::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
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
		// killed self
		pKiller->frags -= 1;
	}
	else if ( ktmp && ktmp->IsPlayer() )
	{
		// killed opponet
		pKiller->frags += 1;
		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	}
	else
	{  // killed by the world
		pKiller->frags -= 1;
	}
	// update the scores
	// killed scores
	
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

//	m_bWaitingForOutcome = true;
	if(IsOver())
	{
		if(m_dRoundTimer.GetTimeLeft())
		{
			m_dRoundTimer.ClearTime();
			MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
			WRITE_SHORT (-1); // -1 freezes the current timer 
			MESSAGE_END();
		}

		m_nResult = RESULT_DEFEAT;
		m_pWinner = m_pLeader;
		m_pLoser = m_pTrailer;

		if(m_pWinner)
		{
			CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			if(pPlayer->m_iDeaths == 0)		
			m_nBonus = DUEL_BONUS_IMMORTAL;
		}
		else
			m_nBonus = DUEL_BONUS_NONE;
		m_dEndMessage.AddTime(END_DELAY);
		MovePlayer(m_pLoser);
	}
	SendFragsLeft();
 // wait 2 seconds incase the winner dies too, otherwise we anouce the winner
	
}


void CGameModeTourney::UpdatePositions()
{
	if(MinimumClientsConnected())
	{
		CBasePlayer* pPlayer0 = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
		CBasePlayer* pPlayer1 = GetClassPtr((CBasePlayer *)&(m_lEntity[1]->v));

		if(pPlayer0->pev->frags > pPlayer1->pev->frags)
		{	
			m_pLeader = pPlayer0->edict();
			m_pTrailer = pPlayer1->edict();
		}
		else if(pPlayer1->pev->frags > pPlayer0->pev->frags)
		{
			m_pLeader = pPlayer1->edict();
			m_pTrailer = pPlayer0->edict();
		}
		else 
		{
			m_pLeader = NULL;
			m_pTrailer = NULL;
		}
	}
}

void CGameModeTourney::TimeIsUp()
{
	m_pLoser = NULL;
	m_pWinner = NULL;
	
	if(MinimumClientsConnected())
	{
		UpdatePositions();

		if(m_pLeader && m_pTrailer &&(m_pLeader->v.frags - m_pTrailer->v.frags >1))
		{
			m_nResult = RESULT_DEFEAT;
			CBasePlayer* pPlayerLoser = GetClassPtr((CBasePlayer *)&(m_pTrailer->v));
			
			m_lEntity.remove(m_pTrailer);
			m_lEntity.push_back(m_pTrailer);

			m_pWinner = m_pLeader;
			m_pLoser = m_pTrailer;
			m_pWinner->v.takedamage = DAMAGE_NO;
			m_pLoser->v.takedamage = DAMAGE_NO;

			MESSAGE_BEGIN( MSG_ALL, gmsgFragsLeft, NULL);
			WRITE_SHORT (0); // GAME
			MESSAGE_END();

			AnounceOutcome();
			m_dRoundTimer.ClearTime();
			m_dEndMessage.ClearTime();
			EndRound();

		}
		else
		{
			m_nSpawnLimit = 0;
			MESSAGE_BEGIN( MSG_ALL, gmsgFragsLeft, NULL);
			WRITE_SHORT (-2); // WIN BY 2
			MESSAGE_END();
		}
	}
	m_dRoundTimer.ClearTime();
}

bool CGameModeTourney::IsOver()
{
	UpdatePositions();
	if(MinimumClientsConnected())
	{
		if(m_bStartSequence)
			return false;
		for(int i = 0; i < MinimumClients(); i++)
		{
			CBasePlayer* pPlayer = GetClassPtr((CBasePlayer*)&(m_lEntity[i]->v));
			if(pPlayer->m_isSpectator)
				return true;
		}
		if(m_pLeader)
		{
			if(m_pLeader->v.frags >= m_nSpawnLimit)
			{
				if(m_pTrailer->v.frags >= m_pLeader->v.frags-1) // must win by 2
				{
					m_nFragsLeft = 2 - (m_pLeader->v.frags - m_pTrailer->v.frags);
					return false;
				}
				else
				{
					m_nFragsLeft = 0;
					return true;
				}
			}
			else // normal case
			{
				m_nFragsLeft = m_nSpawnLimit - m_pLeader->v.frags;
				return m_nFragsLeft==0 ? true : false;
			}
		}
		else
		{
			// m_lEnity[0]->v.frags == m_lEnity[1]->v.frags these are equal at this step
			if(m_lEntity[0]->v.frags >= m_nSpawnLimit-1)
				m_nFragsLeft = 2;
			else
				m_nFragsLeft = m_nSpawnLimit - m_lEntity[0]->v.frags;
			return false;
		}
	}
	// undone case
	m_pLeader = NULL;
	m_pTrailer = NULL;
	return true;

}

void CGameModeTourney::SendFragsLeft()
{
	if(m_nFragsLeft==2 && !m_pLeader && !m_pTrailer)
	{		
		MESSAGE_BEGIN( MSG_ALL, gmsgFragsLeft, NULL);
		WRITE_SHORT (-1); // DUEX
		MESSAGE_END();

		m_nOldFragsLeft = m_nFragsLeft;
				
	}
	else if(m_nOldFragsLeft != m_nFragsLeft)
	{
		
		MESSAGE_BEGIN( MSG_ALL, gmsgFragsLeft, NULL);
		WRITE_SHORT (m_nFragsLeft);
		MESSAGE_END();

		m_nOldFragsLeft = m_nFragsLeft;
	}
}


BOOL CGameModeTourney::CanStartAutoObserver(CBasePlayer* pPlayer)
{
	// no death cam auto spectate
	return FALSE;
}

int CGameModeTourney::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_ACTIVE;
}

int CGameModeTourney::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_ACTIVE;
}


void CGameModeTourney::SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever)
{
	if(pReciever==NULL)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iWins );
			WRITE_SHORT( pSender->m_iLoses );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pReciever->edict() );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
 			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iWins );
			WRITE_SHORT( pSender->m_iLoses );
		MESSAGE_END();
	}
}
