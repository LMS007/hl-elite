//=========== (C) Copyright 1996-2001 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: TFC Team Menu
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "vgui_int.h"
#include "VGUI_Font.h"
#include "VGUI_ScrollPanel.h"
#include "VGUI_TextImage.h"

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hle_menu.h"

//XRES(80), YRES(80), XRES(480), YRES(330));
// Team Menu Dimensions
#define TEAMMENU_TITLE_X				XRES(40)
#define TEAMMENU_TITLE_Y				YRES(32)
#define TEAMMENU_TOPLEFT_BUTTON_X		XRES(15)//XRES(40)
#define TEAMMENU_TOPLEFT_BUTTON_Y		YRES(80)
#define TEAMMENU_BUTTON_SIZE_X			XRES(124)
#define TEAMMENU_BUTTON_SIZE_Y			YRES(24)
#define TEAMMENU_BUTTON_SPACER_Y		YRES(1)//YRES(8)
#define TEAMMENU_WINDOW_X				XRES(80)//XRES(176)
#define TEAMMENU_WINDOW_Y				YRES(80)//YRES(80)
#define TEAMMENU_WINDOW_SIZE_X			XRES(480)//XRES(424)
#define TEAMMENU_WINDOW_SIZE_Y			XRES(330)//YRES(312)
#define TEAMMENU_WINDOW_TITLE_X			XRES(16)
#define TEAMMENU_WINDOW_TITLE_Y			YRES(16)
#define TEAMMENU_WINDOW_TEXT_X			XRES(16)
#define TEAMMENU_WINDOW_TEXT_Y			YRES(48)
#define TEAMMENU_WINDOW_TEXT_SIZE_Y		YRES(178)
#define TEAMMENU_WINDOW_INFO_X			XRES(16)
#define TEAMMENU_WINDOW_INFO_Y			YRES(234)

#define MAP_WINDOW_SIZE_X		XRES(310) // YRES(310)
#define MAP_WINDOW_SIZE_Y		YRES(265)
#define MAP_WINDOW_X			XRES(235)
#define MAP_WINDOW_Y			YRES(135)
      
// Creation

