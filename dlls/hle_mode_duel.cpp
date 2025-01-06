
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


extern int g_bPlayLongjumpSound;
extern cvar_t winsleft;


extern COutput debug_file;

CGameModeDuel::CGameModeDuel()
{
	m_hMessage.x            = 0.45f;
    m_hMessage.y            = 0.4f;
    m_hMessage.a1           = 0.0f;
    m_hMessage.a2           = 0.0f;

    m_hMessage.b1           = 255.0f;
    m_hMessage.g1           = 185.0f;
    m_hMessage.r1           = 120.0f;
	
	m_hMessage.b2           = 0.0f;
	m_hMessage.g2           = 0.0f;
    m_hMessage.r2           = 0.0f;
    
	m_hMessage.channel      = 4;
    m_hMessage.effect       = 2;
    m_hMessage.fxTime       = 0.5;
    m_hMessage.fadeinTime   = 0.02;
    m_hMessage.fadeoutTime  = 0.5;    
    m_hMessage.holdTime     = 4.0;
	m_bJustConnected = true;
	m_bInProgress = false;
	m_bStartSequence = false;
	m_nNumber = 0;
	m_bDeux = false;
	m_bAllowRestrictedSpawning = false;	
	m_pWinner = NULL;
	m_pLoser = NULL;

	m_nBonus = DUEL_BONUS_NONE;
}


void CGameModeDuel::PlayerThink( CBasePlayer *pPlayer )
{
	
	CHalfLifeMultiplay::PlayerThink(pPlayer);				
}

BOOL CGameModeDuel::FPlayerCanRespawn(CBasePlayer *pPlayer) 
{
	return TRUE;
}

void CGameModeDuel::ThinkLimit()
{
	CHalfLifeMultiplay::Think();
}


void CGameModeDuel::Think()
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

	ThinkLimit();	
}/*
void CGameModeDuel::AddClock(float time)
{

	int nNewTime = m_dRoundTimer.GetTimeLeft() + time;
	if(nNewTime >=1000)
	{
		nNewTime=1000;
	}
	if(nNewTime>=roundtime.value)
	{	
		nNewTime = roundtime.value;
	}
	else if(nNewTime<=0)
	{
		nNewTime = 0;
	}
	m_dRoundTimer.AddTime(nNewTime);

	MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
		WRITE_SHORT (nNewTime); // send time left, not time limit
	MESSAGE_END();
}*/


void CGameModeDuel::TimeIsUp()
{
	if(MinimumClientsConnected())
	{

		CBasePlayer* pPlayer0 = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
		CBasePlayer* pPlayer1 = GetClassPtr((CBasePlayer *)&(m_lEntity[1]->v));

		if(pPlayer0->pev->health > pPlayer1->pev->health)
		{
			m_nResult = RESULT_TIMEUP;
			m_pLoser = pPlayer1->edict();
			m_pWinner = pPlayer0->edict();

			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			MovePlayer(m_pLoser);
		}
		else if(pPlayer0->pev->health < pPlayer1->pev->health)
		{
			m_nResult = RESULT_TIMEUP;
			m_pLoser = pPlayer0->edict();
			m_pWinner = pPlayer1->edict();
			m_dRoundTimer.ClearTime();
			m_dEndMessage.AddTime(END_DELAY);
			MovePlayer(m_pLoser);
		
		}
		else // same health
		{
			if(pPlayer0->pev->armorvalue > pPlayer1->pev->armorvalue)
			{
				m_nResult = RESULT_TIMEUP;
				m_pLoser = pPlayer1->edict();
				m_pWinner = pPlayer0->edict();
				m_dRoundTimer.ClearTime();
				m_dEndMessage.AddTime(END_DELAY);
				MovePlayer(m_pLoser);
			}
			else if(pPlayer0->pev->armorvalue < pPlayer1->pev->armorvalue)
			{
				m_nResult = RESULT_TIMEUP;
				m_pLoser = pPlayer0->edict();
				m_pWinner = pPlayer1->edict();

				m_dRoundTimer.ClearTime();
				m_dEndMessage.AddTime(END_DELAY);
				MovePlayer(m_pLoser);
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
			
			}
			else // same health same hev, no win!
			{
				// not really a winner, they are both losers, but we will use thsee pointers at our convience for later
				m_pLoser = pPlayer1->edict();
				m_pWinner = pPlayer0->edict();
				m_nResult = RESULT_DRAW;

				//CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
				edict_t* pTemp = m_lEntity[0];
				m_lEntity.remove(m_lEntity[0]);
				m_lEntity.push_back(pTemp);
				pTemp = m_lEntity[0];
				//pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
				//pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
				m_lEntity.remove(m_lEntity[0]);
				m_lEntity.push_back(pTemp);


				m_dRoundTimer.ClearTime();
				AnounceOutcome();
				m_dEndMessage.ClearTime();
				EndRound();
			}
		}
	}
}

