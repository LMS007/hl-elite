#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_MiniMenu.h"
#include "vgui_hle_menu.h"
#include "pm_shared.h"
#include "..\common\hltv.h"
#include"vgui_ConsolePanel.h"
#include "vgui_scheme.h"

extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];
extern hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS+1];
/*
CMiniData gMiniData;

CMiniData::CMiniData()
{
	
	m_nFragsLeft=0;
	m_nTimeLeft=0;
	m_nWinsLeft=0;
}

  void SetTime(int);
	int m_nTimeLeft;
void CMiniData::SetTime(int nTime)
{
	m_nTimeLeft = nTime;
	gViewPort->m_pMiniMenu->RebuildTime(m_nTimeLeft);
}



CMiniMenu::CMiniMenu() : Panel(XRES(0),YRES(0),XRES(640),YRES(480))
{
		
	m_yourFrags = 0;
	m_leaderFrags = 0;
	m_bTimeLimit = false;
	setBgColor( 0,0,0,255 );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Mini");
	Font *smallfont = pSchemes->getFont(hSmallScheme);
	
	// time panel ///////////////////////////////////////////////////////
	m_pPanel[0]= new Panel(XRES(295), YRES(5), XRES(50),YRES(10));
	m_pPanel[0]->setBgColor(0,0,0,175);
	m_pPanel[0]->setParent(this);
	m_pPanel[0]->setBorder( new LineBorder( Color( 170, 210, 255, 0 ) ) );
	m_pLable[0]= new Label("", XRES(2), YRES(-1), XRES(50),YRES(10));
	
	m_pLable[0]->setBgColor(0,0,0,255);
	m_pLable[0]->setParent(m_pPanel[0]);
	m_pLable[0]->setFont(smallfont);
	m_pLable[0]->setFgColor(170, 210, 255, 0 );
	m_pLable[0]->setContentAlignment(Label::a_west);
	m_pLable[0]->setText("     INFINIT");
	//////////////////////////////////////////////////////////////////////

	// personal frag/wins ////////////////////////////////////////////////
	m_pPanel[1]= new Panel(XRES(265), YRES(5), XRES(25),YRES(10));
	m_pPanel[1]->setBgColor(0,0,0,175);
	m_pPanel[1]->setParent(this);
	m_pPanel[1]->setBorder( new LineBorder( Color( 170, 210, 255, 0 ) ) );

	m_pLable[1]= new Label("", XRES(2), YRES(-1), XRES(50),YRES(10));
	m_pLable[1]->setBgColor(0,0,0,255);
	m_pLable[1]->setParent(m_pPanel[1]);
	m_pLable[1]->setFont(smallfont);
	m_pLable[1]->setFgColor(170, 210, 255, 0 );
	m_pLable[1]->setContentAlignment(Label::a_west);
	
}


void CMiniMenu::RebuildTime(int nTime)
{	return;// UNDONE!!!!!!!!!!!!!!
	if(nTime)
	{
		m_bTimeLimit=true;
		m_nTime = nTime;
		m_nOldTime = 0;
	}
	else
	{
		m_bTimeLimit=false;
		m_nTime = 0;
		m_pLable[0]->setText("     INFINIT"); // fix spacing stuff
	}

}


void CMiniMenu::IncrementTime()
{	return;// UNDONE!!!!!!!!!!!!!!
	if(m_nOldTime + 1 <= gHUD.m_flTime && m_bTimeLimit)
	{
		char cTime[16];
		char cTemp[16];
		*cTemp=NULL;
		*cTime=NULL;
		m_pLable[0]->setFgColor(170, 210, 255, 0 ); // set color 
		if(m_nTime <6)
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
					m_pLable[0]->setFgColor(255, 50, 50, 0 ); // set color 
					strcpy(cTime, "     TIME UP");
					m_pLable[0]->setText(cTime);
					m_bTimeLimit=false;
					return;
			}
		}
		
		if(m_nTime == 60)
		{
			gHUD.m_Clock.SixtySeconds();
		}
		if(m_nTime < 60)
		{
			// send a time sprite warning here tooo!
			m_pLable[0]->setFgColor(0, 255, 0, 0 ); // change to WARN about timelimit
			strcpy(cTime, "        ");
		}
		else if(m_nTime < 3600)
			strcpy(cTime, "        ");
		else if(m_nTime < 86400)
			strcpy(cTime, "     ");
		else
			strcpy(cTime, " ");

		
		long seconds, minutes, hours, days = 0;
		
		seconds = m_nTime % 60;
		minutes = (m_nTime / 60) % 60;
		hours = (m_nTime / 3600) % 24;
		days = m_nTime / 86400;
		if(days != 0)
		{
			strcat(cTime, itoa(days, cTemp, 10));
			strcat(cTime, ":");
		}
		if(hours !=0)
		{
			if(hours <10 )
				strcat(cTime, "0");
			strcat(cTime, itoa(hours, cTemp, 10));
			strcat(cTime, ":");
		}
		if(minutes !=0)
		{
			if(minutes <10 )
				strcat(cTime, "0");
			strcat(cTime, itoa(minutes, cTemp, 10)); 
			strcat(cTime, ":");
		}
		if(!minutes)
			strcat(cTime, "00:");
		if(seconds <10)
			strcat(cTime, "0");
		strcat(cTime, itoa(seconds, cTemp, 10)); 
	
		m_nOldTime = gHUD.m_flTime;
		m_nTime--;
		m_pLable[0]->setText(cTime);
	}
}
*/