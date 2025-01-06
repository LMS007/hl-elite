/***
santagauss
this is the m249 aka SAW from op4
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

enum m249_e
{
	M249_SLOWIDLE1 = 0,
	M249_SLOWIDLE2,
	M249_RELOAD1,
	//M249_RELOAD1,
	M249_HOLSTER,
	M249_DRAW,
	M249_SHOOT1,
	M249_SHOOT2,
	M249_SHOOT3,
};

LINK_ENTITY_TO_CLASS( weapon_m249, CM249 );

int CM249::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = _556_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = M249_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_M249;
	//p->iWeight = M249_WEIGHT;

	return 1;
}

void CM249::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_m249");
	Precache( );
	SET_MODEL(ENT(pev), "models/w_saw.mdl");
	m_iId = WEAPON_M249;
	m_iDefaultAmmo = M249_DEFAULT_GIVE;
	FallInit();
}


void CM249::Precache( void )
{
	PRECACHE_MODEL("models/v_saw.mdl");
	PRECACHE_MODEL("models/w_saw.mdl");
	PRECACHE_MODEL("models/p_saw.mdl");

	PRECACHE_SOUND("weapons/saw_reload.wav");
	PRECACHE_SOUND("weapons/saw_reload2.wav");
	PRECACHE_SOUND ("weapons/saw_fire1.wav");
	PRECACHE_SOUND ("weapons/saw_fire2.wav");
	PRECACHE_SOUND ("weapons/saw_fire3.wav");
	PRECACHE_SOUND ("weapons/357_cock1.wav");
	PRECACHE_SOUND("items/9mmclip1.wav");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shellTE_MODEL

	m_usM249 = PRECACHE_EVENT( 1, "events/m249.sc" );
}

int CM249::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL CM249::Deploy( )
{
	return DefaultDeploy( "models/v_saw.mdl", "models/p_saw.mdl", M249_DRAW, "mp5" );
}

void CM249::Holster( int skiplocal /* = 0 */ )
{
	m_fInSpecialReload = 0;
	m_fInReload = FALSE; // cancel any reload in progress.

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( M249_HOLSTER );
}
	
void CM249::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		//m_flNextPrimaryAttack = 0.15;
		SetNextPrimaryAttackDelay(0.15);
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		//m_flNextPrimaryAttack = 0.15;
		SetNextPrimaryAttackDelay(0.15);
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecDir;

#ifndef CLIENT_DLL
	m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - vecAiming * 45;
#endif

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_556, 1, BULLET_556_DMG, m_pPlayer->pev, m_pPlayer->random_seed, 48.0);	// m249 through walls

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usM249, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.07;
	SetNextPrimaryAttackDelay(0.07);

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.07;
		SetNextPrimaryAttackDelay(0.07);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 5, 10 );
}

void CM249::Reload( void )
{
	if ( ( m_pPlayer->ammo_556 <= 0 ) || ( m_iClip >= M249_MAX_CLIP ) )
		return;
/*
	if (m_fInSpecialReload == 0)
	{
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/saw_reload.wav", 1, ATTN_NORM );
		SendWeaponAnim( M249_RELOAD1 );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;

		m_fInSpecialReload = 0;
		DefaultReload( M249_MAX_CLIP, M249_RELOAD1, 2.0 );
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/saw_reload2.wav", 1, ATTN_NORM );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		ALERT( at_console, "ERROR:M249 specail reload is %i\n", m_fInSpecialReload );
		return;
	}
*/

	if (DefaultReload( M249_MAX_CLIP, M249_RELOAD1, 4.0)) //1 = 2 hle
	{
	//	m_flSoundDelay = 1.5;
	//	EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/saw_reload2.wav", 1, ATTN_NORM );
	}
}

// and this m249 code just gets the m249 workign with the new animation its not complete yet, i just want you to put it in like this. 

void CM249::WeaponIdle( void )
{
	ResetEmptySound( );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if (m_fInSpecialReload)
	{
		Reload();
		return;
	}

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand < 0.85)
	{
		iAnim = M249_SLOWIDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 25.0;
	}
	else
	{
		iAnim = M249_SLOWIDLE2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6.0;
	}
	SendWeaponAnim( iAnim );
}

class CM249ammo : public CBasePlayerAmmo
{
	int DefaultAmout()
	{
		return M249_DEFAULT_GIVE;
	}
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_saw_clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_saw_clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_M249BOX_GIVE, "556", _556_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_556, CM249ammo );


















