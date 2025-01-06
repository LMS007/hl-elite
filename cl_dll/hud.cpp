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
// hud.cpp
//
// implementation of CHud class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "hud_servers.h"
#include "vgui_int.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hle_menu.h"
#include "demo.h"
#include "demo_api.h"
#include "vgui_scorepanel.h"

CHudColor g_hud_color;
CNameList g_Crosshairs;
int g_CrosshairIndex; // hle hack
extern int g_GaussColor;

/*
void CHudColor::loadColors(char* pszColor)
{
	char szTok[16];
	*szTok=NULL;
	strncpy(szTok,pszColor, 15);
	int h_index = atoi(strtok(szTok, " "));
	int v_index = atoi(strtok(NULL, " "));
	R = g_ColorTable[h_index][0];
	G = g_ColorTable[h_index][1];
	B = g_ColorTable[h_index][2];
	brightness = v_index;
	UpdateColors();
}*/

CHudColor::CHudColor()
{ 
	r = g = b = brightness = R = G = B= 0; 
}

CHudColor::CHudColor(int red, int green, int blue, int alpha)
{
	r = g = b = 0;
	this->R = red;
	this->G = green;
	this->B = blue;
	this->brightness = alpha;
	UpdateColors();
}

void CHudColor::UpdateColors()
{
	b = B + brightness;
	if(b > 255)		{ b = 255; }
	else if(b <0)	{ b=0;	   }
	
	r = R + brightness;
	if(r > 255)		{ r = 255; }
	else if(r <0)	{ r=0;	   }

	g = G + brightness; 
	if(g > 255)     { g = 255; }
	else if(g <0)   { g=0;     }
}

void CHudColor::setColors(int Hex)
{
	/*int client_r, client_g, client_b;
	UnpackRGB(client_r, client_g, client_b, Hex);
	r = client_r;
	g = client_g;
	b = client_b; */
}

int CHudColor::getBlue()
{
	return b;
}

int CHudColor::getGreen()
{
	return g;
}
int CHudColor::getRed()
{
	return r;
}

void CHudColor::setInverts(int* r, int* g, int *b)
{
	int *x = r; // largest
	int *y = g; // middle
	int *z = b; // smallest

	
	if(*y<*z)
	{
		int *t = z;
		z = y;
		y = t;
	}
	if(*x<*y)
	{
		int *t = x;
		x = y;
		y = t;
	}
	if(*y<*z)
	{
		int *t = z;
		z = y;
		y = t;
	}
	

	*y = 255 - *y;
	int t = *x;
	*x = *z;
	*z = t;
}


int CHudColor::getPureBlue()
{
	if(brightness>0)
		return b;
	else
		return B;
}

int CHudColor::getPureGreen()
{
	if(brightness>0)
		return g;
	else
		return G;
}
int CHudColor::getPureRed()
{
	if(brightness>0)
		return r;
	else
		return R;
}

void CHudColor::setBlue( int blue)
{
	B = blue;
	UpdateColors();
}

void CHudColor::setGreen( int green)
{
	G = green;
	UpdateColors();
}
void CHudColor::setRed( int red)
{
	R = red;
	UpdateColors();
}

void CHudColor::setBrightness(int Brightness)
{
	brightness = Brightness;
	UpdateColors();
	
}


int CHudColor::maxColor(int r, int g, int b)
{
	int max = 0;
	if(r > max)	max = r;
	if(g > max)	max = g;		
	if(b > max)	max = b;
	return max;
}

int CHudColor::minColor(int r, int g, int b)
{
	int min = 255;
	if(r < min)	min = r;
	if(g < min)	min = g;		
	if(b < min)	min = b;
	return min;
}
/*
void CHudColor::RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = minColor( r, g, b );
	max = maxColor( r, g, b );
	*v = max;				// v
	delta = max - min;
	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
}*/
// newer one
void CHudColor::RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
 { 

    float mn=r,mx=r; 
    int maxVal=0; 
  
    if (g > mx){ mx=g;maxVal=1;} 
    if (b > mx){ mx=b;maxVal=2;}  
    if (g < mn) mn=g; 
    if (b < mn) mn=b;  

    float  delta = mx - mn; 
  
	*v = (g + r + b)/3;
    if( mx != 0 ) 
      *s = delta / mx;  
    else  
    { 
      *s = 0; 
      *h = 0; 
      return; 
    } 
    if (*s==0.0f) 
    { 
      *h=-1; 
      return; 
    } 
    else 
    {  
      switch (maxVal) 
      { 
      case 0:{*h = ( g - b ) / delta;break;}         // yel < h < mag 
      case 1:{*h = 2 + ( b - r ) / delta;break;}     // cyan < h < yel 
      case 2:{*h = 4 + ( r - g ) / delta;break;}     // mag < h < cyan 
      } 
    } 
  
    *h *= 60; 
    if( *h < 0 ) *h += 360; 
	*h = (*h/360)*255;
}