void CGameModeDuel::StartRoundTime()
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


// called from dtor
void CGameModeDuel::ClearList(void)
{
    m_lEntity.clear();
}

// add at bottom of list when a client connects
void CGameModeDuel::AddPlayer(edict_t* pEntity)
{
	m_lEntity.push_back(pEntity);
    UpdateRound();
}

bool CGameModeDuel::LookUpPlayer(edict_t* pEntity)
{
	for(int i = 0; i < m_lEntity.size(); i++)
	{
		if(pEntity == m_lEntity[i])
			return true;
	}
	return false;
}

bool CGameModeDuel::IsInRound(edict_t* pEntity)
{
	if(MinimumClientsConnected())
	{
		for(int i = 0; i < MinimumClients(); i++)
		{
			if(pEntity == m_lEntity[i])
				return true;
		}
	}
	return false;
}


void CGameModeDuel::MovePlayer(edict_t* pEntity)
{
	m_lEntity.remove(pEntity);
	m_lEntity.push_back(pEntity);
	UpdateRound();
	return;
}
// remove from list when client disconnects
void CGameModeDuel::DropPlayer(edict_t* pEntity)
{
	bool inRound = false;
	if(!LookUpPlayer(pEntity))
		return;
 	if(IsInRound(pEntity))		
		inRound = true;	
	m_lEntity.remove(pEntity);
	if(inRound && !m_nResult)
		EndRound();
	return;
}

bool CGameModeDuel::MinimumClientsConnected()
{
	if(m_lEntity.size() >=2)
	{
		if(!FStrEq(STRING(m_lEntity[0]->v.netname), ""))
			return true;
	}
	return false;
}

int CGameModeDuel::MinimumClients()
{
	return 2;
}


void CGameModeDuel::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
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
		if(m_bInProgress)
		{
			pVictim->m_nStreak =0; // frags in a row start over

			if(pVictim->edict() == m_pWinner) // killed self after he won before the round ended
			{
				m_pLoser = NULL;
				m_pWinner = NULL;
				m_nResult = RESULT_TIE;
			}
			else // killed self
			{
				m_pWinner = NULL;
				m_pLoser = pVictim->edict();
				m_nResult = RESULT_SUICIDE;
			}
		}
		pKiller->frags -= 1;
	}
	else if ( ktmp && ktmp->IsPlayer() )
	{
		if(FStrEq(STRING(pVictim->pev->netname), ""))// deals with ghosts
		{
			return;
		}
		if(/*m_pWinner &&*/ m_pWinner == pVictim->edict()) // killed eachother
		{
			m_pLoser = NULL;
			m_pWinner = NULL;
			m_nResult = RESULT_TIE;
		}
		else
		{
			m_nResult = RESULT_DEFEAT;
			m_pLoser = pVictim->edict();
			m_pWinner = peKiller->edict();
		}
		pVictim->m_nStreak=0; // frags in a row ++
		pKiller->frags += IPointsForKill( peKiller, pVictim );
		peKiller->m_nStreak++; // frags in a row ++
		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	}
	else
	{   //killed by the world
		if(pVictim->edict() == m_pWinner)
		{
			m_pLoser = pVictim->edict();;
			m_pWinner = NULL;
			m_nResult = RESULT_TIE;
		}
		else
		{
			m_pLoser = pVictim->edict();
			m_pWinner = NULL;
			m_nResult = RESULT_SUICIDE;
		}	
		pVictim->m_nStreak=0; // frags in a row ++
		pKiller->frags -= 1;
	}
	
	if(m_pWinner)
	{
		
		CBasePlayer* pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
		if(pWinner->m_nStreak==5)
			m_nBonus = DUEL_BONUS_KILLING_SPREE;
		else if(pWinner->m_nStreak==10)
			m_nBonus = DUEL_BONUS_DOMINATION;		
		else if((pWinner->pev->health >= 100) && (pWinner->pev->armorvalue >= 100))
			m_nBonus = DUEL_BONUS_FLAWLESS;
		else if(pWinner->pev->armorvalue >= 100) // no lost of hev (only fall damage occured)
			m_nBonus = DUEL_BONUS_OWNED;
		else if(pWinner->pev->armorvalue >= 90)
			m_nBonus = DUEL_BONUS_PUNISH;
		else if(pWinner->pev->armorvalue <= 0 && pWinner->pev->health < 10)
			m_nBonus = DUEL_BONUS_LUCK;

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

	m_dRoundTimer.ClearTime();
	MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
	WRITE_SHORT (-1); // -1 freezes the current timer 
	MESSAGE_END();
	m_dEndMessage.AddTime(END_DELAY);
	MovePlayer(pVictim->edict());
	// wait 2 seconds incase the winner dies too, otherwise we anouce the winner
}


