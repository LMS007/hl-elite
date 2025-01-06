#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hle_menu.h"
#include "pm_shared.h"
#include "..\common\hltv.h"
#include"vgui_ConsolePanel.h"
#include "vgui_scheme.h"
#include "maps.h"
#include "vgui_hle_menu.h"
#include<VGUI_ScrollPanel.h>
#include<VGUI_ScrollBar.h>
#include<VGUI_Slider.h>
#include<VGUI_IntchangeSignal.h>// NTCHANGESIGNAL_H
#include "hle_maploader.h"
#include <fstream>


#include "windows.h"
#include "winamp.h"

int g_Weight[20];
char g_szWeight[64];


ClientVote g_ClientVote;
//extern cvar_t* cl_weight;

#define MAPS_PER_PAGE 48
int g_nMapPage = 0;


extern cvar_t	*scr_ofsx;
extern cvar_t	*scr_ofsy;
extern cvar_t	*scr_ofsz;
extern cvar_t	*cl_showclock;//hle
extern cvar_t	*cl_showtimer;//hle
extern cvar_t	*cl_showlock;//hle
extern cvar_t	*cl_crosshairs;//hle
extern cvar_t	*cl_dynamic_xhrs;
extern cvar_t	*cl_scrollspeed;
extern cvar_t	*m_pitch;
extern cvar_t	*cl_radar;
extern	cvar_t	*m_filter;
extern cvar_t* sensitivity;
extern cvar_t* cl_gausscolor;

extern int g_isInSpectateMode =0;
extern "C" int g_TeamplayClient;
extern cvar_t* cl_hudhue;
extern cvar_t* cl_hudvalue;

//int g_ColorTable[MAXCOLORS][3];
int g_ColorTable[MAXCOLORS][3];
int g_GaussColor;
extern CHudColor g_hud_color;
extern CWinamp gWinamp;
extern cvar_t	matchstart;

int g_nLockPreview = 0;


char* RemoveTabs(char* pszString)
{
	if(pszString)
	{
		while(*pszString=='\t')
			*pszString++;
		while(*pszString==' ')
			*pszString++;

		int length = strlen(pszString);
		for(int i = 0; i < length; i++)
		{
			if(pszString[i]=='\t')
			{
				pszString[i] =' ' ;
			}
		}
	}
	return pszString;
}

/*
void CMainMenu :: SetActiveInfo( int iShowText )
{
	if (iShowText == 0)
	{
		m_pText->setVisible( false );
	}
	else
	{
		m_pText->setVisible( true );
	}
}*/


HLEMenuButton::HLEMenuButton(Panel* parent, const char* text,int x,int y,int wide,int tall ) : CommandButton( text, x, y, wide, tall)
{
	//Init();
	//setUnHighLightColor(0, 0, 0, 200);
	//setText( text );
	setParent(parent);
	setContentAlignment( vgui::Label::a_west );
	setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );

	SetUnHightLightTextColor(255,255,255, 100);
	SetHightLightTextColor(255,255,255, 0);
}

CHLEMenu::CHLEMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
	bool _iIsInObserverMode = false;
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	m_pFontButtons = pSchemes->getFont(pSchemes->getSchemeHandle( "HLE menu default" ));
	m_pFontButtonsLarge = pSchemes->getFont(pSchemes->getSchemeHandle( "HLE menu large" ));
	m_pFontTitle = pSchemes->getFont(pSchemes->getSchemeHandle( "HLE menu title" ));


	// background color
	m_pBackround = new Panel( 0, 0, ScreenWidth, ScreenHeight);
	m_pBackround->setParent( this );
	m_pBackround->setBgColor( 0, 0, 0, 125);
}


void CHLEMenu :: ResetButtons(void)
{
	int size = m_pPanel->getChildCount();
	int i = 0;
	for(i = 0; i < size; i++)
	{
		((CommandButton*)m_pPanel->getChild(i))->cursorExited();
	}
}

void CMainMenu :: setVisible( bool visible )
{

	//if(visible)
		//PlaySound("common\\wpn_hudon.wav",1);
	CMenuPanel::setVisible(visible);
	if(!g_TeamplayClient) {
		m_pChangeTeam->setDisabled(true);
	}
	else {
		m_pChangeTeam->setDisabled(false);
	}
}


CMainMenu :: CMainMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CHLEMenu(iTrans, iRemoveMe, x,y,wide,tall)
{
	/*
	bool _iIsInObserverMode = false;
	
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	Font* pFontButtons = pSchemes->getFont(pSchemes->getSchemeHandle( "VGUI_MAIN" ));
	Font* titleFont = pSchemes->getFont(pSchemes->getSchemeHandle( "VGUI_TITLE" ));

	// background color
	Panel* bgpanel = new Panel( 0, 0, ScreenWidth, ScreenHeight);
	bgpanel->setParent( this );
	bgpanel->setBgColor( 0, 0, 0, 100);
	*/
	
	// main menu panel
	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (600.0f / 2.0f), (ScreenHeight/ 2.0f) - (258.0f / 2.0f), (600), (258));
	m_pPanel->setParent( m_pBackround );
	m_pPanel->setBgColor( 0, 0, 0, 100);

	// title
	TextPanel* pTitle = new TextPanel( "Main Menu", 26, 6, 588, 30 );
	pTitle->setParent( m_pPanel );
	pTitle->setFgColor(255, 255, 255, 0); //hle
	pTitle->setFont(m_pFontTitle);
    pTitle->setBgColor( 0, 0, 0, 255 );


	HLEMenuButton* modes = new HLEMenuButton(m_pPanel, "Vote Modes", (6), (42), (195), (29));
	modes->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	modes->addActionSignal( new CMenuHandler_ChangeGUIMenu( 31 ) );
	modes->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	modes->setFont(m_pFontButtons);
	

	HLEMenuButton* maps = new HLEMenuButton(m_pPanel, "Vote Maps", (6), (72), (195), (29));
	
	maps->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	maps->addActionSignal( new CMenuHandler_ChangeGUIMenu( 33 ) ); 
	maps->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	maps->setFont(m_pFontButtons);

	HLEMenuButton* settings = new HLEMenuButton(m_pPanel, "Vote Settings", (6), (102), (195), (29));
	settings->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );	
	settings->addActionSignal( new CMenuHandler_ChangeGUIMenu(  32 ) ); 
	settings->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	settings->setFont(m_pFontButtons);
	settings->RecalculateText();
	
	HLEMenuButton* match = new HLEMenuButton(m_pPanel, "Vote Match", (6), (132), (195), (29));
	match->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );	
	match->addActionSignal( new CMenuHandler_ChangeGUIMenu( MENU_MATCH ) ); 
	match->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	match->setFont(m_pFontButtons);

	HLEMenuButton* yes = new HLEMenuButton(m_pPanel, "Accept Vote", (202), (42), (195), (59));
    yes->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	yes->addActionSignal( new CMenuHandler_StringCommand( "yes" ) ); 
	yes->setHighLightColor( 50, 255, 100,150 );
	yes->setUnHighLightColor( 50, 255, 100, 225 );
	yes->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	yes->setFont(m_pFontButtons);

	HLEMenuButton* no = new HLEMenuButton(m_pPanel, "Decline Vote", (202), (102), (195), (59));
    no->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	no->addActionSignal( new CMenuHandler_StringCommand( "no" ) ); 
	no->setHighLightColor( 255, 50, 50, 150 );
	no->setUnHighLightColor( 255, 50, 50, 225 );
	no->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	no->setFont(m_pFontButtons);

	// client settings
	HLEMenuButton* hud = new HLEMenuButton(m_pPanel, "HUD", (398), (42), (195), (29));
	hud->setParent( m_pPanel );
	hud->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	hud->addActionSignal( new CMenuHandler_ChangeGUIMenu( 35 ) );
	hud->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	hud->setFont(m_pFontButtons);


	m_pChangeTeam = new HLEMenuButton(m_pPanel, "Change Team", (398), (72), (195), (29));
	m_pChangeTeam->setParent( m_pPanel );
	m_pChangeTeam->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );	
	m_pChangeTeam->addActionSignal( new CMenuHandler_ChangeGUIMenu( 2 ) ); 
	m_pChangeTeam->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	m_pChangeTeam->setFont(m_pFontButtons);
	

	HLEMenuButton* remove = new HLEMenuButton(m_pPanel, "Remove from play", (398), (102), (195), (29));
	remove->setContentAlignment( vgui::Label::a_west );
	remove->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	remove->addActionSignal( new CMenuHandler_StringCommand("ejectme") );
	remove->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	remove->setFont(m_pFontButtons);

	HLEMenuButton* spectate = new HLEMenuButton(m_pPanel, "Spectate", (398), (132), (195), (29));
	spectate->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	spectate->addActionSignal( new CMenuHandler_StringCommand( "spectate" ) );  
	spectate->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	spectate->setFont(m_pFontButtons);

	
	ExitButton *exit = new ExitButton(m_pPanel,(566), (7), (24), (24));
	exit->setContentAlignment( vgui::Label::a_center );
	exit->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	exit->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	exit->setImage( LoadTGAForRes("exit"));


	/*m_pWinamp = new ExitButton(m_pPanel,(566), (7), (28), (28));
	m_pWinamp->setContentAlignment( vgui::Label::a_center );
	m_pWinamp->setParent( m_pPanel );
	m_pWinamp->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pWinamp->addActionSignal( new CMenuHandler_ChangeGUIMenu( MENU_MP3 ) );
	m_pWinamp->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pWinamp->setUnHighLightColor( 0, 0, 0, 255 );
	m_pWinamp->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	m_pWinamp->setFont(m_pFontButtons);
	//m_pWinamp->setImage( LoadTGAForRes("bolt"));
	m_pWinamp->setImage( LoadTGAForRes("exit"));*/

	HLEMenuButton* play = new HLEMenuButton(m_pPanel, "Play", (6), (162), (587), (59));
	play->setContentAlignment( vgui::Label::a_center );
	play->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	play->addActionSignal( new CMenuHandler_StringCommand( "-spectate" ) );  
	play->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	play->setFont(m_pFontButtonsLarge);
	play->RecalculateText();
	

	HLEMenuButton* cancel = new HLEMenuButton(m_pPanel, gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (398), (222), (195), (29));
	cancel->setContentAlignment(vgui::Label::a_west);
	cancel->addActionSignal(  new CMenuHandler_ClearVote() );
	cancel->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );	
	cancel->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	cancel->setUnHighLightColor( 30, 30, 40, 100 );
	cancel->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	//cancel->addActionSignal( new CMenuHandler_SoundCommand("common/bodysplat.wav"));
	cancel->setFont(m_pFontButtons);

	HLEMenuButton* info = new HLEMenuButton(m_pPanel, gHUD.m_TextMessage.BufferedLocaliseTextString( "Show Server Settings" ), (6), (222), (195), (29));
	info->addActionSignal( new CMenuHandler_StringCommand( "info" ) ); 
	info->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	info->addActionSignal( new CMenuHandler_SoundCommand("common/clickbutton.wav"));
	info->setUnHighLightColor( 30, 30, 40, 100 );
	info->setFont(m_pFontButtons);

	HLEMenuButton* help = new HLEMenuButton(m_pPanel, gHUD.m_TextMessage.BufferedLocaliseTextString( "Help Menu" ), (202), (222), (195), (29));
	help->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	help->addActionSignal(  new CMenuHandler_ClearVote() );
	help->setContentAlignment( vgui::Label::a_center );
	help->addActionSignal( new CMenuHandler_ChangeGUIMenu( 36 ) );
	//help->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	help->setUnHighLightColor( 30, 30, 40, 100 );
	help->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	help->setFont(m_pFontButtons);
}




