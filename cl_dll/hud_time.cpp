#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
//#include "vgui_TeamFortressViewport.h"
//#include "vgui_MiniMenu.h"


#define FADDING_SPEED 2.0f // fades to 0 in FADE_TIME seconds
#define FADDING_SPEED_2 6.0f

extern cvar_t	*cl_showclock;
extern cvar_t	*cl_showtimer;




DECLARE_MESSAGE(m_Count, CountNumber )
DECLARE_MESSAGE(m_Clock, TimeLeft )
DECLARE_MESSAGE(m_Timer, Timer )

//extern CMiniData gMiniData;

int CHudCountDown::Init()
{
	HOOK_MESSAGE(CountNumber);	
	m_nIntensity = 0;
	
	gHUD.AddHudElem( this );
	return 1;
}

int CHudCountDown::VidInit()
{
	for(int i=0; i<6; i++)
		*m_ccSounds[i] = NULL;

	strcpy(m_ccSounds[0], "fvox/engage.wav");
	strcpy(m_ccSounds[1], "fvox/one.wav");
	strcpy(m_ccSounds[2], "fvox/two.wav");
	strcpy(m_ccSounds[3], "fvox/three.wav");
	strcpy(m_ccSounds[4], "fvox/four.wav");
	strcpy(m_ccSounds[5], "fvox/five.wav");

	m_hNumber[0] = LoadSprite("sprites/engage.spr");
	m_hNumber[1] = LoadSprite("sprites/one.spr");
	m_hNumber[2] = LoadSprite("sprites/two.spr");
	m_hNumber[3] = LoadSprite("sprites/three.spr");
	m_hNumber[4] = LoadSprite("sprites/four.spr");
	m_hNumber[5] = LoadSprite("sprites/five.spr");

	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudCountDown::Draw(float flTime)
{
	int r, g, b, x, y;

	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 

	// im gonna assume hl checks for -negative rgb valve if it dont its are teh ghey
	r += m_nIntensity;
	g += m_nIntensity;
	b += m_nIntensity;


	// This should show up at the upper middle
	y = (ScreenHeight/2) - (ScreenHeight/4);
	x = (ScreenWidth/2)-(SPR_Width(m_hNumber[m_nIndex],0)/2);



	// blueish color
	SPR_Set(m_hNumber[m_nIndex], r, g, b );
	SPR_DrawAdditive( 0,  x, y, NULL);
	
	if(m_nIntensity==0)
	{
		PlaySound(m_ccSounds[m_nIndex], 1);
		m_nIntensity = -1;
	}
	if(gHUD.m_flTime > m_fHold)
	{
		m_nIntensity -= gHUD.m_flTimeDelta * (200.0f);
		if(m_nIntensity<-255)
		{	
			m_iFlags &= ~HUD_ACTIVE; // fadded to 0
		}
	}

	return 1;
}

int CHudCountDown::MsgFunc_CountNumber(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	m_nIndex = READ_SHORT();
	m_nIntensity = 0;
	m_fHold = gHUD.m_flTime + 0.5;
	
	if(m_nIndex <0 || m_nIndex >5)
	{
		m_iFlags &= ~HUD_ACTIVE; // disable drawing, message is bogus
		m_nIndex = 0;
	}
	else
	{
		m_iFlags |= HUD_ACTIVE;
	}
	return 1;
}

void CHudCountDown2::BlitNumber(int nNumber)
{
	m_nIndex = 0;
	m_bDraw60 = false;
	if(m_nIndex > 0 || m_nIndex < 5)
	{
		m_nIndex = nNumber;
	}
	m_nIntensity = 0;
	m_iFlags |= HUD_ACTIVE;
}

void CHudCountDown2::Blit60()
{
	m_nIntensity = 0;
	m_bDraw60 = true;
	m_iFlags |= HUD_ACTIVE;
}




int CHudCountDown2::Init()
{

	m_nIntensity = 0;
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudCountDown2::VidInit()
{
	m_hNumber[0] = LoadSprite("sprites/one_s.spr");
	m_hNumber[1] = LoadSprite("sprites/two_s.spr");
	m_hNumber[2] = LoadSprite("sprites/three_s.spr");
	m_hNumber[3] = LoadSprite("sprites/four_s.spr");
	m_hNumber[4] = LoadSprite("sprites/five_s.spr");
	m_h60Seconds = LoadSprite("sprites/60seconds.spr");
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudCountDown2::Draw(float flTime)
{
	int r, g, b, x, y;

	// im gonna assume hl checks for -negative rgb valve if it dont its are teh ghey

	r = g_hud_color.getRed() ;
	g = g_hud_color.getGreen() ;
	b = g_hud_color.getBlue() ; 

	g_hud_color.setInverts(&r, &g, &b);
	
	r+=m_nIntensity;
	g+=m_nIntensity;
	b+=m_nIntensity;

	// This should show up at the upper middle
	y = (ScreenHeight/2) - (ScreenHeight*.85)/2;

	if(m_nIntensity ==0)
	{
		m_nIntensity = -1;
	}
	if(!m_bDraw60)
	{
		x = (ScreenWidth/2)-(SPR_Width(m_hNumber[m_nIndex],0)/2);
		SPR_Set(m_hNumber[m_nIndex], r, g, b );
		SPR_DrawAdditive( 0,  x, y, NULL);
		m_nIntensity -= gHUD.m_flTimeDelta * HLE_FADE;
	}
	else
	{
		x = (ScreenWidth/2)-(SPR_Width(m_h60Seconds,0)/2);
		SPR_Set(m_h60Seconds, r, g, b );
		SPR_DrawAdditive( 0,  x, y, NULL);
		m_nIntensity -= gHUD.m_flTimeDelta * HLE_FADE;
	}

	
	if(m_nIntensity<-255)
	{	
		m_iFlags &= ~HUD_ACTIVE; // fadded to 0
	}
	return 1;
}




int CHudClock::Init()
{

	HOOK_MESSAGE(TimeLeft);
	m_nIntensity = 0;
	m_n60Intensity =0;
	//m_iFlags &= ~HUD_ACTIVE;
	m_iFlags |= HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudClock::VidInit()
{
	
	m_hColen = LoadSprite("sprites/colen.spr");
	m_hClock = LoadSprite("sprites/clock.spr");
	seconds = minutes = hours = days = 0;
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}


int CHudClock::Draw(float flTime)
{
	UpdateTime();
	return 1;
}

/*
CHudClock

x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, pw->iClip, r, g, b);

*/

int CHudClock::MsgFunc_TimeLeft(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int nTime = READ_LONG();
	RebuildTime(nTime);
	return 1;
}


/*
#define SPRITE_HOURS 0
#define SPRITE_MINUTES 1
#define SPRITE_SECONDS 2*/

void CHudClock::SixtySeconds()
{
}

void CHudClock::UpdateTime()
{	
	int r, g, b;
	int x[6];
	int y;
	int y_colen;
	int nColen_spacer = 15;
	
//	
	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 
	ScaleColors(r, g, b, MIN_ALPHA );

	int x_position = 0;

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;


	
	if(m_nOldTime + 1 <= gHUD.m_flTime && m_bTimeLimit)
	{
	
		if(m_nTime < 6)
		{
			switch(m_nTime)
			{
				case 5:
					gHUD.m_Count2.BlitNumber(4);
					break;
				case 4:
					gHUD.m_Count2.BlitNumber(3);
					break;
				case 3:
					gHUD.m_Count2.BlitNumber(2);
					break;
				case 2:
					gHUD.m_Count2.BlitNumber(1);
					break;
				case 1:
					gHUD.m_Count2.BlitNumber(0);
					break;
				case 0:
//					m_pLable[0]->setFgColor(255, 50, 50, 0 ); // set color 
//					strcpy(cTime, "     TIME UP");
//					m_pLable[0]->setText(cTime);
					m_bTimeLimit=false;
					return;
			}
		}
		
		if(m_nTime == 60)
		{
			gHUD.m_Count2.Blit60();
		}

		seconds = m_nTime % 60;
		minutes = (m_nTime / 60) % 60;
		hours = (m_nTime / 3600) % 24;
//		days = m_nTime / 86400;
		m_nOldTime = gHUD.m_flTime;
		m_nTime--;
	}
	y_colen = y+5;
	int xoffset = XRES(320)-60;
	//hours 00:
	if (minutes)
		xoffset = xoffset + 25;
	if(hours)
		xoffset = xoffset + 25;
	

	
	x[0] = xoffset;
	x[1] = xoffset+18;
	//minutes 00:
	x[2] = xoffset+44;
	x[3] = xoffset+62;
	//seconds 00
	x[4] = xoffset+88;
	x[5] = xoffset+106;

	if(cl_showclock->value==0)
		return;

	if(hours !=0)
	{
		if(hours >9)
		{
			gHUD.DrawHudNumber(x[0], y, m_iFlags | DHN_2DIGITS, hours, r, g, b);
		}
		else
		{
			gHUD.DrawHudNumberSmall(x[0], y, m_iFlags | DHN_DRAWZERO, 0, r, g, b);
			gHUD.DrawHudNumberSmall(x[1], y, m_iFlags | DHN_DRAWZERO, hours, r, g, b);
		}
		SPR_Set(m_hColen, r, g, b );
		SPR_DrawAdditive( 0,  x[1]+nColen_spacer, y_colen, NULL);
	}
	if(minutes !=0)
	{
		if(minutes >9)
		{
			gHUD.DrawHudNumberSmall(x[2], y, m_iFlags | DHN_2DIGITS, minutes, r, g, b);
		}
		else
		{
			gHUD.DrawHudNumberSmall(x[2], y, m_iFlags | DHN_DRAWZERO, 0, r, g, b);
			gHUD.DrawHudNumberSmall(x[3], y, m_iFlags | DHN_DRAWZERO, minutes, r, g, b);
		}
		SPR_Set(m_hColen, r, g, b );
		SPR_DrawAdditive( 0,  x[3]+nColen_spacer, y_colen, NULL);
	}
	if(seconds >9)
	{
		gHUD.DrawHudNumberSmall(x[4], y, m_iFlags | DHN_2DIGITS, seconds, r, g, b);
	}
	else
	{
		gHUD.DrawHudNumberSmall(x[4], y, m_iFlags | DHN_DRAWZERO, 0, r, g, b);
		gHUD.DrawHudNumberSmall(x[5], y, m_iFlags | DHN_DRAWZERO, seconds, r, g, b);
	}
	SPR_Set(m_hClock, r, g, b );
	SPR_DrawAdditive( 0,  x[5]+(nColen_spacer*2), y, NULL);

}

void CHudClock::RebuildTime(float nTime)
{	
	m_nTimeLeft = nTime;
	if(m_nTimeLeft > 0)
	{
		m_bTimeLimit=true;
		m_nTime = m_nTimeLeft;
		m_nOldTime = 0;
		m_iFlags |= HUD_ACTIVE;
	}
	else if(m_nTimeLeft == 0)
	{
		m_bTimeLimit=false;
		m_nTime = 0;
		m_iFlags &= ~HUD_ACTIVE;
	}
	else
	{
		m_nOldTime = -1; // freez the time
	}

}


int CHudTimer::Init()
{

	HOOK_MESSAGE(Timer);
	m_nIntensity = 0;
	//m_iFlags |= HUD_ACTIVE;
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}



int CHudTimer::MsgFunc_Timer(const char *pszName, int iSize, void *pbuf)
{

	BEGIN_READ( pbuf, iSize );
	int nTime = READ_SHORT();
	RebuildTime(nTime);
	return 1;

}

void CHudTimer::UpdateTime()
{
	int r, g, b;
	int x;
	int y;

	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 
	ScaleColors(r, g, b, MIN_ALPHA );
	
	int x_position = 0;
	y = YRES(5);

	if(m_nOldTime + 1 <= gHUD.m_flTime && m_nOldTime>=0)
	{
	
		if(m_nTime < 6)
		{
			switch(m_nTime)
			{
				case 5:
					gHUD.m_Count2.BlitNumber(4);
					break;
				case 4:
					gHUD.m_Count2.BlitNumber(3);
					break;
				case 3:
					gHUD.m_Count2.BlitNumber(2);
					break;
				case 2:
					gHUD.m_Count2.BlitNumber(1);
					break;
				case 1:
					gHUD.m_Count2.BlitNumber(0);
					break;
				case 0:
					m_bTimeLimit=false;
					return;
			}
		}
		
		if(m_nTime == 60)
		{
			gHUD.m_Count2.Blit60();
		}

		else if(m_nTime <= 0)
		{
			m_iFlags &= ~HUD_ACTIVE;
		}
		m_nOldTime = gHUD.m_flTime;
		seconds = m_nTime;
		m_nTime--;
	}
	int nNumbersLeft = seconds;
	int nDigits = 0;
	while(nNumbersLeft)
	{	
		nDigits++;
		nNumbersLeft /= 10;
	}

	if(cl_showtimer->value==0)
		return;

	int xoffset = XRES(320)-(8*nDigits);

	x = xoffset;

	if(seconds >99)
	{
		gHUD.DrawHudNumberSmall(x, y, m_iFlags | DHN_3DIGITS, seconds, r, g, b);
	}
	
	else if(seconds >9)
	{
		gHUD.DrawHudNumberSmall(x, y, m_iFlags | DHN_2DIGITS, seconds, r, g, b);
	}
	else
	{
		gHUD.DrawHudNumberSmall(x, y, m_iFlags | DHN_DRAWZERO, seconds, r, g, b);
		if(seconds ==0)
			m_iFlags &= ~HUD_ACTIVE;
	}

}



int CHudMp3::Init()
{

	HOOK_MESSAGE(Timer);
	m_nIntensity = 0;
	//m_iFlags |= HUD_ACTIVE;
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudMp3::VidInit()
{
	m_hPercent = LoadSprite("sprites/percent.spr");
	m_hVolume = LoadSprite("sprites/volume.spr");
	m_nIntensity = 0;
	return 1;

}

void CHudMp3::SetVolume(int volume)
{
	m_fDrawTime = gHUD.m_flTime+2;
	m_pVolume = volume;
	m_iFlags |= HUD_ACTIVE;
	m_nIntensity = 0;
}

void CHudMp3::UpdateTime()
{
	if(m_fDrawTime < gHUD.m_flTime && m_fDrawTime !=0 )
	{
		//m_iFlags &= ~HUD_ACTIVE;
		m_fDrawTime=0;
		m_nIntensity = -1;
	}

	int r, g, b;
	int x;
	int y;

	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue(); 
	
	r+=m_nIntensity;
	g+=m_nIntensity;
	b+=m_nIntensity;

	if(m_nIntensity < 0)
		m_nIntensity -= gHUD.m_flTimeDelta * 200;
	
	if(m_nIntensity<-255)
	{	
		m_nIntensity = 0;
		m_iFlags &= ~HUD_ACTIVE; // fadded to 0
		return;
	}

	int x_position = 0;
	y = YRES(300);

	int nDigits = 0;
	int nNumbersLeft = m_pVolume;
	while(nNumbersLeft)
	{	
		nDigits++;
		nNumbersLeft /= 10;
	}
	if(nDigits == 0)
		nDigits=1;

	int xoffset = XRES(500)-(8*nDigits);

	x = xoffset;

	if(m_pVolume >99)
	{
		gHUD.DrawHudNumberSmall(x, y, m_iFlags | DHN_3DIGITS, m_pVolume, r, g, b);
	}
	
	else if(m_pVolume >9)
	{
		gHUD.DrawHudNumberSmall(x, y, m_iFlags | DHN_2DIGITS, m_pVolume, r, g, b);
	}
	else
	{
		gHUD.DrawHudNumberSmall(x, y, m_iFlags | DHN_DRAWZERO, m_pVolume, r, g, b);
	}
	
	SPR_Set(m_hVolume, r, g, b );
	SPR_DrawAdditive( 0,  x - 33, y-4, NULL);

	SPR_Set(m_hPercent, r, g, b );
	SPR_DrawAdditive( 0,  x + (20*nDigits), y-3, NULL);

}



