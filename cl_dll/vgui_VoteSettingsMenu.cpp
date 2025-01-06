#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"


CVoteSettingsMenu :: CVoteSettingsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );

	int r, g, b, a;

	m_pPanel = new CTransparentPanel( 200, XRES(80), YRES(80), XRES(480), YRES(330));
	m_pPanel->setParent( this );
	m_pPanel->setBorder( new LineBorder( Color(255 * 0.7,170 * 0.7,0,0) ) );

	
  	////////////HEADER///////////////////////////////////////////////
	m_pTitle = new Label( "", XRES(1), YRES(1), XRES(478), YRES(30) );
	m_pTitle->setParent( m_pPanel );
	m_pTitle->setFont( pSchemes->getFont(hTitleScheme) );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
	m_pTitle->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	m_pTitle->setBgColor( r, g, b, a );
	m_pTitle->setContentAlignment( vgui::Label::a_center );
	m_pTitle->setText( "Main Menu" );


	// Vote Modes/////////////////////////////////////////////
	m_pVoteModes = new CommandButton( "Vote Modes", XRES(10), YRES(40), XRES(120), YRES(40));
	m_pVoteModes->setContentAlignment( vgui::Label::a_west );
	m_pVoteModes->setParent( m_pPanel );


	// Cancel Button////////////////////////////////////////////////////////
	m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ), XRES(350), YRES(140), XRES(120), YRES(40));
	m_pCancelButton->setContentAlignment(vgui::Label::a_west);
	m_pCancelButton->setParent( m_pPanel );

}


CVoteSettingsMenu :: CVoteSettingsMenuResetButton(void)
{
	m_pCancelButton->cursorExited();
}