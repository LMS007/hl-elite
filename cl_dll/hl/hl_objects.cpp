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
#include "../hud.h"
#include "../cl_util.h"
#include "../demo.h"

#include "demo_api.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"

#include "pm_defs.h"
#include "event_api.h"
#include "entity_types.h"
#include "r_efx.h"
#include "pm_shared.h" // hle

extern void VectorAngles( const float *forward, float *angles );

extern BEAM *pEgonBeam;
extern BEAM *pEgonBeam2;

void HUD_GetLastOrg( float *org );

void UpdateBeams ( void )
{
	vec3_t forward, vecSrc, vecEnd, origin, angles, right, up;
	vec3_t view_ofs, vecDest;
	pmtrace_t tr;
	cl_entity_t *pthisplayer = gEngfuncs.GetLocalPlayer();
	int idx = pthisplayer->index;
		
	bool bIsEyeObserver = false;
	if(g_iUser1 == OBS_IN_EYE || (g_iUser1 && (gHUD.m_Spectator.m_pip->value == INSET_IN_EYE)) )
	{
		cl_entity_t	 * ent = gEngfuncs.GetEntityByIndex( g_iUser2 );
		VectorCopy ( ent->angles, angles );
		VectorCopy ( ent->origin, origin );
		angles[0]*=-3; // hack
		bIsEyeObserver = true;
		
	}
	
	else 
	{
		gEngfuncs.GetViewAngles( (float *)angles ); // problem hle
		HUD_GetLastOrg( (float *)&origin );
	}

	AngleVectors( angles, forward, right, up );	
	VectorCopy( origin, vecSrc );

	VectorMA( vecSrc, 2048, forward, vecEnd );

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );	
						
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
					
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

	gEngfuncs.pEventAPI->EV_PopPMStates();

	if(bIsEyeObserver)
		vecDest = vecEnd;
	else
		vecDest = tr.endpos;
	
	if ( pEgonBeam )
	{
		//pEgonBeam->width = 3.5;
		//pEgonBeam->amplitude = 0.2;
		pEgonBeam->target = vecDest;
		pEgonBeam->die	  = gEngfuncs.GetClientTime() + 0.1; // We keep it alive just a little bit forward in the future, just in case.
	}
		
	if ( pEgonBeam2 )
	{
		//pEgonBeam2->width = 3.5;
		//pEgonBeam2->amplitude = 0.2;
		pEgonBeam2->target = vecDest;
		pEgonBeam2->die	   = gEngfuncs.GetClientTime() + 0.1; // We keep it alive just a little bit forward in the future, just in case.
	}
}

/*
=====================
Game_AddObjects

Add game specific, client-side objects here
=====================
*/
void Game_AddObjects( void )
{
	if ( pEgonBeam && pEgonBeam2 )
		UpdateBeams();
}
