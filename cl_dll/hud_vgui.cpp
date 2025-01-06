#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_teamfortressviewport.h"
//#include "vgui_hle_menu.h"
//extern TeamFortressViewport *gViewPort;

int CHudVgui::Init( void )
{
	gHUD.AddHudElem( this );
	return 1;
}
int CHudVgui::VidInit( void )
{
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}
int CHudVgui::Draw(float flTime) // VguiThink
{
	return 1;
}
void CHudVgui::ActivateVguiThink(bool bActive)
{
	if(bActive)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
}


int CHudVguiMaps::Init( void )
{
	gHUD.AddHudElem( this );
	return 1;
}
int CHudVguiMaps::VidInit( void )
{
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}
int CHudVguiMaps::Draw(float flTime) // VguiThink
{
//	gViewPort->m_pVMapsMenu->AnimatePage();
	return 1;
}
