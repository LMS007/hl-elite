#ifndef _VGUI_HLE_H
#define _VGUI_HLE_H


#include "VGUI_INTCHANGESIGNAL.h"


#define SENSITIVITY_SCALE 70


#include <windows.h>
#include "winamp.h"
#include "hle_maploader.h"


extern int g_ColorTable[MAXCOLORS][3];
extern cvar_t* cl_nextmap;
extern cvar_t* cl_radar;

extern cvar_t*sensitivity;

extern cvar_t* m_pitch;
extern int g_nLockPreview;

extern cvar_t* cl_gausscolor;
extern int g_GaussColor;

extern CWinamp gWinamp;
extern cvar_t* cl_crosshairs;
extern CNameList g_Crosshairs;
extern int g_CrosshairIndex;


//#include "vgui_TeamFortressViewport.h"
// hle
//================================================================
// First VGUI menu!
//============================================================

class ClientVote
{
	CommandButton *m_pButton;
	char m_szVoteString[256];
	int ur, ug, ub, ua;
	int hr, hg, hb, ha;

public:
	ClientVote();
	bool DualVote();
	void ClearVote();
	int AddVote(char *string, CommandButton*, bool forceVote=false);
	int CallSingleVote(char *string);
};

extern ClientVote g_ClientVote;



class ActionButton : public CommandButton
{
public:

	ActionButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight = false) : CommandButton(text, x, y, wide, tall, bNoHighlight) {}
	virtual void internalMousePressed(MouseCode code)=0;
	virtual void internalMouseReleased(MouseCode code)=0;
};

class TrackButton : public ActionButton
{
private:
	int m_command;
	Label* m_Label;
public:

	TrackButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight = false) : ActionButton(text, x, y, wide, tall, bNoHighlight) {}
	void SetDirection(int command)
	{
		m_command = command;
	}
	void SetLabel(Label* pLabel)
	{
		m_Label = pLabel;
	}
	void internalMousePressed(MouseCode code)
	{
		gWinamp.Cmd_Winamp(m_command+6);
	}
	void internalMouseReleased(MouseCode code)
	{
		char* track = gWinamp.Cmd_Winamp(m_command);
		if(track)
		{
			m_Label->setText(track);
		}
	}
	
};
class CHLEMenu : public CMenuPanel
{
protected:
		bool _iIsInObserverMode;
		Panel	*m_pPanel;
		Panel	*m_pBackround;
		
		Font* m_pFontButtons;;
		Font* m_pFontButtonsLarge;
		Font* m_pFontTitle;
public:
		CHLEMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void ResetButtons();
};
class CMainMenu : public CHLEMenu
{
	private:

		bool _iIsInObserverMode;

		
        /*Label               *m_pTitle;
       
        Label               *m_pBackround_Yes;
        Label               *m_pBackround_No;*/

		/*CommandButton		*m_pInfo;
		CommandButton		*m_pPlay;
		//CommandButton		*m_pQuit;
		CommandButton		*m_pConsole;
		CommandButton		*m_pSpectate;
		CommandButton		*m_pCancelButton;
		CommandButton		*m_pVoteModes;
		CommandButton		*m_pVoteMaps;
		CommandButton		*m_pVoteSettings;
		CommandButton		*m_pChangeTeam;
		CommandButton		*m_pWinamp;
		CommandButton		*m_pRemove;
		CommandButton		*m_pHudMenu;
		CommandButton		*m_pVoteMatch;
		CommandButton		*m_pVoteYes;
		CommandButton		*m_pVoteNo;*/
		HLEMenuButton		*m_pChangeTeam;
		Border				*m_pEditbox;
		TextEntry			*m_pSettingsBox;
		int					m_iShowText;
		Label				*m_pText;

        Label				*m_BottomLable;

	public:
		void ActionTeamSignal(int);
		CMainMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		//void SetActiveInfo( int iShowText );
		void setVisible( bool visible );


};

class CVoteMatchMenu : public CHLEMenu
{
		HLEMenuButton* m_cancel_match;
public:
		CVoteMatchMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void setVisible( bool visible );
};
/*
class CVoteSettingsMenu : public CMenuPanel
{
	private:
		
		Panel				*m_pPanel;
		Label				*m_pTitle;
		TextEntry			*m_pEditbox;
        Label				*m_pBackround;

		// menu comands
		CommandButton		*m_pCancelButton;
		CommandButton		*m_pBackButton;
        CommandButton       *m_pConsole;
		// server vote commands
		CommandButton		*m_pCommand[15];

		CommandButton		*m_pFallDamage;
		CommandButton		*m_pForceRespawn;
		CommandButton		*m_pTimeLimit;
        CommandButton       *m_pMaxSpeed;
		CommandButton		*m_pRestartFull;
		CommandButton		*m_pRestart;
		CommandButton		*m_pLoseLongjump;
		CommandButton		*m_pRestartNormal;
		CommandButton		*m_pLockUnlock;
		CommandButton		*m_pPauseResume;
		CommandButton		*m_pOldSchoolDamage;
		CommandButton		*m_pBunnyHop;
		CommandButton		*m_pReflectDamage;
		CommandButton		*m_pWeaponStay;
		CommandButton		*m_pFriendlyFire;

        CommandButton       *blank;

        Label				*m_BottomLable;
		
		int					*m_iShowText;
		Label				*m_pText;
	public:


		CVoteSettingsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void ResetButtons();
		void SetActiveInfo( int iShowText );
        void getTextBuffer(void);
		void setVisible( bool visible );
};*/


