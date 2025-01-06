#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"


#define FADING_TIME 10.0f // fades to 0 in FADE_TIME seconds

extern cvar_t	*cl_showlock;//hle
extern cvar_t	*cl_radar;
int CHudTLms::g_TeamScores[6][6];

DECLARE_MESSAGE(m_Duel, Bonus )
DECLARE_MESSAGE(m_Tournament, FragsLeft )
DECLARE_MESSAGE(m_Status, HudStatus);
DECLARE_MESSAGE(m_Lms,MenLeft );
DECLARE_MESSAGE(m_Tlms, TeamScores );



int CHudDuel::Init()
{
	HOOK_MESSAGE(Bonus);
	m_nIntensity = 0;
	gHUD.AddHudElem( this );
	//m_fTimeFactor = 255.0f/(FADING_TIME*100.0f); // color change per 1 millisecond based on FADE_TIME seconds
	return 1;
}

int CHudDuel::VidInit()
{
	m_pSprite = NULL;
	m_hFlawless = LoadSprite("sprites/flawless.spr");
	m_hOwned = LoadSprite("sprites/owned.spr");
	m_hImmortal = LoadSprite("sprites/immortal.spr");
	m_hPunish = LoadSprite("sprites/punishment.spr");
	m_hLuck = LoadSprite("sprites/pureluck.spr");
	m_hAnninilation = LoadSprite("sprites/annihilation.spr");
	m_hSpree = LoadSprite("sprites/killingspree.spr");
	m_hDomination = LoadSprite("sprites/domination.spr");
	
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}


int CHudDuel::Draw(float flTime)
{
	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 

	// im gonna assume hl checks for -negative rgb valve if it dont its are teh ghey

	r += m_nIntensity;
	g += m_nIntensity;
	b += m_nIntensity;


	// This should show up at the upper middle

	// blueish color
	if(m_pSprite)
		SPR_Set(*m_pSprite, r, g, b );
	SPR_DrawAdditive( 0,  x, y, NULL);
	if(m_nIntensity==0)
	{
		m_nIntensity = -1;
	}
	m_nIntensity -= gHUD.m_flTimeDelta * HLE_FADE;
	if(m_nIntensity<-255)
	{	
		m_nIntensity = 0;
		m_iFlags &= ~HUD_ACTIVE; // fadded to 0
	}
	return 1;
}

int CHudDuel::MsgFunc_Bonus(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int nType = READ_BYTE();
	m_pSprite = NULL;
	y = (ScreenHeight/2) - (ScreenHeight/4);
	switch (nType)
	{
	case 0:
		m_pSprite = &m_hOwned;
		break;
	case 1:
		m_pSprite = &m_hFlawless;
		break;
	case 2:
		m_pSprite = &m_hImmortal;
		break;
	case 3:
		m_pSprite = &m_hPunish;
		break;
	case 4:
		m_pSprite = &m_hLuck;
		break;
	case 5:
		m_pSprite = &m_hAnninilation;
		break;
	case 6:
		m_pSprite = &m_hSpree;
		break;
	case 7:
		m_pSprite = &m_hDomination;
		break;
	case 8:
		y = (ScreenHeight/2) + (ScreenHeight*.75)/2;
		m_pSprite = &m_hOwned;
		break;
	case 9:
		y = (ScreenHeight/2) + (ScreenHeight*.75)/2;
		m_pSprite = &m_hFlawless;
		break;
	case 10:
		y = (ScreenHeight/2) + (ScreenHeight*.75)/2;
		m_pSprite = &m_hPunish;
		break;
	case 11:
		y = (ScreenHeight/2) + (ScreenHeight*.75)/2;
		m_pSprite = &m_hLuck;
		break;
	}

	
	x = (ScreenWidth/2)-(SPR_Width(*m_pSprite,0)/2);
	m_nIntensity = 0;
	m_iFlags |= HUD_ACTIVE;
	return 1;
}




