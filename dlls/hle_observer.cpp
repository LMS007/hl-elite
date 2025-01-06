//=============================================================================
// observer.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"pm_shared.h"
#include	"hle_flagevents.h"
#include	"game.h"

extern int gmsgCurWeapon;
extern int gmsgSetFOV;
extern int gmsgTeamInfo;

extern int gmsgHideWeapon;
extern int gmsgBattery;
extern int gmsgHealth;
extern int gmsgTrain;
extern int gmsgWeaponList;
extern int gmsgAmmoX;
extern int gmsgScoreInfo;
extern int gmsgFixEgon;
//extern int gmsgHudColor;
extern int gmsgReset;
extern int gmsgChangeXHair;
extern int gmsgRadar;
extern int gmsgLocation;
extern int gmsgIconInfo;
extern int gmsgCtfEvent;

//extern int spectator_flag; //hle

extern int g_teamplay;

extern COutput debug_file;
// Player has become a spectator. Set it up.
// This was moved from player.cpp.
void CBasePlayer::StartObserver( Vector vecPosition, Vector vecViewAngle, int nMode)
{



#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Enter StartObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/

	if(m_isSpectator)
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
			if ( pEnt && pEnt->IsPlayer( ) )
			{
				CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
				if(pPlayer->m_lObservers.lookup(edict())>=0) // two way check
				{
					pPlayer->RemoveSpectator(edict());	
				}
			}
		}
		pev->iuser1 = NULL;
		pev->iuser2 = NULL;
	}

	if(!IsAlive())
	{
		pev->weapons |= (1<<WEAPON_SUIT);
	}
	m_isSpectator = true;
	m_bInMatch = false; 
	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "if_fucking_spawned", "1" );


	// clear any clientside entities attached to this player
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_KILLPLAYERATTACHMENTS );
		WRITE_BYTE( (BYTE)entindex() );
	MESSAGE_END();

	// Holster weapon immediately, to allow it to cleanup
	if (m_pActiveItem)
		m_pActiveItem->Holster( );

	if ( m_pTank != NULL )
	{
		m_pTank->Use( this, this, USE_OFF, 0 );
		m_pTank = NULL;
	}

	// clear out the suit message cache so we don't keep chattering
	SetSuitUpdate(NULL, FALSE, 0);

	// Tell Ammo Hud that the player is dead
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0XFF);
		WRITE_BYTE(0xFF);
	MESSAGE_END();

	// reset FOV
	m_iFOV = m_iClientFOV = 0;
	pev->fov = m_iFOV;

	// Setup flags
	m_iHideHUD = (HIDEHUD_HEALTH | HIDEHUD_WEAPONS);

	m_afPhysicsFlags |= PFLAG_OBSERVER;
	pev->effects = EF_NODRAW;
	pev->view_ofs = g_vecZero;
	pev->angles = pev->v_angle = vecViewAngle;
	pev->fixangle = TRUE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	ClearBits( m_afPhysicsFlags, PFLAG_DUCKING );
	ClearBits( pev->flags, FL_DUCKING );

	pev->deadflag = DEAD_RESPAWNABLE;
	pev->health = 1;
	///////
	pev->movetype = MOVETYPE_NONE; 
	pev->effects |= EF_NODRAW; 

	if(!m_fGameHUDInitialized)
		m_fInitHUD = TRUE;

	// Update Team Status
//	pev->team = 0;
	

	// Remove all the player's stuff
	RemoveAllItems( FALSE );

	// Find a player to watch
	m_flNextObserverInput = 0;
	Observer_SetMode(nMode);

	CEntityNode* pClient = m_lObservers.head();
	while(pClient)
	{
		if(pClient->m_pEntity)
		{
			CBasePlayer* pObserver = GetClassPtr((CBasePlayer *)&(pClient->m_pEntity->v));  // CRASH RIGHT HERE!!!!!
			if(pObserver)
			{
				CEntityNode* pTemp = pClient->m_pNext;
				pObserver->Observer_FindNextPlayer( 0 );
				pClient = pTemp;
				continue;
			}
		}
		pClient = pClient->m_pNext;
	}

	//{
	//	CBasePlayer* pObserver = GetClassPtr((CBasePlayer *)&(m_lObservers[i]->v));
	//	pObserver->Observer_FindNextPlayer( 0 );
	//}

	g_pGameRules->SendScoreInfo(this);

	// radar clean up
	MESSAGE_BEGIN( MSG_ALL, gmsgLocation );
		WRITE_SHORT( entindex() );
		WRITE_BYTE(0);	// 1 = add player to list, 0 = remove player from list
		WRITE_SHORT(0);
		WRITE_SHORT(0);
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ONE, gmsgLocation, NULL, edict() );
		WRITE_SHORT( 0 );
		WRITE_BYTE( 2 );	// 1 = add player to list, 0 = remove player from list, 2 = cleaer list.
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
	MESSAGE_END();

	g_pGameRules->StartObserver(this);

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Exit StartObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/
}