class CVoteModesMenu : public CMenuPanel
{
	private:
		
		Panel	*m_pPanel;
		Label				*m_pTitle;
        Label				*m_pBackround;
        CommandButton       *m_pConsole;
		// menu comands
		CommandButton		*m_pCancelButton;
		CommandButton		*m_pBackButton;

		CommandButton		*m_pBrawl;
		CommandButton		*m_pRiot;
		CommandButton		*m_pDuel;
		CommandButton		*m_pTournament ;
		CommandButton		*m_pTLMS;
		CommandButton		*m_pTAction;
		CommandButton		*m_pMatch;
		CommandButton		*m_pCTF;  
		CommandButton		*m_pHunt; 
		CommandButton		*m_pLMS;  
		CommandButton		*m_pPratice; 
		CommandButton		*m_pEmpty; 
		// server vote commands
		
        Label				*m_BottomLable;
		int					*m_iShowText;
		Label				*m_pText;
	public:

        void ChangePage(int);
		CVoteModesMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void ResetButtons();
		void SetActiveInfo( int iShowText );
		void setVisible( bool visible );
};

class CVoteMapsMenu : public CMenuPanel
{
	private:
		int m_nCurrentDirection;
		int m_nCurrentPos;

		int	scroll_speed;
		int					m_nMaxPage;
		bool                m_bInit;
		Panel				*m_pPanel;
		
		
		Panel				*m_pMapPanel;
		Label				*m_pTitle;
		Label				*m_pBackround;
        Label				*m_BottomLable;
		CommandButton		*m_pBackButton;
        CommandButton		*m_pConsole;
        CommandButton		*m_pCancelButton;		
		Label				*m_pCenterLable;
		Label				*m_pPrevLable;
		Label				*m_pNextLable;
		CommandButton		*m_pPrevious;
		CommandButton		*m_pNext;
		CommandButton		*m_pNextMap;
		

		CImageLabel			*m_pPic;

       
		
		int					*m_iShowText;
		Label				*m_pText;
		SchemeHandle_t		*m_hText;
	public:
		
		Label				*m_pMapImage;
		CommandButton		*map[48]; 
		BitmapTGA			*pTGAMap[1024];
		BitmapTGA			*pTGANullMap;
		BitmapTGA			*pTGADownloadMap;
		bool m_bInitedButtons;
        void InItMapsButtons(void);
		void Scroll(int); // -1 -- back, 0 first page, 1 forward ++
		CVoteMapsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void ResetButtons();
		void setVisible( bool visible );
		virtual void Close();
};




class CClientMenu: public CMenuPanel
{
	private:
		
		int					m_nCurrentSpectators;
		int					m_nMaxPage;
		bool                m_bInit;
		Panel				*m_pPanel;
		Panel				*m_pPanelTop;
		Panel				*m_pPanelBottom;
		Panel				*m_pPanelLeft;		
		Panel				*m_pPanelRight;
		Panel				*m_pPanelCenter;
		Label				*m_pTitle;
		Panel				*m_pSPanel;
		Panel				*m_pSPanel2;
		Label				*m_pBackround;
        Label				*m_BottomLable;
		CommandButton		*m_pBackButton;
		CommandButton		*m_pConsole;
        CommandButton		*m_pRadar;
        CommandButton		*m_pCancelButton;		
		Label				*m_pCenterLable;
		Label				*m_pPrevLable;
		Label				*m_pNextLable;
		CommandButton		*m_pPrevious;
		CommandButton		*m_pNext;

		//ScrollPanel			*m_pScrollPanel;
	
		
//		Slider				*m_pHudColorSlider;
       
		CommandButton		*m_Player[PLAYERS_PER_PAGE];
		char				m_cPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME_LENGTH];
		int					*m_iShowText;
		Label				*m_pText;
		SchemeHandle_t		*m_hText;
		void ClearArray();
	public:

		ScrollBar			*m_pScrollBarLight;
		ScrollBar			*m_pScrollBarHue;
		Label				*m_pLightLable;
		ScrollBar			*m_pScrollBarSensitivity;
		//ScrollBar			*m_pScrollBarGaussColor;
		//Label				*m_pGaussColor;

		Label				*m_pSensitivityLable;
		Label				*m_pHueLable;

		CommandButton		*m_pNextColor;
		CommandButton		*m_pPrevColor;
		Label				*m_pGaussLabel;

		CommandButton		*m_pTimer;
		CommandButton		*m_pClock;
		CommandButton		*m_pLock;
		CommandButton		*m_pInvert;
		CommandButton		*m_pSmooth;
		Label				*m_pCrosshairs;
		CommandButton		*m_pCrossNext;
		CommandButton		*m_pCrossPrev;
		CommandButton		*m_pDynamic;
		CommandButton		*m_pSpriteBlend;
		CommandButton		*m_pSmoothCrosshairs;
		///CommandButton		*m_pRemove;
		CommandButton		*m_pWinamp;


		
		void UpdateClientMenu(void);
		CClientMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void ResetButtons();
		void SetActiveInfo( int iShowText );
		void setVisible( bool visible );

};