void CVoteMatchMenu :: setVisible( bool visible )
{
	CMenuPanel::setVisible(visible);
	if(!gHUD.m_Status.m_matchInProgress) {
		m_cancel_match->setDisabled(true);
	}
	else {
		m_cancel_match->setDisabled(false);
	}
}
CVoteMatchMenu :: CVoteMatchMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CHLEMenu(iTrans, iRemoveMe, x,y,wide,tall)
{

	// main menu panel
	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (462.0f / 2.0f), (ScreenHeight/ 2.0f) - (108.0f / 2.0f), (600), (138));
	m_pPanel->setParent( m_pBackround );
	m_pPanel->setBgColor( 0, 0, 0, 100);

	// title
	TextPanel* pTitle = new TextPanel( "Match Menu", 26, 6, 588, 30 );
	pTitle->setParent( m_pPanel );
	pTitle->setFgColor(255, 255, 255, 0);
	pTitle->setFont(m_pFontTitle);
    pTitle->setBgColor( 0, 0, 0, 255 );
   
                
	HLEMenuButton* match = new HLEMenuButton( m_pPanel, "Match Start", 6, 42, 195, 59);
	match->addInputSignal( new CMenuHandler_VoteFunction("matchstart 1", match, true));
	match->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	match->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	match->setFont(m_pFontButtons);

    HLEMenuButton* match2 = new HLEMenuButton( m_pPanel, "Match Start (Loaded)", 202, 42, 195, 59);
	match2->addInputSignal( new CMenuHandler_VoteFunction("matchstart 2", match2, true));
	match2->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	match2->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	match2->setFont(m_pFontButtons);

	m_cancel_match = new HLEMenuButton( m_pPanel, "Cancel Match", 398, 42, 195, 59);
	m_cancel_match->addInputSignal( new CMenuHandler_VoteFunction("matchstart 0", m_cancel_match, true));
	m_cancel_match->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_cancel_match->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_cancel_match->setFont(m_pFontButtons);

	// standard buttons

	HLEMenuButton* back = new HLEMenuButton( m_pPanel,  "Return", 6, 102, 195, 29);
	back->setParent( m_pPanel );
	back->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	back->addActionSignal( new CMenuHandler_ChangeGUIMenu( MENU_MAIN ) );
	back->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	back->setUnHighLightColor( 30, 30, 40, 100 );
	back->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	back->setFont(m_pFontButtons);

	HLEMenuButton* clear = new HLEMenuButton(m_pPanel, "Clear Selection", 202, 102, 195, 29);
	clear->addActionSignal(  new CMenuHandler_ClearVote() );
	clear->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	clear->setUnHighLightColor( 30, 30, 40, 100 );
	clear->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	clear->setFont(m_pFontButtons);

	HLEMenuButton *cancel= new HLEMenuButton(m_pPanel, "Cancel", 398, 102, 195, 29);
	cancel->addActionSignal(  new CMenuHandler_ClearVote() );
	cancel->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	cancel->setUnHighLightColor( 30, 30, 40, 100 );
	cancel->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	cancel->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	cancel->setFont(m_pFontButtons);

}
/////////////////////////////////////////////////////////////
////////// SETTINGS VOTE MENU////////////////////////////////
/////////////////////////////////////////////////////////////


/*
CVoteSettingsMenu :: CVoteSettingsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{

	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (600.0f / 2.0f), (ScreenHeight/ 2.0f) - (228.0f / 2.0f), (600), (228));
	m_pPanel->setParent( m_pBackround );
	m_pPanel->setBgColor( 0, 0, 0, 100);

	// title
	TextPanel* pTitle = new TextPanel( "Server Settings Menu", 26, 6, 588, 30 );
	pTitle->setParent( m_pPanel );
	pTitle->setFgColor(255, 255, 255, 0); //hle
	pTitle->setFont(m_pFontTitle);
    pTitle->setBgColor( 0, 0, 0, 255 );
    
	HLEMenuButton* button;
	button = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Add to Timelimit" ),  (6), (42), (195), (29));//
	button->setContentAlignment(vgui::Label::a_west);
	button->setParent( m_pPanel );
    //m_pCommand[0]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	button->addInputSignal( new CMenuHandler_VoteFunction("addtime", m_pCommand[0]));
	m_pCommand[0]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[0]->setFont(pFontButtons);
	

	m_pCommand[1] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Add to Round Clock" ),  (6), (72), (195), (29));//
	m_pCommand[1]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[1]->setParent( m_pPanel );
	m_pCommand[1]->addInputSignal( new CMenuHandler_VoteFunction("addclock 60", m_pCommand[1]));
    m_pCommand[1]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[1]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[1]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[1]->setFont(pFontButtons);


	m_pCommand[2] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Game Speed" ),(6), (102), (195), (29));//
	m_pCommand[2]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[2]->setParent( m_pPanel );
    m_pCommand[2]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCommand[2]->addActionSignal( new CMenuHandler_ChangeGUIMenu( 34) );
	m_pCommand[2]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[2]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[2]->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	m_pCommand[2]->setFont(pFontButtons);

	m_pCommand[3] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Spawn Form" ),  (6), (132), (195), (29));//
	m_pCommand[3]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[3]->setParent( m_pPanel );
    //m_pCommand[3]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[3]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_footsteps") );
	m_pCommand[3]->addInputSignal( new CMenuHandler_VoteFunction("mp_spawnform", m_pCommand[3]));
	m_pCommand[3]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[3]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[3]->setFont(pFontButtons);

	m_pCommand[4] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Weapon Stay" ), (6), (162), (195), (29));//
	m_pCommand[4]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[4]->setParent( m_pPanel );
	m_pCommand[4]->addInputSignal( new CMenuHandler_VoteFunction("mp_weaponstay", m_pCommand[4]));
	m_pCommand[4]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[4]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[4]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[4]->setFont(pFontButtons);

	m_pCommand[5] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Force Respawn" ), (202), (42), (195), (29));//
	m_pCommand[5]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[5]->setParent( m_pPanel );
	//m_pCommand[5]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[5]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_weaponstay") ); // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[5]->addInputSignal( new CMenuHandler_VoteFunction("mp_forcerespawn", m_pCommand[5]));
    m_pCommand[5]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[5]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[5]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[5]->setFont(pFontButtons);

	m_pCommand[6] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Fall Damage" ), (202), (72), (195), (29));//
	m_pCommand[6]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[6]->setParent( m_pPanel );
	//m_pCommand[6]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[6]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_forcerespawn") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[6]->addInputSignal( new CMenuHandler_VoteFunction("mp_falldamage", m_pCommand[6]));
    m_pCommand[6]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[6]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[6]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[6]->setFont(pFontButtons);

	m_pCommand[7] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Friendly Fire" ),  (202), (102), (195), (29));//
	m_pCommand[7]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[7]->setParent( m_pPanel );
	//m_pCommand[7]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[7]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_falldamage") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[7]->addInputSignal( new CMenuHandler_VoteFunction("mp_friendlyfire", m_pCommand[7]));
    m_pCommand[7]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[7]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[7]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[7]->setFont(pFontButtons);

	m_pCommand[8] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Foot Steps" ),  (202), (132), (195), (29));
	m_pCommand[8]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[8]->setParent( m_pPanel );
	//m_pCommand[8]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[8]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_friendlyfire") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[8]->addInputSignal( new CMenuHandler_VoteFunction("mp_footsteps", m_pCommand[8]));
    m_pCommand[8]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[8]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[8]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[8]->setFont(pFontButtons);
 
	m_pCommand[9] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Satcheles' Explode" ), (202), (162), (195), (29));
	m_pCommand[9]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[9]->setParent( m_pPanel );
	m_pCommand[9]->addInputSignal( new CMenuHandler_VoteFunction("mp_satchelexplode", m_pCommand[9]));
    m_pCommand[9]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[9]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[9]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[9]->setFont(pFontButtons);


	m_pCommand[10] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Match Start" ), (398), (42), (195), (29));
	m_pCommand[10]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[10]->setParent( m_pPanel );
	//m_pCommand[10]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[10]->addActionSignal( new CMenuHandler_StringCommand( "vote matchstart 1") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[10]->addInputSignal( new CMenuHandler_VoteFunction("matchstart 1", m_pCommand[10]));
    m_pCommand[10]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[10]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[10]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[10]->setFont(pFontButtons);

	m_pCommand[11] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Match Start Stocked" ), (398), (72), (195), (29));
	m_pCommand[11]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[11]->setParent( m_pPanel );
	//m_pCommand[11]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[11]->addActionSignal( new CMenuHandler_StringCommand( "vote matchstart 2") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[11]->addInputSignal( new CMenuHandler_VoteFunction("matchstart 2", m_pCommand[11]));
    m_pCommand[11]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[11]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[11]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[11]->setFont(pFontButtons);

	m_pCommand[12] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel Match" ), (398), (102), (195), (29));
	m_pCommand[12]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[12]->setParent( m_pPanel );
	//m_pCommand[12]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[12]->addActionSignal( new CMenuHandler_StringCommand( "vote matchstart 0") ); 
	m_pCommand[12]->addInputSignal( new CMenuHandler_VoteFunction("matchstart 0", m_pCommand[12]));
    m_pCommand[12]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[12]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[12]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[12]->setFont(pFontButtons);

	m_pCommand[13] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Toss Items" ), (398), (132), (195), (29));	
	m_pCommand[13]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[13]->setParent( m_pPanel );
	//pPauseResume->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[13]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_tossitem") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[13]->addInputSignal( new CMenuHandler_VoteFunction("mp_tossitem", m_pCommand[13]));
    m_pCommand[13]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[13]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[13]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[13]->setFont(pFontButtons);

	m_pCommand[14] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Lose Longjump" ), (398), (162), (195), (29));	
	m_pCommand[14]->setContentAlignment(vgui::Label::a_west);
	m_pCommand[14]->setParent( m_pPanel );
	//m_pCommand[14]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCommand[14]->addActionSignal( new CMenuHandler_StringCommand( "vote mp_loselongjump") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
	m_pCommand[14]->addInputSignal( new CMenuHandler_VoteFunction("mp_loselongjump", m_pCommand[14]));
    m_pCommand[14]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCommand[14]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCommand[14]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCommand[14]->setFont(pFontButtons);

	// satch explode
	// autojump
	// lock teams
	// "mp_showlongjump"	
	*/
	/*
	blank = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( " " ), XRES(315), YRES(229), XRES(149), YRES(45));	
	blank->setContentAlignment(vgui::Label::a_west);
	blank->setParent( m_pPanel );
//	blank->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
//	blank->addActionSignal( new CMenuHandler_StringCommand( "vote mp_tossitem") ); // // it will automaticly vote the oppisit of the server setting 1 or 0; // it will automaticly vote the oppisit of the server setting 1 or 0
  //  blank->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
//	blank->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
//	blank->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	blank->setHighLightColor( 0, 0, 0, 255 );
	blank->setUnHighLightColor( 0, 0, 0, 255 );
	blank->setFont(pFontButtons);*/
