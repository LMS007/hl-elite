

#include    <stdio.h>
#include    <io.h>
#include    <time.h>
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"hle_gamemodes.h"
#include	"gamerules.h" 
#include	"game.h" 
//#include	"items.h"
#include    "hle_utility.h"
#include    "hle_vote.h"
#include "items.h"
#include "hle_flag.h"
#include "hle_maploader.h"
#include "hle_vadmin.h"

//#include "util.h"


extern int gmsgMapNames;//hle
extern int gmsgCountNumber;
extern int gmsgTimeLeft;
extern int gmsgFragsLeft;
extern int gmsgResetHUD;
extern int gmsgHudStatus;
extern int gmsgScoreInfo;
extern int gmsgCtfEvent;
extern int gmsgFlag;
extern CMapList g_MapsServer;

extern COutput debug_file;

extern CMapList g_MapsServer;
extern float g_fTimeOffset;

ofstream g_OutLocFile;

//server cmd function ptrs
void (*ptAdd)(void);
void (*ptRemove)(void);
void (*ptCallMatch)();
void (*ptCreateCTFFile)(void);
void (*ptCompleteCTFFile)(void);
void (*ptEmitPlayer)(void);
void (*ptEjectPlayer)(void);
void (*ptAddTime)(void);
void (*ptAddClock)(void);
void (*ptSubtractTime)(void);

void (*ptACTION)(void);
void (*ptTOURNEY)(void);
void (*ptFFA)(void);
void (*ptTEAM)(void);
void (*ptCTP)(void);
void (*ptCTF)(void);
void (*ptLMS)(void);
void (*ptDUEL)(void);
void (*ptTDUEL)(void);
void (*ptTLMS)(void);
void (*ptTACTION)(void);
void (*ptPRACTICE)(void);


void (*ptCreateLocFile)(void);
void (*ptCompleteLocFile)(void);
void (*ptAddFile)(void);
void (*ptStart)(void);

//void (*ptSendMapsAgain)(void);


//=======================

void SendCountDown(int nNumber)
{
	MESSAGE_BEGIN( MSG_ALL, gmsgCountNumber, NULL);
	WRITE_SHORT (nNumber);
	MESSAGE_END();
}

void FillPlayer(CBasePlayer *pPlayer)
{
    pPlayer->pev->health = pPlayer->pev->max_health;
    pPlayer->pev->armorvalue = pPlayer->max_hev;
}

bool SearchForWeapon(char* pszWeapon)
{
	CBaseEntity* pEntity = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, pszWeapon);
	return pEntity ? true : false;
}

bool SearchForWeaponBanned(char* pszWeapon)
{
	char* pWeapon = strtok(banweapons.string, ";");
	while(pWeapon)
	{
		if(FStrEq(pszWeapon, pWeapon))
		{
			return true;
		}
		pWeapon = strtok(NULL, ";");
		//pWeapon = strtok(pWeapon, ";");
	}
	return false;
}
void ArmPlayer(CBasePlayer *pPlayer)
{

	
	pPlayer->GiveNamedItem( "item_longjump" );

	if(!SearchForWeaponBanned("crowbar"))
		pPlayer->GiveNamedItem( "weapon_crowbar" );
	if(!SearchForWeaponBanned("9mmhandgun"))
		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
	if(!SearchForWeaponBanned("357"))
		pPlayer->GiveNamedItem( "weapon_357" );
	if(!SearchForWeaponBanned("shotgun"))
		pPlayer->GiveNamedItem( "weapon_shotgun" );
	if(!SearchForWeaponBanned("9mmAR"))
		pPlayer->GiveNamedItem( "weapon_9mmAR" );
	if(!SearchForWeaponBanned("crossbow"))
		pPlayer->GiveNamedItem( "weapon_crossbow" );
	if(!SearchForWeaponBanned("gauss"))
		pPlayer->GiveNamedItem( "weapon_gauss" );
	if(!SearchForWeaponBanned("rpg"))
		pPlayer->GiveNamedItem( "weapon_rpg" );
	if(!SearchForWeaponBanned("hornetgun"))
		pPlayer->GiveNamedItem( "weapon_hornetgun" );
	if(!SearchForWeaponBanned("egon"))
		pPlayer->GiveNamedItem( "weapon_egon" );
	if(!SearchForWeaponBanned("satchel"))
		pPlayer->GiveNamedItem( "weapon_satchel" );
	if(!SearchForWeaponBanned("tripmine"))
		pPlayer->GiveNamedItem( "weapon_tripmine" );
	if(!SearchForWeaponBanned("handgrenade"))
		pPlayer->GiveNamedItem( "weapon_handgrenade" );
	if(!SearchForWeaponBanned("snark"))
		pPlayer->GiveNamedItem( "weapon_snark" );
	if(!SearchForWeaponBanned("m249"))
		pPlayer->GiveNamedItem( "weapon_m249" );
	if(!SearchForWeaponBanned("sniperrifle"))
		pPlayer->GiveNamedItem( "weapon_sniperrifle" );
	if(!SearchForWeaponBanned("shockrifle"))
		pPlayer->GiveNamedItem( "weapon_shockrifle" );
	if(!SearchForWeaponBanned("sporelauncher"))
		pPlayer->GiveNamedItem( "weapon_sporelauncher" );
	

}

void ArmPlayerRestricted(CBasePlayer *pPlayer)
{

	pPlayer->GiveNamedItem( "item_longjump" );
	pPlayer->GiveNamedItem( "weapon_crowbar" );
	pPlayer->GiveNamedItem( "weapon_9mmhandgun" );

	if(SearchForWeapon("weapon_357"))
		pPlayer->GiveNamedItem( "weapon_357" );
	if(SearchForWeapon("weapon_shotgun"))
	    pPlayer->GiveNamedItem( "weapon_shotgun" );
	if(SearchForWeapon("weapon_9mmAR"))
		pPlayer->GiveNamedItem( "weapon_9mmAR" );
	if(SearchForWeapon("weapon_crossbow"))
		pPlayer->GiveNamedItem( "weapon_crossbow" );
	if(SearchForWeapon("weapon_gauss"))
		pPlayer->GiveNamedItem( "weapon_gauss" );
	if(SearchForWeapon("weapon_rpg"))
		pPlayer->GiveNamedItem( "weapon_rpg" );
	if(SearchForWeapon("weapon_hornetgun"))
		pPlayer->GiveNamedItem( "weapon_hornetgun" );
	if(SearchForWeapon("weapon_egon"))
		pPlayer->GiveNamedItem( "weapon_egon" );
	if(SearchForWeapon("weapon_satchel"))
		pPlayer->GiveNamedItem( "weapon_satchel" );
	if(SearchForWeapon("weapon_tripmine"))
		pPlayer->GiveNamedItem( "weapon_tripmine" );
	if(SearchForWeapon("weapon_handgrenade"))
		pPlayer->GiveNamedItem( "weapon_handgrenade" );
	if(SearchForWeapon("weapon_snark"))
		pPlayer->GiveNamedItem( "weapon_snark" );
	if(SearchForWeapon("weapon_m249"))
		pPlayer->GiveNamedItem( "weapon_m249" );
	if(SearchForWeapon("weapon_sniperrifle"))
		pPlayer->GiveNamedItem( "weapon_sniperrifle" );
	if(SearchForWeapon("weapon_shockrifle"))
		pPlayer->GiveNamedItem( "weapon_shockrifle" );
	if(SearchForWeapon("weapon_sporelauncher"))
		pPlayer->GiveNamedItem( "weapon_sporelauncher" );
	

}