class CHelpMenu: public CMenuPanel
{
	private:
		
		Panel				*m_pPanel;
		Label				*m_pTitle;
		//int					m_nCommnadButtons;       
		TextPanel			*m_pBriefing;
		ScrollPanel			*m_pScrollPanel;
		CommandButton		*m_pCancelButton;
		CommandButton		*m_pReturnButton;
		Label				*m_pText;
		SchemeHandle_t		*m_hText;
	public:

		CHelpMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void Update();
		virtual void ResetButtons();
		virtual void SetActiveInfo( int iShowText );
		virtual void setVisible( bool visible );
};

class CMp3Player: public CMenuPanel
{
	Panel				*m_pPanel;
	Panel				*m_pTemp;
	Label				*m_pTitle;
	
	CommandButton		*m_pPlay;
	CommandButton		*m_pPause;
	CommandButton		*m_pStop;
	CommandButton			*m_pNextTrack;
	CommandButton			*m_pPrevTrack;
	CommandButton		*m_pClose;
	CommandButton		*m_pShuffel;
	CommandButton		*m_pRepeat;
	ActionButton		*test;
	
	Slider* pSlider;

public:

	ScrollBar			*m_pScrollVolume;
	Label				*m_pSong;
	CMp3Player(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
	virtual void ResetButtons();
	virtual void SetActiveInfo( int iShowText );
	virtual void setVisible( bool visible );
};


class CMenuHandler_SliderBar : public ActionSignal
{
private:
	int	m_iState;
public:
	CMenuHandler_SliderBar( int iState )
	{
		m_iState = iState;
	}

	virtual void actionPerformed(Panel* panel)
	{
		int x = m_iState;
	}
};


/*class CMenuHandler_MapPageChange : public InputSignal
{
protected:
	int m_nDirection;
public:

	CMenuHandler_MapPageChange( int nDir )
	{
		if(nDir==0 || nDir== -1 || nDir==1)
			m_nDirection = nDir;
		else
			nDir = 0;
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		//gViewPort->m_pVMapsMenu->ChangePage(m_nDirection);//(m_nDirection);
	}
	
	virtual void cursorEntered(Panel* panel)
	{
		gViewPort->m_pVMapsMenu->Scroll(m_nDirection);//(m_nDirection);
	};
	virtual void cursorExited(Panel* Panel)
	{
		gViewPort->m_pVMapsMenu->Scroll(0);//(m_nDirection);
	};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};*/


class CMenuHandler_VoteFunction: public InputSignal
{
protected:

	char	m_pszCommand[64];
	int		m_nIndex;
	int		m_iCloseVGUIMenu;
	bool	m_bSingleVote;
	CommandButton*  m_pButton;


public:

	CMenuHandler_VoteFunction(){}
	CMenuHandler_VoteFunction( char* pszVote, CommandButton* button, bool singleVote=false)
	{
		m_pButton = button;
		m_bSingleVote = singleVote;
		*m_pszCommand = NULL;
		strcpy(m_pszCommand, pszVote);
	}

	virtual void mousePressed(MouseCode code,Panel* panel){}
	//mouseReleased mouseDoublePressed mouseReleased
	virtual void cursorEntered(Panel* panel) {};
	virtual void cursorExited(Panel* Panel){};
	virtual void mouseReleased(MouseCode code,Panel* panel) 
	{
		if (m_pButton->getDisabled()==false) 
		{
			// change color of button
			if(m_bSingleVote==false)
				g_ClientVote.AddVote(m_pszCommand, m_pButton);
			else
				//g_ClientVote.CallSingleVote(m_pszCommand);
				g_ClientVote.AddVote(m_pszCommand, m_pButton, true);
		}
		
	};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel){};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};

extern CMapList g_MapsDownloaded;
extern int g_nMapPage;

class CMenuHandler_VoteMap: public CMenuHandler_VoteFunction
{
private:
	int m_nIndex;
	char level[32];
public:

	CMenuHandler_VoteMap( int index, CommandButton* button ) 
	{
		m_nIndex = index;
		m_pButton = button;
		*m_pszCommand = NULL;
		
	}

