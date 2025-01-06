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


/* 
extern Vote* g_pVote; // global vote class pointer
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgMOTD;
*/

extern int gmsgServerName;
extern int g_teamplay;
extern int gmsgGameMode;
extern int gmsgBonus;
extern int gmsgTimer;
extern int gmsgIconInfo;
//extern int gmsgHudColor;
extern int gmsgHudStatus;
extern int gmsgReset;
extern int gmsgCap;
extern int gmsgMenLeft;
extern int g_bPlayLongjumpSound;

#define UPKEEP_TIME 1.5
#define LIFE_DECLINE 1
#define ARMOR_DECLINE 1
#define LIFE_BONUS 10;
#define ARMOR_BONUS 10;

void CGameModeLMS::Think()
{
	CGameModeDuel::Think();

	//if(m_dUpkeep.TimeUp())
	{
		//UpKeep();	
		//m_dUpkeep.AddTime(UPKEEP_TIME);
	}
}

void CGameModeLMS::PlayerThink(CBasePlayer* pPlayer)
{
	CGameModeDuel::PlayerThink(pPlayer);
}

CGameModeLMS::CGameModeLMS() : CGameModeDuel()
{
	m_pLMS = NULL;
}

void CGameModeLMS::UpKeep()
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

void CGameModeLMS::TimeIsUp()
{
	m_pLoser = NULL;
	m_pWinner = NULL;
	m_nResult = RESULT_DRAW;

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

BOOL CGameModeLMS::CanStartObserver(CBasePlayer *pPlayer)
{
	if(!pPlayer->m_isSpectator)
	{
		pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles); // hacky
		UpdateRound();
	}
	return TRUE;
}

void CGameModeLMS::RemovePlayer(CBasePlayer* pPlayer)
{

	pPlayer->m_bIsRemoved = true;

	CanStartObserver(pPlayer);

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( pPlayer->entindex() );
		WRITE_BYTE( 0 );
		WRITE_BYTE( pPlayer->m_isSpectator );
		WRITE_BYTE(	1 );
		WRITE_BYTE(	0 );
	MESSAGE_END();
	
	UpdateRound();

	MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	WRITE_BYTE( 1 );
	MESSAGE_END();
}

void CGameModeLMS::ResumePlayer(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsRemoved = false;

	UpdateRound();

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


void CGameModeLMS::StartRoundTime()
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

void CGameModeLMS::PlayerSpawn( CBasePlayer *pPlayer )
{
	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);

	if(!m_bInProgress)
		UpdateRound();
	///////////////////////
	///START SPAWN CHECK///
	///////////////////////
	pPlayer->m_bUnSpawn = true;
	pPlayer->m_iCaptures = 0;// for LMS and LTS we will use this value as the number of frags per round.
	if(!m_bAllowRestrictedSpawning)	
		return;
	else
		pPlayer->m_bUnSpawn = false;

	/////////////////////
	///END SPAWN CHECK///
	/////////////////////

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
			g_bPlayLongjumpSound =false;
			ArmPlayer(pPlayer);
			LoadPlayer(pPlayer);
			FillPlayer(pPlayer);
			g_bPlayLongjumpSound =true;	
		}
	}
}


void CGameModeLMS::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
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

	int nPlayersLeft = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			if(m_bStartSequence)
			{
				if(!pPlayer->m_bIsRemoved && !FStrEq(STRING(pPlayer->pev->netname), ""))
					nPlayersLeft++;

			}
			else if(pPlayer->IsAlive() && !pPlayer->m_bIsRemoved && !FStrEq(STRING(pPlayer->pev->netname), "")) // bug change this
			{
				nPlayersLeft++;
				m_pLMS = pPlayer->edict(); // we dont really know if he won yet
				
			}
		}
	}
	MESSAGE_BEGIN( MSG_ALL, gmsgMenLeft );
		WRITE_SHORT(nPlayersLeft);
	MESSAGE_END();

	UpdateRound();
}

int CGameModeLMS::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CGameModeLMS::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

void CGameModeLMS::ClientDisconnected(edict_t* pEntity)
{
	CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&pEntity->v);
	pPlayer->m_bIsRemoved = true;
    UpdateRound();

	CHalfLifeMultiplay::ClientDisconnected(pEntity);
}