// Leave observer mode
void CBasePlayer::StopObserver( void )
{

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Enter StopObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/
	
	if ( pev->iuser1 || pev->iuser2 ) // i have no fucking clue why this is here, but it fucking needs it for some gay reaqson, fuck the faggot who coded this shit
	{
		m_isSpectator = false;
		m_bInMatch = true; // hack, change later

		MESSAGE_BEGIN( MSG_ONE, gmsgFixEgon, NULL, edict() );  
		MESSAGE_END();

		

		for (int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
			if ( pEnt && pEnt->IsPlayer( ) )
			{
				CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
				if(pPlayer->m_lObservers.lookup(edict())>=0) // two way check
				{
					pPlayer->RemoveSpectator(edict());	
				}
			}
		}

		
	

		MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pev );
			WRITE_BYTE(FLAG_SPEC_DROP);
			WRITE_BYTE(0);
			WRITE_BYTE(m_nCtfFlagInHand);
		MESSAGE_END();
		
		MESSAGE_BEGIN( MSG_ONE, gmsgLocation, NULL, edict() );
		WRITE_SHORT( 0 );
		WRITE_BYTE( 2 );	// 1 = add player to list, 0 = remove player from list, 2 = cleaer list.
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		MESSAGE_END();

		m_iHideHUD &= ~HIDEHUD_HEALTH;
		m_iHideHUD &= ~HIDEHUD_WEAPONS;
		m_iHideHUD &= ~HIDEHUD_ALL;

		pev->iuser1 = pev->iuser2 = 0; 
		m_hObserverTarget = NULL;
		
		g_pGameRules->ResetPlayerHud(this);

		Spawn();

#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Exit StopObserver\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/

	}
}

// Find the next client in the game for this player to spectate
void CBasePlayer::Observer_FindNextPlayer( bool bReverse )
{
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Enter Observer_FindNextPlayer\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/

	// MOD AUTHORS: Modify the logic of this function if you want to restrict the observer to watching
	//				only a subset of the players. e.g. Make it check the target's team.

	int		iStart;
	CBasePlayer *pPlayer = NULL;
	CBaseEntity *pEnt	 = NULL;

	if ( m_hObserverTarget )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgFixEgon, NULL, edict() );  // this needs testing
		MESSAGE_END();

		CBasePlayer *pPlayer=NULL;
		CBaseEntity *pEnt = UTIL_PlayerByIndex(m_hObserverTarget->entindex());
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			pPlayer = (CBasePlayer *)pEnt;
			if(pPlayer->m_lObservers.lookup(edict())>=0) // two way check
				pPlayer->RemoveSpectator(edict());	
		}
	
		iStart = ENTINDEX( m_hObserverTarget->edict() );
	}

	else
		iStart = ENTINDEX( edict() );
	int	    iCurrent = iStart;
	m_hObserverTarget = NULL;
	int iDir = bReverse ? -1 : 1; 

	do
	{
		iCurrent += iDir;

		// Loop through the clients
		if (iCurrent > gpGlobals->maxClients)
			iCurrent = 1;
		if (iCurrent < 1)
			iCurrent = gpGlobals->maxClients;

		CBaseEntity *pEnt = UTIL_PlayerByIndex( iCurrent );
		if ( !pEnt )
			continue;
		if ( pEnt == this )
			continue;
		if (!pEnt->IsAlive())
			continue;
		if(FStrEq( STRING( pEnt->pev->netname ), "" ))
			continue;
		// Don't spec observers or invisible players
		if ( ((CBasePlayer*)pEnt)->IsObserver() || (pEnt->pev->effects & EF_NODRAW) )
			continue;

		// MOD AUTHORS: Add checks on target here.

		m_hObserverTarget = pEnt;
		break;

	} while ( iCurrent != iStart );

	// Did we find a target?
	if (m_hObserverTarget )
	{
		if(m_hObserverTarget != this)
		{
			// Store the target in pev so the physics DLL can get to it
			pev->iuser2 = ENTINDEX( m_hObserverTarget->edict() );
			pEnt = UTIL_PlayerByIndex(pev->iuser2);
			if ( pEnt && pEnt->IsPlayer( ) )
			{
				m_iHideHUD &= ~HIDEHUD_ALL;
				pPlayer = (CBasePlayer *)pEnt;
				
				g_pGameRules->SetSpectatorHud(this, pPlayer);

				MESSAGE_BEGIN( MSG_ONE, gmsgChangeXHair, NULL, edict() );  
				WRITE_STRING( pPlayer->m_szCrosshairs );
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ONE, gmsgLocation, NULL, edict() );
					WRITE_SHORT( 0 );
					WRITE_BYTE( 2 );	// 1 = add player to list, 0 = remove player from list, 2 = cleaer list.
					WRITE_SHORT( 0 );
					WRITE_SHORT( 0 );
				MESSAGE_END();

				pev->fov = pPlayer->pev->fov;

				pPlayer->AddSpectator(edict());

				UpdateObserverHud(pPlayer);
			}
			// Move to the target
			UTIL_SetOrigin( pev, m_hObserverTarget->pev->origin );
			
 			ALERT( at_console, "Now Tracking %s\n", STRING( m_hObserverTarget->pev->netname ) );
		}
	}
	else
	{
		ALERT( at_console, "No observer targets.\n" );
		Observer_SetMode( OBS_ROAMING );

	}