/*
	m_pBackButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Return" ), (6), (192), (195), (29));
	m_pBackButton->setContentAlignment(vgui::Label::a_west);
	m_pBackButton->setParent( m_pPanel );
	m_pBackButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pBackButton->addActionSignal( new CMenuHandler_ChangeGUIMenu( 30 ) );
	m_pBackButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pBackButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pBackButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pBackButton->setFont(pFontButtons);


	m_pConsole = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Clear Selection" ), (202), (192), (195), (29));
	m_pConsole->setContentAlignment(vgui::Label::a_west);
	m_pConsole->setParent( m_pPanel );
	//m_pConsole->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pConsole->addActionSignal( new CMenuHandler_StringCommand( "toggleconsole") ); //toggleconsole
	m_pConsole->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pConsole->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pConsole->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pConsole->setUnHighLightColor( 30, 30, 40, 100 );
	m_pConsole->setFont(pFontButtons);
	m_pConsole->RecalculateText();

	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (398), (192), (195), (29));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pPanel );
	m_pCancelButton->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCancelButton->setFont(pFontButtons);

}
*/
/*
void CVoteSettingsMenu :: ResetButtons(void)
{
    
	m_pCancelButton->cursorExited();
	m_pBackButton->cursorExited();
    m_pConsole->cursorExited();
	
	for(int i = 0;i<15;i++)
		m_pCommand[i]->cursorExited();
}



void CVoteSettingsMenu :: SetActiveInfo( int iInfo )
{
	CMenuPanel::SetActiveInfo(iInfo);
}


void CVoteSettingsMenu :: setVisible( bool visible )
{
	CMenuPanel::setVisible(visible);
}


void CVoteSettingsMenu::getTextBuffer()
{
    //setText(const char* text,int textLen);
	//virtual void getText(int offset,char* buf,int bufLen);
    char buf[512];
	m_pEditbox->getText(0,buf,512);
    //set some global text char* pointer the the buffer in the text box
    // atoi(that number)
    // change vote... under client.cpp so that if its time or frag or winlimit.. the second parameter is that gloabal!
}
*/
CVoteModesMenu :: CVoteModesMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (462.0f / 2.0f), (ScreenHeight/ 2.0f) - (258.0f / 2.0f), (462), (258));
	m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pPanel->setBgColor( 0, 0, 0, 120);
    
    m_pTitle = new Label( "", (6), (6), (450), (30) );
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setBorder( new LineBorder( Color(0,0,0,0)));
    m_pTitle->setBgColor( 0, 0, 0, 255 );
	m_pTitle->setImage( LoadTGAForRes("modesmenu"));
	m_pTitle->setContentAlignment( vgui::Label::a_center );

	m_pBrawl = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Free For All" ), (6), (42), (224), (29));
	m_pBrawl->setContentAlignment(vgui::Label::a_west);
	m_pBrawl->setParent( m_pPanel );
	//m_pBrawl->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pBrawl->addActionSignal( new CMenuHandler_StringCommand( "vote mode ffa") ); 
	m_pBrawl->addInputSignal( new CMenuHandler_VoteMode("ffa", m_pBrawl));
    m_pBrawl->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pBrawl->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pBrawl->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pBrawl->setFont(pFontButtons);


 
	



	m_pDuel = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Duel" ), (6), (72), (224), (29));
	m_pDuel->setContentAlignment(vgui::Label::a_west);
	m_pDuel->setParent( m_pPanel );
	//m_pDuel->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pDuel->addActionSignal( new CMenuHandler_StringCommand( "vote mode duel") ); 
	m_pDuel->addInputSignal( new CMenuHandler_VoteMode("duel", m_pDuel));
    m_pDuel->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pDuel->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pDuel->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pDuel->setFont(pFontButtons);


  
	m_pTournament  = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Tournament" ), (6), (102), (224), (29));
	m_pTournament ->setContentAlignment(vgui::Label::a_west);
	m_pTournament ->setParent( m_pPanel );
	m_pTournament->addInputSignal( new CMenuHandler_VoteMode("tournament", m_pTournament));
    m_pTournament->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pTournament->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pTournament->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pTournament->setFont(pFontButtons);

	m_pRiot = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Action" ), (6), (132), (224), (29));
	m_pRiot->setContentAlignment(vgui::Label::a_west);
	m_pRiot->setParent( m_pPanel );
	m_pRiot->addInputSignal( new CMenuHandler_VoteMode("action", m_pRiot));
    m_pRiot->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pRiot->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pRiot->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pRiot->setFont(pFontButtons);

	m_pLMS = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Last Man Standing" ), (6), (162), (224), (29));
	m_pLMS->setContentAlignment(vgui::Label::a_west);
	m_pLMS->setParent( m_pPanel );
	//m_pLMS->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pLMS->addActionSignal( new CMenuHandler_StringCommand( "vote mode lms") ); 
	m_pLMS->addInputSignal( new CMenuHandler_VoteMode("lms", m_pLMS));
    m_pLMS->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pLMS->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pLMS->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pLMS->setFont(pFontButtons);


 
	m_pMatch = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Team Play" ), (231), (42), (224), (29));
	m_pMatch->setContentAlignment(vgui::Label::a_west);
	m_pMatch->setParent( m_pPanel );
	//m_pMatch->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
   	//m_pMatch->addActionSignal( new CMenuHandler_StringCommand( "vote mode team") ); 
	m_pMatch->addInputSignal( new CMenuHandler_VoteMode("teamplay", m_pMatch));
    m_pMatch->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pMatch->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pMatch->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pMatch->setFont(pFontButtons);


  
	m_pCTF = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Capture The Flag" ), (231), (72), (224), (29));
	m_pCTF->setContentAlignment(vgui::Label::a_west);
	m_pCTF->setParent( m_pPanel );
	//m_pCTF->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pCTF->addActionSignal( new CMenuHandler_StringCommand( "vote mode ctf") ); 
	m_pCTF->addInputSignal( new CMenuHandler_VoteMode("ctf", m_pCTF));
    m_pCTF->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCTF->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCTF->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCTF->setFont(pFontButtons);

 
	m_pHunt = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Capture The Point" ), (231), (102), (224), (29));
	m_pHunt->setContentAlignment(vgui::Label::a_west);
	m_pHunt->setParent( m_pPanel );
	m_pHunt->addInputSignal( new CMenuHandler_VoteMode("ctp", m_pHunt));
    m_pHunt->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pHunt->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pHunt->setFont(pFontButtons);


	m_pTAction = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Team Action" ), (231), (132), (224), (29));
	m_pTAction->setContentAlignment(vgui::Label::a_west);
	m_pTAction->setParent( m_pPanel );
	m_pTAction->addInputSignal( new CMenuHandler_VoteMode("teamaction", m_pTAction));
    m_pTAction->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pTAction->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pTAction->setFont(pFontButtons);

	m_pTLMS = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Last Team Standing" ), (231), (162), (224), (29));
	m_pTLMS->setContentAlignment(vgui::Label::a_west);
	m_pTLMS->setParent( m_pPanel );
	m_pTLMS->addInputSignal( new CMenuHandler_VoteMode("lts", m_pTLMS));
    m_pTLMS->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pTLMS->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pTLMS->setFont(pFontButtons);


	m_pPratice = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Practice" ), (6), (192), (224), (29));
	m_pPratice->setContentAlignment(vgui::Label::a_west);
	m_pPratice->setParent( m_pPanel );
	m_pPratice->addInputSignal( new CMenuHandler_VoteMode("practice", m_pPratice));
    m_pPratice->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pPratice->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pPratice->setFont(pFontButtons);

	m_pEmpty = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Coming Soon!" ), (231), (192), (224), (29));
	m_pEmpty->setContentAlignment(vgui::Label::a_west);
	m_pEmpty->setParent( m_pPanel ); 
	m_pEmpty->setHighLightColor( 0, 0, 0, 255 );
	m_pEmpty->setUnHighLightColor( 0, 0, 0, 255 );
	m_pEmpty->SetHightLightTextColor(255,0,0,0);
	m_pEmpty->SetUnHightLightTextColor(150,150,150,0);
	m_pEmpty->setFont(pFontButtons);

	
	m_pBackButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Return" ), (6), (222), (149), (29));
	m_pBackButton->setContentAlignment(vgui::Label::a_west);
	m_pBackButton->setParent( m_pPanel );
	m_pBackButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pBackButton->addActionSignal( new CMenuHandler_ChangeGUIMenu( 30 ) );
	m_pBackButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pBackButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pBackButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pBackButton->setFont(pFontButtons);

	

	m_pConsole = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Clear Selection" ), (156), (222), (149), (29));
	m_pConsole->setContentAlignment(vgui::Label::a_west);
	m_pConsole->setParent( m_pPanel );
	//m_pConsole->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pConsole->addActionSignal( new CMenuHandler_StringCommand( "toggleconsole") ); //toggleconsole
	m_pConsole->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pConsole->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pConsole->setUnHighLightColor( 30, 30, 40, 100 );
	m_pConsole->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pConsole->setFont(pFontButtons);



	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (306), (222), (149), (29));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pPanel );
	m_pCancelButton->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCancelButton->setFont(pFontButtons);



}


