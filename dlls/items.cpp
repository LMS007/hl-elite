/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== items.cpp ========================================================

  functions governing the selection/use of weapons for players

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "skill.h"
#include "items.h"
#include "gamerules.h"
#include "game.h"

int g_bPlayLongjumpSound = 1;
extern int gmsgItemPickup;
extern int g_teamplay;

class CWorldItem : public CBaseEntity
{
public:
	void	KeyValue(KeyValueData *pkvd ); 
	void	Spawn( void );
	int		m_iType;
};

LINK_ENTITY_TO_CLASS(world_items, CWorldItem);

void CWorldItem::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "type"))
	{
		m_iType = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CWorldItem::Spawn( void )
{
	CBaseEntity *pEntity = NULL;

	switch (m_iType) 
	{
	case 44: // ITEM_BATTERY:
		pEntity = CBaseEntity::Create( "item_battery", pev->origin, pev->angles );
		break;
	case 42: // ITEM_ANTIDOTE:
		pEntity = CBaseEntity::Create( "item_antidote", pev->origin, pev->angles );
		break;
	case 43: // ITEM_SECURITY:
		pEntity = CBaseEntity::Create( "item_security", pev->origin, pev->angles );
		break;
	case 45: // ITEM_SUIT:
		pEntity = CBaseEntity::Create( "item_suit", pev->origin, pev->angles );
		break;
	}

	if (!pEntity)
	{
		ALERT( at_console, "unable to create world_item %d\n", m_iType );
	}
	else
	{
		pEntity->pev->target = pev->target;
		pEntity->pev->targetname = pev->targetname;
		pEntity->pev->spawnflags = pev->spawnflags;
	}

	REMOVE_ENTITY(edict());
}


void CItem::Spawn( void )
{
//	m_bCanRespawn = TRUE;
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	SetTouch(&CItem::ItemTouch);
	SetThink(&CItem::ItemThink);
	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove( this );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.1;
	m_fTime = gpGlobals->time + 30;
}

extern int gEvilImpulse101;

void CItem::ItemTouch( CBaseEntity *pOther )
{
	// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
	{
		return;
	}

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// ok, a player is touching this item, but can he have it?
	if ( !g_pGameRules->CanHaveItem( pPlayer, this ) )
	{
		// no? Ignore the touch.
		return;
	}

	if (MyTouch( pPlayer ))
	{
		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		SetTouch( NULL );
		
		// player grabbed the item. 
		g_pGameRules->PlayerGotItem( pPlayer, this );
		if ( g_pGameRules->ItemShouldRespawn( this ) == GR_ITEM_RESPAWN_YES )
		{
			Respawn(); 
		}
		else
		{
			UTIL_Remove( this );
		}
	}
	else if (gEvilImpulse101)
	{
		UTIL_Remove( this );
	}
}

CBaseEntity* CItem::Respawn( void )
{
	SetTouch( NULL );
	pev->effects |= EF_NODRAW;

	UTIL_SetOrigin( pev, g_pGameRules->VecItemRespawnSpot( this ) );// blip to whereever you should respawn.

	SetThink ( &CItem::Materialize );
	pev->nextthink = g_pGameRules->FlItemRespawnTime( this ); 
	return this;
}

void CItem::ItemThink( void )
{
	if(pev->spawnflags & SF_NORESPAWN)
	{
		if(m_fTime < gpGlobals->time )
		{
			SetTouch( NULL );
			SetThink(&CItem::SUB_Remove);
			pev->nextthink = gpGlobals->time + .1;
			return;
		}
	}
	pev->nextthink = gpGlobals->time + 0.1;
}

void CItem::Materialize( void )
{
	if(pev->spawnflags & SF_NORESPAWN)
	{
		SetThink( NULL );
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}

	if ( pev->effects & EF_NODRAW )
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
	}

	SetTouch( &CItem::ItemTouch );
	SetThink( &CItem::ItemThink);
}



#define SF_SUIT_SHORTLOGON		0x0001

class CItemSuit : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_suit.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_suit.mdl");
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->weapons & (1<<WEAPON_SUIT) )
			return FALSE;

		if ( pev->spawnflags & SF_SUIT_SHORTLOGON )
			EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
		else
			EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon

		pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);



void CItemBattery::Spawn( void )
{ 
	Precache( );
//	m_bCanRespawn = TRUE;
	SET_MODEL(ENT(pev), "models/w_battery.mdl");
	CItem::Spawn( );
}
void CItemBattery::Precache( void )
{
	PRECACHE_MODEL ("models/w_battery.mdl");
	PRECACHE_SOUND( "items/gunpickup2.wav" );
}