#ifdef HLE_DEBUG2 // DEBUG FILE DUMP ==============
	
		debug_file.OpenFile();
		debug_file << "Exit Observer_FindNextPlayer\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================*/
}


// Handle buttons in observer mode
void CBasePlayer::Observer_HandleButtons()
{
  // Slow down mouse clicks
   if ( m_flNextObserverInput > gpGlobals->time )
     return;

   // Jump changes from modes: Chase to Roaming
   if ( m_afButtonPressed & IN_JUMP )
   {
     if ( pev->iuser1 == OBS_CHASE_LOCKED )
       Observer_SetMode( OBS_CHASE_FREE );

     else if ( pev->iuser1 == OBS_CHASE_FREE )
       Observer_SetMode( OBS_ROAMING );

     else if ( pev->iuser1 == OBS_ROAMING )
       Observer_SetMode( OBS_IN_EYE );

     else if ( pev->iuser1 == OBS_IN_EYE )
       Observer_SetMode( OBS_MAP_FREE );

     else if ( pev->iuser1 == OBS_MAP_FREE )
       Observer_SetMode( OBS_MAP_CHASE );

    else
       Observer_SetMode( OBS_CHASE_LOCKED ); // don't use OBS_CHASE_LOCKED anymore

     m_flNextObserverInput = gpGlobals->time + 0.2;
   }

   // Attack moves to the next player
   if ( m_afButtonPressed & IN_ATTACK && pev->iuser1 != OBS_ROAMING )
   {
     Observer_FindNextPlayer( false );

     m_flNextObserverInput = gpGlobals->time + 0.2;
   }

   // Attack2 moves to the prev player
   else if ( m_afButtonPressed & IN_ATTACK2 && pev->iuser1 != OBS_ROAMING )
   {
     Observer_FindNextPlayer( true );

     m_flNextObserverInput = gpGlobals->time + 0.2;
   }
}



