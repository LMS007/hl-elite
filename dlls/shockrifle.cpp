/***
santagauss
the opposing forces shockrifle
***/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"

// start shock beam
#ifndef CLIENT_DLL

LINK_ENTITY_TO_CLASS( shock_beam, CShockBeam );

CShockBeam *CShockBeam::BeamCreate( void )
{
	CShockBeam *pBeam = GetClassPtr( (CShockBeam *)NULL );
	pBeam->pev->classname = MAKE_STRING("beam");
	pBeam->Spawn();

	return pBeam;
}

void CShockBeam::RemoveEntity()
{
	UTIL_Remove(m_pShockTail);
	UTIL_Remove(m_pShockHead);
	UTIL_Remove( this );
}

void CShockBeam::Spawn( )
{
	Precache( );

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/shock_effect.mdl");

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	m_pShockHead = CSprite::SpriteCreate( "sprites/xspark1.spr", pev->origin, TRUE );
	m_pShockHead->SetTransparency( kRenderTransAdd, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, 150, pev->renderfx );
	m_pShockHead->SetScale( 0.4 );
	m_pShockHead->SetAttachment( edict(), 0 );
	m_pShockHead->pev->framerate = 15;
	m_pShockHead->TurnOn( );

	m_pShockTail = CSprite::SpriteCreate( "sprites/blueflare1.spr", pev->origin, FALSE );
	m_pShockTail->SetTransparency( kRenderTransAdd, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, 40, pev->renderfx );
	m_pShockTail->SetScale( 2.00 );
	m_pShockTail->SetAttachment( edict(), 0 );
	m_pShockHead->TurnOn( );

	SetTouch( &CShockBeam::BeamTouch );
	//SetThink( &CShockBeam::WaterThink );

	pev->health = 1; // hle, 007

	pev->nextthink = gpGlobals->time + 0.1;

	//pev->velocity = gpGlobals->v_forward * 10;
	//pev->angles = UTIL_VecToAngles (pev->velocity);
	
	SetThink( &CShockBeam::Think );
}


void CShockBeam::Think( void )
{
	if(pev->velocity.Length() < 100) {
		pev->velocity = pev->velocity * 2000;
		pev->angles = UTIL_VecToAngles( pev->velocity );
	}
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER || UTIL_PointContents(pev->origin) == CONTENT_SLIME)
	{
		UTIL_Remove(m_pShockTail);
		UTIL_Remove(m_pShockHead);
		WaterShock( );
	}
	pev->nextthink = gpGlobals->time + 0.1;
}
void CShockBeam::Precache( )
{
	PRECACHE_SOUND("weapons/shock_impact.wav");

	PRECACHE_MODEL("models/shock_effect.mdl");
	PRECACHE_MODEL("sprites/blueflare1.spr");
	PRECACHE_MODEL("sprites/xspark1.spr");

	m_usShockFire = PRECACHE_EVENT ( 1, "events/shock.sc" );
}

int	CShockBeam :: Classify ( void )
{
	return	CLASS_NONE;
}

void CShockBeam::BeamTouch( CBaseEntity *pOther )
{
	SetTouch( NULL );
	SetThink( NULL );

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		RemoveEntity();
		return;
	}

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER  || UTIL_PointContents(pev->origin) == CONTENT_SLIME )
	{
		UTIL_Remove(m_pShockTail);
		UTIL_Remove(m_pShockHead);
		WaterShock( );	
		return;
	}

	TraceResult tr = UTIL_GetGlobalTrace( );

	if (pOther->pev->takedamage)
	{
		entvars_t	*pevOwner;
		pevOwner = VARS( pev->owner );

		ClearMultiDamage( );
		UTIL_Remove(m_pShockTail);
		UTIL_Remove(m_pShockHead);

		PLAYBACK_EVENT_FULL( 0, this->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

		pOther->TraceAttack(pevOwner, SHOCK_BEAM_DMG, pev->velocity.Normalize(), &tr, DMG_NEVERGIB ); 

		ApplyMultiDamage( pev, pevOwner );

		pev->velocity = Vector( 0, 0, 0 );
		
		Killed( pev, GIB_NEVER );
	}
	else
	{
		Vector vecDir = pev->velocity;
		UTIL_Remove(m_pShockTail);
		UTIL_Remove(m_pShockHead);

		PLAYBACK_EVENT_FULL( 0, this->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&vecDir, 0.0, 0.0, 0, 0, 0, 1 );
		
		UTIL_Remove(this);
	}
}

