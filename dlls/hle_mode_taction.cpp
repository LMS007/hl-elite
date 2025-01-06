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

extern int gmsgRadar;
extern int gmsgGameMode;
extern int g_bPlayLongjumpSound;
extern int gmsgReset;
extern int gmsgCap;


void CHalfLifeTAction::PlayerSpawn( CBasePlayer *pPlayer )
{

	if(radar.value)
	{
	
		MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, pPlayer->edict() );   // try to show the radar
			WRITE_SHORT( pPlayer->entindex() );
			WRITE_BYTE( 1 );
		MESSAGE_END();
	}
	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	{  
		if(spawnform.value <=0) 
		{
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveNamedItem( "weapon_crowbar" );
			pPlayer->GiveAmmo( 68, "9mm", _9MM_MAX_CARRY );
		}
		else
		{
			g_bPlayLongjumpSound = false;
			ArmPlayer(pPlayer);
			LoadPlayer(pPlayer);
			FillPlayer(pPlayer);	
			g_bPlayLongjumpSound = true;
		}
		CTeamIndicator::Create(pPlayer);
	}
}
 
void CHalfLifeTAction::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		DeathNotice( pVictim, pKiller, pInflictor );

		pVictim->m_iDeaths += 1;

		FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );

		CBasePlayer *peKiller = NULL;

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
			pKiller->frags += CHalfLifeTeamplay::IPointsForKill( peKiller, pVictim );
			GivePlayerBonus(GetClassPtr((CBasePlayer *)pKiller));
			FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
		}
		else
		{  // killed by the world
			pKiller->frags -= 1;
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

		// update the scores
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
		RecountTeams();
	}
}

int CHalfLifeTAction::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CHalfLifeTAction::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}