	virtual void mousePressed(MouseCode code,Panel* panel){}
	//mouseReleased mouseDoublePressed mouseReleased
	virtual void cursorEntered(Panel* panel) {};
	virtual void cursorExited(Panel* Panel){};
	virtual void mouseReleased(MouseCode code,Panel* panel) 
	{
		if(g_nMapPage*48 + m_nIndex >=g_MapsDownloaded.size())
			return;
		sprintf(level, "%s", g_MapsDownloaded[(g_nMapPage*48)+m_nIndex]->getName());
		// change color of button
		if(cl_nextmap->value!=0)
		{
			sprintf(m_pszCommand, "nextmap %s",  level);
		}
		else
			sprintf(m_pszCommand, "changelevel %s",  level);

		g_ClientVote.AddVote(m_pszCommand, m_pButton);
		
	};
		virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel){};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};



class CMenuHandler_VoteMode: public CMenuHandler_VoteFunction // undone 
{
private:
	char mode[32];
	bool click;
public:

	CMenuHandler_VoteMode( char* pszMode, CommandButton* button ) 
	{
		click = false;
		*mode =NULL;
		strcpy(mode, pszMode);
		m_pButton = button;
		*m_pszCommand = NULL;
		
	}

	virtual void mousePressed(MouseCode code,Panel* panel){}
	//mouseReleased mouseDoublePressed mouseReleased
	virtual void cursorEntered(Panel* panel) {};
	virtual void cursorExited(Panel* Panel){};
	virtual void mouseReleased(MouseCode code,Panel* panel) 
	{
		
		sprintf(m_pszCommand, "mode %s", mode);
		g_ClientVote.AddVote(m_pszCommand, m_pButton);
		
	};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel){};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};

//hle map pages

class CMenuHandler_MapCommand : public ActionSignal
{
protected:
	char	m_pszCommand[64];
	int		m_nIndex;
	int		m_iCloseVGUIMenu;

public:
	CMenuHandler_MapCommand( char* pszMap )
	{
		*m_pszCommand = NULL;
		if(pszMap)
			sprintf(m_pszCommand, "vote changelevel %s\n", pszMap);
	}

	virtual void actionPerformed(Panel* panel)
	{
		if(m_pszCommand)
			gEngfuncs.pfnClientCmd(m_pszCommand);
	}
};

class CMenuHandler_ClearVote : public ActionSignal
{
public:

	virtual void actionPerformed(Panel* panel)
	{
		g_nLockPreview = 0;
		cl_nextmap->value=0;
		g_ClientVote.ClearVote();
	}
};


class CVoteSpeedMenu : public CMenuPanel
{
	private:
		
		Panel	*m_pPanel;
		Label				*m_pTitle;
        CommandButton       *m_pConsole;
		CommandButton		*m_pCancelButton;
		CommandButton		*m_pBackButton;
        Label				*m_pBackround;
        Label				*m_BottomLable;

        CommandButton       *speed[3];

		int					*m_iShowText;
		Label				*m_pText;
	public:

		CVoteSpeedMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
		void ResetButtons();
		void SetActiveInfo( int iShowText );
		void setVisible( bool visible );
};



///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////// client settings  menu stuff//////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

class VGUIAPI ChangeHudHue: public IntChangeSignal
{

public:
	virtual void intChanged(int value, Panel* panel)
	{
		if(g_hud_color.m_bLockColors)
			return;
		char buf[32];
		 *buf=NULL;
		sprintf(buf, "hue %d\n", (value/6));
		gEngfuncs.pfnClientCmd(buf);

		////////////////////////////////
		g_hud_color.setRed(g_ColorTable[value][0]);
		g_hud_color.setBlue(g_ColorTable[value][1]);
		g_hud_color.setGreen(g_ColorTable[value][2]);
		gHUD.m_Ammo.ChangeXhairColor();

		//*m_pLightLable;
		//Label				*m_pHueLable;
		if(gViewPort->m_pClientMenu)
		{
			char szVgui[32];
			 *szVgui=NULL;
			sprintf(szVgui, "Hue: %i", (value/6));
			gViewPort->m_pClientMenu->m_pHueLable->setText ( szVgui );
			gHUD.m_Ammo.ChangeXhairColor();
		}

		//UpdateClientMenu
		
	}
};

class VGUIAPI ChangeHudLight: public IntChangeSignal
{

public:
	virtual void intChanged(int value, Panel* panel)
	{
		if(g_hud_color.m_bLockColors)
			return;
		char buf[32];
		*buf=NULL;
		float fvalue = (float)value/MAXCOLORS;
		sprintf(buf, "value %f\n", fvalue);
		gEngfuncs.pfnClientCmd(buf);
		////////////////////////////
		int midpoint = MAXCOLORS / 2;
		if(value > midpoint)	{ value = (value - midpoint)/3; }
		else { value = (midpoint -  value )/-3; }
		g_hud_color.setBrightness(value);
		gHUD.m_Ammo.ChangeXhairColor();

		if(gViewPort->m_pClientMenu)
		{
			char szVgui[32];
			//double dvalue = value;
			//dvalve/=255;
			float fValue (value/255.0f);
			int nValue = fValue*100;
			sprintf(szVgui, "Value: %i", nValue);
			strcat(szVgui, "%%");
			gViewPort->m_pClientMenu->m_pLightLable->setText ( szVgui );
			gHUD.m_Ammo.ChangeXhairColor();
			//gViewPort->m_pRadar->setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 96);
		}
		
		
	}
};