BOOL CGameModeDuel::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return CHalfLifeMultiplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}

void CGameModeDuel::ClientDisconnected(edict_t* pEntity)
{
    
	DropPlayer(pEntity);

	if(MinimumClientsConnected()) // 1 guy left
		m_bJustConnected = true; // next guy gets and extra 10 seconds to connect b4 round starts
	CHalfLifeMultiplay::ClientDisconnected(pEntity);
}

void CGameModeDuel::StartRound()
{
	m_dStartMessage.AddTime(ROUND_DELAY);
	m_dStartRound.AddTime(ROUND_DELAY);
	m_bAllowRestrictedSpawning	= false;
	m_bStartSequence			= true;
	m_bInProgress				= true;
	m_nNumber					= 0;
	m_pWinner					= NULL;
	m_nBonus					= DUEL_BONUS_NONE;
	m_pLoser					= NULL;
	m_nResult					= RESULT_NONE;
}

void CGameModeDuel::EndRound()
{
	m_bInProgress		= false;
	m_nNumber			= 0;
	m_bStartSequence	= false;
	m_nRoundTime		= 0;
	UpdateRound();
}
void CGameModeDuel::UpdateRound()
{
	if(m_bInProgress)
	{
		if(IsOver())
			EndRound();
	}
	else if(MinimumClientsConnected())
	{
		if(m_dEndMessage.GetTimeLeft()!=0)
			return;
		StartRound();

		if(m_bJustConnected)
		{	
			m_bJustConnected = false;
			//m_dStartMessage.AddTime(10);
			//m_dStartRound.AddTime(10);
		}
	}
}


float CGameModeDuel :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}