void CShockBeam::WaterThink( void )
{

}

void CShockBeam::WaterShock( ) 
{ 

	PLAYBACK_EVENT_FULL( 0, this->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 ); 
  
	// Shock custom radius damage (cant shock things not in water.   
	Vector vecSrc = pev->origin; 

	entvars_t *pevInflictor = pev;  
	entvars_t *pevAttacker = VARS( pev->owner );  
	
	float flDamage = SHOCK_BEAM_DMG * 3, flRadius = 180.0, flAdjustedDamage, falloff;  
	CBaseEntity *pEntity = NULL; 

	TraceResult	tr; 
	Vector	vecSpot; 
 
	//falloff = flDamage / flRadius; 
	falloff = 0.38; 
  
	vecSrc.z += 1;// in case on ground 
 

	if ( !pevAttacker )  
		pevAttacker = pevInflictor; 
 

	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, flRadius )) != NULL)  
	{ 
		if ( pEntity->pev->takedamage != DAMAGE_NO ) 
		{ 
			if (!pEntity->pev->waterlevel) 
				continue; 
			//vecSpot = pEntity->BodyTarget( vecSrc ); 
			vecSpot = pEntity->pev->origin; 

			UTIL_TraceLine ( vecSrc, vecSpot, dont_ignore_monsters, ENT(pevInflictor), &tr ); 

			if ( tr.flFraction == 1.0 || tr.pHit == pEntity->edict() ) 
			{
				// the explosion can 'see' this entity, so hurt them!	
				if (tr.fStartSolid)  
				{ 
					// if we're stuck inside them, fixup the position and distance  
					tr.vecEndPos = vecSrc; 
					tr.flFraction = 0.0;  
				} 
				// decrease damage for an ent that's farther from the bomb. 
				flAdjustedDamage = ( vecSrc - tr.vecEndPos ).Length() * falloff;  
				flAdjustedDamage = flDamage - flAdjustedDamage;  

				if ( flAdjustedDamage < 0 )  
					flAdjustedDamage = 0; 

				if (tr.flFraction != 1.0)	
				{ 
					ClearMultiDamage( ); 
					pEntity->TraceAttack( pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize( ), &tr, DMG_SHOCK ); 
					ApplyMultiDamage( pevInflictor, pevAttacker ); 
				} 
				else 
				{ 
					pEntity->TakeDamage ( pevInflictor, pevAttacker, flAdjustedDamage, DMG_SHOCK ); 
				} 
			} 
		}
	} 

// end radius  
UTIL_Remove(this); 

}
/*
void CShockBeam::WaterShock( )
{
	float flDmgRadius;
	float flWaterDmg;

	//flDmgRadius = 160.0;
	//flWaterDmg = SHOCK_BEAM_DMG * 8;
	flDmgRadius = 180.0; 
	flWaterDmg = SHOCK_BEAM_DMG * 3;

	entvars_t *pevOwner = VARS( pev->owner );	// cant fire underwater anymore

	PLAYBACK_EVENT_FULL( 0, this->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	RadiusDamage( pev->origin, pev, pevOwner, flWaterDmg, flDmgRadius, CLASS_NONE, DMG_SHOCK );
	UTIL_Remove(this);
}*/
#endif
// end shock beam

enum shock_e {
	SHOCK_IDLE1 = 0,
	SHOCK_FIRE,
	SHOCK_DRAW,
	SHOCK_HOLSTER,
	SHOCK_IDLE3
};