class VGUIAPI ChangeSensitivity: public IntChangeSignal
{

public:
	virtual void intChanged(int value, Panel* panel)
	{
		if(gViewPort->m_pClientMenu)
		{
			float fValue = value;
			char szVgui[64];
	
			sprintf(szVgui, "sensitivity %f\n", fValue/SENSITIVITY_SCALE);
			gEngfuncs.pfnClientCmd(szVgui);
		

			bool decimals = false;
			int j = 0;

			for(unsigned int i = 0; i < strlen(sensitivity->string); i ++)
			{
				if(decimals)
				{
					if(j>=1)
						sensitivity->string[i] = NULL;	
					j++;
				}

				if(sensitivity->string[i]=='.')
					decimals = true;
			}
			sprintf(szVgui, "Sensitivity: %s", sensitivity->string);
			gViewPort->m_pClientMenu->m_pSensitivityLable->setText( szVgui );
			
			
		}

	}
};



class VGUIAPI ChangeGaussColor: public ActionSignal
{
int m_nDirection;
public:
	ChangeGaussColor(int direction)
	{
		m_nDirection = direction;
	}

	virtual void actionPerformed(Panel* panel)
	{
		
		if(g_hud_color.m_bLockColors)
			return;

		if(gViewPort->m_pClientMenu)
		{
			char szVgui[64];
			int nGaussColor = cl_gausscolor->value;
			nGaussColor += m_nDirection;
			if(nGaussColor > 9)
				nGaussColor = 0;
			else if(nGaussColor < 0)
				nGaussColor = 9;

			sprintf(szVgui, "gauss %i\n", nGaussColor);
			g_GaussColor = nGaussColor;
			gEngfuncs.pfnClientCmd(szVgui);
			gViewPort->m_pClientMenu->m_pGaussLabel->setFgColor(  255, 255, 255, 0 );
			switch(nGaussColor)
			{
			case 0:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 255, 0, 0 );
				gViewPort->m_pClientMenu->m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 1:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 128, 0, 0 );
				break;
			case 2:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 0, 0, 0 );
				break;
			case 3:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 0, 128, 0 );
				break;
			case 4:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 0, 255, 0 );
				break;
			case 5:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  0, 0, 255, 0 );
				break;
			case 6:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  0, 255, 255, 0 );
				gViewPort->m_pClientMenu->m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 7:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  0, 255, 0, 0 );
				gViewPort->m_pClientMenu->m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 8:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 255, 255, 0 );
				gViewPort->m_pClientMenu->m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 9:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  0, 0, 0, 0 );
				gViewPort->m_pClientMenu->m_pGaussLabel->setFgColor(  255, 255, 255, 0 );
				break;
			default:
				gViewPort->m_pClientMenu->m_pGaussLabel->setBgColor(  255, 255, 0, 120 );
				break;
			}
			sprintf(szVgui, "Gauss Color: %i", nGaussColor);
			gViewPort->m_pClientMenu->m_pGaussLabel->setText(szVgui);

		}
	}
};


class VGUIAPI CMenuHandler_ToggleButton: public ActionSignal
{
protected:
	char	m_szSet[256];
	char	m_szReset[256];
	cvar_t* m_pCvar;
	Label* m_pLabel;
public:
	CMenuHandler_ToggleButton(Label* pLabel, char* pSet, char* pReset, cvar_t* pPvar)
	{
		m_pLabel = pLabel;
		m_pCvar = pPvar;
		*m_szSet= NULL;
		*m_szReset = NULL;
		if(pSet)
			strcpy(m_szSet, pSet);
		if(pReset)
			strcpy(m_szReset, pReset);
		
	}
	virtual void actionPerformed(Panel* panel)
	{
		char szVgui[256];
		if(m_pCvar->value!=0) // HLE XXX this crashes the client :)
		{
			m_pLabel->setText(m_szReset);
			sprintf(szVgui, "%s 0\n",m_pCvar->name);
			m_pCvar->value = 0; // hack to get the valve to set right away
			gEngfuncs.pfnClientCmd(szVgui);
		}
		else
		{
			m_pLabel->setText(m_szSet);
			sprintf(szVgui, "%s 1\n",m_pCvar->name);
			m_pCvar->value = 1; // hack to get the valve to set right away
			gEngfuncs.pfnClientCmd(szVgui);
		}
		
	}
};