void CVoteModesMenu :: ResetButtons(void)
{
	m_pTAction->cursorExited();
	m_pTLMS->cursorExited();
	m_pCancelButton->cursorExited();
    m_pConsole->cursorExited();
	m_pMatch->cursorExited();
	m_pDuel->cursorExited();
	m_pRiot->cursorExited();
	m_pBrawl->cursorExited();
	m_pLMS->cursorExited();
	m_pHunt->cursorExited();
	m_pCTF->cursorExited();
	m_pPratice->cursorExited();
	m_pEmpty->cursorExited();
	m_pTournament->cursorExited();
	m_pBackButton->cursorExited();
}

//DECLARE_MESSAGE



void CVoteModesMenu :: SetActiveInfo( int iShowText )
{
	if (iShowText == 0)
	{
		m_pText->setVisible( false );
	}
	else
	{
		m_pText->setVisible( true );
	}
}


void CVoteModesMenu :: setVisible( bool visible )
{

	CMenuPanel::setVisible(visible);
}


/***********************************/



CVoteMapsMenu :: CVoteMapsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
	m_bInitedButtons = false;
	g_nMapPage = 0;

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);
    
	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (612.0f / 2.0f), (ScreenHeight/ 2.0f) - (454.0f / 2.0f), (612), (454));
	m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pPanel->setBgColor( 0, 0, 0, 120);
    
	m_pTitle = new Label( "", (5), (6), (600), (30) );
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setBorder( new LineBorder( Color(0,0,0,0)));
    m_pTitle->setBgColor( 0, 0, 0, 155);
	m_pTitle->setImage( LoadTGAForRes("mapmenu"));
	m_pTitle->setContentAlignment( vgui::Label::a_center );

	m_pMapImage = new Label( "", (5), (41), (602), (102));
	m_pMapImage->setParent( m_pPanel );
    m_pMapImage->setBorder( new LineBorder( Color(0,0,0,0) ) );   
	m_pMapImage->setBgColor( 0, 0, 0, 255);
	//m_pMapImage->setImage( pTGAEye );

	m_pMapPanel = new Panel( (6), (178), (600), (240));
	m_pMapPanel->setParent( m_pPanel );
    //m_pMapPanel->setBorder( new LineBorder( Color(0, 0,0,255) ) );   
	m_pMapPanel->setBgColor( 0, 0, 0, 255);
	
    
	m_pNext = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Next Page  >>     " ), (406), (148), (199), (29));
	m_pNext->setContentAlignment(vgui::Label::a_east);
	m_pNext->setParent( m_pPanel );
	m_pNext->setUnHighLightColor( 30, 30, 40, 100 );
	m_pNext->addActionSignal( new CMenuHandler_ChangePage(1));
	m_pNext->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pNext->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	m_pNext->setFont(pFontButtons);

	m_pNextMap = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "" ), (206), (148), (199), (29));
	m_pNextMap->setParent( m_pPanel );
	m_pNextMap->setContentAlignment(vgui::Label::a_center);
	m_pNextMap->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pNextMap->setUnHighLightColor( 30, 30, 40, 100 );
	m_pNextMap->SetUnHightLightTextColor(0, 50, 255, 0);
	m_pNextMap->addActionSignal( new CMenuHandler_ToggleButtonNoAction(m_pNextMap, "\"vote nextmap\"","\"vote changelevel\"", cl_nextmap));
	m_pNextMap->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pNextMap->setFont(pFontButtons);

	m_pPrevious = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "<<  Previous Page" ), (6), (148), (199), (29));	m_pPrevious->setContentAlignment(vgui::Label::a_west);
	m_pPrevious->setParent( m_pPanel );
	m_pPrevious->addActionSignal( new CMenuHandler_ChangePage(-1));
	m_pPrevious->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pPrevious->setUnHighLightColor( 30, 30, 40, 100 );
	m_pPrevious->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	m_pPrevious->setFont(pFontButtons);
    
	m_pBackButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Return" ), (6), (418), (199), (29));
	m_pBackButton->setContentAlignment(vgui::Label::a_west);
	m_pBackButton->setParent( m_pPanel );
	m_pBackButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pBackButton->addActionSignal( new CMenuHandler_ChangeGUIMenu( 30 ) );
	m_pBackButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pBackButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pBackButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pBackButton->setFont(pFontButtons);

	m_pConsole = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Clear Selection" ), (206), (418), (199), (29));
	m_pConsole->setContentAlignment(vgui::Label::a_west);
	m_pConsole->setParent( m_pPanel );
	m_pConsole->addActionSignal(  new CMenuHandler_ClearVote() );
	//m_pConsole->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pConsole->addActionSignal( new CMenuHandler_StringCommand( "toggleconsole") ); //toggleconsole
	m_pConsole->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pConsole->setUnHighLightColor( 30, 30, 40, 100 );
	m_pConsole->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pConsole->setFont(pFontButtons);	

	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (406), (418), (199), (29));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pPanel );
	m_pCancelButton->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCancelButton->setFont(pFontButtons); 

	CMapList::AddMapsFromFile (&g_MapsClient, "valve/maps/*.bsp");
	CMapList::AddMapsFromFile (&g_MapsClient, "hle/maps/*.bsp");
	
	
	g_MapsClient.addMapSmart("undertow");
	g_MapsClient.addMapSmart("subtransit");
	g_MapsClient.addMapSmart("stalkyard");
	g_MapsClient.addMapSmart("snark_pit");
	g_MapsClient.addMapSmart("rapidcore");
	g_MapsClient.addMapSmart("lambda_bunker");
	g_MapsClient.addMapSmart("gasworks");
	g_MapsClient.addMapSmart("frenzy");
	g_MapsClient.addMapSmart("datacore");
	g_MapsClient.addMapSmart("crossfire");
	g_MapsClient.addMapSmart("bounce");
	g_MapsClient.addMapSmart("boot_camp");

	g_MapsClient.m_bGotMaps = true;

	CMapName* pHead = g_MapsClient[0];

	for(int i = 0; i < g_MapsClient.size(); i++)
	{
		pTGAMap[i] = LoadTGAForRes(pHead->getName());
		pHead = pHead->m_pNext;
	}
	
}


void CVoteMapsMenu :: ResetButtons(void)
{
	m_pNext->cursorExited();
	m_pPrevious->cursorExited();
	m_pCancelButton->cursorExited();
    m_pConsole->cursorExited();
	m_pBackButton->cursorExited();
	m_pNextMap->cursorExited();

    for(int i=0;i <48;i++)
    {
        map[i]->cursorExited();
    }   
}


void CVoteMapsMenu::Close( void )
{
	CMenuPanel::Close();
}

// 0 = no change, -1 = back, 1 = forward
void CVoteMapsMenu::Scroll(	int nDirection)
{
	
	if(g_MapsDownloaded.m_bGotMaps=0)
		return;
	if(nDirection==-1)
	{
		if(g_nMapPage == 0)
			return;

	}
	else if (nDirection== 1)
	{
		if(g_nMapPage*48 >= g_MapsDownloaded.size()-48)
			return;
	}

	g_nMapPage += nDirection; 
	int offset = g_nMapPage*48;
	int i_offset;
	for(int i = offset ; i < 48+offset; i++)
	{
		i_offset = i - offset;
		if(i >=g_MapsDownloaded.size())
		{
			map[i_offset]->setText(" ");
			map[i_offset]->setHighLightColor( 0, 0, 0, 255 );
			map[i_offset]->setUnHighLightColor( 0, 0, 0, 255 );
			//pTGAMap[i-offset] = NULL;//LoadTGAForRes("nopreview");
		}
		else
		{
			
			map[i_offset]->setText(g_MapsDownloaded[i]->getName());
			map[i_offset]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
			map[i_offset]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );

			if(g_MapsDownloaded[i]->getLocal())
			{
				map[i_offset]->SetHightLightTextColor(0,255,0,0);
				//map[i-offset]->SetUnHightLightTextColor(0, 50, 255, 0);
				map[i_offset]->SetUnHightLightTextColor( 150, 185, 225, 0 ); 
				
			}
			else
			{
				map[i_offset]->SetHightLightTextColor(255,0,0,0);
				map[i_offset]->SetUnHightLightTextColor(150,150,150,0);
				
			}
		}
	}		

}
void CVoteMapsMenu::InItMapsButtons(void)
{

    float x_size    =149;
    float y_size    =19; 
    int x_spacer    = 1;
    int y_spacer    = 1;
    int index       = 0;
    int y_max_buttons = 12;
    int x_max_buttons = 4;
    //int r=140, g=172, b=225, a=255;
    int x_pos, y_pos;
    char cChangelevel[128];
     cChangelevel[0]=NULL;
	 m_nCurrentDirection = 0;

	 g_nLockPreview = 0;

	 pTGANullMap = LoadTGAForRes("nopreview");
	 pTGADownloadMap = LoadTGAForRes("download");


	 CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAPS" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	int x, y;
	x = y = 0;
    for(x=0; x< x_max_buttons; x++) // 4 across
    {
        x_pos = (x_size*x)+(x_spacer*x);   
        for(y=0; y<y_max_buttons; y++) // 8 down
        {    
            y_pos = (y_size*y)+(y_spacer*y);

            index = y+(x*y_max_buttons);
            map[index] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString(  " " ), (x_pos), (y_pos), (x_size), (y_size));
	        map[index]->setParent( m_pMapPanel );			
			

			if(index >= g_MapsDownloaded.size()) // fake button
			{
				map[index]->setText(" ");
				map[index]->setHighLightColor( 0, 0, 0, 255 );
				map[index]->setUnHighLightColor( 0, 0, 0, 255 );
		
			}
			else
			{
				map[index]->setContentAlignment(vgui::Label::a_west);
				map[index]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
				map[index]->setFont(pFontButtons); //???
				map[index]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
				map[index]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
				map[index]->setActive(true);
				//map[index]->setText(g_MapsDownloaded[index]->getName());
				map[index]->addInputSignal( new CMenuHandler_VoteMap(index,map[index]  )); // test
				map[index]->addInputSignal( new CMenuHandler_PictureOverMap(index ));
				map[index]->addActionSignal( new CMenuHandler_LockPreview());

				//sprintf(mapname, "%s", g_MapsDownloaded[index]->getName());
				
				
/*
				if(g_MapsDownloaded[index]->getLocal())
				{
					map[index]->SetHightLightTextColor(0,255,0,0);
				}
				else
				{

					map[index]->SetUnHightLightTextColor(150,150,150,0);
					map[index]->SetHightLightTextColor(255,0,0,0);
				}*/
				

			}

           
			cChangelevel[0]=NULL;
        }
    }
	
	m_nCurrentPos=0;
	Scroll(0);
}