BOOL CItemBattery::MyTouch( CBasePlayer *pPlayer )
{
	if ( pPlayer->pev->deadflag != DEAD_NO )
	{
		return FALSE;
	}
	if ((pPlayer->pev->armorvalue < /*MAX_NORMAL_BATTERY*/pPlayer->max_hev) &&
		(pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
	{
		int pct;
		char szcharge[64];
		pPlayer->pev->armorvalue += gSkillData.batteryCapacity;
		pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, /*MAX_NORMAL_BATTERY*/pPlayer->max_hev);
			
		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );
		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();

		CEntityNode* pClient = pPlayer->m_lObservers.head();
		while(pClient)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pClient->m_pEntity );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			pClient = pClient->m_pNext;
		}
			
		// Suit reports new power level
		// For some reason this wasn't working in release build -- round it.
		pct = (int)( (float)(pPlayer->pev->armorvalue * 100.0) * (1.0/ /*MAX_NORMAL_BATTERY*/ pPlayer->max_hev ) + 0.5);
		pct = (pct / 5);
		if (pct > 0)
			pct--;
	
		sprintf( szcharge,"!HEV_%1dP", pct );
		
		//EMIT_SOUND_SUIT(ENT(pev), szcharge);
		pPlayer->SetSuitUpdate(szcharge, FALSE, SUIT_NEXT_IN_30SEC);
		
/*		if(m_bCanRespawn==FALSE)
		{
			UTIL_Remove( this );
			FireTargets( "item_longjump", this, this, USE_TOGGLE, 0 );
			SetTouch( NULL );
			pev->effects |= EF_NODRAW;
		}*/

		return TRUE;		
	}
	return FALSE;
}


CBaseEntity* CItemBattery::Drop(CBasePlayer* pPlayer)
{
	CBaseEntity *pEnt;
	CItem *pTemp=NULL;

	pPlayer->pev->armorvalue -= 15;

	UTIL_MakeVectors( pPlayer->pev->v_angle );
	pEnt = CBaseEntity::Create( "item_battery", pPlayer->pev->origin ,pPlayer->pev->angles, pPlayer->edict() );
	UTIL_SetOrigin( pEnt->pev, pPlayer->pev->origin + pPlayer->pev->view_ofs + gpGlobals->v_forward * 64);

	pEnt->pev->velocity = gpGlobals->v_forward * 400;
	pEnt->pev->angles = UTIL_VecToAngles (pEnt->pev->velocity);
	pEnt->pev->angles.x = 0;

	CItemBattery *pBattery = (CItemBattery *)pEnt;
	pEnt->pev->spawnflags |= SF_NORESPAWN;
	return pEnt;

}


LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);


class CItemAntidote : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_antidote.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_antidote.mdl");
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		pPlayer->SetSuitUpdate("!HEV_DET4", FALSE, SUIT_NEXT_IN_1MIN);
		
		pPlayer->m_rgItems[ITEM_ANTIDOTE] += 1;
		return TRUE;
	}
};



LINK_ENTITY_TO_CLASS(item_antidote, CItemAntidote);


class CItemSecurity : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_security.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_security.mdl");
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		pPlayer->m_rgItems[ITEM_SECURITY] += 1;
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_security, CItemSecurity);


void CItemLongJump::Spawn( void )
{ 
	Precache( );
//	m_bCanRespawn = TRUE;
	SET_MODEL(ENT(pev), "models/w_longjump.mdl");
	CItem::Spawn( );
}

void CItemLongJump::Precache( void )
{
	PRECACHE_MODEL ("models/w_longjump.mdl");
	PRECACHE_MODEL ("models/p_longjump.mdl");
}

BOOL CItemLongJump::MyTouch( CBasePlayer *pPlayer )
{
	if ( pPlayer->m_fLongJump )
	{
		return FALSE;
	}
	if ( ( pPlayer->pev->weapons & (1<<WEAPON_SUIT) ) )
	{
		
		g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "1" );
		pPlayer->m_fLongJump = TRUE;// player now has longjump module	
		if(showlongjump.value)
			CLongJumpFollow::Create(pPlayer);// create the model for the player
		
	/*	if(pPlayer->m_pActiveItem)
		{
			if(pPlayer->m_pActiveItem->m_iId == WEAPON_EGON)
				pPlayer->DisableLongJump(TRUE);
		}*/	

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
			WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();


		CEntityNode* pClient = pPlayer->m_lObservers.head();
		while(pClient)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pClient->m_pEntity );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			pClient = pClient->m_pNext;
		}
		
		if(g_bPlayLongjumpSound)
			EMIT_SOUND_SUIT( pPlayer->edict(), "!HEV_A1" );	// Play the longjump sound UNDONE: Kelly? correct sound?

/*		if(m_bCanRespawn==FALSE)
		{
			UTIL_Remove( this );
			FireTargets( "item_longjump", this, this, USE_TOGGLE, 0 );
			SetTouch( NULL );
			pev->effects |= EF_NODRAW;
		}*/
		return TRUE;		
	}
	
	return FALSE;
}