class VGUIAPI CMenuHandler_ToggleButtonNoAction: public CMenuHandler_ToggleButton
{
	CommandButton* m_pButton;
public:
	CMenuHandler_ToggleButtonNoAction(CommandButton* pButton, char* pSet, char* pReset, cvar_t* pPvar) : CMenuHandler_ToggleButton(pButton, pSet, pReset, pPvar)
	{		
		m_pButton = pButton;
	}
	virtual void actionPerformed(Panel* panel)
	{
		char szVgui[256];
		if(m_pCvar->value!=0)
		{
			m_pButton->SetUnHightLightTextColor( 0, 200, 255, 0 ); 
			m_pButton->SetHightLightTextColor( 0, 200, 255, 0 ); 
			m_pLabel->setText(m_szReset);
			sprintf(szVgui, "%s 0\n",m_pCvar->name);
			m_pCvar->value = 0; // hack to get the valve to set right away
			gEngfuncs.pfnClientCmd(szVgui);
		}
		else
		{
			m_pButton->SetUnHightLightTextColor( 255, 150, 255, 0 ); 
			m_pButton->SetHightLightTextColor( 255, 150, 255, 0 );
			m_pLabel->setText(m_szSet);
			sprintf(szVgui, "%s 1\n",m_pCvar->name);
			m_pCvar->value = 1; // hack to get the valve to set right away
			gEngfuncs.pfnClientCmd(szVgui);
		}
		
	}
};

class CMenuHandler_ToggleButtonDynamic: public CMenuHandler_ToggleButton
{
public:

	CMenuHandler_ToggleButtonDynamic(Label* pLabel, char* pSet, char* pReset, cvar_t* pPvar) : CMenuHandler_ToggleButton(pLabel, pSet, pReset, pPvar)
	{
	}
	virtual void actionPerformed(Panel* panel)
	{
		CMenuHandler_ToggleButton::actionPerformed(panel);
		gHUD.m_Ammo.ChangeXhairColor();
	}
};


class VGUIAPI CMenuHandler_InvertMouse: public ActionSignal
{
protected:

	Label* m_pLabel;
public:

	CMenuHandler_InvertMouse(Label* pLabel)
	{
		m_pLabel = pLabel;
	}
	virtual void actionPerformed(Panel* panel)
	{
		char szVgui[64];
		m_pitch->value *=-1.0f;
		sprintf(szVgui, "m_pitch %f\n", m_pitch->value);
		gEngfuncs.pfnClientCmd(szVgui);
		

		if(m_pitch->value>0)
			m_pLabel->setText("Mouse Reverted");
		else
		
			m_pLabel->setText("Mouse Inverted");
		
		
	}
};






class VGUIAPI CMenuHandler_ChangeGUIMenu: public ActionSignal
{
	int m_nMenu;
public:
	CMenuHandler_ChangeGUIMenu(int nMenu)
	{
		m_nMenu = nMenu;
	}
	virtual void actionPerformed(Panel* panel)
	{
		gViewPort->ShowVGUIMenu(m_nMenu);
	}
};


class VGUIAPI CMenuHandler_ChangeCrosshairs: public ActionSignal
{
protected:

	Label* m_pLabel;
	int m_nDirection;
public:

	CMenuHandler_ChangeCrosshairs(Label* pLabel, int direction)
	{
		m_pLabel = pLabel;
		m_nDirection = direction;

	}
	virtual void actionPerformed(Panel* panel)
	{
		g_CrosshairIndex+=m_nDirection;
		if(g_CrosshairIndex >=g_Crosshairs.size() || g_CrosshairIndex >=MAX_CROSSHAIRS)
			g_CrosshairIndex = 0;
		else if(g_CrosshairIndex <0)
			g_Crosshairs.size() > MAX_CROSSHAIRS ? g_CrosshairIndex = MAX_CROSSHAIRS-1 : g_CrosshairIndex =  g_Crosshairs.size()-1;

		char buf[64];
		sprintf(buf, "xhairs %s\n", g_Crosshairs[g_CrosshairIndex]->getName());
		gEngfuncs.pfnClientCmd(buf);

		sprintf(buf, "X-hairs: %s", g_Crosshairs[g_CrosshairIndex]->getName());
		m_pLabel->setText(buf);
		gHUD.m_Ammo.ChangeXhairColor();
	}
};



class VGUIAPI CMenuHandler_ShowLock: public CMenuHandler_ToggleButton
{
protected:

	Label* m_pLabel;
public:

	CMenuHandler_ShowLock(Label* pLabel, char* pSet, char* pReset, cvar_t* pPvar) : CMenuHandler_ToggleButton( pLabel, pSet, pReset, pPvar)
	{
	}
	virtual void actionPerformed(Panel* panel)
	{
		
		CMenuHandler_ToggleButton::actionPerformed(panel);
		gHUD.m_Status.ToggleLock();
	}
};


class CMenuHandler_ToggleRadar : public ActionSignal
{
	Label* m_pLabel;
public:
	