void CVoteMapsMenu :: setVisible( bool visible )
{

	if(visible==true)
	{
		Scroll(0);
		cl_nextmap->value=0;
		m_pNextMap->SetUnHightLightTextColor( 0, 200, 255, 0 ); 
		m_pNextMap->SetHightLightTextColor( 0, 200, 255, 0 ); 
		m_pNextMap->setText("\"vote changelevel\"");	
	}

	CMenuPanel::setVisible(visible);
}



CVoteSpeedMenu :: CVoteSpeedMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
/*
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (462.0f / 2.0f), (ScreenHeight/ 2.0f) - (108.0f / 2.0f), (462), (108));
	m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pPanel->setBgColor( 0, 0, 0, 120);
    
    m_pTitle = new Label( "", (6), (6), (450), (30) );
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setContentAlignment( vgui::Label::a_center );
	m_pTitle->setBorder( new LineBorder( Color(0,0,0,0)));
    m_pTitle->setBgColor( 0, 0, 0, 255 );
	m_pTitle->setImage( LoadTGAForRes("speedmenu"));

	////////////////////////////////////////////////////////                    
	speed[0] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "\"270\" U.S." ), (6), (42), (149), (29));
	speed[0] ->setContentAlignment(vgui::Label::a_west);
	speed[0] ->setParent( m_pPanel );
	//speed[0] ->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	speed[0]->addInputSignal( new CMenuHandler_VoteFunction("sv_maxspeed 270", speed[0]));
	//speed[0] ->addActionSignal( new CMenuHandler_StringCommand( "vote sv_maxspeed 270") ); 
    speed[0]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	speed[0]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	speed[0]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	speed[0]->setFont(pFontButtons);

    speed[1] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "\"300\" European" ), (156), (42), (149), (29));
	speed[1] ->setContentAlignment(vgui::Label::a_west);
	speed[1] ->setParent( m_pPanel );
	//speed[1] ->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//speed[1] ->addActionSignal( new CMenuHandler_StringCommand( "vote sv_maxspeed 300") ); 
	speed[1]->addInputSignal( new CMenuHandler_VoteFunction("sv_maxspeed 300", speed[1]));
    speed[1]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	speed[1]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	speed[1]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	speed[1]->setFont(pFontButtons);

    speed[2] = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "\"320\" Turbo" ), (306), (42), (149), (29));
	speed[2] ->setContentAlignment(vgui::Label::a_west);
	speed[2] ->setParent( m_pPanel );
	//speed[2] ->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//speed[2] ->addActionSignal( new CMenuHandler_StringCommand( "vote sv_maxspeed 320") ); 
	speed[2]->addInputSignal( new CMenuHandler_VoteFunction("sv_maxspeed 320", speed[2]));
    speed[2]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	speed[2]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	speed[2]->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	speed[2]->setFont(pFontButtons);


	m_pBackButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Return" ), (6), (72), (149), (29));
	m_pBackButton->setContentAlignment(vgui::Label::a_west);
	m_pBackButton->setParent( m_pPanel );
	m_pBackButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pBackButton->addActionSignal( new CMenuHandler_ChangeGUIMenu( MENU_SETTINGS ) );
	m_pBackButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pBackButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pBackButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pBackButton->setFont(pFontButtons);

	m_pConsole = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Clear Selection" ), (156), (72), (149), (29));
	m_pConsole->setContentAlignment(vgui::Label::a_west);
	m_pConsole->setParent( m_pPanel );
	m_pConsole->addActionSignal(  new CMenuHandler_ClearVote() );
	//m_pConsole->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	//m_pConsole->addActionSignal( new CMenuHandler_StringCommand( "toggleconsole") ); //toggleconsole
	m_pConsole->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pConsole->setUnHighLightColor( 30, 30, 40, 100 );
	m_pConsole->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pConsole->setFont(pFontButtons);

	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (306), (72), (149), (29));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pPanel );
	m_pCancelButton->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCancelButton->setFont(pFontButtons);*/
}


void CVoteSpeedMenu :: ResetButtons(void)
{
	
    for(int i = 0; i<3;i++)
    {
	    speed[i]->cursorExited();
    }

    m_pCancelButton->cursorExited();
    m_pConsole->cursorExited();
	m_pBackButton->cursorExited();
}

//DECLARE_MESSAGE



void CVoteSpeedMenu :: SetActiveInfo( int iShowText )
{
	if (iShowText == 0)
	{
		m_pText->setVisible( false );
	}
	else
	{
		m_pText->setVisible( true );
	}
}


void CVoteSpeedMenu :: setVisible( bool visible )
{

	CMenuPanel::setVisible(visible);
}