//================
//CHudTournament
//================


int CHudTournament::Init()
{
	HOOK_MESSAGE(FragsLeft);
	m_nIntensity = 0;
	m_nFragsLeft=0;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudTournament::VidInit()
{
	m_hFragsLeft = LoadSprite("sprites/fragsleft.spr");
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}


int CHudTournament::Draw(float flTime)
{
	
	int r, g, b;
	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 

	// im gonna assume hl checks for -negative rgb valve if it dont its are teh ghey

	r += m_nIntensity;
	g += m_nIntensity;
	b += m_nIntensity;

	// This should show up at the upper part of the screen
	//y = (ScreenHeight/6);
	//x = (ScreenWidth/2)-(SPR_Width(m_hFragsLeft,0)/2);

	wrect_t rect;
	if(m_nFragsLeft==1)
	{
		rect.top=32;
		rect.bottom = 64;
	}
	else if(m_nFragsLeft==0)
	{
		rect.top=64;
		rect.bottom = 96;
	}
	else if(m_nFragsLeft==-1)
	{
		rect.top=96;
		rect.bottom = 128;
	}
	else if(m_nFragsLeft==-2)
	{
		rect.top=128;
		rect.bottom = 160;
	}
	else
	{
		rect.top=0;
		rect.bottom = 32;
	}
	rect.left=0;
	rect.right = 128;
	
	SPR_Set(m_hFragsLeft, r, g, b );
	SPR_DrawAdditive( 0,  x, y, &rect);

	if(m_nFragsLeft>0)
		gHUD.DrawHudNumberSmall(x-26, y+4, m_iFlags | DHN_2DIGITS, m_nFragsLeft, r, g, b);

	if(m_nIntensity==0)
	{
		m_nIntensity = -1;
	}

	m_nIntensity -= gHUD.m_flTimeDelta * HLE_FADE;
	if(m_nIntensity<-255)
	{	
		m_nIntensity = 0;
		m_iFlags &= ~HUD_ACTIVE; // fadded to 0
	}
	return 1;
}

int CHudTournament::MsgFunc_FragsLeft(const char *pszName, int iSize, void *pbuf)
{
	y = (ScreenHeight/6);
	x = (ScreenWidth/2)-(SPR_Width(m_hFragsLeft,0)/2);

	BEGIN_READ( pbuf, iSize );
	m_nFragsLeft = READ_SHORT();
	m_nIntensity = 0;
	m_iFlags |= HUD_ACTIVE;
	return 1;
}


//================
//CHudStatus
//================


int CHudStatus::Init( void )
{
	HOOK_MESSAGE(HudStatus);
	gHUD.AddHudElem( this );
	return 1;
}
int CHudStatus::VidInit( void )
{
	m_ShowMatch = true;
	m_ShowRemoved = true;
	m_matchInProgress = false;

	m_hsLocked.SetSprite("sprites/lock.spr");
	m_hsLocked.SetDest(XRES(320)- SPR_Width(*m_hsLocked.GetSprite(), 0) / 2, 5); // center, no clock in modes with this sprite


	m_hsRemoved.SetSprite("sprites/removed.spr");
	m_hsRemoved.SetDest(5, 5); // upperleft  (no radar in modes with this sprite


	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}
int CHudStatus::Draw(float flTime)
{
	int result = 0;

	if(m_ShowRemoved)
 		result += m_hsRemoved.Draw();
	if(m_ShowMatch)
		result += m_hsLocked.Draw();

	if(result == 0)
	{
		m_iFlags &= ~HUD_ACTIVE;
	}
	return 1;
}

void CHudStatus::ResetColors()
{
	m_hsRemoved.SetColor(&g_hud_color);
	m_hsLocked.SetColor(&g_hud_color);
}

int CHudStatus::MsgFunc_HudStatus(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int nEvent = READ_BYTE(); 
	m_matchInProgress = false;
	switch(nEvent)
	{
	case 0: 
		m_matchInProgress = true;
		m_hsLocked.ResetAnimation();
		break;
	case 1:
		m_hsRemoved.ResetAnimation();
		break;
	case 2: 
		m_hsLocked.StartFade(4);
		break;
	case 3: 
		m_hsRemoved.StartFade(4);
		break;
	}
	m_hsRemoved.SetColor(&g_hud_color);
	m_hsLocked.SetColor(&g_hud_color);

	m_iFlags |= HUD_ACTIVE;
	return 1;
}

void CHudStatus::ToggleLock()
{
	m_ShowMatch = !m_ShowMatch;
	m_iFlags |= HUD_ACTIVE;
}

void CHudStatus::ToggleBreak()
{
	m_ShowRemoved = !m_ShowRemoved;
	m_iFlags |= HUD_ACTIVE;
}


//=======================
// CHudTLms
//=======================

int CHudTLms::Init()
{
	HOOK_MESSAGE(TeamScores);
	//gHUD.AddHudElem( this );
	return 1;
}

int CHudTLms::VidInit()
{
	memset(g_TeamScores, 0, sizeof(g_TeamScores));
		
	//m_hMenLeft = LoadSprite("sprites/lms.spr");
	//m_iFlags &= ~HUD_ACTIVE;
	return 1;
}



int CHudTLms::MsgFunc_TeamScores(const char *pszName, int iSize, void *pbuf)
{

	BEGIN_READ( pbuf, iSize );

	int team = READ_BYTE();
	int wins = READ_BYTE();
	int losses = READ_BYTE();

	g_TeamScores[team][0] = wins;
	g_TeamScores[team][1] = losses;
	
	
	return 1;
}


//=======================
// CHudLms
//=======================





int CHudLms::Init()
{
	HOOK_MESSAGE(MenLeft);
	m_nIntensity = 0;
	m_nMenLeft=0;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudLms::VidInit()
{
	m_hMenLeft = LoadSprite("sprites/lms.spr");
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}



int CHudLms::Draw(float flTime)
{
	
	int r, g, b;
	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 

	// im gonna assume hl checks for -negative rgb valve if it dont its are teh ghey

	r += m_nIntensity;
	g += m_nIntensity;
	b += m_nIntensity;

	wrect_t rect;
	if(m_nMenLeft==1)
	{
		rect.top=32;
		rect.bottom = 64;
	}
/*	else if(m_nFragsLeft==0)
	{
		rect.top=64;
		rect.bottom = 96;
	}
	else if(m_nFragsLeft==-1)
	{
		rect.top=96;
		rect.bottom = 128;
	}
	else if(m_nFragsLeft==-2)
	{
		rect.top=128;
		rect.bottom = 160;
	}*/
	else
	{
		rect.top=0;
		rect.bottom = 32;
	}
	rect.left=0;
	rect.right = 256;
	
	SPR_Set(m_hMenLeft, r, g, b );
	SPR_DrawAdditive( 0,  x, y, &rect);

	if(m_nMenLeft>1)
		gHUD.DrawHudNumberSmall(x+30, y+7, m_iFlags | DHN_2DIGITS, m_nMenLeft, r, g, b);

	if(m_nIntensity==0)
	{
		m_nIntensity = -1;
	}

	m_nIntensity -= gHUD.m_flTimeDelta * HLE_FADE;
	if(m_nIntensity<-255)
	{	
		m_nIntensity = 0;
		m_iFlags &= ~HUD_ACTIVE; // fadded to 0
	}
	return 1;
}

int CHudLms::MsgFunc_MenLeft(const char *pszName, int iSize, void *pbuf)
{
	y = (ScreenHeight/6);
	x = (ScreenWidth/2)-(SPR_Width(m_hMenLeft,0)/2);

	BEGIN_READ( pbuf, iSize );
	m_nMenLeft = READ_SHORT();
	m_nIntensity = 0;
	m_iFlags |= HUD_ACTIVE;
	return 1;
}