#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"

extern TeamFortressViewport *gViewPort;

int CHudCtp::g_CtpScores[6];


enum
{
	BLUE=0,
	RED,
	GREEN,
	YELLOW,
	PURPLE,
	ORANGE,
};

DECLARE_MESSAGE(m_Ctp, CtpEvent )
DECLARE_MESSAGE(m_Ctp, CtpTime )

int CHudCtp::Init( void )
{ 
	HOOK_MESSAGE(CtpTime);
	HOOK_MESSAGE(CtpEvent);
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
	
}

int CHudCtp::VidInit( void )
{
	m_nTotalHeight = XRES(200);
	m_nWidth = YRES(5);
	m_nOldTime = 0;	
	m_nTotalFlags = 0;

	//m_hsFlag.SetSprite("sprites/flag_mini.spr");
	m_hsFlag = LoadSprite("sprites/flag_mini2.spr"); 
	m_hX = LoadSprite("sprites/x.spr"); 
	
	//x = (ScreenWidth)*0.35f/* - (SPR_Width(*m_hsFlag.GetSprite(),0))*3;*/;
	x = XRES(240);
	//y = YRES(480)-(SPR_Height(m_hsFlag,0));
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	//m_hsFlag.SetDest( x, y);
	

	nMyScore = 0;
	for(int i =0;i<6;i++)
	{
		g_CtpScores[i]=0;
		m_nFlags[i]=0;
		m_nHeight[i]=0;
		m_nTeam[i]=i;
	}

	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudCtp::Draw(float flTime)
{
	if (gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)))
	{
		if (! (gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly() )
		{

			int b = g_hud_color.getBlue();
			int r = g_hud_color.getRed();
			int g = g_hud_color.getGreen();
			int a = 255;

			if (m_fFade)
			{
				m_fFade -= (gHUD.m_flTimeDelta * 20);
				if (m_fFade <= 0)
				{
					a = MIN_ALPHA;
					m_fFade = 0;
				}
				// Fade the health number back to dim
				a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;
			}
			else
				a = MIN_ALPHA;

			// If health is getting low, make it bright red
				
			//GetPainColor( r, g, b );
			ScaleColors(r, g, b, a );
			gHUD.DrawHudNumber(x+35, y, m_iFlags | DHN_2DIGITS, nMyScore, r, g, b);

			SPR_Set(m_hX, r, g, b );
			SPR_DrawAdditive( 0,  x+35, y+5, NULL);

			SPR_Set(m_hsFlag, r, g, b );
			SPR_DrawAdditive( 0,  x, y, NULL);
		}
	}

	int result = 0;
	int nStart = (YRES(220));

	for(int i = 0; i < 6; i++)
	{
		switch(m_nTeam[i])
		{
		case BLUE:
			FillRGBA(10, nStart, m_nWidth, m_nHeight[i], 0, 0, 255, 150);
			break;
		case RED:
			FillRGBA(10, nStart, m_nWidth, m_nHeight[i], 255, 0, 0, 150);
			break;
		case YELLOW:
			FillRGBA(10, nStart, m_nWidth, m_nHeight[i], 255, 255, 0, 150);
			break;
		case GREEN:	
			FillRGBA(10, nStart, m_nWidth, m_nHeight[i], 0, 255, 0, 150);			
			break;
		case PURPLE:
			FillRGBA(10, nStart, m_nWidth, m_nHeight[i], 255, 0, 255, 150);
			break;
		case ORANGE:	
			FillRGBA(10, nStart, m_nWidth, m_nHeight[i], 255, 100, 0, 150);			
			break;
		}
		nStart += m_nHeight[i];
	}
	UpdateScores();
	return 1;
}

void CHudCtp::CalcRecs()
{	int i;
	for(i =0;i<6;i++)
		m_nTeam[i]=i;

	m_nHeight[RED] = (int)(((float)m_nFlags[RED]/(float)m_nTotalFlags)*m_nTotalHeight);
	m_nHeight[BLUE] = (int)(((float)m_nFlags[BLUE]/(float)m_nTotalFlags)*m_nTotalHeight);
	m_nHeight[GREEN] = (int)(((float)m_nFlags[GREEN]/(float)m_nTotalFlags)*m_nTotalHeight);
	m_nHeight[YELLOW] = (int)(((float)m_nFlags[YELLOW]/(float)m_nTotalFlags)*m_nTotalHeight);
	m_nHeight[PURPLE] = (int)(((float)m_nFlags[PURPLE]/(float)m_nTotalFlags)*m_nTotalHeight);
	m_nHeight[ORANGE] = (int)(((float)m_nFlags[ORANGE]/(float)m_nTotalFlags)*m_nTotalHeight);

	for ( i=0; i<6-1; i++) 
	{
		for (int j=0; j<6-1-i; j++)
		{
			if (m_nHeight[j+1] < m_nHeight[j]) 
			{ 
				int height = m_nHeight[j];   
				m_nHeight[j] = m_nHeight[j+1];
				m_nHeight[j+1] = height;

				int team = m_nTeam[j];   
				m_nTeam[j] = m_nTeam[j+1];
				m_nTeam[j+1] = team;
			}
		}
	}
}