void LoadPlayer(CBasePlayer *pPlayer)
{
     for(int i =0; i < MAX_ITEM_TYPES; i++)
    {
       
        CBasePlayerItem *pPlayerItem = pPlayer->m_rgpPlayerItems[ i ]; 
        CBasePlayerWeapon *gun = (CBasePlayerWeapon*)pPlayerItem;

        if (pPlayer->m_rgpPlayerItems[i])
        {
		
            while ( pPlayerItem )
		    {
                switch(gun->m_iId)
                {
                    case WEAPON_GLOCK:
                    {
                        gun->m_iClip = GLOCK_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = _9MM_MAX_CARRY;
                 
                        break;
                    }
                    case WEAPON_PYTHON:
                    {
                        gun->m_iClip = PYTHON_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = _357_MAX_CARRY;
                        break;
                    }
                    case WEAPON_MP5:
                    {
                        gun->m_iClip = MP5_MAX_CLIP;  // give 50 not 25
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = _9MM_MAX_CARRY;
                        pPlayer->m_rgAmmo[gun->m_iSecondaryAmmoType] = M203_GRENADE_MAX_CARRY;
                        break;
                    }
                    case WEAPON_CROSSBOW:
                    {
                        gun->m_iClip = CROSSBOW_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = BOLT_MAX_CARRY;
                        break;
                    }
                    case WEAPON_SHOTGUN:
                    {
                        gun->m_iClip = SHOTGUN_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = BUCKSHOT_MAX_CARRY;
                        break;
                    }
                    case WEAPON_RPG:
                    {
                        gun->m_iClip = RPG_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = ROCKET_MAX_CARRY;
                        break;
                    }
                    case WEAPON_EGON:
                    {
                        gun->m_iClip = EGON_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = URANIUM_MAX_CARRY;
                        break;
                    }
                    case WEAPON_GAUSS:
                    {
                        gun->m_iClip = GAUSS_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = URANIUM_MAX_CARRY;
                        break;
                    }
                    case WEAPON_HORNETGUN:
                    {
                        gun->m_iClip = HORNETGUN_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = HORNET_MAX_CARRY;
                        break;
                    }
                    case WEAPON_HANDGRENADE:
                    {
                        gun->m_iClip = HANDGRENADE_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = HANDGRENADE_MAX_CARRY;
                        break;
                    }
                    case WEAPON_TRIPMINE:
                    {
                        gun->m_iClip = TRIPMINE_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = TRIPMINE_MAX_CARRY;
                        break;
                    }
                    case WEAPON_SATCHEL:
                    {
                        gun->m_iClip = SATCHEL_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = SATCHEL_MAX_CARRY;
                        break;
                    }
                    case WEAPON_SNARK:
                    {
                        gun->m_iClip = SNARK_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = SNARK_MAX_CARRY;
                        break;
                    }
					case WEAPON_SNIPER:
                    {
                        gun->m_iClip = SNIPER_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = _762_MAX_CARRY;
                        break;
                    }
					case WEAPON_M249:
                    {
                        gun->m_iClip = M249_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = _556_MAX_CARRY;
                        break;
                    }
					case WEAPON_SHOCK:
                    {
                        gun->m_iClip = SHOCK_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = SHOCK_MAX_CARRY;
                        break;
                    }
					case WEAPON_SPORE:
                    {
                        gun->m_iClip = SPORE_MAX_CLIP;
	                    pPlayer->m_rgAmmo[gun->m_iPrimaryAmmoType] = SPORE_MAX_CARRY;
                        break;
                    }
                }
                gun = (CBasePlayerWeapon*)gun->m_pNext;
                pPlayerItem = pPlayerItem->m_pNext;
            }

        }
			 
    }

}


void HLE_EndGame()
{
	MESSAGE_BEGIN( MSG_ALL, gmsgResetHUD, NULL );
		WRITE_BYTE( 0 );
	MESSAGE_END();
}

CMatchStart::CMatchStart()
{
	m_hudtMessage.x            = 0.40f;
    m_hudtMessage.y            = 0.2f;
    m_hudtMessage.a1           = 0.0f;
    m_hudtMessage.a2           = 0.0f;
    m_hudtMessage.b1           = 0.0f;
    m_hudtMessage.b2           = 255.0f;
    m_hudtMessage.g1           = 255.0f;
    m_hudtMessage.g2           = 255.0f;
    m_hudtMessage.r1           = 0.0f;
    m_hudtMessage.r2           = 255.0f;
    m_hudtMessage.channel      = 3;
    m_hudtMessage.effect       = EFFECT;
    m_hudtMessage.fxTime       = FX_TIME;
    m_hudtMessage.fadeinTime   = FADE_TIME_IN;
    m_hudtMessage.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtMessage.holdTime     = 5;
	m_bInProgess = false;
	m_nMatchStart = MATCHSTART_NONE;
}
int CMatchStart::getMatch()
{
	return m_nMatchStart;
}
bool CMatchStart::isMatch()
{
	if(m_nMatchStart)
		return true;
	return false;
}

void CMatchStart::callMatch( ) 
{
	if(CMD_ARGC()>1)
		g_pGameRules->MatchStart(atoi(CMD_ARGV(1)));
	else
		g_pGameRules->MatchStart(matchstart.value);
}


void CMatchStart::setMatch(int nMatch) 
{
	char cBuf[512];
	*cBuf = NULL;
	if((nMatch < 1) || (nMatch > 3))
	{
		m_dDelay.ClearTime();
		
		if(m_nMatchStart!=0)
		{
			m_hudtMessage.r1 = 255;
			m_hudtMessage.b1 = 0;
			m_hudtMessage.g1 = 255;
			UTIL_HudMessageAll(m_hudtMessage, "\"matchstart\" has been reset \nMatch play is over");
		}
		m_hudtMessage.b1           = 0.0f;
		m_hudtMessage.g1           = 255.0f;
		m_hudtMessage.r1           = 0.0f;
		m_bInProgess = false;

		MESSAGE_BEGIN( MSG_ALL, gmsgHudStatus, NULL);
		WRITE_BYTE (2); 
		MESSAGE_END();

		m_nMatchStart = MATCHSTART_NONE;
	}
	else
	{
		m_nMatchStart = nMatch;

		if(m_nMatchStart == 1)
		{
			m_dDelay.AddTime(5); 
			strcat(cBuf, "Unloaded match restart in 10 seconds\nThe teams are now locked");
		}
		
		else if(m_nMatchStart ==2)
		{
			m_dDelay.AddTime(5); 
			strcat(cBuf, "Fully Loaded match restart in 10 seconds\nThe teams are now locked");
		}
		else
		{
			strcat(cBuf, "None start matchstart\nThe teams are now locked");
			startMatch();
		}
		
		
		UTIL_HudMessageAll(m_hudtMessage, cBuf);

		MESSAGE_BEGIN( MSG_ALL, gmsgHudStatus, NULL);
		WRITE_BYTE (0); 
		MESSAGE_END();

		
	}
}