CClientMenu :: CClientMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{

	int asd =	  204;
	int r = (204 | 0xFF  );
	int HueChange = 2;
	int color[MAXCOLORS][3];
	int sign = 1;
	char szTemp[3];
	*szTemp = NULL;

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	for(int j = 0; j <MAXCOLORS; j++)
	{

		color[j][0]=0;
		color[j][1]=0;
		color[j][2]=0;
		*g_ColorTable[j]=NULL;
//		strcpy(g_ColorTable[j], "0x00"); // we want alpha to be full!
	}
	color[0][0]=0xFF;
	for(int i = 1; i < MAXCOLORS; i++)
	{
		
	
		color[i][HueChange] = color[i-1][HueChange] + sign;
		if(HueChange==0)
		{
			color[i][1] = color[i-1][1];
			color[i][2] = color[i-1][2];
		}
		else if(HueChange==1)
		{
			color[i][0] = color[i-1][0];
			color[i][2] = color[i-1][2];
		}
		else
		{	
			color[i][1] = color[i-1][1];
			color[i][0] = color[i-1][0];
		}

		
		if(i % 0xFF == 0)
		{
			HueChange++;
			if(HueChange==3)
				HueChange=0;
			if(sign!=1)
				sign=1;
			else 
				sign=-1;
		}
	
		g_ColorTable[i][0] = color[i][0]; 
		g_ColorTable[i][1] = color[i][1]; 
		g_ColorTable[i][2] = color[i][2]; 
	}

	g_ColorTable[0][0]=255;
	g_ColorTable[0][1]=0;
	g_ColorTable[0][2]=0;

	

//    CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
    SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );

	//m_pPanel = new Panel( XRES(80), YRES(80), XRES(480), YRES(330));
	m_pPanel = new Panel( (ScreenWidth / 2.0f) - (600.0f / 2.0f), (ScreenHeight/ 2.0f) - (258.0f / 2.0f), (600), (258));
	m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pPanel->setBgColor( 0, 0, 0, 255);

	m_pPanelTop = new Panel(0, 0, 600, 102);
	m_pPanelTop->setParent( m_pPanel );
	m_pPanelTop->setBgColor( 0, 0, 0, 120);
    
	m_pPanelLeft = new Panel(1, 102, 204, 156);
	m_pPanelLeft->setParent( m_pPanel );
	m_pPanelLeft->setBgColor( 0, 0, 0, 120);
	
	m_pPanelRight = new Panel(394, 102, 204, 156);
	m_pPanelRight->setParent( m_pPanel );
	m_pPanelRight->setBgColor( 0, 0, 0, 120);

	m_pPanelBottom = new Panel(205, 158, 189, 100);
	m_pPanelBottom->setParent( m_pPanel );
	m_pPanelBottom->setBgColor( 0, 0, 0, 120);

	m_pPanelCenter = new Panel(204, 102, 191, 57);
	m_pPanelCenter->setParent( m_pPanel );
	m_pPanelCenter->setBgColor( 0, 0, 0, 255);
	m_pPanelCenter->setBorder( new LineBorder( Color(0, 0,0,0) ) );


    m_pTitle = new Label( "", (6), (6), (588), (30) );
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setContentAlignment( vgui::Label::a_center );
	m_pTitle->setBorder( new LineBorder( Color(0,0,0,0)));
    m_pTitle->setBgColor( 0, 0, 0, 255 );
	m_pTitle->setImage( LoadTGAForRes("clientmenu"));


	/*m_pSPanel = new Panel( (458), (42), (138), (58));
	m_pSPanel->setParent( m_pPanel );
    m_pSPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pSPanel->setBgColor( 255, 255, 255, 0);

	m_pSPanel2 = new Panel( (6), (42), (452), (58));
	m_pSPanel2->setParent( m_pPanel );
    m_pSPanel2->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pSPanel2->setBgColor( 0, 0, 0, 0);*/

	m_pScrollBarHue = new ScrollBar( (6), (44), (448), (16), false );
	m_pScrollBarHue->setRange(0,MAXCOLORS-1);
	m_pScrollBarHue->setParent(m_pPanel);
	m_pScrollBarHue->setBorder( new LineBorder( Color(0,0,0,0)));
	m_pScrollBarHue->addIntChangeSignal(new ChangeHudHue());
	
	m_pHueLable = new Label( "Hue: ", (456), (44), (136), (16) );
	m_pHueLable->setParent( m_pPanel );
	m_pHueLable->setBorder( new LineBorder( Color(0,0,0,0)));   
	m_pHueLable->setBgColor( 0, 0, 0, 120 );
	m_pHueLable->setFont(pFontButtons);
	m_pHueLable->setContentAlignment(vgui::Label::a_west);
	m_pHueLable->setFgColor( 255, 255, 255, 0 );

	m_pScrollBarLight = new ScrollBar((6), (62), (448), (16), false );
	m_pScrollBarLight->setRange(0,MAXCOLORS-1);
	m_pScrollBarLight->setParent(m_pPanel);
	m_pScrollBarLight->setBorder( new LineBorder( Color(0,0,0,0)));
	m_pScrollBarLight->addIntChangeSignal(new ChangeHudLight());

	m_pLightLable = new Label( "Value: ", (456), (62), (136), (16) );
	m_pLightLable->setParent( m_pPanel );
	m_pLightLable->setBorder( new LineBorder( Color(0, 0,0,0)));   
	m_pLightLable->setBgColor(  0, 0, 0, 120 );
	m_pLightLable->setFont(pFontButtons);
	m_pLightLable->setContentAlignment(vgui::Label::a_west);
	m_pLightLable->setFgColor( 255, 255, 255, 0 );

	m_pScrollBarSensitivity = new ScrollBar((6), (80), (448), (16), false );
	m_pScrollBarSensitivity->setRange(0,MAXCOLORS-1);
	m_pScrollBarSensitivity->setParent(m_pPanel);
	m_pScrollBarSensitivity->setBorder( new LineBorder( Color(0,0,0,0)));
	m_pScrollBarSensitivity->addIntChangeSignal(new ChangeSensitivity());

	m_pSensitivityLable = new Label( "Sensitivity: ", (456), (80), (136), (16) );
	m_pSensitivityLable->setParent( m_pPanel );
	m_pSensitivityLable->setBorder( new LineBorder( Color(0, 0,0,0)));   
	m_pSensitivityLable->setBgColor(  0, 0, 0, 120 );
	m_pSensitivityLable->setFont(pFontButtons);
	m_pSensitivityLable->setContentAlignment(vgui::Label::a_west);
	m_pSensitivityLable->setFgColor( 255, 255, 255, 0 );


	m_pPrevColor = new CommandButton( "", (6), (102), (29), (29));
	m_pPrevColor->setContentAlignment( vgui::Label::a_center );
	m_pPrevColor->setParent( m_pPanel );
	m_pPrevColor->addActionSignal( new ChangeGaussColor(-1));
	m_pPrevColor->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pPrevColor->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pPrevColor->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pPrevColor->setFont(pFontButtons);
	m_pPrevColor->setImage( LoadTGAForRes("left"));
	

	m_pGaussLabel = new Label( "Gauss Color: ", (36), (102), (135), (29) );
	m_pGaussLabel->setParent( m_pPanel );
	m_pGaussLabel->setBorder( new LineBorder( Color(0, 0,0,0)));   
	m_pGaussLabel->setBgColor(  0, 0, 0, 120 );
	m_pGaussLabel->setFont(pFontButtons);
	m_pGaussLabel->setContentAlignment(vgui::Label::a_center);
	m_pGaussLabel->setFgColor( 255, 255, 255, 0 );

	m_pNextColor = new CommandButton( "", (172), (102), (29), (29));
	m_pNextColor->setContentAlignment( vgui::Label::a_center );
	m_pNextColor->setParent( m_pPanel );
	m_pNextColor->addActionSignal( new ChangeGaussColor(1));
	m_pNextColor->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pNextColor->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pNextColor->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pNextColor->setFont(pFontButtons);
	m_pNextColor->setImage( LoadTGAForRes("right"));

	m_pWinamp = new CommandButton( "Winamp Player", (6), (132), (195), (29));
	m_pWinamp->setContentAlignment( vgui::Label::a_center );
	m_pWinamp->setParent( m_pPanel );
	m_pWinamp->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pWinamp->addActionSignal( new CMenuHandler_ChangeGUIMenu( MENU_MP3 ) );
	m_pWinamp->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pWinamp->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pWinamp->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	m_pWinamp->setFont(pFontButtons);
	/*
	m_pRemove = new CommandButton( "Remove from play", (6), (132), (195), (29));
	m_pRemove->setContentAlignment( vgui::Label::a_center );
	m_pRemove->setParent( m_pPanel );
	m_pRemove->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pRemove->addActionSignal( new CMenuHandler_StringCommand("ejectme") );
	m_pRemove->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pRemove->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pRemove->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pRemove->setFont(pFontButtons);*/

	m_pInvert = new CommandButton( "", (6), (162), (195), (29));
	m_pInvert->setContentAlignment( vgui::Label::a_center );
	m_pInvert->setParent( m_pPanel );
	m_pInvert->addActionSignal( new CMenuHandler_InvertMouse(m_pInvert));
	m_pInvert->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pInvert->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pInvert->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pInvert->setFont(pFontButtons);

	m_pSmooth = new CommandButton( "", (6), (192), (195), (29));
	m_pSmooth->setContentAlignment( vgui::Label::a_center );
	m_pSmooth->setParent( m_pPanel );
	m_pSmooth->addActionSignal( new CMenuHandler_ToggleButton(m_pSmooth, "Mouse Filtered", "Mouse Unfiltered", m_filter ));
	m_pSmooth->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pSmooth->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pSmooth->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pSmooth->setFont(pFontButtons);


	m_pRadar = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "" ), 202, (162), (97), (29));
	m_pRadar->setContentAlignment(vgui::Label::a_center);
	m_pRadar->setParent( m_pPanel );
	m_pRadar->addActionSignal(  new CMenuHandler_ToggleRadar(m_pRadar) );
	m_pRadar->setFont(pFontButtons);
	m_pRadar->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pRadar->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pRadar->addActionSignal( new CMenuHandler_SoundCommand("vgui/changemenu.wav"));
	if(cl_radar->value) m_pRadar->setText("Radar: On");
	else m_pRadar->setText("Radar: Off");


	m_pLock = new CommandButton( "", (300), (162), (97), (29));
	m_pLock->setContentAlignment( vgui::Label::a_center );
	m_pLock->setParent( m_pPanel );
	m_pLock->addActionSignal( new CMenuHandler_ShowLock(m_pLock, "Lock On", "Lock Off", cl_showlock));
	m_pLock->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pLock->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pLock->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pLock->setFont(pFontButtons);
		

	m_pClock = new CommandButton( "", (202), (192), (97), (29));
	m_pClock->setContentAlignment( vgui::Label::a_center );
	m_pClock->setParent( m_pPanel );
	m_pClock->addActionSignal( new CMenuHandler_ToggleButton(m_pClock, "Clock On", "Clock Off", cl_showclock));
	m_pClock->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pClock->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pClock->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pClock->setFont(pFontButtons);

	
	m_pTimer = new CommandButton( "", (300), (192), (97), (29));
	m_pTimer->setContentAlignment( vgui::Label::a_center );
	m_pTimer->setParent( m_pPanel );
	m_pTimer->addActionSignal( new CMenuHandler_ToggleButton(m_pTimer, "Timer On", "Timer Off", cl_showtimer));
	m_pTimer->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pTimer->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pTimer->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pTimer->setFont(pFontButtons);


	

	m_pCrosshairs = new Label( "", (428), (102), (135), (29) );
	m_pCrosshairs->setContentAlignment( vgui::Label::a_center );
	m_pCrosshairs->setParent( m_pPanel );
	m_pCrosshairs->setFont(pFontButtons);
	m_pCrosshairs->setBgColor( 150, 185, 225, 0 );
	m_pCrosshairs->setFgColor(  0, 0, 0, 0 );


	m_pCrossPrev = new CommandButton( "", (398), (102), (29), (29));
	m_pCrossPrev->setContentAlignment( vgui::Label::a_center );
	m_pCrossPrev->setParent( m_pPanel );
	m_pCrossPrev->addActionSignal( new CMenuHandler_ChangeCrosshairs(m_pCrosshairs, -1));
	m_pCrossPrev->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCrossPrev->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCrossPrev->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCrossPrev->setFont(pFontButtons);
	m_pCrossPrev->setImage( LoadTGAForRes("left"));

	m_pCrossNext = new CommandButton( "", (564), (102), (29), (29));
	m_pCrossNext->setContentAlignment( vgui::Label::a_center );
	m_pCrossNext->setParent( m_pPanel );
	m_pCrossNext->addActionSignal( new CMenuHandler_ChangeCrosshairs(m_pCrosshairs, 1));
	m_pCrossNext->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCrossNext->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCrossNext->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCrossNext->setFont(pFontButtons);
	m_pCrossNext->setImage( LoadTGAForRes("right"));

	

	m_pDynamic = new CommandButton( "", (398), (132), (195), (29));
	m_pDynamic->setContentAlignment( vgui::Label::a_center );
	m_pDynamic->setParent( m_pPanel );
	m_pDynamic->addActionSignal( new CMenuHandler_ToggleButtonDynamic( m_pDynamic, "X-hairs: Colored","X-hairs: Original" , cl_dynamic_xhrs) ); //toggleconsole
	m_pDynamic->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pDynamic->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pDynamic->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pDynamic->setFont(pFontButtons);

	m_pSmoothCrosshairs = new CommandButton( "", (398), (162), (195), (29));
	m_pSmoothCrosshairs->setContentAlignment( vgui::Label::a_center );
	m_pSmoothCrosshairs->setParent( m_pPanel );
	m_pSmoothCrosshairs->addActionSignal( new CMenuHandler_ToggleButtonDynamic( m_pSmoothCrosshairs, "Direct3D Sprites","OpenGL Sprites" , gEngfuncs.pfnGetCvarPointer("vid_d3d")) ); //toggleconsole
	m_pSmoothCrosshairs->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pSmoothCrosshairs->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pSmoothCrosshairs->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pSmoothCrosshairs->setFont(pFontButtons);
	//vid_d3d 

	m_pSpriteBlend = new CommandButton( "", (398), (192), (195), (29));
	m_pSpriteBlend->setContentAlignment( vgui::Label::a_center );
	m_pSpriteBlend->setParent( m_pPanel );
	m_pSpriteBlend->addActionSignal( new CMenuHandler_ToggleButtonDynamic( m_pSpriteBlend, "Alpha Blend","No Alpha" , gEngfuncs.pfnGetCvarPointer("gl_spriteblend")) ); //toggleconsole
	m_pSpriteBlend->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pSpriteBlend->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pSpriteBlend->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pSpriteBlend->setFont(pFontButtons); 

	
	m_pBackButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Return" ), (6), (222), (195), (29));
	m_pBackButton->setContentAlignment(vgui::Label::a_west);
	m_pBackButton->setText("Return");
	m_pBackButton->setParent( m_pPanel );
	m_pBackButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pBackButton->addActionSignal( new CMenuHandler_ChangeGUIMenu(  30 ) );
	m_pBackButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pBackButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pBackButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));

	m_pBackButton->setFont(pFontButtons);
	

	
	m_pConsole = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Console" ), (202), (222), (195), (29));
	m_pConsole->setContentAlignment(vgui::Label::a_west);
	m_pConsole->setParent( m_pPanel );
	m_pConsole->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pConsole->addActionSignal( new CMenuHandler_StringCommand( "toggleconsole") ); //toggleconsole
	m_pConsole->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pConsole->setUnHighLightColor( 30, 30, 40, 100 );
	m_pConsole->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pConsole->setFont(pFontButtons);

	
	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (398), (222), (195), (29));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pPanel );
	m_pCancelButton->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pCancelButton->setFont(pFontButtons);
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));


	
	//if(!strcmp(cl_gausscolor->string, "lms"))
	//	g_GaussColor = 9;
