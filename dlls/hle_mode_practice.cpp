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
#include    "hle_mode_practice.h"


// client dll messages	

extern int gmsgReset;
extern int gmsgGameMode;
extern int g_bPlayLongjumpSound ;

int g_PracticeMode;  // specical global for dealing with ammo count

void CGameModePractice::PlayerSpawn( CBasePlayer *pPlayer )
{
	//pPlayer->pev->takedamage = DAMAGE_NO;  // cant take damage in pratice mode

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	{  
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

extern int gmsgMapNames;



void CGameModePractice::InitHUD( CBasePlayer *pl )
{
	CHalfLifeMultiplay::InitHUD(pl);
	
}

int CGameModePractice::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CGameModePractice::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