CTeamMenuPanel::CTeamMenuPanel(int iTrans, int iRemoveMe, int x,int y,int wide,int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	SchemeHandle_t hTeamWindowText = pSchemes->getSchemeHandle( "Briefing Text" );
	SchemeHandle_t hTeamInfoText = pSchemes->getSchemeHandle( "Text" );

//	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	// get the Font used for the Titles
	Font *pTitleFont = pSchemes->getFont( hTitleScheme );
	int r, g, b, a;


	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
	
	// Create the Info Window
	//m_pTeamWindow  = new Panel( TEAMMENU_WINDOW_X, TEAMMENU_WINDOW_Y, TEAMMENU_WINDOW_SIZE_X, TEAMMENU_WINDOW_SIZE_Y );
	m_pTeamWindow = new Panel( (ScreenWidth / 2.0f) - (462.0f / 2.0f), (ScreenHeight/ 2.0f) - (168.0f / 2.0f), (462), (168));
	m_pTeamWindow->setParent( this );
	m_pTeamWindow->setBorder( new LineBorder( Color(0,0,0,0 )) );
	m_pTeamWindow->setBgColor( 0, 0, 0, 120);


	m_pLabel = new Label( "", (6), (6), (450), (30) );
	m_pLabel->setParent( m_pTeamWindow );
	m_pLabel->setContentAlignment( vgui::Label::a_center );
	m_pLabel->setBorder( new LineBorder( Color(0,0,0,50)));
    m_pLabel->setBgColor( 0, 0, 0, 255 );
	m_pLabel->setImage( LoadTGAForRes("teammenu"));

	
	m_pPictureLabel = new Label( "", (156), (42), (150), (90) );
	m_pPictureLabel->setParent( m_pTeamWindow );
	m_pPictureLabel->setBorder( new LineBorder( Color(0,0,0,0)));
    m_pPictureLabel->setBgColor( 0, 0, 0, 255 );

	for (int i = 0; i < 3; i++)
	{
		char sz[256]; 
		int iYPos = 42 + (30*(i));

		// Team button

		m_pButtons[i] = new CommandButton( "", 6, iYPos, 149, 29);
		m_pButtons[i]->setParent( m_pTeamWindow );
		m_pButtons[i]->setContentAlignment( vgui::Label::a_west );

		m_pButtons[i]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
		m_pButtons[i]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );

		m_pButtons[i]->addActionSignal( new CMenuHandler_SoundCommand("common/wpn_select.wav"));
		m_pButtons[i]->setFont(pFontButtons);

		sprintf( sz, "%d", i+1 );
		m_pButtons[i]->addActionSignal( new CMenuHandler_ChangeTeam(sz, true ) );
		m_pButtons[i]->addInputSignal( new CMenuHandler_TeamPictire(i) );
	}

	int i;
	for (i = 3; i < 6; i++)
	{
		char sz[256]; 
		int iYPos = 42 + (30* (i-3));

		// Team button

		m_pButtons[i] = new CommandButton( "", 306, iYPos, 149, 29);
		m_pButtons[i]->setParent( m_pTeamWindow );
		m_pButtons[i]->setContentAlignment( vgui::Label::a_west );
		//m_pButtons[i]->setVisible( false );

		m_pButtons[i]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
		m_pButtons[i]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );

		m_pButtons[i]->addActionSignal( new CMenuHandler_SoundCommand("common/wpn_select.wav"));
		m_pButtons[i]->setFont(pFontButtons);
		sprintf( sz, "%d", i+1 );
		m_pButtons[i]->addActionSignal( new CMenuHandler_ChangeTeam(sz, true ) );
		//m_pButtons[i]->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
		m_pButtons[i]->addInputSignal( new CMenuHandler_TeamPictire(i) );
		m_pButtons[i]->setPaintBackgroundEnabled(true);
		

	}

	m_pTeamPics[0] = LoadTGAForRes("blueteam");
	m_pTeamPics[1] = LoadTGAForRes("redteam");
	m_pTeamPics[2] = LoadTGAForRes("greenteam");
	m_pTeamPics[3] = LoadTGAForRes("yellowteam");
	m_pTeamPics[4] = LoadTGAForRes("purpleteam");
	m_pTeamPics[5] = LoadTGAForRes("orangeteam");

	
	// Create the Cancel button
	m_pMainMenu = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Main Menu" ), (6), (132), (149), (29));
	m_pMainMenu->setContentAlignment(vgui::Label::a_west);
	m_pMainMenu->setParent( m_pTeamWindow );
	m_pMainMenu->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pMainMenu->addActionSignal( new CMenuHandler_ChangeGUIMenu( 30 ) );
	m_pMainMenu->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pMainMenu->setUnHighLightColor( 30, 30, 40, 100 );
	m_pMainMenu->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pMainMenu->setFont(pFontButtons);

	

	m_pSpectateButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Spectate" ), (156), (132), (149), (29));
	m_pSpectateButton->setContentAlignment(vgui::Label::a_west);
	m_pSpectateButton->setParent( m_pTeamWindow );
	m_pSpectateButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pSpectateButton->addActionSignal( new CMenuHandler_StringCommand( "spectate" ) );  
	m_pSpectateButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pSpectateButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pSpectateButton->setFont(pFontButtons);
	m_pSpectateButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );



	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), (306), (132), (149), (29));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pTeamWindow );
	m_pCancelButton->addActionSignal(  new CMenuHandler_ClearVote() );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( 30, 30, 40, 100 );
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("vgui/clickbutton.wav"));
	m_pCancelButton->setFont(pFontButtons);


}


//-----------------------------------------------------------------------------
// Purpose: Called everytime the Team Menu is displayed
//-----------------------------------------------------------------------------
void CTeamMenuPanel::Update( void )
{

	m_pMainMenu->cursorExited();
	m_pCancelButton->cursorExited();
	m_pSpectateButton->cursorExited();
	for (int i = 0; i < 6; i++)
	{
		m_pButtons[i]->setText(gViewPort->GetTeamName(i+1));
		m_pButtons[i]->cursorExited();
		if(*gViewPort->GetTeamName(i+1)==NULL)
		{
			m_pButtons[i]->setHighLightColor( 0, 0, 0, 255 );
			m_pButtons[i]->setUnHighLightColor( 0, 0, 0, 255 );
		}
		else
		{
			m_pButtons[i]->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
			m_pButtons[i]->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
		}

	}
}
