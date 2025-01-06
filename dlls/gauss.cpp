/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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

// kyles gauss file


#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )


#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"
#include "game.h"
#include "beamdef.h"
#include "pm_shared.h"


BEAM*p_vBeam;


extern float g_flWeaponCheat;

#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

enum gauss_e {
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

LINK_ENTITY_TO_CLASS( weapon_displacer, CGauss ); // new displacer is now a gauss hold onto your butts - santagauss
LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );


float CGauss::GetFullChargeTime( void )
{
#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		return 1.5;
	}

	return 4;
}

#ifdef CLIENT_DLL
extern int g_irunninggausspred;
#endif



CGaussBeam* CGaussBeam::CreateGaussBeam( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner )
{
	
	CGaussBeam *pGaussBeam = GetClassPtr( (CGaussBeam *)NULL );

	UTIL_SetOrigin( pGaussBeam->pev, vecOrigin );
	pGaussBeam->pev->angles = vecAngles;
	pGaussBeam->pev->owner = pOwner->edict();

    pGaussBeam->m_pBeam = CBeam::BeamCreate( "sprites/gauss_beam5.spr", 8 );
    pGaussBeam->m_pBeam->PointEntInit(vecAngles, pGaussBeam->entindex());//pOwner->entindex());
    //pGaussBeam->m_pBeam->SetEndPos(vecOrigin);
	pGaussBeam->m_pBeam->SetScrollRate( 0 );
	pGaussBeam->m_pBeam->SetBrightness( 255 );

	return pGaussBeam;
}
void CGaussBeam::Spawn()
{
    m_pBeam = NULL;
}

CGaussBeam::~CGaussBeam()
{
    if(m_pBeam)
    {
        m_pBeam->SetBrightness(0);
	    UTIL_Remove( m_pBeam );
        m_pBeam = NULL;
    }
}



void CGauss::FadeThink(void)
{
#ifdef CLIENT_DLL

	
#endif
    pev->nextthink = gpGlobals->time + 0.02;
}

void CGauss::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_gauss");
	Precache( );
	m_iId = WEAPON_GAUSS;
	SET_MODEL(ENT(pev), "models/w_gauss.mdl");
	m_iDefaultAmmo = GAUSS_DEFAULT_GIVE;
	FallInit();// get ready to fall down.
    //SetThink(FadeThink);
}


void CGauss::Precache( void )
{
	PRECACHE_MODEL("models/w_gauss.mdl");
	PRECACHE_MODEL("models/v_gauss.mdl");
	PRECACHE_MODEL("models/p_gauss.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/gauss2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("ambience/pulsemachine.wav");
    PRECACHE_MODEL( "sprites/glow03.spr" );


    m_iGlow[0] = PRECACHE_MODEL( "sprites/glow03.spr" );
    m_iGlow[1] = PRECACHE_MODEL( "sprites/gauss_spark1.spr" );
    m_iGlow[2] = PRECACHE_MODEL( "sprites/gauss_spark2.spr" );
    m_iGlow[3] = PRECACHE_MODEL( "sprites/gauss_spark3.spr" );
    m_iGlow[4] = PRECACHE_MODEL( "sprites/gauss_spark4.spr" );
    m_iGlow[5] = PRECACHE_MODEL( "sprites/gauss_spark5.spr" );
    m_iGlow[6] = PRECACHE_MODEL( "sprites/gauss_spark6.spr" );
    m_iGlow[7] = PRECACHE_MODEL( "sprites/gauss_spark7.spr" );
	m_iGlow[8] = PRECACHE_MODEL( "sprites/gauss_spark7.spr" );

    m_iBalls[0] = PRECACHE_MODEL( "sprites/gauss_spark0.spr" );
    m_iBalls[1] = PRECACHE_MODEL( "sprites/gauss_spark1.spr" );
    m_iBalls[2] = PRECACHE_MODEL( "sprites/gauss_spark2.spr" );
    m_iBalls[3] = PRECACHE_MODEL( "sprites/gauss_spark3.spr" );
    m_iBalls[4] = PRECACHE_MODEL( "sprites/gauss_spark4.spr" );
    m_iBalls[5] = PRECACHE_MODEL( "sprites/gauss_spark5.spr" );
    m_iBalls[6] = PRECACHE_MODEL( "sprites/gauss_spark6.spr" );
    m_iBalls[7] = PRECACHE_MODEL( "sprites/gauss_spark7.spr" );
	m_iBalls[8] = PRECACHE_MODEL( "sprites/gauss_spark8.spr" );

    m_iBeam[0] = PRECACHE_MODEL( "sprites/gauss_beam0.spr" );
    m_iBeam[1] = PRECACHE_MODEL( "sprites/gauss_beam1.spr" );
    m_iBeam[2] = PRECACHE_MODEL( "sprites/gauss_beam2.spr" );
    m_iBeam[3] = PRECACHE_MODEL( "sprites/gauss_beam3.spr" );
    m_iBeam[4] = PRECACHE_MODEL( "sprites/gauss_beam4.spr" );
    m_iBeam[5] = PRECACHE_MODEL( "sprites/gauss_beam5.spr" );
    m_iBeam[6] = PRECACHE_MODEL( "sprites/gauss_beam6.spr" );
    m_iBeam[7] = PRECACHE_MODEL( "sprites/gauss_beam7.spr" );
	m_iBeam[8] = PRECACHE_MODEL( "sprites/gauss_beam8.spr" );

//    m_iTrail = PRECACHE_MODEL("sprites/gauss_beam7.spr" );

	m_usGaussFire = PRECACHE_EVENT( 1, "events/gauss.sc" );
	m_usGaussSpin = PRECACHE_EVENT( 1, "events/gaussspin.sc" );
}

int CGauss::AddToPlayer( CBasePlayer *pPlayer )
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

int CGauss::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_GAUSS;
	p->iFlags = 0;
//	p->iWeight = GAUSS_WEIGHT;
//	p->iWeight = m_pPlayer->m_nWeight[WEAPON_GAUSS];


	return 1;
}