void CGameModeDuel::PlayerSpawn( CBasePlayer *pPlayer )
{
	//BOOL		addDefault;
	//CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	

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


void CGameModeDuel::VersesMessage()
{
	// clean up the list
	m_lEntity.clean();
	char buf[128];
	*buf=NULL;
	CBasePlayer *pPlayer;
	entvars_t *pev = &m_lEntity[0]->v;
	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pev = &m_lEntity[0]->v;
	const char* defender = STRING(pev->netname);
	pev = &m_lEntity[1]->v;
	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pev = &m_lEntity[1]->v;
	const char* challanger = STRING(pev->netname);
	strcpy(buf, defender);
	strcat(buf," vs. ");
	strcat(buf, challanger);
	float fLength = (float)strlen(buf);
	float fOffset = fLength*=0.0035;
	m_hMessage.x = 0.5-fOffset;
	UTIL_HudMessageAll(m_hMessage, buf);
}


void CGameModeDuel::AnounceOutcome()
{
	char buf[128];
	*buf=NULL;
	CBasePlayer* pWinner;
	CBasePlayer* pLoser;

	//if(!MinimumClientsConnected())
	//{
	//	UpdateRound();
	//	return;
	//}

	switch(m_nResult)
	{

	case RESULT_NONE:

		break;

	case RESULT_DRAW:

		if(MinimumClientsConnected())
		{
			m_pWinner->v.takedamage = DAMAGE_NO;
			m_pLoser->v.takedamage = DAMAGE_NO;

			pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));

			SendScoreInfo(pWinner);

			SendScoreInfo(pLoser);
		}
		strcpy(buf, "Draw");
		break;

	case RESULT_TIE:
		
		strcpy(buf, "Tie");
		break;
	case RESULT_TIMEUP:
	
		if(MinimumClientsConnected())
		{
			
			strncpy(buf, "Time up\n", 128);
			strncat(buf, STRING(m_pWinner->v.netname), 128);
			strncat(buf," has forced the win over ", 128);
			strncat(buf, STRING(m_pLoser->v.netname),128 );;
			m_pWinner->v.takedamage = DAMAGE_NO;
			m_pLoser->v.takedamage = DAMAGE_NO;

			CBasePlayer* pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			CBasePlayer* pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			
			pWinner->m_iWins+=1;
			pLoser->m_iLoses+=1;

			SendScoreInfo(pWinner);

		}
		break;
		

	case RESULT_SUICIDE://///////////////////

		if(MinimumClientsConnected())
		{
			m_lEntity[0]->v.takedamage = DAMAGE_NO;
			// give the winner a win
			pWinner = GetClassPtr((CBasePlayer *)&(m_lEntity[0]->v));
			pWinner->m_iWins+=1;

			strncpy(buf, STRING(m_pLoser->v.netname), 128);
			strncat(buf," has committed suicide ", 128);

			SendScoreInfo(pWinner);

		}
		break;

	case RESULT_FORFEIT://///////////////////

		pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
		pLoser->m_iWins-=1;

		strncpy(buf, STRING(m_pLoser->v.netname), 128);
		strncat(buf," has forfeited", 128);

		SendScoreInfo(pLoser);

		break;

	case RESULT_DEFEAT://////////////////////
		{

		if(MinimumClientsConnected())
		{
			
			strncpy(buf, STRING(m_pWinner->v.netname), 128);
			strncat(buf," has defeated ", 128);
			strncat(buf, STRING(m_pLoser->v.netname),128 );

			// add a win to this player
			pLoser = GetClassPtr((CBasePlayer *)&(m_pLoser->v));
			pWinner = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
			pWinner->pev->takedamage = DAMAGE_NO;
			m_pWinner->v.takedamage = DAMAGE_NO;
			
			pWinner->m_iWins+=1;			
			pLoser->m_iLoses+=1;

			if(m_nBonus == DUEL_BONUS_ANNIAILATION)
			{
				MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
				WRITE_BYTE(5);
				MESSAGE_END();
			}
			else if(m_nBonus == DUEL_BONUS_KILLING_SPREE)
			{
				MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
				WRITE_BYTE(6);
				MESSAGE_END();
			}
			else if(m_nBonus == DUEL_BONUS_DOMINATION)
			{
				MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
				WRITE_BYTE(7);
				MESSAGE_END();
			}
			else if(m_nBonus == DUEL_BONUS_FLAWLESS)
			{
				if((m_pWinner->v.health >= 100) && (m_pWinner->v.armorvalue >= 100))
				{
					MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
					WRITE_BYTE(1);
					MESSAGE_END();
				}
				else goto chkowned;
			}
			else if(m_nBonus == DUEL_BONUS_OWNED)
			{
chkowned:
				if(m_pWinner->v.armorvalue >= 100)
				{
					MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
					WRITE_BYTE(0);
					MESSAGE_END();
				}
				else goto chkpunish;
			}
			else if(m_nBonus == DUEL_BONUS_PUNISH)
			{
chkpunish:
				if(m_pWinner->v.armorvalue >= 90)
				{
					MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
					WRITE_BYTE(3);
					MESSAGE_END();
				}
			}
			else if(m_nBonus == DUEL_BONUS_IMMORTAL)
			{
				CBasePlayer* pPlayer = GetClassPtr((CBasePlayer *)&(m_pWinner->v));
				if(pPlayer->m_iDeaths == 0)
				{
					MESSAGE_BEGIN( MSG_ALL, gmsgBonus, NULL);
					WRITE_BYTE(2);
					MESSAGE_END();
				}
			}
			SendScoreInfo(pWinner);
			SendScoreInfo(pLoser);
		}
		break;
		}
	default:
		break;
	}

	float fLength = (float)strlen(buf);
	float fOffset = fLength*=0.0035;
	m_hMessage.x = 0.5-fOffset;
	UTIL_HudMessageAll(m_hMessage, buf);
}