	CMenuHandler_ToggleRadar(Label* label)
	{
		m_pLabel = label;
	}
	virtual void actionPerformed(Panel* panel)
	{
		char szVgui[256];
		if(cl_radar->value!=0)
		{
			//m_pButton->SetUnHightLightTextColor( 0, 200, 255, 0 ); 
			//m_pButton->SetHightLightTextColor( 0, 200, 255, 0 ); 
			m_pLabel->setText("Radar Off");
			sprintf(szVgui, "%s 0\n",cl_radar->name);
			cl_radar->value = 0; // hack to get the value to set right away
			gEngfuncs.pfnClientCmd(szVgui);
			gHUD.m_Radar.HideRadar();
//			gViewPort->HideRadar();
		}
		else
		{
			//m_pButton->SetUnHightLightTextColor( 255, 150, 255, 0 ); 
			//m_pButton->SetHightLightTextColor( 255, 150, 255, 0 );
			m_pLabel->setText("Radar On");
			sprintf(szVgui, "%s 1\n",cl_radar->name);
			cl_radar->value = 1; // hack to get the value to set right away
			gEngfuncs.pfnClientCmd(szVgui);
			//if(!gHUD.m_Radar.RadarOn())
			gHUD.m_Radar.ShowRadar();
		}
		
	}
};


class VGUIAPI CActionWinamp: public ActionSignal
{
protected:

	int m_nCommand;
	//Label* m_pLabel;
public:

	CActionWinamp(int command)
	{
	//	m_pLabel = pLabel;
		m_nCommand = command;
	}
	virtual void actionPerformed(Panel* panel)
	{
		char* track = gWinamp.Cmd_Winamp(m_nCommand);
		if(track)
		{
			gViewPort->m_pMp3->m_pSong->setText(track);
		}
		
	}
};

//3KYMOO2 guy who cut mom off

class VGUIAPI AdjustVolume: public IntChangeSignal
{

public:
	virtual void intChanged(int value, Panel* panel)
	{
		if(gViewPort->m_pMp3)
		{
			HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
			if(!hwWinamp)
			{
				return;
			}
			char szVgui[32];
			int volume = value /4;
			
			

			SendMessage(hwWinamp,WM_WA_IPC,volume,IPC_SETVOLUME);
			//SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON2, 0);
			float cvar_value = (float)volume*(100.0f/255.0f);
			sprintf(szVgui, "  %i", (int)cvar_value);
			strcat(szVgui, "%%");
			gViewPort->m_pMp3->m_pSong->setText(szVgui);
			gHUD.m_Winamp.SetVolume(cvar_value);
			
			cvar_value/=100.0f;
			gEngfuncs.Cvar_SetValue("mp3volume", cvar_value); // set in disk			
			cvar_t* m_mp3volume = gEngfuncs.pfnGetCvarPointer( "mp3volume" );
			m_mp3volume->value = cvar_value; // hacl to set right away in memory
			
			

		}
	}
};

class CMenuHandler_VolumeContorl : public InputSignal
{
protected:
	int m_nDirection;
public:

	CMenuHandler_VolumeContorl(  )
	{
		
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		//gViewPort->m_pVMapsMenu->ChangePage(m_nDirection);//(m_nDirection);
	}
	
	virtual void cursorEntered(Panel* panel)
	{
		//gViewPort->m_pVMapsMenu->Scroll(m_nDirection);//(m_nDirection);
	};
	virtual void cursorExited(Panel* Panel)
	{
		HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
		if(!hwWinamp)
		{
			return;
		}
		char* track = gWinamp.GetCurrentTrack(hwWinamp);
		if(track)
			gViewPort->m_pMp3->m_pSong->setText(track);
	
	};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};


class VGUIAPI CMenuHandler_ToggleMp3Command: public ActionSignal
{
protected:

	Color	m_cSet;
	Color	m_cReset;
	CommandButton* m_pLabel;
	int m_on, m_off;
	int m_state;
public:
	CMenuHandler_ToggleMp3Command(CommandButton* pLabel, Color* cSet, Color* cReset, int on, int off, int state) 
	{
		m_state = state;
		m_pLabel = pLabel;
		m_on = on;
		m_off = off;
		m_cSet = *cSet;


		m_cReset = *cReset;

		if(!m_state)
		{
			m_state = 0;
			m_pLabel->SetHightLightTextColor(m_cReset[0], m_cReset[1], m_cReset[2], m_cReset[3]);
			m_pLabel->SetUnHightLightTextColor(m_cReset[0], m_cReset[1], m_cReset[2], m_cReset[3]);
		}
		else
		{
			m_state = 1;
			m_pLabel->SetHightLightTextColor(m_cSet[0], m_cSet[1], m_cSet[2], m_cSet[3]);
			m_pLabel->SetUnHightLightTextColor(m_cSet[0], m_cSet[1], m_cSet[2], m_cSet[3]);
		}
	}
	virtual void actionPerformed(Panel* panel)
	{
		if(m_state)
		{
			m_state = 0;
			//m_pLabel->setFgColor(m_cReset[0], m_cReset[1], m_cReset[2], m_cReset[3]);
			m_pLabel->SetHightLightTextColor(m_cReset[0], m_cReset[1], m_cReset[2], m_cReset[3]);
			m_pLabel->SetUnHightLightTextColor(m_cReset[0], m_cReset[1], m_cReset[2], m_cReset[3]);
			gWinamp.Cmd_Winamp(m_off);
		}
		else
		{
			m_state = 1;
			//m_pLabel->setFgColor(m_cSet[0], m_cSet[1], m_cSet[2], m_cSet[3]);
			m_pLabel->SetHightLightTextColor(m_cSet[0], m_cSet[1], m_cSet[2], m_cSet[3]);
			m_pLabel->SetUnHightLightTextColor(m_cSet[0], m_cSet[1], m_cSet[2], m_cSet[3]);
			//m_pLabel->setFgColor(255, 0, 0, 100);
			gWinamp.Cmd_Winamp(m_on);		
		}
	}
};