//	else
		g_GaussColor = cl_gausscolor->value;
	


	if(cl_dynamic_xhrs->value == 0)
		m_pDynamic->setText("X-hairs: Original");
	
	else
		m_pDynamic->setText("X-hairs: Colored");
	m_pDynamic->setTextAlignment(vgui::Label::a_center);

	if(m_pitch->value <0)
		m_pInvert->setText("Mouse Inverted");
	else
		m_pInvert->setText("Mouse Reverted");

	if(m_filter->value)
		m_pSmooth->setText("Mouse Filtered");
	else
		m_pSmooth->setText("Mouse Unfiltered");

	if(cl_showclock->value)
		m_pClock->setText("Clock On");
	else
		m_pClock->setText("Clock Off");

	if(cl_showtimer->value)
		m_pTimer->setText("Timer On");
	else
		m_pTimer->setText("Timer Off");

	if(cl_showlock->value)
		m_pLock->setText("Lock On");
	else
		m_pLock->setText("Lock Off");
	if(CVAR_GET_FLOAT("vid_d3d")==0)
		m_pSmoothCrosshairs->setText("OpenGL Sprites");
	else
		m_pSmoothCrosshairs->setText("Direct3D Sprites");
	if(CVAR_GET_FLOAT("gl_spriteblend")==0)
		m_pSpriteBlend->setText("No Alpha");
	else
		m_pSpriteBlend->setText("Alpha Blend");
	

	char buf[64];
	sprintf(buf, "X-hairs: %s", cl_crosshairs->string);
	m_pCrosshairs->setText(buf);


	int hue = cl_hudhue->value*6;
	int value = cl_hudvalue->value*MAXCOLORS;
	int nSensitivityPos = (sensitivity->value*SENSITIVITY_SCALE);
	int nGaussColor = (cl_gausscolor->value*((float)MAXCOLORS/9.0f));

	if(hue >= MAXCOLORS)
		hue = MAXCOLORS-1;
	if(hue < 0)
		hue = 0;

	if(value >= MAXCOLORS)
		value = MAXCOLORS-1;
	if(value < 0)
		value = 0;

	//g_hud_color.setHueRed(g_ColorTable[hue][0]);
	//g_hud_color.setHueGreen(g_ColorTable[hue][1]);
	//g_hud_color.setHueBlue(g_ColorTable[hue][2]);
	
	Slider* pSlider = m_pScrollBarHue->getSlider();
	pSlider->setValue(hue);
	
	pSlider = m_pScrollBarLight->getSlider();
	pSlider->setValue(value);

	pSlider = m_pScrollBarSensitivity->getSlider();
	pSlider->setValue(nSensitivityPos);

