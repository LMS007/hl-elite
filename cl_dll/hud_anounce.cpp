#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hle_flagevents.h"


#define FADE_SPEED 2 


DECLARE_MESSAGE(m_Anounce, Anounce )


int CHudAnounce::Init()
{
	HOOK_MESSAGE(Anounce);
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudAnounce::VidInit()
{

	CHudColor red(255, 50, 25, 0);
	CHudColor blue(0, 120, 205, 0);
	CHudColor green(0, 255, 0, 0);
	CHudColor yellow(255, 255, 0, 0);
	CHudColor purple(255, 25, 255, 0);
	CHudColor orange(255, 100, 0, 0);
	CHudColor white(175, 175, 175, 0);



	int y = ScreenHeight/2;
	int x = 0;
	m_hsTeams[0].SetDest(x, y);
	m_hsTeams[0].SetColor(&blue);
	m_hsTeams[0].SetActive(false);

	m_hsTeams[1].SetSprite("sprites/red.spr");
	m_hsTeams[1].SetColor(&red);
	m_hsTeams[1].SetActive(false);

	m_hsTeams[2].SetSprite("sprites/green.spr");
	m_hsTeams[2].SetColor(&green);
	m_hsTeams[2].SetActive(false);

	m_hsTeams[3].SetSprite("sprites/yellow.spr");
	m_hsTeams[3].SetColor(&yellow);
	m_hsTeams[3].SetActive(false);

	m_hsTeams[4].SetSprite("sprites/purple.spr");
	m_hsTeams[4].SetColor(&purple);
	m_hsTeams[4].SetActive(false);

	m_hsTeams[5].SetSprite("sprites/orange.spr");
	m_hsTeams[5].SetColor(&orange);
	m_hsTeams[5].SetActive(false);

	
	
	m_hsTeamWins[0].SetSprite("sprites/bluewins.spr");
	x = (ScreenWidth/2)-(SPR_Width(*m_hsTeamWins[0].GetSprite(),0)/2);
	m_hsTeamWins[0].SetDest(x, y);
	m_hsTeamWins[0].SetColor(&blue);
	m_hsTeamWins[0].SetActive(false);

	m_hsTeamWins[1].SetSprite("sprites/redwins.spr");
	m_hsTeamWins[1].SetDest(x, y);
	m_hsTeamWins[1].SetColor(&red);
	m_hsTeamWins[1].SetActive(false);

	m_hsTeamWins[0].SetSprite("sprites/greenwins.spr");
	m_hsTeamWins[0].SetDest(x, y);
	m_hsTeamWins[0].SetColor(&green);
	m_hsTeamWins[0].SetActive(false);

	m_hsTeamWins[0].SetSprite("sprites/yellowwins.spr");
	m_hsTeamWins[0].SetDest(x, y);
	m_hsTeamWins[0].SetColor(&yellow);
	m_hsTeamWins[0].SetActive(false);

	m_hsTeamWins[4].SetSprite("sprites/purplewins.spr");
	m_hsTeamWins[4].SetDest(x, y);
	m_hsTeamWins[4].SetColor(&purple);
	m_hsTeamWins[4].SetActive(false);

	m_hsTeamWins[5].SetSprite("sprites/orangewins.spr");
	m_hsTeamWins[5].SetDest(x, y);
	m_hsTeamWins[5].SetColor(&orange);
	m_hsTeamWins[5].SetActive(false);

	m_hsLTS.SetSprite("sprites/lts.spr");
	x = (ScreenWidth/2)-(SPR_Width(*m_hsLTS.GetSprite(),0)/2);
	m_hsLTS.SetDest(x, y);
	m_hsLTS.SetColor(&g_hud_color);
	m_hsLTS.SetActive(false);

	m_hsNoLTS.SetSprite("sprites/nolts.spr");
	x = (ScreenWidth/2)-(SPR_Width(*m_hsLTS.GetSprite(),0)/2);
	m_hsNoLTS.SetDest(x, y);
	m_hsNoLTS.SetColor(&g_hud_color);
	m_hsNoLTS.SetActive(false);

	m_hsVersus.SetSprite("sprites/verses.spr");
	x = (ScreenWidth/2)-(SPR_Width(*m_hsLTS.GetSprite(),0)/2);
	m_hsVersus.SetDest(x, y);
	m_hsVersus.SetColor(&g_hud_color);
	m_hsVersus.SetActive(false);

	m_hsTie.SetSprite("sprites/tie.spr");
	x = (ScreenWidth/2)-(SPR_Width(*m_hsLTS.GetSprite(),0)/2);
	m_hsTie.SetDest(x, y);
	m_hsTie.SetColor(&g_hud_color);
	m_hsTie.SetActive(false);

	m_hsTimeup.SetSprite("sprites/timeup.spr");
	x = (ScreenWidth/2)-(SPR_Width(*m_hsTimeup.GetSprite(),0)/2);
	m_hsTimeup.SetDest(x, y);
	m_hsTimeup.SetColor(&g_hud_color);
	m_hsTimeup.SetActive(false);
	
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudAnounce::Draw(float flTime)
{
	
	int result = 0;

	result |= m_hsTie.Draw();
	result |= m_hsVersus.Draw();		
	result |= m_hsNoLTS.Draw();
	result |= m_hsLTS.Draw();
	result |= m_hsTimeup.Draw();

	for (int i = 0; i <6; i++)
	{
		result |= m_hsTeamWins[i].Draw();
		result |= m_hsTeams[i].Draw();
	}
	
	if(result == 0)
	{
		m_iFlags &= ~HUD_ACTIVE;
	}
	return 1;
}


int CHudAnounce::MsgFunc_Anounce(const char *pszName, int iSize, void *pbuf)
{ 
	BEGIN_READ( pbuf, iSize );
	int nEvent = READ_BYTE(); 
	int nTeam1, nTeam2, nTeamWon;
	switch(nEvent)
	{
	case 0: 
		
		nTeam1 = READ_BYTE()-1; 
		nTeam2 = READ_BYTE()-1; 
		m_hsTeams[nTeam1].SetDest( (ScreenWidth/2) - SPR_Width(*m_hsLTS.GetSprite(),0)-16, ScreenHeight/2);
		m_hsTeams[nTeam1].StartFade(4, 2);
		m_hsTeams[nTeam2].SetDest((ScreenWidth/2)+16, ScreenHeight/2);
		m_hsTeams[nTeam2].StartFade(4, 2);
		m_hsVersus.StartFade(4, 2);
		break;
	case 1: 
		nTeamWon = READ_BYTE()-1; 
		m_hsTeamWins[nTeamWon].StartFade(4, 2);
		break;
	case 2: 
		switch(nEvent)
		{
		case 0:
			
			m_hsLTS.StartFade(4, 2);
			break;	
		case 1:
			m_hsNoLTS.StartFade(4, 2);
			break;	
		case 2:
			m_hsTie.StartFade(4, 2);
			break;	
		case 3:
			m_hsTimeup.StartFade(4, 2);
			break;
		}
		break;
	}
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

//////////////////////