// Attempt to change the observer mode
void CBasePlayer::Observer_SetMode( int iMode )
{
	// Just abort if we're changing to the mode we're already in
    if ( iMode == pev->iuser1 )
		return;

    // is valid mode ?
    if ( iMode < OBS_CHASE_LOCKED || iMode > OBS_MAP_CHASE )
		iMode = OBS_IN_EYE; // now it is

   // if we are not romaing, we need a valid target to track
	if ( (iMode != OBS_ROAMING && iMode != OBS_MAP_FREE) && pev->iuser2 == NULL)
	{
		Observer_FindNextPlayer( false );
		//return; // test for recursion fix, this might cause problems
	}

	else if(iMode == OBS_ROAMING || iMode == OBS_MAP_FREE || iMode == OBS_NONE)
	{
		m_iHideHUD = (HIDEHUD_HEALTH | HIDEHUD_WEAPONS);
		MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, edict() );   // hide the radar is spectating
			WRITE_SHORT( entindex() );
			WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else
	{
		
		CBaseEntity *pEnt = UTIL_PlayerByIndex( pev->iuser2 );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			UpdateObserverHud(pPlayer);
		}

	}


   // if we didn't find a valid target switch to roaming
   if (m_hObserverTarget == NULL)
   {
     //ClientPrint( pev, HUD_PRINTCENTER, "#Spec_NoTarget" );
     iMode = OBS_ROAMING;
   }
   // set spectator mode
   pev->iuser1 = iMode;

   // set target if not roaming
   if (iMode == OBS_ROAMING || iMode == OBS_MAP_FREE)
     pev->iuser2 = 0;
   else
   {
	   //m_fInitHUD = TRUE;
		pev->iuser2 = ENTINDEX( m_hObserverTarget->edict() );
		m_iHideHUD &= ~HIDEHUD_HEALTH;
		m_iHideHUD &= ~HIDEHUD_WEAPONS;

   }
}

void CBasePlayer::UpdateObserverWeaponList()
{

}

void CBasePlayer::UpdateObserverHud(CBasePlayer *pTarget )
{
	if(!pTarget)
		return;
	int i = 0;
	m_iHideHUD &= ~HIDEHUD_HEALTH;
	m_iHideHUD &= ~HIDEHUD_WEAPONS;
	
	
	MESSAGE_BEGIN( MSG_ONE, gmsgHideWeapon, NULL, pev );
		WRITE_BYTE( pTarget->m_iHideHUD );
	MESSAGE_END();

	int iHealth = max( pTarget->pev->health, 0 );  // make sure that no negative health values are sent

	// send "health" update message
	MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, pev );
		WRITE_BYTE( iHealth );
	MESSAGE_END();

	if(pTarget->m_bHasFlag)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pev );
			WRITE_BYTE(FLAG_HAND);
			WRITE_BYTE(0);
			WRITE_BYTE(pTarget->m_nCtfFlagInHand);
		MESSAGE_END();

	/*	MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pev );
			WRITE_BYTE(FLAG_SPEC_HOLD);
			WRITE_BYTE(0);
			WRITE_BYTE(pTarget->m_nCtfFlagInHand);
		MESSAGE_END();*/
	}
	else
	{
		/*MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pev );
			WRITE_BYTE(FLAG_CAPTURED);
			WRITE_BYTE(0);
			WRITE_BYTE(pFlag->m_nFlagColor);
		MESSAGE_END();*/

		MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pev );
			WRITE_BYTE(FLAG_SPEC_DROP);
			WRITE_BYTE(0);
			WRITE_BYTE(pTarget->m_nCtfFlagInHand);
		MESSAGE_END();
	}


	ASSERT( gmsgBattery > 0 );
	// send "health" update message
	MESSAGE_BEGIN( MSG_ONE, gmsgBattery, NULL, pev );
		WRITE_SHORT((int)pTarget->pev->armorvalue);
	MESSAGE_END();

	CBasePlayerWeapon* pWpn = (CBasePlayerWeapon*)pTarget->m_pActiveItem;
	if(pWpn)
	{
		int state = 0;
		if ( pTarget->m_fOnTarget )
			state = WEAPON_IS_ONTARGET;
		else
			state = 1;
		MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
			WRITE_BYTE( state );
			WRITE_BYTE( pWpn->m_iId );
			WRITE_BYTE( pWpn->m_iClip );
		MESSAGE_END();
	}

	for(i=0; i < MAX_AMMO_SLOTS; i++)
	{
		ASSERT( pTarget->m_rgAmmo[i] >= 0 );
		ASSERT( pTarget->m_rgAmmo[i] < 255 );
			// send "Ammo" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pev );
			WRITE_BYTE( i );
			WRITE_BYTE( max( min( pTarget->m_rgAmmo[i], 254 ), 0 ) );  // clamp the value to one byte
		MESSAGE_END();
	}

}