BOOL CGauss::Deploy( )
{
	m_pPlayer->m_flPlayAftershock = 0.0;
	return DefaultDeploy( "models/v_gauss.mdl", "models/p_gauss.mdl", GAUSS_DRAW, "gauss" );
}

void CGauss::Holster( int skiplocal /* = 0 */ )
{
	PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );
	
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	SendWeaponAnim( GAUSS_HOLSTER );
	m_fInAttack = 0;
}


void CGauss::PrimaryAttack()
{
#ifdef CLIENT_DLL
	// fixes the double shot gauss primary fire bug, 0.05 is maximum frame time at 20fps
	if(gpGlobals->time - m_flLastPrimaryFireTime < m_flPrevPrimaryAttack - 0.05) 
	{
		return;
	}
#endif
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		//m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		SetNextPrimaryAttackDelay(0.15);
		SetNextSecondaryAttackDelay(0.15);
		return;
	}

	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] < 1 )
	{
		PlayEmptySound( );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}
	/*if(gpGlobals->time - m_flLastPrimaryFireTime  < 0.05) {
		int i  = 0;
	}
	else {
		int i  = 0;
	}*/

	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	m_fPrimaryFire = TRUE;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;

	StartFire();
	m_fInAttack = 0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	//m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.2;
	SetNextPrimaryAttackDelay(0.2);


}

void CGauss::SecondaryAttack()
{
	// don't fire underwater


	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		if ( m_fInAttack != 0 )
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			SendWeaponAnim( GAUSS_IDLE );
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound( );
		}

               
		//m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		SetNextPrimaryAttackDelay(0.5);
		SetNextSecondaryAttackDelay(0.5);
		return;
	}

	if ( m_fInAttack == 0 )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_fPrimaryFire = FALSE;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		// spin up
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		SendWeaponAnim( GAUSS_SPINUP );
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH;
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{      
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 1000 )
		{
#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
			}
			else
			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.3;
			}
		}

		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			return;
		}
		
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
		{
			// don't eat any more ammo after gun is fully charged.
			m_pPlayer->m_flNextAmmoBurn = 1000;
			#ifdef CLIENT_DLL
				this->PrintState(true);
			#endif
		}
		else {
			#ifdef CLIENT_DLL
				this->PrintState(false);
			#endif
		}

		int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 150 / GetFullChargeTime() ) + 100;
		if ( pitch > 250 ) 
			 pitch = 250;
		
		// ALERT( at_console, "%d %d %d\n", m_fInAttack, m_iSoundState, pitch );

		if ( m_iSoundState == 0 )
			ALERT( at_console, "sound state %d\n", m_iSoundState );

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		// m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 10 )
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
				
#ifndef CLIENT_DLL
			m_pPlayer->TakeDamage( VARS(eoNullEntity), VARS(eoNullEntity), 50, DMG_SHOCK );
			UTIL_ScreenFade( m_pPlayer, Vector(255,128,0), 2, 0.5, 128, FFADE_IN );
