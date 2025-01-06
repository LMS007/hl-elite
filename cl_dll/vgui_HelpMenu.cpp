/*#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hle_menu.h"
#include "pm_shared.h"
#include "..\common\hltv.h"
#include"vgui_ConsolePanel.h"
#include "vgui_scheme.h"
*/


#include "vgui_int.h"
#include "VGUI_Font.h"
#include "VGUI_ScrollPanel.h"
#include "VGUI_TextImage.h"


#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hle_menu.h"

//gViewPort->ShowVGUIMenu(36);

int CHelpMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);

void ResetButtons();
void SetActiveInfo( int iShowText );
void setVisible( bool visible );

void CHelpMenu :: setVisible( bool visible )
{
	CMenuPanel::setVisible(visible);
}

CHelpMenu :: CHelpMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	Font* pFontButtons = pSchemes->getFont(hButtonScheme);

	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "VGUI_MAIN" );
	SchemeHandle_t hTeamWindowText = pSchemes->getSchemeHandle( "Briefing Text" );
	SchemeHandle_t hTeamInfoText = pSchemes->getSchemeHandle( "Text" );

	int r, g, b, a;

	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );


	m_pPanel = new Panel( XRES(80), YRES(60), XRES(480), YRES(360));
	m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(0, 0,0,0) ) );   
	m_pPanel->setBgColor( 0, 0, 0, 120);

	m_pTitle = new Label( "Help Menu", XRES(5), YRES(5), XRES(469), YRES(35) );
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setContentAlignment( vgui::Label::a_center );
	m_pTitle->setBorder( new LineBorder( Color(0,0,0,50)));
    m_pTitle->setBgColor( 150, 185, 225, 120 );

		// Create the Map Briefing panel

	m_pScrollPanel = new CTFScrollPanel( XRES(10), YRES(40), XRES(460), YRES(275));
	m_pScrollPanel->setParent(m_pPanel); 
	m_pScrollPanel->setScrollBarVisible(false, false);

	m_pBriefing = new TextPanel("", 15,10, YRES(460), YRES(275) );
	m_pBriefing->setParent( m_pScrollPanel->getClient() );
	m_pBriefing->setFont( pSchemes->getFont(hTeamWindowText) );	
	pSchemes->getFgColor( hTeamWindowText, r, g, b, a );
	//m_pBriefing->setFgColor( 255, 255, 255, 255 );
	pSchemes->getBgColor( hTeamWindowText, r, g, b, a );
	m_pBriefing->setBgColor( r, g, b, a );

	m_pCancelButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Close" ), XRES(350), YRES(320), XRES(100), YRES(29));
	m_pCancelButton->setParent( m_pPanel );
	m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pCancelButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pCancelButton->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pCancelButton->addActionSignal( new CMenuHandler_SoundCommand("common/wpn_select.wav"));
	m_pCancelButton->setFont(pFontButtons);

	m_pReturnButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Main Menu" ), XRES(20), YRES(320), XRES(100), YRES(29));
	m_pReturnButton->setParent( m_pPanel );
	m_pReturnButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
	m_pReturnButton->addActionSignal( new CMenuHandler_ChangeGUIMenu( 30 ) );
	m_pReturnButton->setHighLightColor( COMMAND_BUTTON_HIGHLIGHT_RED, COMMAND_BUTTON_HIGHLIGHT_GREEN, COMMAND_BUTTON_HIGHLIGHT_BLUE, COMMAND_BUTTON_HIGHLIGHT_ALPHA );
	m_pReturnButton->setUnHighLightColor( COMMAND_BUTTON_RED, COMMAND_BUTTON_GREEN, COMMAND_BUTTON_BLUE, COMMAND_BUTTON_ALPHA );
	m_pReturnButton->addActionSignal( new CMenuHandler_SoundCommand("common/wpn_select.wav"));
	m_pReturnButton->setFont(pFontButtons);
}


void CHelpMenu ::Update()
{
	char sz[256]; /*
	char szTitle[256]; 
	char *ch;

	
	strcpy( sz, level );
	ch = strchr( sz, '/' );
	if (!ch)
		ch = strchr( sz, '\\' );
	strcpy( szTitle, ch+1 );
	ch = strchr( szTitle, '.' );
	*ch = '\0';
	m_pMapTitle->setText( szTitle );
	*ch = '.';

	// Update the map briefing
	strcpy( sz, level );
	ch = strchr( sz, '.' );
	*ch = '\0';
	strcat( sz, ".txt" );*/
	*sz = NULL;
	strcpy(sz, "help/help.txt");
	char *pfile = (char*)gEngfuncs.COM_LoadFile( sz, 5, NULL );
	if (pfile)
	{
		m_pBriefing->setText( pfile );
		// Get the total size of the Briefing text and resize the text panel
		int iXSize, iYSize;
		m_pBriefing->getTextImage()->getTextSize( iXSize, iYSize );
		m_pBriefing->setSize( iXSize, iYSize );
		gEngfuncs.COM_FreeFile( pfile );
	}
	m_pScrollPanel->validate();
}


void CHelpMenu :: ResetButtons(void)
{
	m_pReturnButton->cursorExited();
	m_pCancelButton->cursorExited();
}
void CHelpMenu :: SetActiveInfo( int iInfo )
{
	CMenuPanel::SetActiveInfo(iInfo);
}
