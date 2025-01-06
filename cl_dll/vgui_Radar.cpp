#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hle_menu.h"
#include "pm_shared.h"
#include "..\common\hltv.h"
#include"vgui_ConsolePanel.h"
#include "vgui_scheme.h"
#include "maps.h"
#include<VGUI_ScrollPanel.h>
#include<VGUI_ScrollBar.h>
#include<VGUI_Slider.h>
#include<VGUI_IntchangeSignal.h>// NTCHANGESIGNAL_H

/*
#define PI_HALF			1.5708
#define PI				3.14159
#define THREE_PI_HALF	4.71239
#define TWO_PI			6.28318
#define PI_180			0.017453

//int CRadar::m_nEntIndexOrigin = 0;


CRadar :: CRadar(int x, int y, int wide, int tall) : Panel( x,y,wide,tall)
{
	
	//int r = g_hud_color.getRed();
	//int g = g_hud_color.getGreen();
	//int b = g_hud_color.getBlue(); 

	//setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 96);
	int pixelsize = XRES(3);
	if(pixelsize < 4)
		pixelsize = 4;

	LineBorder *border = new LineBorder(Color(0, 0, 0, 128));
//	setBorder(border);
//	setPaintBorderEnabled(true);

	/*int Y = 0;
	int X = 0;

	radius = XRES(39);

	for ( Y = -radius; Y < YRES(39); Y++ )
	{
		X = sqrt(radius*radius-Y*Y);
		X = radius-X;
		m_pBackground[Y+radius] = new Panel( X, Y+YRES(39), 2*(radius-X), 1);
		//m_pBackground[Y+radius]->setBgColor(255, 0, 0, 200);
		m_pBackground[Y+radius]->setParent( this );
		//m_pBackground[Y+radius]->setBorder(border);
	}*/

	/*for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		m_pPlayers[i] = new Panel(0, 0, pixelsize,pixelsize);
		m_pPlayers[i]->setParent( this );
		m_pPlayers[i]->setBorder(border);
		m_pPlayers[i]->setVisible(false);
		//m_pPlayers[1]->setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 0);
	}*/
/*
	m_pOrigin = new Panel(XRES(39),XRES(39), pixelsize,pixelsize);
	m_pOrigin->setBgColor(255, 255, 255, 0);
	m_pOrigin->setParent( this );
	m_pOrigin->setBorder(border);

	m_dist_factor = 0.015;
	m_pos_x = XRES(39);
	m_pos_y = YRES(39);

	m_hPlayer = gHUD.GetSprite( gHUD.GetSpriteIndex( "radar_player" ) );

}

void CRadar :: setVisible( bool visible )
{
	// set these for each playa here
	//old_origin;
	//old_angles;
	//////////

	/*for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		m_pPlayers[i]->setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 0);
		m_pPlayers[i]->setVisible(false);
	}*//*

	int bound = radius*2;
	for(int i = 0; i  < bound; i++)
	{
		m_pBackground[i]->setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 210); 
	}
	//m_pPlayers[1]->setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 0);

	setBgColor(g_hud_color.getRed(), g_hud_color.getGreen() , g_hud_color.getBlue(), 255);
	Panel::setVisible(visible);
}

CRadar :: ResetButtons(void)
{
	//->cursorExited();
}

void UpdatePrediction()
{
	
}


int CRadar::DrawSprite(SpriteHandle_t* pSprite, int x, int y)
{
	int sprite = gHUD.GetSpriteIndex( "radar_player" );
	
	SPR_Set(gHUD.GetSprite(sprite), g_hud_color.getRed(), g_hud_color.getGreen(), g_hud_color.getBlue() );
	SPR_DrawAdditive( 0,  x, y, &gHUD.GetSpriteRect(sprite));
	
	
	return 1;
}

void CRadar::UpdateRadar()
{
	
	UpdatePrediction();

	cl_entity_t *base_player = gEngfuncs.GetEntityByIndex( m_nEntIndexOrigin );
	if(!base_player)
		return;

	static float real_angle, raw_angle, x_offset, y_offset, distance;
	static /*int*/ /*float x, y, res_x, res_y;

	for (int i = 1; i < MAX_PLAYERS; i++ )
	{
		cl_entity_t *target_player = gEngfuncs.GetEntityByIndex( i );
//		m_pPlayers[i]->setVisible(false);

		if(target_player)
		{
			if (target_player->player && target_player != base_player )
			{
				
				if(g_PlayerInfoList[i].spectator)
				{
					 continue;  // dont track spectators
				}
				if ( g_PlayerInfoList[i].name == NULL )
					continue; // empty player slot, skip
				if ( *g_PlayerInfoList[i].name ==  NULL )
					continue;
				if(g_PlayerExtraInfo[i].teamnumber  == g_PlayerExtraInfo[base_player->index].teamnumber) // only view same team
				{
					
					// test new prediction coords here
					// the idea is to draw from lag_origin to origin

					/*if(target_player->origin != target_player->old_origin
					{
						target_player->lag_origin = target_player->old_origin;
						target_player->old_origin = target_player->origin;
					}*/
					///*

//					x_offset = /*base_player->origin[0]*/v_origin[0] - target_player->origin[0];
//					y_offset = /*base_player->origin[1]*/v_origin[1] - target_player->origin[1];
//					distance = sqrt(x_offset*x_offset + y_offset*y_offset);
//					if(distance > 2667.0f) // hard coded for every resolution
//					{
//						continue;
//					}
/*
					
					if(x_offset == 0)	x_offset = 1;
					if(y_offset == 0)	y_offset = 1;
					
					raw_angle = acos(y_offset / distance);
					
					if(x_offset >=0)
						raw_angle *= -1;

					if(raw_angle < -PI_HALF && raw_angle > -PI)
						raw_angle = raw_angle + THREE_PI_HALF;
					else 
						raw_angle -=PI_HALF;
					
//					real_angle= raw_angle - (PI_180)* /*base_player->angles[1];*/ // v_angles[1];
//
/*
					if(real_angle <-PI)
						real_angle += TWO_PI;
					else if(real_angle > PI)
					{
						real_angle -= TWO_PI;
					}
					
					
					distance *=m_dist_factor;

					x = (sin(real_angle) * distance);
					y = (cos(real_angle) * distance);

					res_x =  XRES(x);
					res_y =  XRES(y);

					DrawSprite(&m_hPlayer, m_pos_x-res_x, m_pos_y-res_y);

					//m_pPlayers[i]->setPos(m_pos_x - res_x, m_pos_y - res_y);
					//m_pPlayers[i]->setVisible(true);
					//m_pPlayers[i]->repaint();
					
				}
			}	
		}
	}
}
*/