#endif
			SendWeaponAnim( GAUSS_IDLE );
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
}

//=========================================================
// StartFire- since all of this code has to run and then 
// call Fire(), it was easier at this point to rip it out 
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names 
//=========================================================
void CGauss::StartFire( void )
{
	float flDamage;
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	//Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_up * -8 + gpGlobals->v_right * 8;
	Vector vecSrc = m_pPlayer->GetGunPosition( );   

	
	if ( gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime() )
	{
		flDamage = 200;
	}
	else
	{
		flDamage = 200 * (( gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime() );
	}

	if ( m_fPrimaryFire )
	{
		// fixed damage on primary attack
#ifdef CLIENT_DLL
		flDamage = GAUSS_DMG;
#else 
		flDamage = GAUSS_DMG;
#endif
	}

	if (m_fInAttack != 3)
	{
		//ALERT ( at_console, "Time:%f Damage:%f\n", gpGlobals->time - m_pPlayer->m_flStartCharge, flDamage );

#ifndef CLIENT_DLL
		float flZVel = m_pPlayer->pev->velocity.z;

		
		if (!m_fPrimaryFire)
		{
			if(m_pPlayer->m_bHasFlag) // cant jump high if you have to flag
			{	
				m_pPlayer->pev->velocity = (m_pPlayer->pev->velocity - gpGlobals->v_forward * flDamage * (5*(1-flaghandicap.value)));
			}
			else
				m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * flDamage * 5;
		}
		if ( !g_pGameRules->IsMultiplayer())
		{
			// in deathmatch, gauss can pop you up into the air. Not in single play or ctf if you have the flag
			m_pPlayer->pev->velocity.z = flZVel;

		}
#endif
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	}

	// time until aftershock 'static discharge' sound
	m_pPlayer->m_flPlayAftershock = gpGlobals->time + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.3, 0.8 );

	Fire( vecSrc, vecAiming, flDamage );
}





//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/********************************************************************/
/********************************************************************/
/*****************FIRE*FIRE*FIRE*FIRE*FIRE*FIRE*FIRE*****************/
/********************************************************************/
/********************************************************************/
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////





void CGauss::Fire( Vector vecOrigSrc, Vector vecDir, float flDamage )
{
	char output_str[256];
	memset(output_str, 0, 256);
	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;

	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;
	edict_t		*pentIgnore;
	float flMaxFrac = 1.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 10;

	pentIgnore = ENT( m_pPlayer->pev );

	#ifdef CLIENT_DLL
	if ( m_fPrimaryFire == false )
		 g_irunninggausspred = true;
	#endif


	// The main firing event is sent unreliably so it won't be delayed.


	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussFire, 0.0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, flDamage, 0.0, 0, m_pPlayer->m_nGaussColor, m_fPrimaryFire ? 1 : 0, 0 );

	// This reliable event is used to stop the spinning sound
	// It's delayed by a fraction of second to make sure it is delayed by 1 frame on the client
	// It's sent reliably anyway, which could lead to other delays

	PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, m_pPlayer->m_nGaussColor, 0, 1 );


