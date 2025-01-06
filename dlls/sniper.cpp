/****
santagauss
ok here we go first weapon ever coded by me.
opposing forces sniper rifle
weapon_sniperrifle
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"

enum sniper_e {
	SNIPER_DRAW = 0,
	SNIPER_SLOWIDLE1,
	SNIPER_FIRE,
	SNIPER_FIRELAST,
	SNIPER_RELOAD1,
	SNIPER_RELOAD2,
	SNIPER_RELOAD3,
	SNIPER_SLOWIDLE2,
	SNIPER_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_sniperrifle, CSniper );

int CSniper::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762";
	p->iMaxAmmo1 = _762_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SNIPER_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_SNIPER;
	//p->iWeight = SNIPER_WEIGHT;

	return 1;
}

void CSniper::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_sniperrifle");
	Precache( );
	m_iId = WEAPON_SNIPER;
	SET_MODEL(ENT(pev), "models/w_m40a1.mdl");
	m_iDefaultAmmo = SNIPER_DEFAULT_GIVE;
	FallInit();
}

int CSniper::AddToPlayer( CBasePlayer *pPlayer )
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

void CSniper::Precache( void )
{
	PRECACHE_MODEL("models/v_m40a1.mdl");
	PRECACHE_MODEL("models/w_m40a1.mdl");
	PRECACHE_MODEL("models/p_m40a1.mdl");

	PRECACHE_SOUND("weapons/sniper_reload_first_seq.wav");
	PRECACHE_SOUND("weapons/sniper_reload_second_seq.wav");
	PRECACHE_SOUND("weapons/sniper_bolt1.wav");
	PRECACHE_SOUND("weapons/sniper_fire.wav");
	PRECACHE_SOUND("weapons/sniper_zoom.wav");
	
	m_usFireSniper = PRECACHE_EVENT( 1, "events/sniper.sc" );
}

BOOL CSniper::Deploy( )
{
	return DefaultDeploy( "models/v_m40a1.mdl", "models/p_m40a1.mdl", SNIPER_DRAW, "bow" );
}


void CSniper::Holster( int skiplocal /* = 0 */ )
{
	m_fInSpecialReload = 0;
	m_fInReload = FALSE; // cancel any reload in progress.

	if ( m_fInZoom )
		SecondaryAttack();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	SendWeaponAnim( SNIPER_HOLSTER );
}

void CSniper::SecondaryAttack( void )
{
	if ( m_pPlayer->pev->fov != 0 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
		m_fInZoom = 0;
	}
	else if ( m_pPlayer->pev->fov != 15 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 15;
		m_fInZoom = 1;
	}
	
	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sniper_zoom.wav", 1, ATTN_NORM, AMBIENT_SOUND_LARGERADIUS /* 0 */, 100);
}

void CSniper::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		//m_flNextPrimaryAttack = 0.25;
		SetNextPrimaryAttackDelay(0.25);
		return;
	}

	if (m_iClip <= 0)
	{
		if (!m_fFireOnEmpty)
			Reload( );
		else
		{
			PlayEmptySound( );
			//m_flNextPrimaryAttack = 0.25;
			SetNextPrimaryAttackDelay(0.25);
		}
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );


	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc = m_pPlayer->GetGunPosition( );
	Vector vecDir = gpGlobals->v_forward;

    int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireSniper, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;
	SetNextPrimaryAttackDelay(2.0);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 4, 7 );