CBaseEntity* CItemLongJump::Drop(CBasePlayer* pPlayer)
{
	CBaseEntity *pEnt;
	CItem *pTemp=NULL;

	pPlayer->DropPlayerItem("ljf"); // no can

	UTIL_MakeVectors( pPlayer->pev->v_angle );
	pEnt = CBaseEntity::Create( "item_longjump", pPlayer->pev->origin ,pPlayer->pev->angles, pPlayer->edict() );
	UTIL_SetOrigin( pEnt->pev, pPlayer->pev->origin + pPlayer->pev->view_ofs + gpGlobals->v_forward * 64);

	pEnt->pev->velocity = gpGlobals->v_forward * 400;
	pEnt->pev->angles = UTIL_VecToAngles (pEnt->pev->velocity);
	pEnt->pev->angles.x = 0;

	CItemLongJump *pLongJump = (CItemLongJump *)pEnt;
//	pLongJump->m_bCanRespawn = FALSE; // hack to prevet this from respawning because its not a "placed" longjump
	pEnt->pev->spawnflags |= SF_NORESPAWN;

	g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "0" );
	pPlayer->m_fLongJump = FALSE;
	return pEnt;
}


LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );

void CLongJumpFollow::Spawn()
{
	Precache( );
	UTIL_SetOrigin( pev, pev->origin );
	pev->movetype = MOVETYPE_FOLLOW;
	pev->solid = SOLID_NOT;
	//pev->effects |= EF_NODRAW;
	pev->effects &= ~EF_NODRAW;
	pev->sequence = 2;
	pev->framerate = 1.0;
	SET_MODEL(ENT(pev), "models/p_longjump.mdl");
	m_pOwner = NULL;
	pev->nextthink = gpGlobals->time + 0.2;

}

void CLongJumpFollow::Precache()
{
	PRECACHE_MODEL ("models/p_longjump.mdl");
}
BOOL CLongJumpFollow::MyTouch(CBasePlayer* pPlayer)
{
	return FALSE;
}

void CLongJumpFollow::Create(CBasePlayer* pPlayer)
{
	CBaseEntity *pEnt = CBaseEntity::Create( "ljf", pPlayer->pev->origin, pPlayer->pev->angles, pPlayer->edict() );
	CLongJumpFollow *pFollow = (CLongJumpFollow *)pEnt;
	pFollow->pev->owner = pPlayer->edict();
	pFollow->m_pOwner = pPlayer;
	pFollow->pev->aiment = ENT(pPlayer->pev);
}


void CLongJumpFollow::Remove( )
{
	m_pOwner = NULL;
	SetThink(NULL);
	UTIL_Remove( this );
	
	
	//SetThink(SUB_Remove);
	//pev->nextthink = gpGlobals->time + 0.1;
}


LINK_ENTITY_TO_CLASS( ljf, CLongJumpFollow );

void CTeamIndicator::SetTeam(CBasePlayer* pPlayer)
{
}


void CTeamIndicator::Create(CBasePlayer* pPlayer)
{
	CBaseEntity *pEnt = CBaseEntity::Create( "_ti", pPlayer->pev->origin, pPlayer->pev->angles, pPlayer->edict() );
	CTeamIndicator *pFollow = (CTeamIndicator *)pEnt;
	pFollow->pev->owner = pPlayer->edict();
	pFollow->m_pOwner = pPlayer;
	pFollow->pev->aiment = ENT(pPlayer->pev);
	//sprintf(pFollow->m_szTeam, pPlayer->m_szTeamName);
	pFollow->pev->iuser1 = pPlayer->m_szTeamName[0];

	pFollow->pev->solid = SOLID_NOT;

	


		switch(pFollow->pev->iuser1)
	{
	case 'B':
		pFollow->pev->skin = 1;
		break;
	case 'R':
		pFollow->pev->skin = 2;
		break;
	case 'G':
		pFollow->pev->skin = 3;
		break;
	case 'Y':
		pFollow->pev->skin = 4;
		break;
	case 'P':
		pFollow->pev->skin = 5;
		break;
	case 'O':
		pFollow->pev->skin = 6;
		break;
		
	}
}

void CTeamIndicator::Spawn( void )
{
	Precache( );
	UTIL_SetOrigin( pev, pev->origin );
	pev->movetype = MOVETYPE_FOLLOW;
	pev->scale = 2;
	pev->solid = SOLID_NOT;
	pev->effects &= ~EF_NODRAW;
	pev->framerate = 1.0;		
	SET_MODEL(ENT(pev), "models/p_teamid.mdl");
	m_pOwner = NULL;

	//pev->rendermode = kRenderTransColor;
	//pev->renderamt = 50;
	//pev->renderfx = kRenderFxNone;
	//pev->rendercolor.x=255;
	//pev->rendercolor.y=255;
	//pev->rendercolor.z=0;
	//pev->nextthink = gpGlobals->time + 0.2;
	SetThink(&CTeamIndicator::ThinkId);
	pev->nextthink = gpGlobals->time + 0.1;

}

void CTeamIndicator::Precache()
{
	
	PRECACHE_MODEL ("models/p_teamid.mdl");
}

void CTeamIndicator::ThinkId()
{
	//pev->nextthink = gpGlobals->time + 0.1;
}



void CTeamIndicator::Remove( )
{
	m_pOwner = NULL;
	SetThink(NULL);
	UTIL_Remove( this );
}


LINK_ENTITY_TO_CLASS( _ti, CTeamIndicator );