class CMenuHandler_ChangeTrack : public InputSignal
{
protected:
	
	int m_Command;
public:

	CMenuHandler_ChangeTrack( int Dir )
	{
		m_Command = Dir;
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		gWinamp.Cmd_Winamp(m_Command+6);		
	}
	
	virtual void cursorEntered(Panel* panel);
	virtual void cursorExited(Panel* Panel);

	virtual void mouseReleased(MouseCode code,Panel* panel)
	{
	
		char* track = gWinamp.Cmd_Winamp(m_Command);
		if(track)
		{
			gViewPort->m_pMp3->m_pSong->setText(track);
		}

	}
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};


class VGUIAPI CMenuHandler_ChangePage: public ActionSignal
{
	int m_nDirection;
public:
	CMenuHandler_ChangePage(int direction)
	{
		m_nDirection = direction;
	}
	virtual void actionPerformed(Panel* panel)
	{
		gViewPort->m_pVMapsMenu->Scroll(m_nDirection);
	}
};

class VGUIAPI CMenuHandler_LockPreview: public ActionSignal
{
public:

	virtual void actionPerformed(Panel* panel)
	{
		if(g_nLockPreview)
			g_nLockPreview = 0;
		else
			g_nLockPreview = 1;
	}
};


class CMenuHandler_PictureOverMap : public InputSignal
{
protected:
	
	int index;

public:

	CMenuHandler_PictureOverMap( int Index )
	{
		index = Index;
	}

	virtual void mousePressed(MouseCode code,Panel* panel){};	
	virtual void cursorEntered(Panel* panel)
	{
		if(!g_nLockPreview)
		{
			if(index+(g_nMapPage*48) < g_MapsDownloaded.size())
			{
				CMapName* pMap = g_MapsDownloaded[index+(g_nMapPage*48)];
 				if(pMap->getLocal())
				{
					if(pMap->getPreview()>=0)
					{
						BitmapTGA* picture = gViewPort->m_pVMapsMenu->pTGAMap[pMap->getPreview()];
						if(!picture)
							gViewPort->m_pVMapsMenu->m_pMapImage->setImage( gViewPort->m_pVMapsMenu->pTGANullMap);
						else
							gViewPort->m_pVMapsMenu->m_pMapImage->setImage( picture );
					}
					
				}
				else
				{
					gViewPort->m_pVMapsMenu->m_pMapImage->setImage( gViewPort->m_pVMapsMenu->pTGADownloadMap);
				}
			}

			
		}
	};
	virtual void cursorExited(Panel* Panel){};
	virtual void mouseReleased(MouseCode code,Panel* panel){};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};

class CMenuHandler_TeamPictire: public InputSignal
{
protected:

	int		m_nIndex;

public:

	CMenuHandler_TeamPictire( int index)
	{
		m_nIndex = index;
	}

	virtual void mousePressed(MouseCode code,Panel* panel){}
	virtual void cursorEntered(Panel* panel) 
	{
		
		if(!stricmp(gViewPort->GetTeamName(m_nIndex+1), "blue"))
			gViewPort->m_pTeamMenu->m_pPictureLabel->setImage(gViewPort->m_pTeamMenu->m_pTeamPics[0]);
		if(!stricmp(gViewPort->GetTeamName(m_nIndex+1), "red"))
			gViewPort->m_pTeamMenu->m_pPictureLabel->setImage(gViewPort->m_pTeamMenu->m_pTeamPics[1]);
		if(!stricmp(gViewPort->GetTeamName(m_nIndex+1), "green"))
			gViewPort->m_pTeamMenu->m_pPictureLabel->setImage(gViewPort->m_pTeamMenu->m_pTeamPics[2]);
		if(!stricmp(gViewPort->GetTeamName(m_nIndex+1), "yellow"))
			gViewPort->m_pTeamMenu->m_pPictureLabel->setImage(gViewPort->m_pTeamMenu->m_pTeamPics[3]);
		if(!stricmp(gViewPort->GetTeamName(m_nIndex+1), "purple"))
			gViewPort->m_pTeamMenu->m_pPictureLabel->setImage(gViewPort->m_pTeamMenu->m_pTeamPics[4]);
		if(!stricmp(gViewPort->GetTeamName(m_nIndex+1), "orange"))
			gViewPort->m_pTeamMenu->m_pPictureLabel->setImage(gViewPort->m_pTeamMenu->m_pTeamPics[5]);
		
	};
	virtual void cursorExited(Panel* Panel){};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel){};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

};


//gEngfuncs.GetMousePosition( mx, my );

/*
WINAMP_FFWD5S
WINAMP_REW5S*/

#endif