CMatchStart::~CMatchStart()
{
	//m_Players.ClearQueue();
}
void CMatchStart::startMatch()
{ 

	m_Players.ClearQueue();

	m_bInProgess = true;

	// fix to add more to the timelimit
	g_fTimeOffset = gpGlobals->time;
	if(m_nMatchStart!=3)
	{
		RemoveAllEntites(); 
		RespawnAllEntites();
	}
		
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;				
			// fix to set the time correctly
			pPlayer->m_fOldTime = 0;			
		
			pPlayer->pev->frags = 0; 
			pPlayer->m_iDeaths = 0;
			pPlayer->m_iCaptures = 0;
			pPlayer->m_iSaves = 0;
			pPlayer->m_iWins = 0;

			g_pGameRules->SendScoreInfo(pPlayer);
			

			if(!pPlayer->m_isSpectator)
			{
				pPlayer->m_bInMatch = true;
				
				if(m_nMatchStart!=3)
				{
					pPlayer->RemoveAllItems( FALSE );
					int spawnformtemp = spawnform.value;				
					
					if(getMatch() == MATCHSTART_FULL)
						spawnform.value = 1;
					pPlayer->Spawn();
					spawnform.value = spawnformtemp;
				}
			}
		}
	}
	m_dDelay.ClearTime();

#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "CMatchStart::startMatch(); END\n";
		debug_file << m_nMatchStart << endl;
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

}

void RespawnAllEntites() 
{
	CBaseEntity *pEntity = NULL;
	CBasePlayerAmmo* pAmmo = NULL;
	CItem* pItem = NULL;
	CBasePlayerWeapon* pWpn = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, "func_healthcharger");
	while (pEntity)
	{
		CWallHealth* pWKit = (CWallHealth*)pEntity;
		pWKit->Recharge();
        pEntity = UTIL_FindEntityByClassname( pEntity, "func_healthcharger");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "func_recharge");
	while (pEntity)
	{
		CRecharge* pRchrg = (CRecharge*)pEntity;
		pRchrg->Recharge();
        pEntity = UTIL_FindEntityByClassname( pEntity, "func_recharge");
	}

	pEntity = UTIL_FindEntityByClassname( pEntity, "item_healthkit");
	while (pEntity)
	{
		pItem = (CItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_healthkit");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_battery");
	while (pEntity)
	{
		pItem = (CItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_battery");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_longjump");
	while (pEntity)
	{
		pItem = (CItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_longjump");
	}
	//////////// ammo ////////////
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_9mmclip");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_9mmclip");
	}

	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_9mmAR");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_9mmAR");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_9mmbox");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_9mmbox");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_ARgrenades");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_ARgrenades");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_buckshot");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_buckshot");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_357");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_357");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_rpgclip");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_rpgclip");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_gaussclip");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_gaussclip");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_crossbow");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_crossbow");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_crossbow");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_crossbow");
	}
	// op4 ammo
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_762");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_762");
	}/*
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_spore");
	
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_spore");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_sporeball");		// spore has 2 ammo types, 1 for dropping
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_sporeball");
	}*/
	pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_556");
	while (pEntity)
	{
		pAmmo = (CBasePlayerAmmo*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ammo_556");
	}
	//////////// weapons ////////////
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_crowbar");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_crowbar");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_9mmhandgun");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_9mmhandgun");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_357");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_357");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_shotgun");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_shotgun");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_9mmAR");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_9mmAR");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_rpg");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_rpg");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_gauss");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_gauss");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_crossbow");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_crossbow");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_egon");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_egon");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_satchel");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_satchel");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_handgrenade");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_handgrenade");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_tripmine");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_tripmine");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_snark");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_snark");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_hornetgun");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_hornetgun");
	}
	//
	// op4 weapons
	//
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_m249");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_m249");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_sniperrifle");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_sniperrifle");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_shockrifle");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_shockrifle");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_sporelauncher");
	while (pEntity)
	{
		pWpn = (CBasePlayerWeapon*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_sporelauncher");
	}
}
void ResetFlags()
{
	CBaseEntity* pEntity = NULL;;
	pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	while (pEntity)
	{
		CFlagFollow* pFlag = (CFlagFollow*)pEntity;
		pFlag->Reset();
        pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
	while (pEntity)
	{
		CFlagStatic* pFlag = (CFlagStatic*)pEntity;
		pFlag->Reset();
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
	}			
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf");
	while (pEntity)
	{
		CFlag* pFlag = (CFlag*)pEntity;
		pFlag->Reset();
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf_dropped");
	while (pEntity)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
			WRITE_SHORT(pEntity->entindex());	
			WRITE_SHORT(0);
			WRITE_SHORT(0);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_BYTE(2);				
		MESSAGE_END();

		UTIL_Remove( pEntity );
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf_dropped");
	}
	CFlag::ReturnFlag(RED);
	CFlag::ReturnFlag(BLUE);
	CFlag::ReturnFlag(GREEN);
	CFlag::ReturnFlag(PURPLE);
	CFlag::ReturnFlag(YELLOW);
	CFlag::ReturnFlag(ORANGE);

	MESSAGE_BEGIN( MSG_ALL, gmsgCtfEvent );
		WRITE_BYTE(RESET_ALL_FLAGS);
		WRITE_BYTE(0);
		WRITE_BYTE(0);
	MESSAGE_END();
}	


void RemoveAllEntites() 
{
	CBaseEntity *pEntity = NULL;
	
	pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	while (pEntity)
	{
		FireTargets( "flag_follow", pEntity, pEntity, USE_TOGGLE, 0 );
		UTIL_Remove( pEntity ); 
        pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
		
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "rpg_rocket");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			STOP_SOUND( pEntity->edict(), CHAN_VOICE, "weapons/rocket1.wav" );
			FireTargets( "rpg_rocket", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "rpg_rocket");
		
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weaponbox");
	while (pEntity)
	{
		//if (pEntity->IsAlive())
		// weaponboxs are not alived =P
		{
			FireTargets( "weaponbox", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity );
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "weaponbox");
	}		

	pEntity = UTIL_FindEntityByClassname( pEntity, "crossbow_bolt");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "crossbow_bolt", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity );
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "crossbow_bolt");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_handgrenade");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "weapon_handgrenade", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity );
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_handgrenade");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "hornet");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "hornet", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "hornet");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "grenade");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "grenade", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "grenade");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "monster_snark");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "monster_snark", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "monster_snark");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "monster_satchel");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "monster_satchel", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "monster_satchel");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "monster_tripmine");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			CTripmineGrenade* pNade = (CTripmineGrenade*)pEntity; // super fucking hack CTripmineGrenade is in weapon.h now
			pNade->KillBeam(); 
			FireTargets( "monster_tripmine", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "monster_tripmine");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "shock_beam");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			FireTargets( "spore", pEntity, pEntity, USE_TOGGLE, 0 );
			((CShockBeam*)pEntity)->RemoveEntity();
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "shock_beam");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "spore");
	while (pEntity)
	{
		if (pEntity->IsAlive())
		{
			if(!((CSpore*)pEntity)->m_bInPod)
			{
				FireTargets( "spore", pEntity, pEntity, USE_TOGGLE, 0 );
				((CSpore*)pEntity)->RemoveEntity();
			}
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "spore");
	}
	
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt && pEnt->IsPlayer( ) )
		{
		 	CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			if (pPlayer->m_pActiveItem)
			{
				pPlayer->m_pActiveItem->Holster( );
			}

		}
	}

}