void CGameModeDuel::ToggleSpawningRestriction()
{
	g_bPlayLongjumpSound =! g_bPlayLongjumpSound;
	m_bAllowRestrictedSpawning =! m_bAllowRestrictedSpawning;
}

void CGameModeDuel::SpawnValidPlayers()
{
				
	CBasePlayer* pPlayer=NULL;
	if(!MinimumClientsConnected())
	{
		UpdateRound();
		return;
	}
	g_bPlayLongjumpSound = false;
	int i = 0;
	
	CEntityNode* pTemp = m_lEntity.head();
	
	while(pTemp)
	{
		pPlayer = GetClassPtr((CBasePlayer *)&(pTemp->m_pEntity->v));	
		
		if(pTemp)
		{
			if(i < MinimumClients())
			{
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
		i++;
		pTemp = pTemp->m_pNext;
	}
	g_bPlayLongjumpSound = true;
}

/*
void CGameModeDuel::ASDFASDF()
{
	CBasePlayer* pPlayer=NULL;
	if(!MinimumClientsConnected())
	{
		UpdateRound();
		return;
	}
	
	g_bPlayLongjumpSound = false;
	//for(int i=0; i < m_lEntity.size(); i++) 
	int players = m_lEntity.size();
	CEntityNode* pTemp = m_lEntity.head();
	
	while(pTemp)
	{
		//pPlayer = //GetClassPtr((CBasePlayer *)&(pTemp->m_pEntity->v));	
		//pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[i]->v));
		if(pTemp)
		{
			if(players < MinimumClients()) // valid players, this will be always just the first 2 players in list
			{
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
			}
		}
		pTemp = pTemp->m_pNext;
	}
	g_bPlayLongjumpSound = true;
}

*/
BOOL CGameModeDuel::CanStartObserver(CBasePlayer *pPlayer)
{
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Enter CanStartObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/

	if(!pPlayer->m_isSpectator)
	{
		if(IsInRound(pPlayer->edict()))
		{
			if(!m_bStartSequence)
			{

				MESSAGE_BEGIN( MSG_ALL, gmsgTimer, NULL);
				WRITE_SHORT (-1); 
				MESSAGE_END();

				m_nResult = RESULT_FORFEIT;
				m_pWinner = NULL;
				m_pLoser = pPlayer->edict();
				m_dEndMessage.AddTime(END_DELAY);
				MovePlayer(pPlayer->edict());
				m_lEntity[0]->v.takedamage = DAMAGE_NO; // immediatly stop damage
			}
		}
		#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Exit CanStartObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/
		return TRUE;
	}
			#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Exit CanStartObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/
	return FALSE;
}

BOOL CGameModeDuel::CanStopObserver(CBasePlayer *pPlayer)
{
	return FALSE;
}

int CGameModeDuel::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CGameModeDuel::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

bool CGameModeDuel::IsOver()
{
	if(MinimumClientsConnected())
	{
		CBasePlayer* pPlayer = NULL;
		int nAlive=0;
		for(int i = 0; i < MinimumClients(); i++)
		{
			pPlayer = GetClassPtr((CBasePlayer *)&(m_lEntity[i]->v));
			if(pPlayer->IsAlive())
				nAlive++;
		}
		if(nAlive < MinimumClients())
		{
			if(!m_bStartSequence)
				return true;
		}
	}
	return false;
}


void CGameModeDuel::RemovePlayer(CBasePlayer* pPlayer)
{

	pPlayer->m_bIsRemoved = true;

	if(CanStartObserver(pPlayer))
	{
		pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
	}
	else
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
			WRITE_BYTE( pPlayer->entindex() );
			WRITE_BYTE( 0 );
			WRITE_BYTE( pPlayer->m_isSpectator );
			WRITE_BYTE(	1 );
			WRITE_BYTE(	0 );
		MESSAGE_END();
	}

	DropPlayer(pPlayer->edict());

	MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
	WRITE_BYTE( 1 );
	MESSAGE_END();
}

void CGameModeDuel::ResumePlayer(CBasePlayer* pPlayer)
{
	pPlayer->m_bIsRemoved = false;
	if(!LookUpPlayer(pPlayer->edict()))
		AddPlayer(pPlayer->edict());

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


void CGameModeDuel::SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever)
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


