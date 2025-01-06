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
#include	"hle_flag.h"
#include	"player.h"


extern int gmsgTeamInfo;

extern int g_fGameOver;
extern CVoiceGameMgr	g_VoiceGameMgr;
extern int gmsgGameMode;
extern int gmsgTeamNames;
extern int gmsgCtfFlag;
extern int gmsgReturnBlue;
extern int gmsgReturnRed;
extern int gmsgColor;
extern int gmsgDeathMsg;
//extern int gmsgHudColor;
extern int gmsgScoreInfo;
extern int gmsgHudStatus;
extern int gmsgCtfEvent;
extern int gmsgReset;
extern int gmsgRadar;
extern int gmsgFlag;

extern float g_fTimeOffset;
extern CVoteStructure* g_pVote;

// change this
extern int g_nMaxTeams;

extern inline int FNullEnt( CBaseEntity *ent ) { return (ent == NULL) || FNullEnt( ent->edict() ); }
extern DLL_GLOBAL CBaseEntity	*g_pLastSpawn;
//extern edict_t *EntSelectTeamSpawnPoint( CBaseEntity *pPlayer );
extern BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot );

extern char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
extern int num_teams;
extern int team_scores[MAX_TEAMS];

extern COutput debug_file;

CGameModeCTF::CGameModeCTF() 
{

	m_nNumber = 0;
	for(int i = 1; i < 7; i++)
	{
		m_nCaptures[i] = 0;
	}
	if(g_nMaxTeams==0 && FStrEq(gamemode.string, "ctf")) // applies for ctp , so we cant assume its ctf 
	{
		m_nMaxTeams = 2; // default
		RecountTeams();
	}
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
		
		debug_file.OpenFile();
		debug_file << "CGameModeCTF::CGameModeCTF()\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

}
	
void CGameModeCTF::Think()
{									
	if(m_MatchStart.isMatch())
	{
		if(m_MatchStart.m_dDelay.TimeUp())
		{
			m_MatchStart.m_dDelay.AddTime(1);// add 1 second for the next count
			SendCountDown(MAX_SPRITE_NUMBER - m_nNumber);
			m_nNumber++;

			if(m_nNumber > MAX_SPRITE_NUMBER)
			{
				m_MatchStart.startMatch();
				m_nNumber = 0;
				ResetFlags();
				for(int i = 1; i<7;i++)
				{
					m_nCaptures[i]=0;
				}
			}
		}
	}

	for(int i =1; i < 7; i++)
	{
		if(m_nCaptures[i] >= winlimit.value && winlimit.value !=0)
		{
 			GoToIntermission();
		}
	}
	CHalfLifeMultiplay::Think();
}

BOOL CGameModeCTF::IsTeamplay( void )
{
	return TRUE;
}


void CGameModeCTF :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		DeathNotice( pVictim, pKiller, pInflictor );
		CBasePlayer *peKiller = NULL;
		pVictim->m_iDeaths++;

		CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );
		if ( ktmp && (ktmp->Classify() == CLASS_PLAYER) )
			peKiller = (CBasePlayer*)ktmp;

		if ( pVictim->pev == pKiller )  
		{  // killed self
			pKiller->frags -= 1;

		}
		else if ( ktmp && ktmp->IsPlayer() )
		{
			// if a player dies in a deathmatch game and the killer is a client, award the killer some points
			pKiller->frags += IPointsForKill( peKiller, pVictim );
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
	}
}


void CGameModeCTF::CapturedFlag(CBasePlayer *pPlayer)
{

	
	//EMIT_SOUND(ENT(pPlayer->pev), CHAN_BODY, "weapons/xbow_hitbod2.wav", 1, ATTN_NORM); 
	//endgame

	//FillPlayer(pPlayer);
	//LoadPlayer(pPlayer);
	//pPlayer->AddPointsToTeam(1, false);
//	PlaySound( flag_captured.wav
	
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *ent = UTIL_PlayerByIndex( i );
		CBaseEntity *plr = NULL;
		if (ent && ent->IsPlayer())
		{
			plr = (CBasePlayer*)ent;
			EMIT_SOUND(ENT(plr->pev), CHAN_STATIC, "flag_scored.wav", 1, ATTN_NORM);
		}
	}

	
	int team = GetTeamIndex( pPlayer ) + 1;

	MESSAGE_BEGIN( MSG_ALL, gmsgCtfEvent );
		WRITE_BYTE(FLAG_SCORED);
		WRITE_BYTE(team);
		WRITE_BYTE(pPlayer->m_nCtfFlagInHand);
	MESSAGE_END();


	
	m_nCaptures[team]++;
	pPlayer->m_iCaptures++;

	SendScoreInfo(pPlayer);
	
	// up date the scoreboards


}


BOOL CGameModeCTF::CanStartObserver(CBasePlayer *pPlayer)
{
	if(m_MatchStart.isMatch())
	{
		pPlayer->m_bInMatch = false;
		m_MatchStart.m_Players.RemovePlayer(pPlayer->edict());
	}
	if(pPlayer->m_bHasFlag)
	{
		CBaseEntity* pEntity = NULL;
		while (pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow"))
		{
			CFlagFollow* pFl = (CFlagFollow*)pEntity;
			if(pFl->m_pOwner == pPlayer)
			{
				pPlayer->RemoveFlag();
				UTIL_Remove( pEntity );
				break;
			}
		}
	}
	return TRUE;
}

BOOL CGameModeCTF::RestrictObserver( void )
{
	return TRUE;
}



void CGameModeCTF::SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever)
{
	if(pReciever==NULL)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iSaves );
			WRITE_SHORT( pSender->m_iCaptures );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pReciever->edict() );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
 			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iSaves );
			WRITE_SHORT( pSender->m_iCaptures );
		MESSAGE_END();
	}
}