// from here down is how i apply damage decide what the bullet will do
#ifndef CLIENT_DLL
	edict_t	*pentIgnore = ENT( m_pPlayer->pev );
	TraceResult tr, wall_tr;
	Vector vecNewDir;

	float fWallPassSize = 0;
	float fWallAngle = 0;
	float flMaxFrac = 1;
	float flDamage = BULLET_762_DMG;
	int iWallHits = 0;
	int iCountHits = 0;

	while ( iCountHits < 8 && iWallHits < 3 )
	{
		Vector vecDest = (vecSrc + vecDir * 8192);
		flDamage = ( BULLET_762_DMG - ( (iCountHits + iWallHits) * 5 ) );
		iCountHits++;

		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);	// make a line return a result

		if (tr.fAllSolid)
		{
			break;
		}

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);	// make pointer to what i hit

		if (pEntity == NULL)
		{
			break;
		}

		if ( pEntity == m_pPlayer )
		{
			ALERT( at_console, "762 hit owner\n" );
			continue;
		}

		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB );
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}

		if ( pEntity->ReflectGauss() )	// lol this func should be called is wall or some shit any how i can use it
		{	// i am a wall do this

			if(UTIL_PointContents(tr.vecEndPos) == CONTENT_SKY)
				break;

			pentIgnore = NULL;

			iWallHits++;


			UTIL_Sparks( tr.vecEndPos );
			UTIL_DecalTrace( &tr, DECAL_GUNSHOT1 );	// make a gunshot deccal at entrance point

			if ( iWallHits > 2 )
			{
				break;
			}

			fWallAngle = -DotProduct(tr.vecPlaneNormal, vecDir);	// take the difference of the wall normal and vecDir

			if ( fWallAngle < 0.3 )
			{	// reflect off surface

				vecNewDir = (2.0 * tr.vecPlaneNormal * fWallAngle + vecDir);	// set new vector direction
				flMaxFrac = flMaxFrac - tr.flFraction;	// working here
				vecDir = vecNewDir;
				vecSrc = tr.vecEndPos + vecDir * 8;	// new location interesting set it out a bit from the wall
				vecDest = vecSrc + vecDir * 8192;	// new far out point. 
			}
			else
			{
				UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, pentIgnore, &wall_tr);	// trace forward again
	
				if (wall_tr.fAllSolid)
				{
					break;
				}

				// trace backwards to find exit point	
				UTIL_TraceLine( wall_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, pentIgnore, &wall_tr);

				fWallPassSize += ( (wall_tr.vecEndPos - tr.vecEndPos).Length( ) );	// thickness of wall


				if ( fWallPassSize > 40.0 )	// set max penitration either can do 32 @ 40 or just 32 for 16...
				{
					break;
				}

				UTIL_Sparks( wall_tr.vecEndPos );
				UTIL_DecalTrace( &wall_tr, DECAL_GUNSHOT1 );	// gunshot decal at exit point

				vecSrc = wall_tr.vecEndPos + vecDir;
			}
		}
		else
		{	// not bsp model
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
		}
		// ok i hit something now set my fall off into the vecDir
		if ( vecDir.z > -0.95 )
			vecDir.z -= .055;
		else
			vecDir.z = -0.9999;
	}
#endif
}


void CSniper::Reload( void )
{
	if ( m_pPlayer->pev->fov != 0 )
		SecondaryAttack();

	if ( ( m_pPlayer->ammo_762 <= 0 ) || ( m_iClip >= SNIPER_MAX_CLIP ) )
		return;

	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( SNIPER_RELOAD1 );
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sniper_reload_first_seq.wav", 1, ATTN_NORM );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5;
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 3.0;
		//m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 3.0;
		SetNextPrimaryAttackDelay(3.0);
		SetNextSecondaryAttackDelay(3.0);
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;

		m_fInSpecialReload = 0;
		DefaultReload( SNIPER_MAX_CLIP, SNIPER_RELOAD2, 1.5 );
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sniper_reload_second_seq.wav", 1, ATTN_NORM );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		ALERT( at_console, "ERROR:Sniper Rifle specail reload is %i\n", m_fInSpecialReload );
		return;
	}
}

void CSniper::WeaponIdle( void )
{
	ResetEmptySound( );

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if (m_fInSpecialReload)
	{
		Reload();
		return;
	}

	if (m_iClip)
	{
		SendWeaponAnim( SNIPER_SLOWIDLE1);
	}
	else
	{
		SendWeaponAnim( SNIPER_SLOWIDLE2 );
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 30;
}

class CSniperAmmo : public CBasePlayerAmmo
{
	int DefaultAmout()
	{
		return SNIPER_DEFAULT_GIVE;
	}
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_m40a1clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_m40a1clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_762CLIP_GIVE, "762", _762_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS( ammo_762, CSniperAmmo );