void ShowGameInfo(CBasePlayer* pPlayer, bool All, float time)
{
	char print_string[1024];
    print_string[0]=NULL;

    hudtextparms_t      h_text;
	h_text.x            = 0.05f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 255.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 255.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 255.0f;
    h_text.r2            = 255.0f;
    h_text.channel      = 4;
    h_text.effect       = 0;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = time;
    
	strncat(print_string, "Game Settings\n------------\n", 1023);  
	strncat(print_string, "mp_gamemode ", 1023);
    strncat(print_string, gamemode.string, 1023);
	
    strncat(print_string, "\nmp_winlimit ", 1023);
    //strncat(print_string, UTIL_dtos3(int(winlimit.value)), 1023);
	strncat(print_string, winlimit.string, 3);

    strncat(print_string, "\nmp_fraglimit ", 1023);
    //strncat(print_string, UTIL_dtos4(int(fraglimit.value)), 1023);
	strncat(print_string, fraglimit.string, 3);

    strncat(print_string, "\nmp_spawnlimit ", 1023);
    strncat(print_string, spawnlimit.string, 3);
    
    strncat(print_string, "\nsv_maxspeed ", 1023);
    strncat(print_string, g_maxspeed->string, 3);

    strncat(print_string, "\nmp_weaponstay ", 1023);
    strncat(print_string, weaponstay.string, 1);

    strncat(print_string, "\nmp_friendlyfire ", 1023);
    strncat(print_string, friendlyfire.string, 1);

    strncat(print_string, "\nmp_forcerespawn ", 1023);
    strncat(print_string, forcerespawn.string, 1);
   
    strncat(print_string, "\nmp_falldamage ", 1023);
    strncat(print_string, falldamage.string, 1);

    strncat(print_string, "\nmp_allowvotes ", 1023);
    strncat(print_string, allowvotes.string, 1);

    strncat(print_string, "\nmp_spawnform ", 1023);
    strncat(print_string, spawnform.string, 1);

//    strncat(print_string, "\nmp_tossitem ", 1023);
 //   strncat(print_string, tossitem.string, 1);

	strncat(print_string, "\nmp_loselongjump ", 1023);
    strncat(print_string, loselongjump.string, 1);

	strncat(print_string, "\nmp_showlongjump ", 1023);
    strncat(print_string, showlongjump.string, 1);


	strncat(print_string, "\nmp_radar ", 1023);
    strncat(print_string, radar.string, 1);

	if(All == true)
	{
		UTIL_HudMessageAll( h_text, print_string);
		//ShowPlayerInfo(pPlayer, true, time);
	}
	else
	{
		UTIL_HudMessage(pPlayer, h_text, print_string);
		//ShowPlayerInfo(pPlayer, false, time);
	}
	

}

void ShowPlayerInfo(CBasePlayer* pPlayer, bool All, float time)
{
	char print_string[1024];
    print_string[0]=NULL;

    hudtextparms_t      h_text;
	h_text.x            = 0.02f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 30.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 85.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 255.0f;
    h_text.r2           = 255.0f;
    h_text.channel      = 3;
    h_text.effect       = 0;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = time+2;

	strcpy(print_string, "Players' ID's\n------------\n");
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			strncat(print_string,STRING(pPlayer->pev->netname), 1023);
			strncat(print_string," ", 1023);
			strncat(print_string,GETPLAYERAUTHID(pPlayer->edict()), 1023);
			strncat(print_string,"\n", 1023);
		}
	}

	if(All == true)
	{
		UTIL_HudMessageAll( h_text, print_string);
	}
	else
	{
		UTIL_HudMessage(pPlayer, h_text, print_string);
	}
}



CEntityNode::~CEntityNode() {}

CEntityNode::CEntityNode()
{
	m_pEntity = NULL;
	m_pNext	= NULL;
}


CEntityList::CEntityList()
{
	m_pHead = NULL;
	m_nSize = 0;
}

CEntityList::~CEntityList()
{
	clear();
}

