#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"hle_gamemodes.h"
#include	"hle_gamemodes.h"
#include	"game.h"
#include	"items.h"
#include	"voice_gamemgr.h"
#include    "hle_vote.h"
#include    "client.h"
#include    "hle_mode_action.h"


// client dll messages	
extern int gmsgScoreInfo;
extern int gmsgCurWeapon;
extern int gmsgGameMode;
extern int gmsgTimer;
//extern int gmsgHudColor;
extern int gmsgReset;
extern int g_bPlayLongjumpSound ;

CGameModeAction::CGameModeAction()
{
    /*m_hudtAction.x            = 0.45f;
    m_hudtAction.y            = 0.1f;
    m_hudtAction.a1           = 0.0f;
    m_hudtAction.a2           = 0.0f;
    m_hudtAction.b1           = 0.0f;
    m_hudtAction.b2           = 255.0f;
    m_hudtAction.g1           = 100.0f;
    m_hudtAction.g2           = 255.0f;
    m_hudtAction.r1           = 255.0f;
    m_hudtAction.r2            = 255.0f;
    m_hudtAction.channel      = 3;
    m_hudtAction.effect       = 2;
    m_hudtAction.fxTime       = 0.5;
    m_hudtAction.fadeinTime   = 0.1;
    m_hudtAction.fadeoutTime  = 2;    
    m_hudtAction.holdTime     = 5;*/

//    m_nServerMaps = 0;
//    m_bMapListInIt = FALSE;
//    m_ppMapList = NULL;

}


void CGameModeAction::PlayerSpawn( CBasePlayer *pPlayer )
{

//	BOOL		addDefault;
//	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	
	/*addDefault = TRUE;

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	if ( addDefault )*/
	{  
//      if(addDefault)
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
		}
	}
}


void CGameModeAction::InitHUD( CBasePlayer *pl )
{
	
	CHalfLifeMultiplay::InitHUD(pl);
	
}

void CGameModeAction::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
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
		pKiller->frags += CGameModeAction::IPointsForKill( peKiller, pVictim );
		//LoadPlayer(peKiller);// give full ammo
        //FillPlayer(peKiller); // give some life and hev
		GivePlayerBonus(peKiller);
		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	}
	else
	{  // killed by the world
		pKiller->frags -= 1;
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
}


int CGameModeAction::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	//return GR_PLR_DROP_GUN_ACTIVE;
	return GR_PLR_DROP_GUN_NO;
}

int CGameModeAction::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	//return GR_PLR_DROP_GUN_ACTIVE;
	return GR_PLR_DROP_AMMO_NO;
}