/*	pSlider = m_pScrollBarGaussColor->getSlider();
	pSlider->setValue(nGaussColor);*/

	int midpoint = MAXCOLORS / 2;
	if(value > midpoint)	{ value = (value - midpoint)/3; }
	else { value = (midpoint -  value )/-3; }
	g_hud_color.setBrightness(value);
	gHUD.m_Ammo.ChangeXhairColor();


	char szVgui[64];
	float fValue (value/255.0f);
	int nValue = fValue*100;
	sprintf(szVgui, "Value: %i", nValue);
	strcat(szVgui, "%%");
	m_pLightLable->setText ( szVgui );

	*szVgui=NULL;
	sprintf(szVgui, "Hue: %i", (hue/6));
	m_pHueLable->setText ( szVgui );

	*szVgui=NULL;
	//sprintf(szVgui, "Sensitivity: %s", sensitivity->string);
	//strcpy(szVgui, "Sensitivity:");
	//strncat(szVgui, sensitivity->string, 5);
	//m_pSensitivityLable->setText( szVgui );

	bool decimals = false;
	int _j = 0;
	for(unsigned int _i = 0; _i < strlen(sensitivity->string); _i ++)
	{
		if(decimals)
		{
			if(_j>1)
				sensitivity->string[_i] = NULL;	
			_j++;
		}
		if(sensitivity->string[_i]=='.')
			decimals = true;
	}
	sprintf(szVgui, "Sensitivity: %s", sensitivity->string);
	m_pSensitivityLable->setText( szVgui );


	m_pGaussLabel->setFgColor(  255, 255, 255, 0 );

	switch((int)cl_gausscolor->value)
	{
			case 0:
				m_pGaussLabel->setBgColor(  255, 255, 0, 0 );
				m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 1:
				m_pGaussLabel->setBgColor(  255, 128, 0, 0 );
				break;
			case 2:
				m_pGaussLabel->setBgColor(  255, 0, 0, 0 );
				break;
			case 3:
				m_pGaussLabel->setBgColor(  255, 0, 128, 0 );
				break;
			case 4:
				m_pGaussLabel->setBgColor(  255, 0, 255, 0 );
				break;
			case 5:
				m_pGaussLabel->setBgColor(  0, 0, 255, 0 );
				break;
			case 6:
				m_pGaussLabel->setBgColor(  0, 255, 255, 0 );
				m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 7:
				m_pGaussLabel->setBgColor(  0, 255, 0, 0 );
				m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 8:
				m_pGaussLabel->setBgColor(  255, 255, 255, 0 );
				m_pGaussLabel->setFgColor(  0, 0, 0, 0 );
				break;
			case 9:
				m_pGaussLabel->setBgColor(  0, 0, 0, 0 );
				break;

			default:
				m_pGaussLabel->setBgColor(  255, 128, 0, 0 );
				break;
	}
	*szVgui=NULL;
	sprintf(szVgui, "Gauss Color: %i", (int)cl_gausscolor->value);
	m_pGaussLabel->setText(szVgui);




	// LOAD WEAPON WEIGHTS... bad place change later
	// set defaults
	g_Weight[0] = 0;
	g_Weight[1] = 0;
	g_Weight[2] = 10;
	g_Weight[3] = 15;
	g_Weight[4] = 15;
	g_Weight[5] = 15;
	g_Weight[6] = 10;
	g_Weight[7] = 15;
	g_Weight[8] = 20;
	g_Weight[9] = 20;
	g_Weight[10] = 20;
	g_Weight[11] = 10;
	g_Weight[12] = -5;
	g_Weight[13] = -5;
	g_Weight[14] = -5;
	g_Weight[15] = -5;
	g_Weight[16] = 20;
	g_Weight[17] = 15;
	g_Weight[18] = 15;
	g_Weight[19] = 20;

	std::ifstream weightFile;
	weightFile.open("hle/weight.txt");

	gHUD.m_Ammo.ChangeXhairColor();
		
	int len = 0;
	if(weightFile.is_open()) // overwrite if we have a file
	{
		char line[256];
		
		while(!weightFile.eof())
		{
				weightFile.getline(line,255 );
				char* tabed_command = RemoveTabs(line);
				char* command = strtok(tabed_command, " ");
				
				if(!command)
				{
					continue;
				}
				if(!strncmp(command, "//", 2))
				{
					continue;
				}
				// remove tabs
				
				// end remove tabs
				if(!stricmp(command, "crowbar"))
				{
					command = strtok(NULL, " ");
					if(command)
						g_Weight[1] = atoi(command);
				}
				else if(!stricmp(command, "9mmhandgun"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[2] = atoi(command);
				}
				else if(!stricmp(command, "357"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[3] = atoi(command);
				}
				else if(!stricmp(command, "9mmAR"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[4] = atoi(command);
				}
				else if(!stricmp(command, "crossbow"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[6] = atoi(command);
				}
				else if(!stricmp(command, "shotgun"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[7] = atoi(command);
				}
				
				else if(!stricmp(command, "rpg"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[8] = atoi(command);
				}
				else if(!stricmp(command, "gauss"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[9] = atoi(command);
				}
				else if(!stricmp(command, "egon"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[10] = atoi(command);
				}
				else if(!stricmp(command, "hornetgun"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[11] = atoi(command);
				}
				
				else if(!stricmp(command, "handgranade"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[12] = atoi(command);
				}
				else if(!stricmp(command, "tripmine"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[13] = atoi(command);
				}
				else if(!stricmp(command, "satchel"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[14] = atoi(command);
				}
				else if(!stricmp(command, "snark"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[15] = atoi(command);
				}
				else if(!stricmp(command, "sniperrifle"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[16] = atoi(command);
				}
				else if(!stricmp(command, "m249"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[17] = atoi(command);
				}
				else if(!stricmp(command, "shockrifle"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[18] = atoi(command);
				}
				else if(!stricmp(command, "sporelauncher"))
				{
					command = strtok(NULL, " ");
					if(command)
					g_Weight[19] = atoi(command);
				}
				else
					continue;
			}
		
	}
	
	weightFile.close();

	char temp[4];
	*temp = NULL;

	strcat(g_szWeight, "weight ");
	for(int weight =0; weight<20; weight++)
	{
		strncat(g_szWeight,itoa(g_Weight[weight], temp, 10),2);
		strcat(g_szWeight, ";");
	}
	strcat(g_szWeight, "\n");

	// winamp crap
	gWinamp.Pre_HUD_Init();
	gWinamp.Post_Hook_SayText("");

	
	scr_ofsx->value		= 0;
	scr_ofsy->value		= 0;
	scr_ofsz->value		= 0;

	//
	
	//
	
}
//m_pSpectatorPanel->m_ExtraInfo->setText ( szText );

void CClientMenu :: UpdateClientMenu(void)
{
	int iTextWidth, iTextHeight;
	m_pHueLable->getTextSize( iTextWidth, iTextHeight );
	int xPos = ScreenWidth/2 - ( (iTextWidth + XRES( 10 ))/2);
	m_pHueLable->setBounds( xPos, YRES( 35 ), iTextWidth + XRES(10), YRES( 15 ) );

	m_pLightLable->getTextSize( iTextWidth, iTextHeight );
	xPos = ScreenWidth/2 - ( (iTextWidth + XRES( 10 ))/2);
	m_pLightLable->setBounds( xPos, YRES( 35 ), iTextWidth + XRES(10), YRES( 15 ) );
}
	


void CClientMenu :: ResetButtons(void)
{
	m_pCancelButton->cursorExited();
	m_pBackButton->cursorExited();
	m_pConsole->cursorExited();
	m_pPrevColor->cursorExited();
	m_pNextColor->cursorExited();
	//m_pRemove->cursorExited();
	m_pWinamp->cursorExited();
	m_pInvert->cursorExited();
	m_pSmooth->cursorExited();
	m_pRadar->cursorExited();
	m_pClock->cursorExited();
	m_pTimer->cursorExited();
	m_pLock->cursorExited();
	m_pCrossPrev->cursorExited();
	m_pCrossNext->cursorExited();
	m_pDynamic->cursorExited();
	m_pSmoothCrosshairs->cursorExited();


}

void CClientMenu :: SetActiveInfo( int iShowText )
{
	m_pText->setVisible( true );
}

void CClientMenu :: setVisible( bool visible )
{
	CMenuPanel::setVisible(visible);
}




ClientVote::ClientVote()
{
	*m_szVoteString=NULL;
	m_pButton = NULL;
}

void ClientVote::ClearVote()
{
	if(m_pButton)
	{
		m_pButton->setHighLightColor(hr, hg, hb, ha);
		m_pButton->setUnHighLightColor(ur, ug, ub, ua);
		m_pButton=NULL;
		*m_szVoteString=NULL;
	}
}

bool ClientVote::DualVote()
{
	return strcmp(m_szVoteString, "") ? true : false;
}
int ClientVote::CallSingleVote(char *string)
{
	char svrcmd[256];
	sprintf(svrcmd, "vote %s\n", string);
	*m_szVoteString=NULL;
	gViewPort->HideTopMenu();
	gEngfuncs.pfnClientCmd(svrcmd);
	return 1;
}
// forceCall will force the vote even if only 1 vote is in the queue.
int ClientVote::AddVote(char *string, CommandButton *pButton, bool forceCall)
{
	if(!strcmp(m_szVoteString, "") && forceCall==false)
	{
		m_pButton = pButton;
		m_pButton->getHighLightColor(&hr, &hg, &hb, &ha);
		m_pButton->getUnHighLightColor(&ur, &ug, &ub, &ua);
		m_pButton->setHighLightColor(100, 200, 255, 50);
		m_pButton->setUnHighLightColor(100, 200, 255, 100);
		strcpy(m_szVoteString, string);
		return 0;
		
	}
	if(strcmp(m_szVoteString, string) && strcmp(m_szVoteString, "")) // dual vote
	{
		char temp[256];
		if(!strncmp(m_szVoteString, "mode", 4)) // swap em so the mode will work, HACK!!!
		{
			sprintf(temp, "%s", m_szVoteString);
			sprintf(m_szVoteString, "%s", string);
			sprintf(string, "%s", temp);
		}
		char svrcmd[256];
		sprintf(svrcmd, "vote %s,%s\n", m_szVoteString, string);
		*m_szVoteString=NULL;
		if(m_pButton)
		{
			m_pButton->setHighLightColor(hr, hg, hb, ha);
			m_pButton->setUnHighLightColor(ur, ug, ub, ua);
			m_pButton=NULL;
		}
		gViewPort->HideTopMenu();
		gEngfuncs.pfnClientCmd(svrcmd);
		return 2;
	}
	else // single vote, voted for the same thing twice
	{
		char svrcmd[256];
		sprintf(svrcmd, "vote %s\n", string);
		*m_szVoteString=NULL;
		if(m_pButton)
		{
			m_pButton->setHighLightColor(hr, hg, hb, ha);
			m_pButton->setUnHighLightColor(ur, ug, ub, ua);
			m_pButton=NULL;
		}
		gViewPort->HideTopMenu();
		gEngfuncs.pfnClientCmd(svrcmd);
		return 1;
	}
}



void CMp3Player :: setVisible( bool visible )
{
	HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
	if(hwWinamp)
	{
		char* track = gWinamp.GetCurrentTrack(hwWinamp);
		if(track )
			m_pSong->setText(track);
	}
	else
		m_pSong->setText(" Off  - WINAMP 2.9x or 5.x required.");
	Panel::setVisible(visible);
}


void CMp3Player :: ResetButtons(void)
{
	m_pClose->cursorExited();
	m_pShuffel->cursorExited();
	m_pRepeat->cursorExited();
	m_pPlay->cursorExited();
	m_pPrevTrack->cursorExited();
	m_pPause->cursorExited();
	m_pStop->cursorExited();
	m_pNextTrack->cursorExited();
}


CMp3Player :: CMp3Player(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "mp3 text" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	Color green(0, 255, 0, 0);
	Color gray(150, 185, 225, 0);
	

	m_pPanel = new Panel( XRES(320)-166, YRES(240)-37, (332), (74+6));
	m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pPanel->setBgColor( 0, 0, 0, 120);
    
    m_pTitle = new Label( "", (2), (2), (328), (22) );
	m_pTitle->setFgColor(255, 255, 255, 100);
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setContentAlignment( vgui::Label::a_center );
	m_pTitle->setBorder( new LineBorder( Color(0,0,0,50)));
    m_pTitle->setBgColor( 150, 185, 225, 120 );
	m_pTitle->setFont(pFontButtons);
	m_pTitle->setImage(LoadTGAForRes("winamp"));


	m_pSong = new Label( "", (2), (18+6), (328), (16) );
	m_pSong->setParent( m_pPanel );
	m_pSong->setContentAlignment( vgui::Label::a_west );
	m_pSong->setBorder( new LineBorder( Color(0,0,0,50)));
    m_pSong->setBgColor( 150, 185, 225, 0 );
	m_pSong->setFont(pFontButtons);

	
	m_pClose = new CommandButton( "", (311), (5), (16), (16));
	m_pClose->setContentAlignment( vgui::Label::a_center );
	m_pClose->setParent( m_pPanel );
	m_pClose->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pClose->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pClose->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pClose->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pClose->setImage( LoadTGAForRes("exit"));

	m_pScrollVolume = new ScrollBar((2), (35+6), (196), (16), false );
	m_pScrollVolume->setRange(0,255*4);
	m_pScrollVolume->setParent(m_pPanel);
	m_pScrollVolume->setBorder( new LineBorder( Color(0,0,0,0)));
	
	m_pScrollVolume->addIntChangeSignal(new AdjustVolume());

	pSlider = m_pScrollVolume->getSlider();
	
	cvar_t* m_mp3volume = gEngfuncs.pfnGetCvarPointer( "mp3volume" );
	if(m_mp3volume)
		pSlider->setValue(m_mp3volume->value*255.0f*4.0f);
	
	pSlider->addInputSignal( new CMenuHandler_VolumeContorl());


	HWND hwWinamp = FindWindow("Winamp v1.x",NULL);

	m_pShuffel = new CommandButton( "Shuffle", (200), (36+6), (64), (16));
	m_pShuffel->setContentAlignment( vgui::Label::a_north );
	m_pShuffel->setParent( m_pPanel );
	m_pShuffel->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pShuffel->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pShuffel->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	
	m_pShuffel->setFont(pFontButtons);

	int shuf = 0;
	if(hwWinamp)
	{	
		shuf = SendMessage(hwWinamp,WM_WA_IPC,0,IPC_GET_SHUFFLE);	
	}
	m_pShuffel->addActionSignal( new CMenuHandler_ToggleMp3Command(m_pShuffel, &green, &gray, SHUFFLE_ON, SHUFFLE_OFF, shuf));
		
	

	m_pRepeat = new CommandButton( "Repeat", (266), (36+6), (64), (16));
	m_pRepeat->setContentAlignment( vgui::Label::a_north );
	m_pRepeat->setParent( m_pPanel );
	m_pRepeat->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pRepeat->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pRepeat->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	
	
	m_pRepeat->setFont(pFontButtons);

	int reap = 0;
	if(hwWinamp)
	{	
		reap = SendMessage(hwWinamp,WM_WA_IPC,0,IPC_GET_REPEAT);
	}
	m_pRepeat->addActionSignal( new CMenuHandler_ToggleMp3Command(m_pRepeat, &green, &gray, REPEAT_ON, REPEAT_OFF, reap));

		

	m_pPrevTrack = new CommandButton( "", (2), (54+6), (64), (16));
	m_pPrevTrack->setContentAlignment( vgui::Label::a_west );
	m_pPrevTrack->setParent( m_pPanel );
	m_pPrevTrack->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pPrevTrack->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pPrevTrack->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pPrevTrack->addActionSignal( new CActionWinamp(3));
	
	m_pPrevTrack->setImage( LoadTGAForRes("prev"));

	m_pPlay = new CommandButton( "", (68), (54+6), (64), (16));
	m_pPlay->setContentAlignment( vgui::Label::a_west );
	m_pPlay->setParent( m_pPanel );
	m_pPlay->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pPlay->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pPlay->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pPlay->setImage( LoadTGAForRes("play") );
	m_pPlay->addActionSignal( new CActionWinamp(1));

	m_pPause = new CommandButton( "", (134), (54+6), (64), (16));
	m_pPause->setContentAlignment( vgui::Label::a_west );
	m_pPause->setParent( m_pPanel );
	m_pPause->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pPause->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pPause->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pPause->addActionSignal( new CActionWinamp(2));

	m_pPause->setImage( LoadTGAForRes("pause") );

	m_pStop = new CommandButton( "", (200), (54+6), (64), (16));
	m_pStop->setContentAlignment( vgui::Label::a_west );
	m_pStop->setParent( m_pPanel );
	m_pStop->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pStop->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pStop->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pStop->addActionSignal( new CActionWinamp(0));

	
	m_pStop->setImage(  LoadTGAForRes("stop") );
	
	m_pNextTrack = new CommandButton( "", (266), (54+6), (64), (16));
	m_pNextTrack->setContentAlignment( vgui::Label::a_center );
	m_pNextTrack->setParent( m_pPanel );
	m_pNextTrack->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pNextTrack->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pNextTrack->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pNextTrack->setImage( LoadTGAForRes("next") );
	m_pNextTrack->addActionSignal( new CActionWinamp(4));
	
}

void CMp3Player :: SetActiveInfo( int iShowText )
{

}