void CHudCtp::UpdateScores()
{
	if(m_nOldTime + 1 <= gHUD.m_flTime)
	{
		for(int i = 0; i < 6; i++)
			g_CtpScores[i]+=m_nFlags[i];
		m_nOldTime = gHUD.m_flTime;
	}
}


int CHudCtp::MsgFunc_CtpEvent(const char *pszName, int iSize, void *pbuf)
{
	int Old = nMyScore;

	BEGIN_READ( pbuf, iSize );
	int nResult  = 0;
	nResult += m_nFlags[BLUE] = READ_BYTE(); 
	nResult += m_nFlags[RED] = READ_BYTE();  
	nResult += m_nFlags[GREEN] = READ_BYTE(); 
	nResult += m_nFlags[YELLOW] = READ_BYTE();
	nResult += m_nFlags[PURPLE] = READ_BYTE(); 
	nResult += m_nFlags[ORANGE] = READ_BYTE(); 

	int nMyTeam = READ_BYTE();
	
	if(nMyTeam == 0)
		nMyScore = 0;
	else
		nMyScore = m_nFlags[nMyTeam-1];

	nResult += m_nTotalFlags = 0;

	if(nMyScore != Old)
	{
		m_fFade = FADE_TIME;
	}
	
	m_nOldTime = gHUD.m_flTime;

	for(int i = 0; i < 6; i++)
		m_nTotalFlags += m_nFlags[i];

	//CalcRecs();  // this was the ctp bar, removed because radar is better
	if(nResult == 0)
	{
		for(int i = 0; i < 6; i++)
			g_CtpScores[i]=0;
		m_iFlags &= ~HUD_ACTIVE;
	}
	else
	{
	///	m_hsFlag.SetColor(&g_hud_color);
	//	m_hsFlag.ResetAnimation();
		m_iFlags |= HUD_ACTIVE;
	}
	return 1;
}


int CHudCtp::MsgFunc_CtpTime(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int nTime = READ_SHORT(); 
	int nTeam = READ_BYTE(); 
	
	g_CtpScores[nTeam-1] = nTime;
	m_nOldTime = gHUD.m_flTime;
	gViewPort->UpdateOnPlayerInfo();
	m_iFlags |= HUD_ACTIVE;
	return 1;
}




DECLARE_MESSAGE(m_Cap, Cap )

int CHudCap::Init( void )
{
	HOOK_MESSAGE(Cap);
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
	
}

int CHudCap::VidInit( void )
{
	m_plus = LoadSprite("sprites/plus.spr");

	m_bFade = m_bActive = false;
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudCap::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;
	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	r = g_hud_color.getRed()+m_fIntensity;
	g = g_hud_color.getGreen()+m_fIntensity;
	b = g_hud_color.getBlue()+m_fIntensity; 
	/*
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			a = 128;
			m_fFade = 0;
		}

		// Fade the health number back to dim

		a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;

	}
	else
		a = MIN_ALPHA;

	ScaleColors (r, g, b, a );
*/
/*	int offset_num;
	
	if(m_nCapacity >99)
		offset_num = 60;
	else if(m_nCapacity >9)
		offset_num = 40;
	else
		 offset_num = 20;*/
	SPR_Set(m_plus, r, g, b );
	SPR_DrawAdditive( 0,  x0-16, y, NULL);
	gHUD.DrawHudNumberSmall(x0, y, m_iFlags | DHN_2DIGITS, m_nHealthCapacity, r, g, b);
				
	SPR_Set(m_plus, r, g, b );
	SPR_DrawAdditive( 0,  x1-16, y, NULL);
	gHUD.DrawHudNumberSmall(x1, y, m_iFlags | DHN_2DIGITS, m_nHevCapacity, r, g, b);

	if(m_fIntensity <=-255)
	{
		m_iFlags &= ~HUD_ACTIVE;
	
	}

	m_fIntensity -= gHUD.m_flTimeDelta * 75.0f;

	return 1;
}
	

int CHudCap::MsgFunc_Cap(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	
	m_nHealthCapacity = READ_SHORT(); 
	m_nHevCapacity = READ_SHORT(); 

	m_bActive = true;
	m_bFade = false;
	m_fIntensity = 0;

//	int offset_num;

	if(m_nHealthCapacity == 0 && m_nHevCapacity == 0)
	{
		if(m_iFlags != HUD_ACTIVE)
			return 1;
		m_bFade = true;	
	}
	m_fFade = FADE_TIME ;

/*	if(m_nCapacity >99)
		offset_num = 70;
	else if(m_nCapacity > 9)
		offset_num = 55;
	else
		offset_num = 30;*/

	int HealthWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;
	int CrossWidth = gHUD.GetSpriteRect(gHUD.m_Health.m_HUD_cross).right - gHUD.GetSpriteRect(gHUD.m_Health.m_HUD_cross).left;
	x0 = CrossWidth + HealthWidth / 2;
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2 - 30;
	x1 = ScreenWidth/5;
	x1 += (gHUD.m_Battery.m_prc1->right - gHUD.m_Battery.m_prc1->left);

	m_iFlags |= HUD_ACTIVE;
	return 1;
}
