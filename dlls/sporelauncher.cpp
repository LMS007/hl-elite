/***
santagauss
the opposing forces spore launcher
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"

enum sporeEventType_e {	// put in the first event int.
	SPORE_FIRE1 = 0,
	SPORE_FIRE2,
	SPORE_FLY,
	SPORE_BOUNCE,
	SPORE_EXPLODE,
};


//start spores entity
#ifndef CLIENT_DLL

LINK_ENTITY_TO_CLASS( spore, CSpore );

CSpore *CSpore::SporeCreate( const Vector &vecOrigin, const Vector &vecAngles, int MoveType, edict_t *pentOwner )
{
	CSpore *pSpore = GetClassPtr( (CSpore *)NULL );
	pSpore->pev->classname = MAKE_STRING("spore");
	pSpore->pev->owner = pentOwner;
	pSpore->pev->origin = vecOrigin;
	pSpore->pev->angles = vecAngles;
	pSpore->pev->movetype = MoveType;
	pSpore->Spawn();

	return pSpore;
}

void CSpore::Spawn( )
{
	Precache( );
	
	m_bInPod = false;
	pev->solid = SOLID_TRIGGER;
	SET_MODEL(ENT(pev), "models/spore.mdl");

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	pev->health = 1; // hle, 007

	if (pev->movetype)
		MakeExplosive();
	else
	{
		SetTouch( NULL );
		SetThink( NULL );
	}
}

void CSpore::Precache( )
{
	PRECACHE_MODEL("models/spore.mdl");

	PRECACHE_SOUND("weapons/splauncher_impact.wav");
	PRECACHE_SOUND("weapons/spore_hit1.wav");
	PRECACHE_SOUND("weapons/spore_hit2.wav");
	PRECACHE_SOUND("weapons/spore_hit3.wav");

	m_usSporeExplode = PRECACHE_EVENT( 1, "events/spore_explode.sc" );

	PRECACHE_MODEL("sprites/sporedebris.spr");
	PRECACHE_MODEL("sprites/glow01.spr");
	m_sExplodeSpr1 = PRECACHE_MODEL("sprites/spore_exp_c_01.spr");
	m_sExplodeSpr2 = PRECACHE_MODEL("sprites/spore_exp_01.spr");
}

void CSpore::RemoveEntity()
{
	UTIL_Remove(m_pSporeSprite);
	UTIL_Remove( this );
	return;
}

void CSpore::MakeExplosive( void )
{
	m_flFlyTimeStart = gpGlobals->time;
	bHitOnce = FALSE;

	m_pSporeSprite = CSprite::SpriteCreate( "sprites/glow01.spr", pev->origin, FALSE );
	m_pSporeSprite->SetTransparency( kRenderTransAdd, 128, 255, 0, 50, pev->renderfx );
	m_pSporeSprite->SetScale( 0.6 );
	m_pSporeSprite->SetAttachment( edict(), 0 );

	SetTouch( &CSpore::SporeTouch );
	SetThink( &CSpore::ExplodeThink );

	pev->nextthink = gpGlobals->time + 0.15;
}

void CSpore::SporeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		RemoveEntity();
		return;
	}

	if ( pev->movetype == MOVETYPE_BOUNCEMISSILE )
	{
		if ( pOther->IsPlayer() )	// push them in primary now too.
		{
			if (pOther->edict() == pev->owner)	// move out if no push
			{
				return;
			}
			Vector vecPushDir = (pOther->pev->origin - pev->origin).Normalize();
			pOther->pev->velocity = pOther->pev->velocity + vecPushDir * 650;
		}

		SetTouch( NULL );
		SetThink( NULL );
		Explode( );
		return;
	}
	else if ( (pOther->pev->takedamage) || (gpGlobals->time >= m_flFlyTimeStart + 1.3) )
	{
		if ( pOther->IsPlayer() )
		{
			if (!bHitOnce)
			{
				if (pOther->edict() == pev->owner)
					return;
			}
			Vector vecPushDir = (pOther->pev->origin - pev->origin).Normalize();
			pOther->pev->velocity = pOther->pev->velocity + vecPushDir * 650;
		}

		SetTouch( NULL );
		SetThink( NULL );
		Explode( );
		return;
	}
	else
	{
		TraceResult	tr = UTIL_GetGlobalTrace( );

		Vector vecPlaneNorm = tr.vecPlaneNormal;
		
		PLAYBACK_EVENT_FULL( 0, this->edict(), m_usSporeExplode, 0.0, (float *)&g_vecZero, (float *)&vecPlaneNorm, 0.0, 0.0, SPORE_BOUNCE, 0, 0, 0 );

		if (!bHitOnce)	bHitOnce = TRUE;
	}
}

void CSpore :: ExplodeThink( void )
{
	if ( gpGlobals->time >= m_flFlyTimeStart + 10 )
	{
		Explode( );
		return;
	}

	Vector vecVelocity = pev->velocity;

	if ( pev->movetype == MOVETYPE_BOUNCEMISSILE )
	{
		if ( vecVelocity.Length() < 700 )
		{
			pev->movetype = MOVETYPE_BOUNCE;	// make realistic if it slows down too much
		}
	}

	PLAYBACK_EVENT_FULL( 0, this->edict(), m_usSporeExplode, 0.0, (float *)&g_vecZero, (float *)&vecVelocity, 0.0, 0.0, SPORE_FLY, 0, 0, 0 );
//	if (!bHitOnce)	bHitOnce = TRUE;	// if bugs or exploits put back in
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.06, 0.12 );
}

void CSpore::Explode( )
{
	UTIL_Remove(m_pSporeSprite);
	pev->model = iStringNull;

	BOOL bHitBSP = FALSE;
	entvars_t	*pevOwner;
	pevOwner = VARS( pev->owner );
	TraceResult	tr = UTIL_GetGlobalTrace( );
	Vector vecPlaneNorm = -(pev->velocity);	// should take care of my event set to -, if it hits a wall its over written

	float sporeDmg = (pev->velocity.Length() * 0.09);
	if(sporeDmg > 200)
		sporeDmg = 200;

	//ALERT( at_console, "Spore Damage = %f\n", sporeDmg ); // 007 leave this in for .d so we can get some readout on this weapon.

	if ( tr.flFraction != 1.0 && tr.pHit)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if ( pEntity->IsBSPModel() )
		{
			bHitBSP = TRUE;
		}
	}

	if ( bHitBSP )
	{
		TraceResult tr2;
		Vector vecSrc = tr.vecEndPos;
		vecPlaneNorm = tr.vecPlaneNormal;
		Vector vecDest = vecSrc + vecPlaneNorm * (sporeDmg / 4);

		UTIL_TraceLine(tr.vecEndPos, vecDest, dont_ignore_monsters, ENT(pev), &tr2);

		pev->origin = vecSrc + (vecPlaneNorm * (tr2.flFraction*(sporeDmg / 8)) );
	}

	PLAYBACK_EVENT_FULL( 0, this->edict(), m_usSporeExplode, 0.0, (float *)&g_vecZero, (float *)&vecPlaneNorm, sporeDmg, 0.0, SPORE_EXPLODE, 0, bHitBSP, 0 );

	RadiusDamage( pev->origin, pev, pevOwner, sporeDmg, sporeDmg * 1.3/*radius*/, CLASS_NONE, DMG_BLAST );

	UTIL_Remove(this);
}

