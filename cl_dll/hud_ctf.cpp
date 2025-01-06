#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hle_flagevents.h"
//#include "vgui_TeamFortressViewPort.h"

//extern TeamFortressViewport *gViewPort;
#define FADE_SPEED 2 // fades to 0 in FADE_TIME seconds


DECLARE_MESSAGE(m_Ctf, CtfEvent )
//DECLARE_MESSAGE(m_Flag, CtfFlag )


int CHudCtf::Init()
{
	HOOK_MESSAGE(CtfEvent);
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudCtf::VidInit()
{
	//(ScreenWidth*.50)-(SPR_Width( *m_hsRedReturned.GetSprite(),0)/2)
	CHudColor red(255, 50, 25, 0);
	CHudColor blue(0, 120, 205, 0);
	CHudColor green(0, 255, 0, 0);
	CHudColor yellow(255, 255, 0, 0);
	CHudColor purple(255, 25, 255, 0);
	CHudColor orange(255, 100, 0, 0);

	CHudColor* pColor[6];
	pColor[0]=&blue;
	pColor[1]=&red;
	pColor[2]=&green;
	pColor[3]=&yellow;
	pColor[4]=&purple;
	pColor[5]=&orange;

	char returned_spr[32];
	char scored_spr[32];
	m_hsIndex = NULL;

	m_hsIndicator.SetSprite("sprites/ctf_indicator.spr");

	for(int i = 0; i < 6; i++)
	{
		flags[i]=NULL;

		sprintf(returned_spr, "sprites/flag%i_returned.spr", i+1);
		sprintf(scored_spr, "sprites/team%i_scored.spr", i+1);

		m_hsReturned[i].SetSprite(returned_spr);
		m_hsReturned[i].SetDest((ScreenWidth/2)-  (SPR_Width(*m_hsReturned[i].GetSprite(),0)/2), (ScreenHeight/2) + (ScreenHeight*.75)/2);
		m_hsReturned[i].SetColor(pColor[i]);
		m_hsReturned[i].SetActive(false);

		m_hsScored[i].SetSprite(scored_spr);
		m_hsScored[i].SetDest((ScreenWidth/2)-  (SPR_Width(*m_hsScored[i].GetSprite(),0)/2), (ScreenHeight/2) - (ScreenHeight*.75)/2);
		m_hsScored[i].SetColor(pColor[i]);
		m_hsScored[i].SetActive(false);
	
		m_hsFlagMini[i].SetSprite("sprites/flag_mini.spr");
		m_hsFlagMini[i].SetColor(pColor[i]);
		m_hsFlagMini[i].SetActive(false);

		m_hsFlagMiniHand[i].SetSprite("sprites/flag_mini_hand.spr");
		m_hsFlagMiniHand[i].SetColor(pColor[i]);
		m_hsFlagMiniHand[i].SetActive(false);
	}

	sprite_heigth = SPR_Height(*m_hsFlagMini[0].GetSprite(),0);
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudCtf::Draw(float flTime)
{
	int result = 0;
	CHudColor white(255, 255, 255, 0);
	CHudColor red(255, 50, 25, 0);

	m_hsIndicator.Draw();
	for(int i = 0; i < 6; i++)
	{
		result |= m_hsReturned[i].Draw();
		result |= m_hsScored[i].Draw();
		result |= m_hsFlagMini[i].Draw();
		result |= m_hsFlagMiniHand[i].Draw();
		

	}		

	if(!result)	m_iFlags &= ~HUD_ACTIVE;	

	return 1;
}


int CHudCtf::MsgFunc_CtfEvent(const char *pszName, int iSize, void *pbuf)
{ 
	// first byte = event
	// second byte = team
	int i, j;
	BEGIN_READ( pbuf, iSize );
	int nEvent = READ_BYTE(); 
	int nTeam = READ_BYTE() -1; 
	int nFlag = READ_BYTE() -1; 
	bool found = false;
	switch(nEvent)
	{
	case FLAG_SCORED: // scored 
		
		if(&m_hsFlagMiniHand[nFlag]==m_hsIndex)
		{
			m_hsIndicator.StartFade(4);
			m_hsIndex = NULL;
		}

		for( i =0; i<6;i++)
		{
			if(flags[i]==&m_hsFlagMiniHand[nFlag] || flags[i]==&m_hsFlagMini[nFlag])
			{
				flags[i]=NULL;
			}
		}

		m_hsScored[nTeam].ResetAnimation();
		m_hsScored[nTeam].StartFade(4, 3);
		m_hsFlagMiniHand[nFlag].StartFade(4);
		break;

	case FLAG_RETURNED: // returned
		
		if(&m_hsFlagMiniHand[nFlag]==m_hsIndex)
		{
			m_hsIndicator.StartFade(4);
			m_hsIndex = NULL;
		}

		for( i =0; i<6;i++)
		{
			if(flags[i]==&m_hsFlagMiniHand[nFlag] || flags[i]==&m_hsFlagMini[nFlag])
			{
				flags[i]=NULL;
			}
		}
		m_hsReturned[nFlag].ResetAnimation();
		m_hsReturned[nFlag].StartFade(4, 3);
		m_hsFlagMini[nFlag].StartFade(4);
		m_hsFlagMiniHand[nFlag].StartFade(4);
		break;

	case FLAG_DROPPED: // dropped
		
		
		for( i =0; 
		
		
		
		i<6;i++)
		{
			if(flags[i]==&m_hsFlagMiniHand[nFlag] || flags[i]==&m_hsFlagMini[nFlag])
			{
				flags[i]=&m_hsFlagMini[nFlag];
				found = true;
				break;
			}
		}
		if(!found)
		{
			for( j=0;j<6;j++)
			{
				if(flags[j]==NULL)
				{
					flags[j]=&m_hsFlagMini[nFlag];
					break;
				}
			}
		}
		if(&m_hsFlagMiniHand[nFlag]==m_hsIndex)
		{
			m_hsIndicator.StartFade(4);
			m_hsIndex = NULL;
		}
		m_hsFlagMiniHand[nFlag].SetActive(false);
		m_hsFlagMini[nFlag].ResetAnimation();
		break;
	case FLAG_SPEC_HOLD:
		m_hsIndex=&m_hsFlagMiniHand[nFlag];
		break;
	case FLAG_SPEC_DROP:
		m_hsIndicator.SetActive(false);
		m_hsIndex = NULL;
		
		break;

	case FLAG_HAND:
		
		m_hsIndex = &m_hsFlagMiniHand[nFlag];
		// warning: no "break", this is intended here

	case FLAG_CAPTURED: // captured
	

		for( i =0; i<6;i++)
		{
			if(flags[i]==&m_hsFlagMini[nFlag] || flags[i]==&m_hsFlagMiniHand[nFlag])
			{
				m_hsFlagMini[nFlag].SetActive(false);
				flags[i]=&m_hsFlagMiniHand[nFlag];
				goto OUT;
			}
		}

		for( j=0;j<6;j++)
		{
			if(flags[j]==NULL)
			{
				flags[j]=&m_hsFlagMiniHand[nFlag];
				break;
			}
		}
OUT:
 		m_hsFlagMiniHand[nFlag].ResetAnimation();
		break;

	case RESET_ALL_FLAGS: // reset all
		for(int i = 0; i <6; i++)
		{	m_hsReturned[i].StartFade(4);
			m_hsFlagMini[i].StartFade(4);
			m_hsFlagMiniHand[i].StartFade(4);
			m_hsScored[i].StartFade(4);
			flags[i]=NULL;
		}
		m_hsIndicator.StartFade(4);
		m_hsIndex = NULL;
		break;

	}
	
	for (i = 0; i < 6; i++)
	{
		if(flags[i])
			flags[i]->SetDest( 10,  ScreenHeight - 80 - (5*i+i*sprite_heigth));
	}

	if(m_hsIndex)
	{
		CHudColor color;
		int x, y;
		m_hsIndex->GetColor(&color);
		m_hsIndex->GetDest(&x, &y);

		m_hsIndicator.SetColor(&color);
		m_hsIndicator.SetDest(x+32, y);
		m_hsIndicator.ResetAnimation();
	}
	
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

//////////////////////


int CHudFlag::Init()
{

	//HOOK_MESSAGE(CtfFlag);
	gHUD.AddHudElem( this );
	return 1;
}

void CHudFlag::InitHUDData( void )
{
	m_iFlags &= ~HUD_ACTIVE;;
}

int CHudFlag::VidInit()
{
	CHudColor white(255, 255, 255, 0);

//	m_hsRedFlag.SetSprite("sprites/flag_red.spr");
//	m_hsBlueFlag.SetSprite("sprites/flag_blue.spr");
//	m_hsGreenFlag.SetSprite("sprites/flag_blue.spr");
//	m_hsYellowFlag.SetSprite("sprites/flag_blue.spr");
//	m_hsPurpleFlag.SetSprite("sprites/flag_blue.spr");
//	m_hsOrangeFlag.SetSprite("sprites/flag_blue.spr");
	
	int x = (ScreenWidth)-((ScreenWidth*.99));
	int y = (ScreenHeight) - (ScreenHeight*.80);

	m_hsRedFlag.SetDest(x, y);
	m_hsBlueFlag.SetDest(x, y);
	m_hsGreenFlag.SetDest(x, y);
	m_hsYellowFlag.SetDest(x, y);
	m_hsPurpleFlag.SetDest(x, y);
	m_hsOrangeFlag.SetDest(x, y);

	m_hsRedFlag.SetColor(&white);
	m_hsBlueFlag.SetColor(&white);
	m_hsGreenFlag.SetColor(&white);
	m_hsYellowFlag.SetColor(&white);
	m_hsPurpleFlag.SetColor(&white);
	m_hsOrangeFlag.SetColor(&white);

	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}


int CHudFlag::Draw(float flTime)
{
	int result = 0;
	result |= m_hsRedFlag.Draw();
	result |= m_hsBlueFlag.Draw();		
	result |= m_hsPurpleFlag.Draw();
	result |= m_hsGreenFlag.Draw();
	result |= m_hsOrangeFlag.Draw();
	result |= m_hsYellowFlag.Draw();
	if(result == 0)
	{
		m_iFlags &= ~HUD_ACTIVE;
	}
	return 1;
}


int CHudFlag::MsgFunc_CtfFlag(const char *pszName, int iSize, void *pbuf)
{

	BEGIN_READ( pbuf, iSize );
	int nEvent = READ_BYTE(); 
	switch(nEvent)
	{
	case 0: 
		m_hsRedFlag.StartFade(4);
		m_hsBlueFlag.StartFade(4);
		m_hsOrangeFlag.StartFade(4);
		m_hsPurpleFlag.StartFade(4);
		m_hsGreenFlag.StartFade(4);
		m_hsYellowFlag.StartFade(4);
		break;
	case 1: // blue captured
		m_hsBlueFlag.ResetAnimation();
		break;
	case 2: // blue captured
		m_hsRedFlag.ResetAnimation();
		break;
	case 3: // blue captured
		m_hsYellowFlag.ResetAnimation();
		break;
	case 4: // blue captured
		m_hsGreenFlag.ResetAnimation();
		break;
	case 5: // blue captured
		m_hsPurpleFlag.ResetAnimation();
		break;
	case 6: // removed flags
		m_hsOrangeFlag.ResetAnimation();
		break;
		
	
	}
	m_iFlags |= HUD_ACTIVE;
	return 1;
}