bool CGameModeLMS::IsOver()
{
	m_pLMS	= NULL;   
	int nPlayersLeft = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			if(m_bStartSequence)
			{
				if(!pPlayer->m_bIsRemoved && !FStrEq(STRING(pPlayer->pev->netname), ""))
					nPlayersLeft++;

			}
			else if(pPlayer->IsAlive() && !pPlayer->m_bIsRemoved && !FStrEq(STRING(pPlayer->pev->netname), ""))
			{
				nPlayersLeft++;
				m_pLMS = pPlayer->edict(); // we dont really know if he won yet
				
			}
		}
	}
	if(nPlayersLeft<=1)
		return true;
	m_pLMS = NULL;
	return false;
}

void CGameModeLMS::UpdateRound()
{
	
	int	nPlayers = 0;
	if(m_bInProgress)
	{
		if(IsOver() /*&& !m_bAllowRestrictedSpawning*/) 
		{
			m_dRoundTimer.ClearTime();
			m_bStartSequence = false;
			MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
			WRITE_SHORT (-1); // -1 freezes the current timer 
			MESSAGE_END();
			m_dEndMessage.AddTime(END_DELAY); // congrads
		}/*
		else if(!m_dRoundTimer.TimeUp())
		{
			m_pLMS = NULL; // we lied you did'nt win 
			EndRound();
		}	*/					
	} 
	else
	{
		if(MinimumClientsConnected())
			StartRound();
	}
}


void CGameModeLMS::VersesMessage()
{
	char buf[128];
	*buf=NULL;
	sprintf(buf, "Last Man Standing wins");
	UTIL_HudMessageAll(m_hMessage, buf);
}


void CGameModeLMS::AnounceOutcome()
{
	m_dEndMessage.ClearTime();
	char buf[64];
	*buf=NULL;

		
	/*if(!MinimumClientsConnected())
	{
		UpdateRound();
		return;
	}*/

	if(!m_pLMS) // no one won!
	{
		strcpy(buf, "No Man Stands");
	}
	else
	{
		if(m_nBonus==5) // for ANAINILATION, everyone sees it
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgBonus);
			WRITE_BYTE(m_nBonus);
			MESSAGE_END();
		}

		entvars_t *pevWinner = &m_pLMS->v;
		pevWinner->takedamage = DAMAGE_NO;
		strncpy(buf, STRING(m_pLMS->v.netname), 64);
		strncat(buf," is the LMS", 64);
		CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)(pevWinner));
		pPlayer->m_iWins++;
		
		SendScoreInfo(pPlayer);
		
	}

	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			pPlayer->pev->takedamage = DAMAGE_NO; // no more shooting at this point	
		}
	}

	float fLength = (float)strlen(buf);
	float fOffset = fLength*=0.0035;
	m_hMessage.x = 0.5-fOffset;
	UTIL_HudMessageAll(m_hMessage, buf);
}

bool CGameModeLMS::IsInRound(edict_t* pEntity)
{
	CBasePlayer *pPlayer;
	entvars_t *pev = &pEntity->v;
	pPlayer = GetClassPtr((CBasePlayer *)pev);
	return pPlayer->IsAlive()==0 ? false : true;
}



void CGameModeLMS::SpawnValidPlayers()
{
	g_bPlayLongjumpSound = false;
	m_nSpawnedPlayers = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			if(!pPlayer->m_bIsRemoved)
			{
				pPlayer->m_nStreak = 0; // we will use this value for counting round frags in lms
				pPlayer->RemoveAllItems( TRUE );
				if(pPlayer->m_isSpectator)
					pPlayer->StopObserver();
				else 
				{
					pPlayer->Spawn();

					SendScoreInfo(pPlayer);
				}
				m_nSpawnedPlayers++;
			}
			pPlayer->pev->takedamage = DAMAGE_YES; // fair game
	
		}
	}
	g_bPlayLongjumpSound = true;
}

bool CGameModeLMS::MinimumClientsConnected()
{
	int nPlayers=0;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer* pPlayer = (CBasePlayer*)pEnt;
			if(!pPlayer->m_bIsRemoved)
				nPlayers++;
		}
	}
	return (nPlayers >= MinimumClients()) ? true : false;
}


BOOL CGameModeLMS::FPlayerCanRespawn(CBasePlayer*)
{
	return TRUE;
}


void CGameModeLMS::GivePlayerBonus(CBasePlayer* pPlayer)
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