void CHudColor::HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}


class CHLVoiceStatusHelper : public IVoiceStatusHelper
{
public:
	virtual void GetPlayerTextColor(int entindex, int color[3])
	{
		color[0] = color[1] = color[2] = 255;

		if( entindex >= 0 && entindex < sizeof(g_PlayerExtraInfo)/sizeof(g_PlayerExtraInfo[0]) )
		{
			int iTeam = g_PlayerExtraInfo[entindex].teamnumber;

			if ( iTeam < 0 )
			{
				iTeam = 0;
			}

			iTeam = iTeam % iNumberOfTeamColors;

			color[0] = iTeamColors[iTeam][0];
			color[1] = iTeamColors[iTeam][1];
			color[2] = iTeamColors[iTeam][2];
		}
	}

	virtual void UpdateCursorState()
	{
		gViewPort->UpdateCursorState(); // bad kyle!
	}

	virtual int	GetAckIconHeight()
	{
		return ScreenHeight - gHUD.m_iFontHeight*3 - 6;
	}

	virtual bool			CanShowSpeakerLabels()
	{
		if( gViewPort && gViewPort->m_pScoreBoard )
			return !gViewPort->m_pScoreBoard->isVisible();
		else
			return false;
	}
};
static CHLVoiceStatusHelper g_VoiceStatusHelper;


extern client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

extern cvar_t *sensitivity;
cvar_t *cl_lw = NULL;

void ShutdownInput (void);

//DECLARE_MESSAGE(m_Logo, Logo)

// REMOVED BY 007 bcaeuse we need message space
/*int __MsgFunc_Logo(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Logo(pszName, iSize, pbuf );
}*/

//DECLARE_MESSAGE(m_Logo, Logo)
int __MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_ResetHUD(pszName, iSize, pbuf );
}

int __MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_InitHUD( pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_ViewMode(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_ViewMode( pszName, iSize, pbuf );
	return 1;
}


int __MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_SetFOV( pszName, iSize, pbuf );
}
    /**          
int __MsgFunc_Weight(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Weight( pszName, iSize, pbuf );
}
*/
int __MsgFunc_FixEgon(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_FixEgon( pszName, iSize, pbuf );
}
int __MsgFunc_ChangeXHair( const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_ChangeXHair( pszName, iSize, pbuf );
}
int __MsgFunc_Concuss(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Concuss( pszName, iSize, pbuf );
}

int __MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_GameMode( pszName, iSize, pbuf );
}




// TFFree Command Menu
void __CmdFunc_OpenCommandMenu(void)
{
	if ( gViewPort )
	{
		gViewPort->ShowCommandMenu( gViewPort->m_StandardMenu );
	}
}

// TFC "special" command
void __CmdFunc_InputPlayerSpecial(void)
{
	if ( gViewPort )
	{
		gViewPort->InputPlayerSpecial();
	}
}

void __CmdFunc_CloseCommandMenu(void)
{
	if ( gViewPort )
	{
		gViewPort->InputSignalHideCommandMenu();
	}
}

void __CmdFunc_ForceCloseCommandMenu( void )
{
	if ( gViewPort )
	{
		gViewPort->HideCommandMenu();
	}
}

void __CmdFunc_ToggleServerBrowser( void )
{
	if ( gViewPort )
	{
		gViewPort->ToggleServerBrowser();
	}
}