#endif
// end spores

enum slauncher_e {
	SLAUNCHER_IDLE1 = 0,
	SLAUNCHER_PET,
	SLAUNCHER_RELOAD1,
	SLAUNCHER_RELOAD2,
	SLAUNCHER_RELOAD3,
	SLAUNCHER_FIRE,
	SLAUNCHER_HOLSTER,
	SLAUNCHER_DRAW,
	SLAUNCHER_IDLE2,
};

LINK_ENTITY_TO_CLASS( weapon_sporelauncher, CSporeLauncher );

int CSporeLauncher::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "spores";
	p->iMaxAmmo1 = SPORE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SPORE_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SPORE;
	//p->iWeight = SPORE_WEIGHT;

	return 1;
}

void CSporeLauncher::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_sporelauncher");
	Precache( );
	m_iId = WEAPON_SPORE;
	SET_MODEL(ENT(pev), "models/w_spore_launcher.mdl");
	m_iDefaultAmmo = SPORE_DEFAULT_GIVE;
	FallInit();// get ready to fall
}

void CSporeLauncher::Precache( void )
{
	PRECACHE_MODEL("models/v_spore_launcher.mdl");
	PRECACHE_MODEL("models/w_spore_launcher.mdl");
	PRECACHE_MODEL("models/p_spore_launcher.mdl");

	PRECACHE_SOUND("weapons/splauncher_fire.wav");
	PRECACHE_SOUND("weapons/splauncher_altfire.wav");	
	PRECACHE_SOUND("weapons/splauncher_reload.wav");
	PRECACHE_SOUND("weapons/splauncher_pet.wav");

	UTIL_PrecacheOther("spore");

	m_usSporeFire = PRECACHE_EVENT( 1, "events/spore.sc" );
}

