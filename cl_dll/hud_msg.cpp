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
//
//  hud_msg.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "vgui_teamfortressviewport.h"
#include "vgui_scorepanel.h"
#include "hle_maploader.h"
#include "locs.h"


#define MAX_CLIENTS 32

extern BEAM *pEgonBeam;
extern BEAM *pEgonBeam2;
extern char g_szWeight[];


extern "C" int g_TeamplayClient;
//extern "C" cvar_t * p_bunnyhop;

extern CNameList g_Crosshairs;
extern cvar_t* cl_crosshairs;
extern int g_CrosshairIndex;
extern float p_fVolumeTime;
int g_nKeepText = 0;

/// USER-DEFINED SERVER MESSAGE HANDLERS

int CHud :: MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ(pbuf, iSize );
	g_nKeepText = READ_BYTE();
	ASSERT( iSize == 0 );

	// clear all hud data
	HUDLIST *pList = m_pHudList;

	while ( pList )
	{
		if ( pList->p )
			pList->p->Reset();
		pList = pList->pNext;
	}

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_iConcussionEffect = 0;

	return 1;
}

void CAM_ToFirstPerson(void);

void CHud :: MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf )
{
	CAM_ToFirstPerson();
}


void CHud :: MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf )
{
	//p_bunnyhop = gEngfuncs.pfnGetCvarPointer("bunnyhop");
	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	//EMIT_SOUND_DYN ( ENT(pev), CHAN_BODY, "player/pl_ladder4.wav", 1, ATTN_NORM, 0, 70 );	break;

	while (pList)
	{
		if ( pList->p )
			pList->p->InitHUDData();
		pList = pList->pNext;
	}
	
	char szColors[32];
	*szColors=NULL;
	int top_color = (int)CVAR_GET_FLOAT( "topcolor" );
	int bottom_color = (int)CVAR_GET_FLOAT( "bottomcolor" );
	sprintf(szColors, "mycolors %i %i\n", top_color, bottom_color);
	
	ServerCmd( g_szWeight );
	ServerCmd( szColors );



	m_bWeightSent = true;
	// hle
	//Probably not a good place to put this.
}


int CHud :: MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	//int nGameMode;
	char szGameMode[32]; 
	char szVersion[32];	
	char szServerName[256];
	*szGameMode=NULL;
	*szVersion=NULL;
	*szServerName=NULL;
	
	//nGameMode = READ_BYTE();
	
	p_fVolumeTime = 0; //hack for winamp volume; i dont know where eles to put this

	sprintf(szGameMode, READ_STRING());
	sprintf(szVersion, READ_STRING());
	sprintf(szServerName, READ_STRING());
	g_TeamplayClient = READ_BYTE();

	
	gViewPort->m_pScoreBoard->InstallColumns(szGameMode, szVersion, szServerName);
//	gViewPort->UpdateOnPlayerInfo();
	gHUD.m_Locs.LoadClientLocs();
	return 1;
}


int CHud :: MsgFunc_Damage(const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	Vector	from;
	int		i;
	float	count;
	
	BEGIN_READ( pbuf, iSize );
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i=0 ; i<3 ; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return 1;
}

int CHud :: MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iConcussionEffect = READ_BYTE();
	if (m_iConcussionEffect)
		this->m_StatusIcons.EnableIcon("dmg_concuss",255,160,0);
	else
		this->m_StatusIcons.DisableIcon("dmg_concuss");
	return 1;
}

/*
int CHud::MsgFunc_Weight(const char *pszName,  int iSize, void *pbuf)
{
	//if(!m_bWeightSent)
	{
		ServerCmd( g_szWeight );
		m_bWeightSent = true;
	}
	return 1;
}

*/

extern BEAM *pEgonBeam;
extern BEAM *pEgonBeam2;


int CHud::MsgFunc_FixEgon(const char *pszName,  int iSize, void *pbuf)
{
	if(pEgonBeam)
	{
		pEgonBeam->die = 0.0;
		pEgonBeam = NULL;
	}
	if(pEgonBeam2)
	{
		pEgonBeam2->die = 0.0;
		pEgonBeam2 = NULL;
	}
	return 1;
}


int CHud::MsgFunc_ChangeXHair(const char *pszName,  int iSize, void *pbuf)
{
	// this function does not set the cvar_t
	BEGIN_READ( pbuf, iSize );
	char* pszCrosshairs = READ_STRING();
	int valve = 0;
	g_CrosshairIndex = -1;
	for(int j=0; j < g_Crosshairs.size() && j < MAX_CROSSHAIRS; j++)
	{
		if(!strcmp(pszCrosshairs, g_Crosshairs[j]->getName()))
			g_CrosshairIndex = j; // starting index, subject to change
		
		else if(!strcmp("HLE-VALVe", g_Crosshairs[j]->getName()))
			valve = j; 
		
	}	

	if(g_CrosshairIndex == -1)
		g_CrosshairIndex = valve; // in case we dont have em, use "valve"
	
	return 1;
}