//LINK_ENTITY_TO_CLASS( weapon_hornetgun, CShock );	// KILL THE HORNET PLEASE
LINK_ENTITY_TO_CLASS( weapon_shockrifle, CShock );

int CShock::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ShockBeams";
	p->iMaxAmmo1 = SHOCK_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 5;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_SHOCK;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD; 
	//p->iWeight = SHOCK_WEIGHT;

	return 1;
}

void CShock::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_shockrifle");
	Precache( );
	m_iId = WEAPON_SHOCK;
	SET_MODEL(ENT(pev), "models/w_shock.mdl");
	m_iDefaultAmmo = SHOCK_DEFAULT_GIVE;
	FallInit();// get ready to fall down.
}

void CShock::Precache( void )
{
	PRECACHE_MODEL("models/v_shock.mdl");
	PRECACHE_MODEL("models/w_shock.mdl");
	PRECACHE_MODEL("models/p_shock.mdl");

	PRECACHE_SOUND("weapons/shock_draw.wav");
	PRECACHE_SOUND("weapons/shock_fire.wav");
	PRECACHE_SOUND("weapons/shock_recharge.wav");

	m_usShockFire = PRECACHE_EVENT ( 1, "events/shock.sc" );

	UTIL_PrecacheOther("shock_beam");
}

int CShock::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = SHOCK_MAX_CARRY;

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}


BOOL CShock::Deploy( )
{
	Reload();
	EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/shock_draw.wav", 1, ATTN_NORM );
	return DefaultDeploy( "models/v_shock.mdl", "models/p_shock.mdl", SHOCK_DRAW, "hive" );
	//m_flRechargeTime = gpGlobals->time + 0.4;	// dont want people to exploit the max reload rate cause of empty hack
}

void CShock::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( SHOCK_HOLSTER );
	//HACK - can't select shockrifle if it's empty select on empty flag draws no model?,but works?
	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;
	}
}


void CShock::PrimaryAttack()
{
	Reload( );

	if (m_pPlayer->pev->waterlevel == 3)
	{	// throw a little head shake weapon anim in here 
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.50;
		SetNextPrimaryAttackDelay(0.5);
		return;
	}

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.50;
		SetNextPrimaryAttackDelay(0.5);
		return;
	}

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_pPlayer->pev->v_angle);

	CBaseEntity *pBeam = CBaseEntity::Create( "shock_beam", m_pPlayer->GetGunPosition( ), m_pPlayer->pev->v_angle, m_pPlayer->edict() );

	/*Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	
	anglesAim.x		= -anglesAim.x;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir	 = gpGlobals->v_forward;
	
	pBeam->pev->origin = vecSrc;
	pBeam->pev->angles = anglesAim;
	pBeam->pev->owner = m_pPlayer->edict();

	pBeam->pev->velocity = vecDir * 5001;
	pBeam->pev->speed = 1;*/

	pBeam->pev->velocity = gpGlobals->v_forward;
	pBeam->pev->angles = UTIL_VecToAngles (pBeam->pev->velocity);
#endif
	m_flRechargeTime = gpGlobals->time + 0.5;
	
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	
	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 1, 0 );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	//m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.15;
	SetNextPrimaryAttackDelay(0.12);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
	{
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		SetNextPrimaryAttackDelay(0.15);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 5, 10 );
}


void CShock::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= SHOCK_MAX_CARRY)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < SHOCK_MAX_CARRY && m_flRechargeTime < gpGlobals->time)
	{
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/shock_recharge.wav", 1, ATTN_NORM );
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		//m_flRechargeTime = gpGlobals->time + 0.3;
		m_flRechargeTime += 0.35;
	}
}


void CShock::WeaponIdle( void )
{
	Reload( );

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand < 0.9)
	{
		iAnim = SHOCK_IDLE3;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0;
	}
	else
	{
		iAnim = SHOCK_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
	}
	SendWeaponAnim( iAnim );
}