int CSporeLauncher::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CSporeLauncher::Deploy( )
{
	return DefaultDeploy( "models/v_spore_launcher.mdl", "models/p_spore_launcher.mdl", SLAUNCHER_DRAW, "rpg" );
}

void CSporeLauncher::Holster( int skiplocal /* = 0 */ )
{
	m_fInSpecialReload = 0;
	m_fInReload = FALSE; // cancel any reload in progress.

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( SLAUNCHER_HOLSTER );
}

void CSporeLauncher::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		//m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
		SetNextPrimaryAttackDelay(0.7);
		SetNextSecondaryAttackDelay(0.7);
		Reload( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;

	m_iClip--;

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );

	CSpore *pSpore = CSpore::SporeCreate( m_pPlayer->GetGunPosition( ), m_pPlayer->pev->v_angle, MOVETYPE_BOUNCEMISSILE, m_pPlayer->edict() );
	pSpore->pev->velocity = gpGlobals->v_forward * 1400;
#endif

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSporeFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, SPORE_FIRE1, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
	//m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
	SetNextPrimaryAttackDelay(0.7);
	SetNextSecondaryAttackDelay(0.7);

	if (m_iClip >= 1)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
	m_fInSpecialReload = 0;
}

void CSporeLauncher::SecondaryAttack()
{
	if (m_iClip <= 0)
	{
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		//m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
		SetNextPrimaryAttackDelay(0.7);
		SetNextSecondaryAttackDelay(0.7);
		Reload( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;

	m_iClip--;

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );

	CSpore *pSpore = CSpore::SporeCreate( m_pPlayer->GetGunPosition( ), m_pPlayer->pev->v_angle, MOVETYPE_BOUNCE, m_pPlayer->edict() );
	pSpore->pev->velocity = gpGlobals->v_forward * 1000;
#endif

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSporeFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, SPORE_FIRE2, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
	//m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
	SetNextPrimaryAttackDelay(0.75);
	SetNextSecondaryAttackDelay(0.75);
	if (m_iClip >= 1)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
	m_fInSpecialReload = 0;
}

void CSporeLauncher::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SPORE_MAX_CLIP)
		return;

	// don't reload until recoil is done look for exploits
//	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
//		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( SLAUNCHER_RELOAD1 );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		//m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
		SetNextPrimaryAttackDelay(0.7);
		SetNextSecondaryAttackDelay(0.7);
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/splauncher_reload.wav", 1, ATTN_NORM );

		SendWeaponAnim( SLAUNCHER_RELOAD2 );

		m_flNextReload = UTIL_WeaponTimeBase() + 1.2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.2;
	}
	else
	{
		// Add them to the animal =]
		m_iClip++;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void CSporeLauncher::WeaponIdle( void )
{
	ResetEmptySound( );

	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		}
		else if (m_fInSpecialReload != 0)
		{
			if (m_iClip != 5 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// end reload
				SendWeaponAnim( SLAUNCHER_RELOAD3 );
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand < 0.70)
			{
				iAnim = SLAUNCHER_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (18.0);
			}
			else if (flRand < 0.90)
			{
				iAnim = SLAUNCHER_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (4.0);
			}
			else
			{
				iAnim = SLAUNCHER_PET;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (4.0);
				EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/splauncher_pet.wav", 1, ATTN_NORM );

			}
			SendWeaponAnim( iAnim );
		}
	}
}