void CGameModeCTF::InitHUD( CBasePlayer *pPlayer )
{
	CHalfLifeTeamplay::InitHUD( pPlayer );
	
  	CBaseEntity* pEntity = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf");
	while (pEntity)
	{
		CFlag* pFlag = (CFlag*)pEntity;
		int effect = pFlag->pev->effects & EF_NODRAW;
		if(effect != EF_NODRAW)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgFlag, NULL, pPlayer->edict() );
				WRITE_SHORT(pFlag->entindex());
				WRITE_SHORT(0);					// flag holder (player)
				WRITE_SHORT(pFlag->pev->origin[0]);
				WRITE_SHORT(pFlag->pev->origin[1]);
				WRITE_BYTE(pFlag->m_nFlagColor);
				WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
			MESSAGE_END();
		}				
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf");
	}

	pEntity = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf_dropped");
 	while (pEntity)
	{
		CFlag* pFlag = (CFlag*)pEntity;
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgFlag, NULL, pPlayer->edict() );
				WRITE_SHORT(pFlag->entindex());
				WRITE_SHORT(0);					// flag holder (player)
				WRITE_SHORT(pFlag->pev->origin[0]);
				WRITE_SHORT(pFlag->pev->origin[1]);
				WRITE_BYTE(pFlag->m_nFlagColor);
				WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pPlayer->edict() );
				WRITE_BYTE(FLAG_DROPPED);
				WRITE_BYTE(0);
				WRITE_BYTE(pFlag->m_nFlagColor);
			MESSAGE_END();
		}				
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf_dropped");
	}

	pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	while (pEntity)
	{
		CFlagFollow* pFlag = (CFlagFollow*)pEntity;
		{
			if(pFlag->m_pOwner)
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgFlag, NULL, pPlayer->edict() );
					WRITE_SHORT(pFlag->entindex());
					WRITE_SHORT(pFlag->m_pOwner->entindex());
					WRITE_SHORT(pFlag->pev->origin[0]);
					WRITE_SHORT(pFlag->pev->origin[1]);
					WRITE_BYTE(pFlag->m_nFlagColor);
					WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pPlayer->edict() );
					WRITE_BYTE(FLAG_CAPTURED);
					WRITE_BYTE(0);
					WRITE_BYTE(pFlag->m_nFlagColor);
				MESSAGE_END();
			}
		}				
        pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	}

	/*for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( plr )
		{

		}
	}*/
}


///////////////////////////////
///////////////////////////////
///////////////////////////////
///////////////////////////////



CCtfTriggerKill::CCtfTriggerKill()
{

}

void CCtfTriggerKill::Spawn ( void )
{
	//SetThink( Think );
	
	bActive = true; // set it on
	SetTouch( &CCtfTriggerKill::TouchKill );


		if (pev->angles != g_vecZero)
		SetMovedir(pev);
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	if ( CVAR_GET_FLOAT("showtriggers") == 0 )
		SetBits( pev->effects, EF_NODRAW );

	UTIL_SetOrigin( pev, pev->origin );	

	if(pev->spawnflags & 1)
	
		bActive = false;		// starts off
	else
		bActive = true;

	pev->nextthink = gpGlobals->time + 0.1;
}


void CCtfTriggerKill::TouchKill( CBaseEntity *pOther )
{
	if(bActive)
	{
		if(pOther)
		{
			if ( !pOther->IsPlayer() )
				return;

			CBasePlayer *pPlayer = (CBasePlayer *)pOther;
			if(g_pGameRules->GetTeamIndex( pPlayer ) + 1 != m_nZoneColor)
				pPlayer->TakeDamage( pev, pev, 1000, DMG_CRUSH );
		}
	}
}



void CCtfTriggerKill::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if(bActive)
		bActive = false;
	else
		bActive = true;
}

void CCtfTriggerKill::KeyValue( KeyValueData *pkvd )
{
	
	int nColor;
	if (FStrEq(pkvd->szKeyName, "zone_color"))
	{
		nColor = atoi(pkvd->szValue);
		nColor = atoi(pkvd->szValue);
		switch(nColor)
		{
		case 1:
			m_nZoneColor = RED;
			break;
		case 2:
			m_nZoneColor =BLUE;
			break;
		case 3:
			m_nZoneColor = GREEN;
			break;
		case 4:
			m_nZoneColor = YELLOW;
			break;
		case 5:
			m_nZoneColor = PURPLE;
			break;
		case 6:
			m_nZoneColor = ORANGE;
			break;
		default:
			m_nZoneColor = NULL;
			break;
		}
	
		pkvd->fHandled = TRUE;
	}

	else
		CBaseEntity::KeyValue( pkvd );
}

void CGameModeCTF::MatchStart(int nType)
{
	m_MatchStart.setMatch(nType);
}

BOOL CGameModeCTF::MatchInProgress(void)
{
	return m_MatchStart.isMatch();
}

void CGameModeCTF::ResetPlayerHud(CBasePlayer* pPlayer)
{
	CHalfLifeTeamplay::ResetPlayerHud(pPlayer);

/*	MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pPlayer->edict());
	WRITE_BYTE(0);
	MESSAGE_END();*/
}

void CGameModeCTF::SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget)
{
	CHalfLifeTeamplay::SetSpectatorHud(pPlayer, pTarget);

/*	MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pPlayer->edict() );
		WRITE_BYTE(0);
	MESSAGE_END();

	if(pTarget->m_bHasFlag)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pPlayer->edict() );
		WRITE_BYTE(GetTeamIndex( pTarget ) + 1);
		MESSAGE_END();
	}*/
}

LINK_ENTITY_TO_CLASS( ctf_kill_zone, CCtfTriggerKill );