#ifndef CLIENT_DLL // 4211
        


	int OldTakeDamage = m_pPlayer->pev->takedamage;
	m_pPlayer->pev->takedamage = DAMAGE_NO;

	while (flDamage > 10 && nMaxHits > 0)
	{
	  
		nMaxHits--;

		// ALERT( at_console, "." );
		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);
		if (tr.fAllSolid || (UTIL_PointContents(tr.vecEndPos) == CONTENT_SKY))
        {
			break;
        }

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (pEntity == NULL)
        {
			break;
		}	

		if ( fFirstBeam )
		{    
			m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
			fFirstBeam = 0;
	
			nTotal += 26;
		}

		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecDir, &tr, DMG_BULLET );
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}     

		if ( pEntity->ReflectGauss() )
		{
			float n;

			pentIgnore = NULL;

			n = -DotProduct(tr.vecPlaneNormal, vecDir);

			if (n < 0.5) // 60 degrees
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				Vector r;
			 

				r = 2.0 * tr.vecPlaneNormal * n + vecDir;
				flMaxFrac = flMaxFrac - tr.flFraction;
				vecDir = r;
				vecSrc = tr.vecEndPos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;
                 
				m_pPlayer->RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, flDamage * n, CLASS_NONE, DMG_BLAST );   

				nTotal += 34;

				// lose energy
				if (n == 0) n = 0.1; 
                flDamage = flDamage * (1 - n);//hle gauss reflect hax
				
			}
			else
			{
				nTotal += 13;

				// limit it to one hole punch
				if (fHasPunched)
                {
					break;
                }
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( !m_fPrimaryFire )
				{	
					// we are just goign to punch through walls a completely different way. this way for consistency - santagauss
					int pointChecks = 0;
					Vector vecWGpoint = tr.vecEndPos;
					
					while(pointChecks <= 10)
					{	// check for a open spot to make wall gauss - santagauss
						pointChecks++;

						vecWGpoint = vecWGpoint + vecDir * 16;

						if(UTIL_PointContents(vecWGpoint) != CONTENT_SOLID)
						{	// now we do wall gauss
							//doWallGauss = 1;
							//output_str
							
							
							flDamage *= (0.9 - 0.07*(pointChecks-1));  
							float damage_radius = 300.0; 
							//flDamage *= (1 - 0.0875*pointChecks);
							//float damage_radius = 256.0;
							/*ClientPrint( m_pPlayer->pev, HUD_PRINTTALK, 
								UTIL_VarArgs( "pointChecks: %i, flDamage: %f\n", 
								 pointChecks, flDamage));*/

						/*	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecWGpoint );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( vecWGpoint.x );	// Send to PAS because of the sound
		WRITE_COORD(vecWGpoint.y );
		WRITE_COORD( vecWGpoint.z );
		WRITE_SHORT( g_sModelIndexWExplosion );
		WRITE_BYTE( 5  ); // scale * 10
		WRITE_BYTE(  5 ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();*/


							::RadiusDamage( vecWGpoint, pev, m_pPlayer->pev, flDamage, damage_radius, CLASS_NONE, DMG_BLAST, HITGROUP_CHEST );	// gauss no headshot

							break;
						}
					}

					if(pointChecks > 10)
					{
						 //ALERT( at_console, "blocked %f\n", n );
						break;
					}
					vecSrc = vecWGpoint + vecDir;
					// and thats all there is to the new wal gauss
					/*
					UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, pentIgnore, &beam_tr);
					if (!beam_tr.fAllSolid)
					{
						
						// trace backwards to find exit point
						UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, pentIgnore, &beam_tr);


						float n = (beam_tr.vecEndPos - tr.vecEndPos).Length( );

						if (n < flDamage)
						{
							if (n == 0) n = 1;   
                            flDamage -= n;
							if(flDamage > 160)
							{
								flDamage = 160;
							}
							// ALERT( at_console, "punch %f\n", n );
							nTotal += 21;

							// exit blast damage
							//m_pPlayer->RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, CLASS_NONE, DMG_BLAST );
							float damage_radius;
							
							damage_radius = flDamage * 2.0;

							Vector vecWGPoint = beam_tr.vecEndPos + vecDir * 18;

							if(UTIL_PointContents(vecWGPoint) != CONTENT_SOLID)
							{
								::RadiusDamage( vecWGPoint, pev, m_pPlayer->pev, flDamage, damage_radius, CLASS_NONE, DMG_BLAST );
								CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );
							}

							nTotal += 53;

							vecSrc = beam_tr.vecEndPos + vecDir;
						}
						else
						{
							break;
						}
						
					}
					else
					{
						 //ALERT( at_console, "blocked %f\n", n );
						break;
					}
					*/
				}
				else
				{
					//ALERT( at_console, "blocked solid\n" );
					break;
				}
			}
		}    

		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
		}
	}
	m_pPlayer->pev->takedamage = OldTakeDamage;

#endif
	// ALERT( at_console, "%d bytes\n", nTotal );
}


void CGauss::WeaponIdle( void )
{
	ResetEmptySound( );

	// play aftershock static discharge
	if ( m_pPlayer->m_flPlayAftershock && m_pPlayer->m_flPlayAftershock < gpGlobals->time )
	{
		switch (RANDOM_LONG(0,2))
		{
		case 0:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 1:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro5.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 2:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro6.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 3:	break; // no sound
		}
		m_pPlayer->m_flPlayAftershock = 0.0;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	}
	else
	{
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5)
		{
			iAnim = GAUSS_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		}
		else if (flRand <= 0.75)
		{
			iAnim = GAUSS_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		}
		else
		{
			iAnim = GAUSS_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
		}

		return;
		SendWeaponAnim( iAnim );
		
	}
}



class CGaussAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_gaussammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}

	int DefaultAmout()
	{
		return AMMO_URANIUMBOX_GIVE;
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_gaussammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_gaussclip, CGaussAmmo );


#endif