//
// Spore Ammo 
//
#ifndef CLIENT_DLL

enum sporeammo_e {
	SPOREAMMO_GROW = 0,
	SPOREAMMO_SPAWNUP,
	SPOREAMMO_SNATCHUP,
	SPOREAMMO_SPAWNDN,
	SPOREAMMO_SNATCHDN,
	SPOREAMMO_IDLE1,
	SPOREAMMO_IDLE2,
};

LINK_ENTITY_TO_CLASS( ammo_spore, CSporeAmmo );

void CSporeAmmo::Spawn( void )
{
	SET_MODEL(ENT(pev), "models/spore_ammo.mdl"); 
	pev->angles.x -= 90;	// HACK! GBX is GAY
	pev->origin.z += 16;	// HACK! GBX is GAY
	UTIL_SetOrigin( pev, pev->origin );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize(pev, Vector(-16, -16, -16), Vector(16, 16, 16));

	// determine the position of the spore
	// the roll is completely broken and i cant fix it or all the op4 maps ill have to be edited OK

	{	
		Vector vecAngles = pev->angles;	
		float	angle, sinr, sinp, siny, cosr, cosp, cosy;

		angle = vecAngles.y * (M_PI*2 / 360);
		siny = sin(angle);
		cosy = cos(angle);
		angle = vecAngles.x * (M_PI*2 / 360);
		sinp = sin(angle);
		cosp = cos(angle);
//		angle = vecAngles.z * (M_PI*2 / 360);	// because the roll is broken
//		sinr = sin(angle);		// i am just assigning the result of a zero angle
//		cosr = cos(angle);		// just to save some clocks.
		sinr = 0;		// above^
		cosr = 1;		// above^

		vecDirForward.x = -(cosr*sinp*cosy+-sinr*-siny);	// HACK! negative value GBX is GAY
		vecDirForward.y = -(cosr*sinp*siny+-sinr*cosy);		// HACK! negative value GBX is GAY
		vecDirForward.z = cosr*cosp;						// HACK! this is actually the UP angle not forward

		vecDirUp.x = cosp*cosy;
		vecDirUp.y = cosp*siny;
		vecDirUp.z = -sinp;

		vecDirRight.x = (-1*sinr*sinp*cosy+-1*cosr*-siny);
		vecDirRight.y = (-1*sinr*sinp*siny+-1*cosr*cosy);
		vecDirRight.z = -1*sinr*cosp;

		vecSporigin = pev->origin + vecDirForward * 8;
	}

	bHasSpore = FALSE;
	pev->sequence = SPOREAMMO_SNATCHDN;
	pev->frame = 0;
	ResetSequenceInfo( );

	SetThink( &CSporeAmmo::SporeThink );
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 1.0;
}

void CSporeAmmo::Precache( void )
{
	PRECACHE_MODEL("models/spore_ammo.mdl");
	PRECACHE_MODEL("models/w_sporesack.mdl"); // ULGY FUCKING HACK BECAUSE Sporesac will not precahse in its own fucking entity
	PRECACHE_MODEL("models/spore.mdl");
	PRECACHE_SOUND("weapons/spore_ammo.wav");
	UTIL_PrecacheOther("spore");
}

void CSporeAmmo::AmmoTouch( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
		return;

	if (pOther->GiveAmmo( AMMO_SPORE_GIVE, "spores", SPORE_MAX_CARRY ) != -1)
	{
		UTIL_Remove(m_pMySpore);
		m_pMySpore = NULL;
		bHasSpore = FALSE;

		EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/spore_ammo.wav", 1, ATTN_NORM);

		pev->sequence = SPOREAMMO_SNATCHDN;
		pev->frame = 0;
		ResetSequenceInfo( );

		SetThink( &CSporeAmmo::SporeThink );
		SetTouch( NULL );
		pev->nextthink = gpGlobals->time + 1.0;
	}
}