// TFFree Command Menu Message Handlers
int __MsgFunc_ValClass(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ValClass( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamNames(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamNames( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_Feign(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_Feign( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_Detpack(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_Detpack( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_VGUIMenu(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_VGUIMenu( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_MOTD( pszName, iSize, pbuf );
	return 0;
}


int __MsgFunc_BuildSt(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_BuildSt( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_RandomPC(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_RandomPC( pszName, iSize, pbuf );
	return 0;
}
 
int __MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ServerName( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_IconInfo( const char *pszName, int iSize, void *pbuf )
{
	if (gViewPort)
		return gViewPort->MsgFunc_IconInfo( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ScoreInfo( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamScore( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_PlayerID(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_PlayerID( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamInfo( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamOrder(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamOrder( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_AllowSpec(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_AllowSpec( pszName, iSize, pbuf );
	return 0;
}

 
// This is called every time the DLL is loaded
void CHud :: Init( void )
{
	//HOOK_MESSAGE( Logo ); removed by 007 for msg space
	HOOK_MESSAGE( ResetHUD );
	HOOK_MESSAGE( GameMode );
	HOOK_MESSAGE( InitHUD );
	HOOK_MESSAGE( SetFOV );
	HOOK_MESSAGE( Concuss );
	HOOK_MESSAGE( FixEgon );
	HOOK_MESSAGE( ChangeXHair );
//	HOOK_MESSAGE( Weight );
	m_bWeightSent = false;

	// TFFree CommandMenu
	HOOK_COMMAND( "+commandmenu", OpenCommandMenu );
	HOOK_COMMAND( "-commandmenu", CloseCommandMenu );
	HOOK_COMMAND( "ForceCloseCommandMenu", ForceCloseCommandMenu );
	HOOK_COMMAND( "special", InputPlayerSpecial );
	HOOK_COMMAND( "togglebrowser", ToggleServerBrowser );

	HOOK_MESSAGE( ViewMode );
	HOOK_MESSAGE( ValClass );
	HOOK_MESSAGE( TeamNames );
	HOOK_MESSAGE( Feign );
	HOOK_MESSAGE( Detpack );
	HOOK_MESSAGE( MOTD );
	HOOK_MESSAGE( BuildSt );
	HOOK_MESSAGE( RandomPC );
	HOOK_MESSAGE( ServerName );
	HOOK_MESSAGE( ScoreInfo );
	HOOK_MESSAGE( IconInfo);
	HOOK_MESSAGE( TeamScore );
	HOOK_MESSAGE( PlayerID );
	HOOK_MESSAGE( TeamInfo );
	HOOK_MESSAGE( TeamOrder );
	HOOK_MESSAGE( AllowSpec );


	// VGUI Menus
	HOOK_MESSAGE( VGUIMenu );

	CVAR_CREATE( "hud_classautokill", "1", FCVAR_ARCHIVE | FCVAR_USERINFO );		// controls whether or not to suicide immediately on TF class switch
	CVAR_CREATE( "hud_takesshots", "0", FCVAR_ARCHIVE );		// controls whether or not to automatically take screenshots at the end of a round


	m_iLogo = 0;
	m_iFOV = 0;

	CVAR_CREATE( "zoom_sensitivity_ratio", "1.2", 0 );
	default_fov = CVAR_CREATE( "default_fov", "90", FCVAR_ARCHIVE | FCVAR_USERINFO );
	m_pCvarStealMouse = CVAR_CREATE( "hud_capturemouse", "1", FCVAR_ARCHIVE );
	m_pCvarDraw = CVAR_CREATE( "hud_draw", "1", FCVAR_ARCHIVE );

	cl_lw = gEngfuncs.pfnGetCvarPointer( "cl_lw" );

	m_pSpriteList = NULL;

	// Clear any old HUD list
	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;

	m_Ammo.Init();
	m_Health.Init();
	m_SayText.Init();
	m_Spectator.Init();
	m_Geiger.Init();
	m_Train.Init();
	m_Battery.Init();
	m_Flash.Init();
	m_Message.Init();
	m_StatusBar.Init();
	m_DeathNotice.Init();
	m_AmmoSecondary.Init();
	m_TextMessage.Init();
	m_StatusIcons.Init();
	GetClientVoiceMgr()->Init(&g_VoiceStatusHelper, (vgui::Panel**)&gViewPort);
    m_MapList.Init(); // hle
	m_Count.Init(); // hle
	m_Count2.Init();
	m_Color.Init();
	m_Duel.Init();
	m_Status.Init();
	m_Tournament.Init();
	m_Menu.Init();
	m_Ctf.Init();
	m_Lms.Init();
	m_Tlms.Init();
	m_Cap.Init();
	m_Ctp.Init();
	m_Flag.Init();
	m_Clock.Init();
	m_Timer.Init();
	m_Locs.Init();
	m_Radar.Init();
	m_Winamp.Init();	
	m_Anounce.Init();
	ServersInit();

	MsgFunc_ResetHUD(0, 0, NULL );
}

// CHud destructor
// cleans up memory allocated for m_rg* arrays
CHud :: ~CHud()
{
	delete [] m_rghSprites;
	delete [] m_rgrcRects;
	delete [] m_rgszSpriteNames;

	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	ServersShutdown();
}

// GetSpriteIndex()
// searches through the sprite list loaded from hud.txt for a name matching SpriteName
// returns an index into the gHUD.m_rghSprites[] array
// returns 0 if sprite not found
int CHud :: GetSpriteIndex( const char *SpriteName )
{
	// look through the loaded sprite name list for SpriteName
	char* pszBuf;
	for ( int i = 0; i < m_iSpriteCount; i++ )
	{
		pszBuf = m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH);
		if ( strncmp( SpriteName, pszBuf , MAX_SPRITE_NAME_LENGTH ) == 0 )
			return i;
	}

	return -1; // invalid sprite
}

void CHud :: VidInit( void )
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	// ----------
	// Load Sprites
	// ---------
//	m_hsprFont = LoadSprite("sprites/%d_font.spr");
	
	m_hsprLogo = 0;	
	m_hsprCursor = 0;

	if (ScreenWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	// Only load this once
	if ( !m_pSpriteList )
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			int j;
			for (j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
					m_iSpriteCount++;
				p++;
			}

			// allocated memory for sprite handle arrays
 			m_rghSprites = new SpriteHandle_t[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for ( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
				{
					char sz[256];
					sprintf(sz, "sprites/%s.spr", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					strncpy( &m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH );

					index++;
				}

				p++;
			}
		}
	}
	else
	{
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for ( int j = 0; j < m_iSpriteCountAllRes; j++ )
		{
			if ( p->iRes == m_iRes )
			{
				char sz[256];
				sprintf( sz, "sprites/%s.spr", p->szSprite );
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}

			p++;
		}
	}

	// assumption: number_1, number_2, etc, are all listed and loaded sequentially
	m_HUD_number_0 = GetSpriteIndex( "number_0" );

	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;

	m_Ammo.VidInit();
	m_Health.VidInit();
	m_Spectator.VidInit();
	m_Geiger.VidInit();
	m_Train.VidInit();
	m_Battery.VidInit();
	m_Flash.VidInit();
	m_Message.VidInit();
	m_StatusBar.VidInit();
	m_DeathNotice.VidInit();
	m_SayText.VidInit();
	m_Menu.VidInit();
	m_AmmoSecondary.VidInit();
	m_TextMessage.VidInit();
	m_StatusIcons.VidInit();
	GetClientVoiceMgr()->VidInit();
	m_Spectator.VidInit();
    m_MapList.VidInit(); // hle
	m_Count.VidInit();
	m_Count2.VidInit();
	m_Duel.VidInit();
	m_Status.VidInit();
	m_Tournament.VidInit();
	m_Ctf.VidInit();
	m_Lms.VidInit();
	m_Tlms.VidInit();
	m_Cap.VidInit();
	m_Ctp.VidInit();
	m_Color.VidInit();
	m_Flag.VidInit();
	m_Clock.VidInit();
	m_Anounce.VidInit();
	m_Timer.VidInit();
	//m_Locs.VidInit();
	m_Radar.VidInit();
	m_Winamp.VidInit();
}

// REMOVED BY 007 bcaeuse we need message space
/*int CHud::MsgFunc_Logo(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iLogo = READ_BYTE();

	return 1;
}
*/
float g_lastFOV = 0.0;

/*
============
COM_FileBase
============
*/
// Extracts the base name of a file (no path, no extension, assumes '/' as path separator)
void COM_FileBase ( const char *in, char *out)
{
	int len, start, end;

	len = strlen( in );
	
	// scan backward for '.'
	end = len - 1;
	while ( end && in[end] != '.' && in[end] != '/' && in[end] != '\\' )
		end--;
	
	if ( in[end] != '.' )		// no '.', copy to end
		end = len-1;
	else 
		end--;					// Found ',', copy to left of '.'


	// Scan backward for '/'
	start = len-1;
	while ( start >= 0 && in[start] != '/' && in[start] != '\\' )
		start--;

	if ( in[start] != '/' && in[start] != '\\' )
		start = 0;
	else 
		start++;

	// Length of new sting
	len = end - start + 1;

	// Copy partial string
	strncpy( out, &in[start], len );
	// Terminate it
	out[len] = 0;
}

/*
=================
HUD_IsGame

=================
*/
int HUD_IsGame( const char *game )
{
	const char *gamedir;
	char gd[ 1024 ];

	gamedir = gEngfuncs.pfnGetGameDirectory();
	if ( gamedir && gamedir[0] )
	{
		COM_FileBase( gamedir, gd );
		if ( !stricmp( gd, game ) )
			return 1;
	}
	return 0;
}

/*
=====================
HUD_GetFOV

Returns last FOV
=====================
*/
float HUD_GetFOV( void )
{
	if ( gEngfuncs.pDemoAPI->IsRecording() )
	{
		// Write it
		int i = 0;
		unsigned char buf[ 100 ];

		// Active
		*( float * )&buf[ i ] = g_lastFOV;
		i += sizeof( float );

		Demo_WriteBuffer( TYPE_ZOOM, i, buf );
	}

	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
	{
		g_lastFOV = g_demozoom;
	}
	return g_lastFOV;
}


int CHud::MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int newfov = READ_BYTE();
	int def_fov = CVAR_GET_FLOAT( "default_fov" );

	//Weapon prediction already takes care of changing the fov. ( g_lastFOV ).
	if ( cl_lw && cl_lw->value )
		return 1;

	g_lastFOV = newfov;

	if ( newfov == 0 )
	{
		m_iFOV = def_fov;
	}
	else
	{
		m_iFOV = newfov;
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == def_fov )
	{  
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{  
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * ((float)newfov / (float)def_fov) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	return 1;
}


void CHud::AddHudElem(CHudBase *phudelem)
{
	HUDLIST *pdl, *ptemp;

//phudelem->Think();

	if (!phudelem)
		return;

	pdl = (HUDLIST *)malloc(sizeof(HUDLIST));
	if (!pdl)
		return;

	memset(pdl, 0, sizeof(HUDLIST));
	pdl->p = phudelem;

	if (!m_pHudList)
	{
		m_pHudList = pdl;
		return;
	}

	ptemp = m_pHudList;

	while (ptemp->pNext)
		ptemp = ptemp->pNext;

	ptemp->pNext = pdl;
}

float CHud::GetSensitivity( void )
{
	return m_flMouseSensitivity;
}

///////////////////////////////

#define _FADING_TIME 5.0f // fades to 0 in FADE_TIME seconds

DECLARE_MESSAGE(m_Color, Color)
//DECLARE_MESSAGE(m_Color, HudColor)
//DECLARE_MESSAGE(m_Color, GaussColor)
DECLARE_MESSAGE(m_Color, Reset)

extern cvar_t* cl_gausscolor;


void CHudHleColors::InitHUDData()
{	
//m_nOriginalHudColor = cl_hudcolor->value;
CHudBase::CHudBase();
}


int CHudHleColors::Init()
{
	
	//HOOK_MESSAGE(GaussColor);
	//HOOK_MESSAGE(HudColor);
	HOOK_MESSAGE(Color);
	HOOK_MESSAGE(Reset);

	m_iFlags &= ~HUD_ACTIVE;
	//gHUD.AddHudElem( this );
	return 1;
}

int CHudHleColors::VidInit()
{
	
//	m_nOriginalHudColor = cl_hudcolor->value;
	m_nOriginalGaussColor = cl_gausscolor->value;
	return 1;
}


int CHudHleColors::Draw(float flTime)
{
	return 1;
}
/*
int CHudHleColors::MsgFunc_GaussColor(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int nColor = READ_BYTE();

	if(nColor > 9)
		g_GaussColor= 9;
	
	else if(nColor <0)
		g_GaussColor = 0;
	else
		g_GaussColor = nColor;

	return 1;
}
*/
extern cvar_t* cl_hudhue;
extern cvar_t* cl_hudvalue;

int CHudHleColors::MsgFunc_Reset(const char *pszName, int iSize, void *pbuf)
{
//	if(!strcmp(cl_gausscolor->string, "lms"))
//		g_GaussColor = 9;
//	else
		g_GaussColor = cl_gausscolor->value;
	//gEngfuncs.pfnClientCmd("exec config.cfg\n");
	//cl_gausscolor
	g_hud_color.setRed(0);
	g_hud_color.setGreen(0);
	g_hud_color.setBlue(0);
	g_hud_color.setBrightness(0);
	int hue = cl_hudhue->value*6;
	int value = cl_hudvalue->value*MAXCOLORS;
	int nSensitivityPos = (sensitivity->value*SENSITIVITY_SCALE);
	int nGaussColor = (cl_gausscolor->value*(MAXCOLORS/8));

	if(hue >= MAXCOLORS)
		hue = MAXCOLORS-1;
	if(hue < 0)
		hue = 0;
	if(value >= MAXCOLORS)
		value = MAXCOLORS-1;
	if(value < 0)
		value = 0;

	g_hud_color.setRed(g_ColorTable[hue][0]);
	g_hud_color.setBlue(g_ColorTable[hue][1]);
	g_hud_color.setGreen(g_ColorTable[hue][2]);
	
	int midpoint = MAXCOLORS / 2;
	if(value > midpoint)	{ value = (value - midpoint)/3; }
	else { value = (midpoint -  value )/-3; }
	g_hud_color.setBrightness(value);

	for(int j=0; j < g_Crosshairs.size() && j < MAX_CROSSHAIRS; j++)
	{
		if(!strcmp(cl_crosshairs->string, g_Crosshairs[j]->getName()))
		{
			g_CrosshairIndex = j; // starting index, subject to change
		}
	}
	gHUD.m_Ammo.ChangeXhairColor();
	return 1;
}

int CHudHleColors::MsgFunc_Color(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int nHue = READ_BYTE();
	int nValue = READ_BYTE();
	float fTempValue = (float)nValue - 50.0f;
	fTempValue*=5.1f;
	nValue = (int)fTempValue;
	

	// we DONT set the cvar_t's, thats the trick!
	g_hud_color.setRed(g_ColorTable[nHue*6][0]);
	g_hud_color.setBlue(g_ColorTable[nHue*6][1]);
	g_hud_color.setGreen(g_ColorTable[nHue*6][2]);
	g_hud_color.setBrightness(nValue);
	gHUD.m_Status.ResetColors();
	gHUD.m_Ammo.ChangeXhairColor();

	//BEGIN_READ( pbuf, iSize );
	int nColor = READ_BYTE();
	if(nColor==-1)
		return 1;

	if(nColor > 9)
		g_GaussColor= 9;
	
	else if(nColor <0)
		g_GaussColor = 0;
	else
		g_GaussColor = nColor;

	return 1;
}



CHudSprite::CHudSprite()
{
	intensity = -255;
	time = 0;
	hold = 0;
	active = false;
	render_method = 0;
}

void CHudSprite::SetRenderMethod(int nMethod)
{
	render_method = nMethod;
}

void CHudSprite::SetSprite(char* pSprite)
{
	sprite = LoadSprite(pSprite); 
	SetSource(0, 0, SPR_Width(sprite, 0), SPR_Height(sprite, 0));
	active = false;

}

void CHudSprite::SetDest(int x, int y)
{
	this->x = x;
	this->y = y;
}

void CHudSprite::SetSource(int left, int top, int right, int bottom)
{
	source.left = left;
	source.right = right;
	source.top = top;
	source.bottom=bottom;
}

void CHudSprite::GetColor(CHudColor* color)
{
	color->setBlue(b);
	color->setGreen(g);
	color->setRed(r);
}
void CHudSprite::GetDest(int* x, int*y)
{
	*x = this->x;
	*y = this->y;
}

void CHudSprite::StartFade(float speed, float hold )
{
	intensity -=1;
	time = gHUD.m_flTime;
	this->speed = speed;
	this->hold = hold+gHUD.m_flTime;
}

void CHudSprite::ResetAnimation()
{
	active = true;
	intensity = 0;
}

void CHudSprite::SetColor(CHudColor* color)
{
	this->r = color->getRed();
	this->g = color->getGreen();
	this->b = color->getBlue();
}

void CHudSprite::SetActive(bool active)
{
	this->active = active;
}
SpriteHandle_t* CHudSprite::GetSprite()
{
	return &sprite; 
}

int CHudSprite::Draw()
{
	if(active)
	{
		if(gHUD.m_flTime>hold)
		{
			if(intensity < 0)
			{		
				if(intensity<=-255) // crash BUG
				{	
					intensity = -255;
					active = false;
					return 0;
				}
				intensity -= gHUD.m_flTimeDelta * (float)(speed*50);
			}
		}
	
		SPR_Set(sprite, r+intensity, g+intensity, b+intensity );

		if(render_method==1)
			SPR_DrawHoles( 0,  x, y, &source);
		else if(render_method==2)
			SPR_Draw( 0,  x, y, &source);
		else
			SPR_DrawAdditive( 0,  x, y, &source);
		return 1;
	}
	return 0;
}