void CEntityList::push_back( edict_t* pNewEntity )
{
	if(!pNewEntity) 
		return;

	CEntityNode* pNew = new CEntityNode();
	CEntityNode* pParent = NULL;
	CEntityNode* pTemp = m_pHead;
	pNew->m_pEntity = pNewEntity; 

	while(pTemp)
	{
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	if(!m_pHead)
		m_pHead = pNew;	
	else
		pParent->m_pNext =  pNew;	
	
	m_nSize++;
}

void CEntityList::clean()
{
	CEntityNode* pTemp = m_pHead;
	CEntityNode* pParent = NULL;
	CEntityNode* pNext = NULL;
	while(pTemp)
	{
		if(FStrEq(STRING(pTemp->m_pEntity->v.netname), ""))
		{
			pNext = pTemp->m_pNext;
			if(pTemp==m_pHead)
			{
				m_pHead = pNext;
				pParent = m_pHead;
			}
			else
				pParent->m_pNext = pNext;

			delete pTemp;
			m_nSize--;
			pTemp = pNext;		
		}
		else
		{
			pParent = pTemp;
			pTemp = pTemp->m_pNext;
		}
		
	}
}
int CEntityList::lookup(edict_t* pEntity)
{
	CEntityNode* pTemp = m_pHead;
	for(int i = 0; i < m_nSize; i++)
	{
		if(pEntity == pTemp->m_pEntity)
			return i;
		pTemp = pTemp->m_pNext;
	}
	return -1;

}

void CEntityList::clear()
{
	CEntityNode* pTemp = m_pHead;
	//for(int i=0; i < m_nSize; i++)
	while(pTemp)
	{
		m_pHead = m_pHead->m_pNext;
		//if(pTemp)
		delete pTemp;
		pTemp = m_pHead;
	}
	m_nSize = 0;
	m_pHead = NULL;
}

//////////////////////////////////////////////
// deletes  the nIndex  pEntity in the list //
//////////////////////////////////////////////

void CEntityList::remove(int nIndex)
{
	if(nIndex >= m_nSize || (nIndex < 0))
		return;

	CEntityNode* pTemp = m_pHead;
	CEntityNode* pParent = NULL;

	for(int i=0; i < m_nSize; i++)
	{		
		if(i == nIndex)
		{
			if(!pParent) // implies pTemp == m_pHead;
				m_pHead = m_pHead->m_pNext;
			else
				pParent->m_pNext = pTemp->m_pNext;			
			delete pTemp;
			break;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	m_nSize--;
}


////////////////////////////////////////////////////////
// deletes only the first identical pEntity in the list //
////////////////////////////////////////////////////////

void CEntityList::remove(edict_t* pEntity)
{
	if(pEntity == NULL)
		return;
	
	CEntityNode* pTemp = m_pHead;
	CEntityNode* pParent = NULL;

	while(pTemp)
	{
		if(pTemp->m_pEntity == pEntity)
		{
			if(!pParent) // implies pTemp == m_pHead;
				m_pHead = pTemp->m_pNext;
			else
				pParent->m_pNext = pTemp->m_pNext;	
			delete pTemp;
			break;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	m_nSize--;
}

int CEntityList::size()
{
	return this->m_nSize;
}

CEntityNode* CEntityList::head()
{
	return this->m_pHead;
}
edict_t* CEntityList::operator [] ( int nIndex )
{
	if(nIndex >= m_nSize)
		return NULL;

	CEntityNode* pTemp = m_pHead;
	
	for(int i=0; i < m_nSize; i++)
	{
		if(i == nIndex)
			break;
		pTemp = pTemp->m_pNext;
	}
	return pTemp->m_pEntity; // crash BUG hle
}




CMapPackage::CMapPackage()
{
	Reset();
}

void CMapPackage::Reset()
{
	m_nMapsSent = 0;
	m_dPackageDelay.AddTime(1);
	m_bMapsToSend = true;
}

void CMapPackage::UpdateSending()
{
	if(!g_MapsServer.m_bGotMaps)
		return;
	if(m_nMapsSent==0)
	{
		m_pStart = (CMapName*)g_MapsServer.getHead();
	}
	if(m_nMapsSent >= g_MapsServer.size())
		m_bMapsToSend = false;

}


void CMapPackage::SendPackage(edict_t* pReciever)
{
	if(m_bMapsToSend)
	{
		UpdateSending();
		if(m_bMapsToSend) // ugly hax
		{
			m_pStart = g_MapsServer.SendMapsToClient(pReciever, m_pStart, MAPS_PER_PACKAGE);
			m_nMapsSent += MAPS_PER_PACKAGE;
		}
		
	}
}




CPlayerInfo::CPlayerInfo(CBasePlayer* pPlayer)
{
	m_dTime.AddTime(resumetime.value);
	m_nWonID = atol(GETPLAYERAUTHID(pPlayer->edict()));
	m_nWins = pPlayer->m_iWins;
	m_nDeaths = pPlayer->m_iDeaths;
	m_nFrags = pPlayer->pev->frags;
	m_nCaptures = pPlayer->m_iCaptures;
	m_nSaves = pPlayer->m_iSaves;
	*m_szTeam=NULL;
	strcpy(m_szTeam, pPlayer->m_szTeamName);
	m_pNext = NULL;
}

void CPlayerInfo::ResetPlayer(CBasePlayer* pPlayer)
{
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "CPlayerInfo::ResetPlayer()\n";
		debug_file << (char*)STRING(pPlayer->pev->netname) << endl;
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	pPlayer->m_iWins = m_nWins;
	pPlayer->m_iDeaths = m_nDeaths;
	pPlayer->pev->frags = m_nFrags;
	pPlayer->m_iSaves = m_nSaves;
	*pPlayer->m_szTeamName = NULL;
	g_pGameRules->ChangePlayerTeam(pPlayer, m_szTeam, true, false);
}

CPlayerRecoverQueue::CPlayerRecoverQueue()
{
	m_nSize = 0;
	m_pHead = NULL;
}

CPlayerRecoverQueue::~CPlayerRecoverQueue()
{
	ClearQueue();

#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "CPlayerRecoverQueue::~CPlayerRecoverQueue()\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================
}

void CPlayerRecoverQueue::AddPlayer(edict_t* pClient)
{
	CBasePlayer* pPlayer = GetClassPtr((CBasePlayer*)&pClient->v);
	CPlayerInfo* pTemp = m_pHead;
	CPlayerInfo* pParent = NULL;
	CPlayerInfo* pNewInfoNode = new CPlayerInfo(pPlayer);
	while(pTemp)
	{
		pParent=pTemp;
		pTemp = pTemp->m_pNext;
	}

	if(!pParent)
		m_pHead = pNewInfoNode;
		
	else
		pParent->m_pNext = pNewInfoNode;
	
	m_nSize++;

}

CPlayerInfo* CPlayerRecoverQueue::GetHead()
{
	return m_pHead;
}

CPlayerInfo* CPlayerRecoverQueue::LocatePlayer(CBasePlayer* pPlayer)
{
	long wonID = atol(GETPLAYERAUTHID(pPlayer->edict()));
	CPlayerInfo* pTemp = m_pHead;
	while(pTemp)
	{
		if(wonID == pTemp->m_nWonID) // crash on this line hle BUG
			return pTemp;
		else
			pTemp = pTemp->m_pNext;
	}
	return NULL; // not in list
}

void CPlayerRecoverQueue::RemovePlayer(edict_t* pClient)
{
	long wonID = atol(GETPLAYERAUTHID(pClient));
	CPlayerInfo* pTemp = m_pHead;
	CPlayerInfo* pParent = NULL;
	while(pTemp)
	{
		if(wonID == pTemp->m_nWonID)
		{
			CPlayerInfo* pTemp2 = pTemp->m_pNext;  
			delete pTemp;
			if(!pParent)
				m_pHead = pTemp2;
			else
				pParent = pTemp2;
			m_nSize--;
			return;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
}

bool CPlayerRecoverQueue::AttemptedToRecover(edict_t* pClient)
{
	CBasePlayer* pPlayer = GetClassPtr((CBasePlayer*)&pClient->v);
	CPlayerInfo* pInfo = LocatePlayer(pPlayer);
	if(pInfo)
	{
		pInfo->ResetPlayer(pPlayer);
		pPlayer->m_bInMatch = true;
		if(pPlayer->m_isSpectator)
			pPlayer->StopObserver();
		else
			pPlayer->Spawn();
		pPlayer->m_bUnSpawn = false;
		RemovePlayer(pPlayer->edict());
		return true;
	}
	return false;
}

void CPlayerRecoverQueue::ClearQueue()
{
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "CPlayerRecoverQueue::ClearQueue(); Start\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	CPlayerInfo* pTemp = m_pHead;
	CPlayerInfo* pParent = NULL;
	while(pTemp)
	{
		pParent = pTemp->m_pNext;
		delete pTemp;
		pTemp = pParent;
	}
	m_pHead = NULL;
	m_nSize=0;
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "CPlayerRecoverQueue::ClearQueue(); End\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

}

void CPlayerRecoverQueue::UpdateQueue()
{
	CPlayerInfo* pTemp = m_pHead;
	CPlayerInfo* pParent = NULL;
	while(pTemp)
	{
		if(pTemp->m_dTime.TimeUp())
		{
			CPlayerInfo* pTemp2 = pTemp->m_pNext;  
			delete pTemp;
			if(!pParent)
				m_pHead = pTemp2;
			else
				pParent = pTemp2;
			m_nSize--;
			return;
		}	
	}
}

void CreateCTPFile() // server command: "create_ctp_file"
{
	char buf[64];
	sprintf(buf, "hle/modes/ctp/%s",STRING(gpGlobals->mapname) );
	CGameModeCTP::g_OutCtpFile.open(buf);	

	CBaseEntity* pEntity = NULL;

	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
	while (pEntity)
	{
		FireTargets( "flag_follow", pEntity, pEntity, USE_TOGGLE, 0 );
		UTIL_Remove( pEntity ); 
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
	
	}
}

void CompleteCTPFile() // server command: "complete_ctp_file"
{
	if(CGameModeCTP::g_OutCtpFile.is_open())
		CGameModeCTP::g_OutCtpFile.close();
}


void EjectPlayer()// works by server id's
{
	CBaseEntity *pEnt = UTIL_PlayerByIndex( atoi(CMD_ARGV(1)));
	if (pEnt &&	pEnt->IsPlayer() )
	{
		g_pGameRules->RemovePlayer((CBasePlayer*)pEnt);
	}
}

void EmitPlayer()// works by server id's
{
	CBaseEntity *pEnt = UTIL_PlayerByIndex( atoi(CMD_ARGV(1)));
	if (pEnt &&	pEnt->IsPlayer() )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
		g_pGameRules->ChangePlayerTeam(pPlayer,  CMD_ARGV(2), TRUE, TRUE);
		g_pGameRules->ResumePlayer(pPlayer);
	}
}

void AddClock()
{
	char szBuf[256];
	*szBuf=NULL;
	float fNewTime =  atof(CMD_ARGV(1));
	g_pGameRules->AddClock(fNewTime);
}
void AddTime()
{
	char szBuf[256];
	*szBuf=NULL;
	float fNewTime =  timelimit.value+atof(CMD_ARGV(1));
	sprintf(szBuf, "mp_timelimit %f\n", fNewTime);
	SERVER_COMMAND(szBuf);
}
void SubtractTime()
{
	char szBuf[256];
	*szBuf=NULL;
	float fNewTime=  timelimit.value-atof(CMD_ARGV(1));
	sprintf(szBuf, "mp_timelimit %f\n", fNewTime);
	SERVER_COMMAND(szBuf);
}

void ACTIONmode()
{
	//strcpy(gamemode.string, "action");
	CVAR_SET_STRING("mp_gamemode", "action");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}

void LMSmode()
{
	//strcpy(gamemode.string, "lms");
	CVAR_SET_STRING("mp_gamemode", "lms");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}
void TEAMmode()
{
	//strcpy(gamemode.string, "teamplay");
	CVAR_SET_STRING("mp_gamemode", "teamplay");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}
void FFAmode()
{
	//strcpy(gamemode.string, "ffa");
	CVAR_SET_STRING("mp_gamemode", "ffa");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}
void CTFmode()
{
	//strcpy(gamemode.string, "ctf");
	CVAR_SET_STRING("mp_gamemode", "ctf");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}
void CTPmode()
{
	//strcpy(gamemode.string, "ctp");
	CVAR_SET_STRING("mp_gamemode", "ctp");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}
void DUELmode()
{
	//strcpy(gamemode.string, "duel");
	CVAR_SET_STRING("mp_gamemode", "duel");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}
void TOURNEYmode()
{
	//strcpy(gamemode.string, "tournament");
	CVAR_SET_STRING("mp_gamemode", "tournament");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}

void TLMSmode()
{
	//strcpy(gamemode.string, "lts");
	CVAR_SET_STRING("mp_gamemode", "lts");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}

void TDUELmode()
{
	//strcpy(gamemode.string, "lts");
	CVAR_SET_STRING("mp_gamemode", "teamduel");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}


void TACTIONmode()
{
	//strcpy(gamemode.string, "teamaction");
	CVAR_SET_STRING("mp_gamemode", "teamaction");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}

void PRACTICEmode()
{
	//strcpy(gamemode.string, "practice");
	CVAR_SET_STRING("mp_gamemode", "practice");
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
	SERVER_COMMAND(szCommand);
}

void CreateLocFile()
{
	char buf[64];
	sprintf(buf, "hle/locs/%s.loc",STRING(gpGlobals->mapname) );
	g_OutLocFile.open(buf);	
}
void CompleteLocFile()
{
	if(g_OutLocFile.is_open())
		g_OutLocFile.close();
}

void DropQueue::Clear() 
{
	Clear(m_pHead);
}

void Tourney()
{
	tournament.value = 1;
	const char* ptourney = CMD_ARGV(1);
	if(ptourney)
	{
		char buf[128];
		sprintf(buf, "exec tournaments/%s\n", ptourney);
		SERVER_COMMAND(buf);
		sprintf(buf, "changelevel %s\n", STRING(gpGlobals->mapname));
		SERVER_COMMAND(buf);
		SERVER_EXECUTE( );
		g_pGameRules->AddClock(roundtime.value); // start timer for matchstart
		tournament.value = 0;
	}
}
void DropQueue::Clear(DropQueueNode* pRoot) 
{
	bool head;
	pRoot == m_pHead ? head = true : head = false;
	DropQueueNode* pTemp = pRoot;
	DropQueueNode* pNext = NULL;
	while(pTemp)
	{
		pNext = pTemp->m_pNext;
		delete pTemp;
		pTemp = pNext;
	}
	if(head)
		m_pHead = NULL;
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "DropQueue::Clear()\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================
}

DropQueue::DropQueue()
{
	m_pHead = NULL;
}
DropQueue::~DropQueue()
{
	Clear(m_pHead);
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "DropQueue::~DropQueue()\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

}

float DropQueue::Full()
{
	DropQueueNode* pTemp = m_pHead;
	DropQueueNode* pParent = NULL;
	int i = 0;
	while(pTemp)
	{
		i++;
		if(pTemp->m_dDelay.TimeUp())
		{
			Clear(pTemp);
			if(pParent)
				pParent->m_pNext = NULL;
			return 0.0f; // queue not full
		}
		else if(i == 30)
			return pTemp->m_dDelay.GetTimeLeft(); // queue full
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	return 0.0f; // queue not full
}
void DropQueue::Add()
{
	DropQueueNode* pNew = new DropQueueNode();
	DropQueueNode* pTemp = m_pHead;
	m_pHead = pNew;
	m_pHead->m_pNext = pTemp;
}



void SendMapsToClients()
{
	if(g_MapsServer.m_bGotMaps == false)
	{
		g_MapsServer.clear();

		g_MapsServer.addMapSmart("undertow");
		g_MapsServer.addMapSmart("subtransit");
		g_MapsServer.addMapSmart("stalkyard");
		g_MapsServer.addMapSmart("snark_pit");
		g_MapsServer.addMapSmart("rapidcore");
		g_MapsServer.addMapSmart("lambda_bunker");
		g_MapsServer.addMapSmart("gasworks");
		g_MapsServer.addMapSmart("frenzy");
		g_MapsServer.addMapSmart("datacore");
		g_MapsServer.addMapSmart("crossfire");
		g_MapsServer.addMapSmart("bounce");
		g_MapsServer.addMapSmart("boot_camp");

		CMapList::AddMapsFromFile(&g_MapsServer, "hle/maps/*.bsp");
		CMapList::AddMapsFromFile(&g_MapsServer, "valve/maps/*.bsp");	
		g_MapsServer.m_bGotMaps = true;
	}

	// loop through all clients, count number of players on each team
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr && plr->IsPlayer())
		{
			((CBasePlayer*)plr)->m_MapPackage.Reset();
		}

	}
}

void AddServerCommands()
{
	ptCallMatch = &CMatchStart::callMatch;
	ADD_SERVER_COMMAND("matchstart", ptCallMatch);
	ptAdd = &CVirtualAdmin::Add;
	ADD_SERVER_COMMAND("add", ptAdd);
	ptRemove = &CVirtualAdmin::Remove;	
	ADD_SERVER_COMMAND("remove", ptRemove);

	ptEmitPlayer = &EmitPlayer;
	ADD_SERVER_COMMAND("emit", ptEmitPlayer);
	ptEjectPlayer = &EjectPlayer;
	ADD_SERVER_COMMAND("eject", ptEjectPlayer);
	ptAddTime = &AddTime;
	ADD_SERVER_COMMAND("addtime", ptAddTime);
	ptSubtractTime = &SubtractTime;
	ADD_SERVER_COMMAND("subtracttime", ptSubtractTime);

	ptAddClock = &AddClock;
	ADD_SERVER_COMMAND("addclock", ptAddClock);

	ptACTION = &ACTIONmode;
	ADD_SERVER_COMMAND("action", ptACTION);

	ptTOURNEY = &TOURNEYmode;
	ADD_SERVER_COMMAND("tournament", ptTOURNEY);

	ptFFA = &FFAmode;
	ADD_SERVER_COMMAND("ffa", ptFFA);

	ptTEAM = &TEAMmode;
	ADD_SERVER_COMMAND("teamplay", ptTEAM);

	ptCTP = &CTPmode;
	ADD_SERVER_COMMAND("ctp", ptCTP);

	ptCTF = &CTFmode;
	ADD_SERVER_COMMAND("ctf", ptCTF);

	ptLMS = &LMSmode;
	ADD_SERVER_COMMAND("lms", ptLMS);

	ptTLMS = &TLMSmode;
	ADD_SERVER_COMMAND("lts", ptTLMS);

	ptTDUEL = &TDUELmode;
	ADD_SERVER_COMMAND("teamduel", ptTDUEL);

	ptTACTION = &TACTIONmode;
	ADD_SERVER_COMMAND("teamaction", ptTACTION);

	ptPRACTICE = &PRACTICEmode;
	ADD_SERVER_COMMAND("practice", ptPRACTICE);

	
	ptDUEL = &DUELmode;
	ADD_SERVER_COMMAND("duel", ptDUEL);

	ptCreateLocFile = &CreateLocFile;
	ADD_SERVER_COMMAND("create_loc_file", ptCreateLocFile);

	ptCompleteLocFile = &CompleteLocFile;
	ADD_SERVER_COMMAND("complete_loc_file", ptCompleteLocFile);

	// ctp stuff
	ptCreateCTFFile = &CreateCTPFile;
	ptCompleteCTFFile = &CompleteCTPFile;
	ADD_SERVER_COMMAND("create_ctp_file", ptCreateCTFFile);
	ADD_SERVER_COMMAND("complete_ctp_file", ptCompleteCTFFile);

	ptStart = &Tourney;
	ADD_SERVER_COMMAND("start", ptStart);
	
	//ptSendMapsAgain = &SendMapsAgain;
	//ADD_SERVER_COMMAND("updatemaps", ptSendMapsAgain);


}


CBaseEntity* TossItem(CBasePlayer* pPlayer, const char* classname)
	{
	CBaseEntity *pEnt;
	UTIL_MakeVectors( pPlayer->pev->v_angle );
	pEnt = CBaseEntity::Create( (char*)classname, pPlayer->pev->origin ,pPlayer->pev->angles, pPlayer->edict() );
	UTIL_SetOrigin( pEnt->pev, pPlayer->pev->origin + pPlayer->pev->view_ofs + gpGlobals->v_forward * 64);

	pEnt->pev->velocity = gpGlobals->v_forward * 400;
	pEnt->pev->angles = UTIL_VecToAngles (pEnt->pev->velocity);
	pEnt->pev->angles.x = 0;
	pEnt->pev->spawnflags |= SF_NORESPAWN;
	return pEnt;
}



//===================
//SpawnPointsDM
//===================


void SpawnPointsDM::CreateSpawnList()
{

	ClearList();
	CBaseEntity *pSpot=NULL;
	
	pSpot = UTIL_FindEntityByClassname( NULL, "info_player_deathmatch");
	while(pSpot)
	{
		m_Spawns[m_nSpawnPoints++] = (CBaseDMStart*)pSpot;	
		if(m_nSpawnPoints>=64)
			break;
		
		pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
	}

}

extern BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot );

edict_t* SpawnPointsDM::FindRandomSpawnPoint(CBasePlayer* pPlayer)
{
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "Enter SpawnPointsDM::FindRandomSpawnPoint\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	if(m_nSpawnPoints==0)
		return NULL;
	

	int spot = RANDOM_LONG(0,m_nSpawnPoints-1);
	int loops = 0;
	// now check this spot to make sure no one is around, else loop thu until we find an open spot
	// else clearn out an area terminater style and spawn the player

	int i = spot;

	while(loops < m_nSpawnPoints)
	{
		if(i >= m_nSpawnPoints)
			i=0;
		if(i==m_nLastSpawn)
		{
			i++;
			loops++;
			continue;
		}
		if(m_Spawns[i])
		{
			if(IsSpawnPointValid( pPlayer, m_Spawns[i] ) )
			{
				m_nLastSpawn = i;
				break;
			}
		}
		i++;
		loops++;		
	}

	if(!m_Spawns[i])
	{
		if(m_Spawns[spot])
		{
			i = spot;
			m_nLastSpawn = i;
		}
		else
		{
			return NULL;
		}
	}

	CBaseEntity *ent = NULL;
	// clear the area for the spawn
	while ( (ent = UTIL_FindEntityInSphere( ent, m_Spawns[i]->pev->origin, 128 )) != NULL ) // CRASH HERE!!!
	{
		// if ent is a client, kill em (unless they are ourselves)
		if ( ent->IsPlayer() && !(ent->edict() == pPlayer->edict()) )
			ent->TakeDamage( VARS(INDEXENT(0)), VARS(INDEXENT(0)), 300, DMG_GENERIC );
	}
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "Return SpawnPointsDM::FindRandomSpawnPoint\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	return m_Spawns[i]->edict();
};

void SpawnPointsDM::ClearList()
{

	memset( &m_Spawns, 0, sizeof( m_Spawns ) );
	m_nSpawnPoints = 0;
	m_nLastSpawn = -1;
}



//===================
//SpawnPointsTeam
//===================


void SpawnPointsTeam::ClearList()
{
	
		for(int i = 0; i < 7; i++)
		{	
			m_nTeamPoints[i] = 0;
			memset( &m_TeamSpawns[i], 0, sizeof( m_TeamSpawns[i] ) );
		}
		m_nLastSpawn = -1;
}
void SpawnPointsTeam::CreateSpawnList()
	{

	ClearList();
	CBaseEntity *pSpot=NULL;
	
	pSpot = UTIL_FindEntityByClassname( NULL, "info_player_deathmatch");
	while(pSpot)
	{
		int nTeam = ((CBaseDMStart*)pSpot)->m_nTeam;
		if(nTeam >=0 && nTeam < 7)
		{

			if(m_nTeamPoints[nTeam] >= 64)
				break;
			m_TeamSpawns[nTeam][m_nTeamPoints[nTeam]++] = (CBaseDMStart*)pSpot;
		}
		pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
	}
}

edict_t* SpawnPointsTeam::FindRandomSpawnPoint(CBasePlayer* pPlayer	)
{
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "Enter SpawnPointsTeam::FindRandomSpawnPoint\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	int nPlayerTeam = g_pGameRules->GetTeamIndex( pPlayer )+1;
	
	if(nPlayerTeam < 0 || nPlayerTeam > 6)
	{
		// player is not on a team or he is spectating, give him a fully radnom spot
		nPlayerTeam = RANDOM_LONG(0,6);
	}
	
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "nPlayerTeam\n";
		debug_file << nPlayerTeam << endl;
		debug_file.CloseFile();
#endif	// END FILE DUMP =========================

	// check to see if there are any spots on this team
	if(m_nTeamPoints[nPlayerTeam]==0)
	{
		int _loops = 0;
		while(_loops<=7)
		{
			if(m_nTeamPoints[nPlayerTeam]>0)  // if there are no spots, get any random spot
				break;
			nPlayerTeam++;
			_loops++;
			if(nPlayerTeam > 6)
				nPlayerTeam = 0;
		}
	}

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "nPlayerTeam after loop\n";
		debug_file << nPlayerTeam  << endl;
		debug_file.CloseFile();
#endif	// END FILE DUMP =========================



	if(m_nTeamPoints[nPlayerTeam]==0)
	{
		return NULL;
	}
	//
	// get a random spot
	//
	int spot = RANDOM_LONG(0,m_nTeamPoints[nPlayerTeam]-1);
	int loops = 0;
	// now check this spot to make sure no one is around, else loop thu until we find an open spot
	// else clearn out an area terminater style and spawn the player
	//for(int i = spots; i < m_nTeamPoints[nPlayerTeam]; i++)
	int i = spot;
	while(loops < m_nTeamPoints[nPlayerTeam])
	{
		if(i>=m_nTeamPoints[nPlayerTeam])
			i=0;	
		if(i==m_nLastSpawn)
		{
			loops++;
			i++;
			continue;
		}
		if(m_TeamSpawns[nPlayerTeam][i])
		{
			if(IsSpawnPointValid( pPlayer, m_TeamSpawns[nPlayerTeam][i] ) )
			{
				m_nLastSpawn=i;
				break;
			}
		}	
		loops++;
		i++;
	}

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "loops\n";
		debug_file << loops  << endl;
		debug_file.CloseFile();
#endif	// END FILE DUMP =========================

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "m_nLastSpawn\n";
		debug_file << m_nLastSpawn  << endl;
		debug_file.CloseFile();
#endif	// END FILE DUMP =========================



	if(!m_TeamSpawns[nPlayerTeam][i])
	{
		if(m_TeamSpawns[nPlayerTeam][spot])
		{
			i = spot;
			m_nLastSpawn = i;
		}
		else
		{
			return NULL;
		}
	}

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "i\n";
		debug_file << i  << endl;
		debug_file.CloseFile();
#endif	// END FILE DUMP =========================

	CBaseEntity *ent = NULL;
	// clear the area for the spawn
	while ( (ent = UTIL_FindEntityInSphere( ent, m_TeamSpawns[nPlayerTeam][i]->pev->origin, 128 )) != NULL )
	{
		// if ent is a client, kill em (unless they are ourselves)
		if ( ent->IsPlayer() && !(ent->edict() == pPlayer->edict()) )
			ent->TakeDamage( VARS(INDEXENT(0)), VARS(INDEXENT(0)), 300, DMG_GENERIC );
	}
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============	
		debug_file.OpenFile();
		debug_file << "Return SpawnPointsTeam::FindRandomSpawnPoint\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	return m_TeamSpawns[nPlayerTeam][i]->edict();

}


//====================================
//CSystemCount public member functions
//====================================

CSystemCount::CSystemCount()
{
	m_time = 0;
}

CSystemCount::~CSystemCount()
{
}

void CSystemCount::StartClock()
{
	m_time = clock();
}

void CSystemCount::Delay(long delay)
{
	delay *=1000;
	long x = clock() + delay;
	while( clock() < x ){}
}

long CSystemCount::StopClock()
{
	return (clock() - m_time); 
}

//===============================
//COutput public member functions
//===============================

COutput::COutput()
{
	*m_szFileName=NULL;
	m_pOutputFilePointer = NULL; // if no file name we output to the screen
}

COutput::COutput(char* pFileName)
{
	*m_szFileName=NULL;
	m_pOutputFilePointer = NULL;
	if(pFileName)
    {
		strcpy(m_szFileName, pFileName);
        //m_oFile.open(pFileName);
        //m_pOutputFilePointer = &m_oFile;
    }
    else
    {
        //cout << "ERROR: COutput::COutput(char*) Invalid file type" << endl;
    }
}

void COutput::CreateFile()
{
	m_oFile.open(m_szFileName);
	m_pOutputFilePointer = &m_oFile;
}

void COutput::OpenFile()
{
	m_oFile.open(m_szFileName, ios::app);
	m_pOutputFilePointer = &m_oFile;
}

void COutput::CloseFile()
{
	m_oFile.close();
	m_pOutputFilePointer = NULL;
}
void COutput::SetOutputFileName(char* pFileName)
{
	*m_szFileName=NULL;
	if(pFileName)
    {
		strcpy(m_szFileName, pFileName);
        //m_oFile.open(pFileName);
        //m_pOutputFilePointer = &m_oFile;
    }
    else
    {
        //cout << "ERROR: COutput::COutput(char*) Invalid file type" << endl;
    }
}

COutput::~COutput()
{
	if(m_oFile)
	{

		if(m_oFile.is_open())
			m_oFile.close();
	}
}

ostream& operator << (COutput& Output, int nNum)
{
	char cOutPut[2048];
	*cOutPut = NULL;
	char *pTimeLine;

	///////////////////
	// hand system time
	_ftime( &Output.m_TimeBuff );
	pTimeLine = ctime( & ( Output.m_TimeBuff.time ) );
	strtok(&pTimeLine[20], "\n");

	// add the output string onto the time string
	sprintf(cOutPut, "[%.19s.%hu %s] %i", pTimeLine, Output.m_TimeBuff.millitm, &pTimeLine[20], nNum );
	//Output.OpenFile();
	return *Output.m_pOutputFilePointer << (const char*)cOutPut;
	//Output.CloseFile();
}
ostream& operator << (COutput& Output, char* pText)
{
	char cOutPut[2048];
	*cOutPut = NULL;
	char *pTimeLine;


	///////////////////
	// hand system time
	_ftime( &Output.m_TimeBuff );
	pTimeLine = ctime( & ( Output.m_TimeBuff.time ) );
	strtok(&pTimeLine[20], "\n");

	// add the output string onto the time string
	sprintf(cOutPut, "[%.19s.%hu %s] %s", pTimeLine, Output.m_TimeBuff.millitm, &pTimeLine[20], pText );
	//Output.OpenFile();
	return *Output.m_pOutputFilePointer << (const char*)cOutPut;
	//Output.CloseFile();
	
}


 


bool HasPermission(CBasePlayer* pPlayer, char* pCommand);
void SetPermission(CBasePlayer* pPlayer);
void AddPlayerToGroup(CBasePlayer* pPlayer, int nGroup);
void RemovePlayerFromGroup(CBasePlayer* pPlayer, int nGroup);
void AddPermisionToGroup(CBasePlayer* pPlayer, int nGroup, char* pszCommand);
void RemovePermisionFromGroup(CBasePlayer* pPlayer, int nGroup, char* pszCommand);
void SendMapsToClients();