int CSporeAmmo::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if (bHasSpore) // CRASHED HERE STOP HHERE 
	{
		bHasSpore = FALSE;

		float x, y, z;

		x = RANDOM_FLOAT( -1.0, 1.0 );
		y = RANDOM_FLOAT( -1.0, 1.0 );
		z = x * x + y * y;

		Vector vecDir = vecDirForward + x * VECTOR_CONE_40DEGREES.x * vecDirRight + y * VECTOR_CONE_40DEGREES.y * vecDirUp;
		
		m_pMySpore->pev->movetype = MOVETYPE_BOUNCE;
		m_pMySpore->pev->velocity = vecDir * RANDOM_FLOAT( 300, 700 );
		m_pMySpore->m_bInPod = false;
		m_pMySpore->MakeExplosive();
		m_pMySpore = NULL;

		pev->sequence = SPOREAMMO_SNATCHDN;
		pev->frame = 0;
		ResetSequenceInfo( );

		SetThink( &CSporeAmmo::SporeThink );
		SetTouch( NULL );
		pev->nextthink = gpGlobals->time + 1.0;
		return 1;
	}
	return 0;
}

void CSporeAmmo::SporeThink( void )
{
	if(!bHasSpore)
	{
		if(pev->sequence == SPOREAMMO_SNATCHDN)
		{
			pev->sequence = SPOREAMMO_GROW;
			pev->frame = 0;	
			ResetSequenceInfo( );
			pev->framerate = 0.0;
			pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 4.0, 8.0 );
		}
		else if(pev->sequence == SPOREAMMO_GROW)
		{
			bHasSpore = TRUE;

			m_pMySpore = CSpore::SporeCreate( vecSporigin, pev->angles, MOVETYPE_NONE, ENT(pev) );
			m_pMySpore->m_bInPod = true;
			m_pMySpore->pev->angles = pev->angles + Vector(90,0,0);

			pev->sequence = SPOREAMMO_SPAWNDN;
			pev->frame = 0;
			ResetSequenceInfo( );
			pev->nextthink = gpGlobals->time + 5.0;

			SetTouch( &CSporeAmmo::AmmoTouch );
		}
		else
		{	// something went wrong
			ALERT( at_console, "ERROR: spore_ammo animation not handled - No Ammo.\n" );
			pev->sequence = SPOREAMMO_SNATCHDN;
			pev->frame = 0;
			ResetSequenceInfo( );
			pev->nextthink = gpGlobals->time + 1.0;
		}
	}
	else if(pev->sequence == SPOREAMMO_SPAWNDN)
	{
		pev->sequence = SPOREAMMO_IDLE1;
		pev->frame = RANDOM_LONG(0,255);
		ResetSequenceInfo( );
		pev->framerate = RANDOM_FLOAT( 0.1, 0.4 );
		SetThink( NULL );
	}
	else 
	{	// something went wrong
		ALERT( at_console, "ERROR: spore_ammo animation not handled - Has Ammo.\n" );
		SetThink( NULL );
		SetTouch( &CSporeAmmo::AmmoTouch );
	}
}

class CBaseSpore : public CBasePlayerAmmo	// we need to get this a new model and also set it to some larger amount like 3
{
	int DefaultAmout()
	{
		return AMMO_SPORESACK_GIVE ;
	}
	void Spawn( void )
	{ 
		//PRECACHE_MODEL("models/w_sporesack.mdl.mdl");
		//Precache( );
		SET_MODEL(edict(), "models/w_sporesack.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL("models/sack.mdl");
		PRECACHE_SOUND("weapons/spore_ammo.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
						
		int bResult = (pOther->GiveAmmo( AMMO_SPORESACK_GIVE , "spores", SPORE_MAX_CARRY)  != -1);
		
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/spore_ammo.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};

LINK_ENTITY_TO_CLASS( ammo_sporeball, CBaseSpore );
#endif

//
// End Spore Ammo 
//
