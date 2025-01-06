#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"


extern cvar_t *cl_radar;
extern globalvars_t  *gpGlobals;
extern vec3_t v_origin;		// last view origin
extern vec3_t v_angles;		// last view angle
extern "C" int g_TeamplayClient;

DECLARE_MESSAGE(m_Radar, Radar )
DECLARE_MESSAGE(m_Radar, Flag )
DECLARE_MESSAGE(m_Radar, Location )

#define RADAR_UPDATE_DELAY 0.05f

#define PI_HALF			1.5708
#define PI				3.14159
#define THREE_PI_HALF	4.71239
#define TWO_PI			6.28318
#define PI_180			0.017453


CRadarList::CRadarList()
{
	m_pHead = NULL;
}
CRadarNode* CRadarList::AddNode(int id, float x, float y, int color, int player)
{
	CRadarNode* pTemp = m_pHead;
	CRadarNode* pParent = NULL;
	CRadarNode* pReturn = NULL;
	while(pTemp)
	{
		if(pTemp->m_nID == id && id != 0)
		{
			pTemp->m_fPosX = x;
			pTemp->m_fPosY = y;
			pTemp->m_nColor = color;
			pTemp->m_nIntensity = 255;
			pTemp->m_nPlayer = player;
			return pTemp;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
		
	}
	
	if(pParent)
	{
		pParent->m_pNext = pReturn = new CRadarNode(id, x, y, color, player);
		pParent->m_pNext->m_pNext = NULL;
	}
	else
	{
		m_pHead = pReturn= new CRadarNode(id, x, y, color, player);
		m_pHead->m_pNext = NULL;
	}
	return pReturn;
}

void CRadarList::RemoveNode(CRadarNode* flag)
{
	CRadarNode* pTemp = m_pHead;
	CRadarNode* pParent = NULL;
	while(pTemp)
	{
		if(pTemp == flag)
		{
			if(pParent)
			{
				pParent->m_pNext = pTemp->m_pNext;
				delete pTemp;
			}
			else
			{
				m_pHead = m_pHead->m_pNext;
				delete pTemp;
			}
			return;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
}


CRadarNode* CRadarList::FindNode(int id)
{
	CRadarNode* pTemp = m_pHead;
	while(pTemp)
	{
		if(pTemp->m_nID == id)
		{
			return pTemp;
		}
		pTemp = pTemp->m_pNext;
	}
	return NULL;
}


void CRadarList::SetFade(int id)
{
	
	CRadarNode* pTemp = m_pHead;
	while(pTemp)
	{
		if(pTemp->m_nID == id)
		{
			pTemp->m_nIntensity-=1;
			return;
		}
		pTemp=pTemp->m_pNext;
	}
	}
void CRadarList::ClearNodes()
{
	CRadarNode* pTemp = m_pHead;
	CRadarNode* pNext = NULL;
	while(pTemp)
	{
		pNext = pTemp->m_pNext;
		delete pTemp;
		pTemp = pNext;
	}
	m_pHead = NULL;
}
CRadarList::~CRadarList()
{
	ClearNodes();
}

//bool g_RadarOn;

CHudRadar::CHudRadar()
{
}

int CHudRadar::Init()
{
	HOOK_MESSAGE(Radar);
	HOOK_MESSAGE(Location);
	HOOK_MESSAGE(Flag);
	
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

int CHudRadar::VidInit()
{
	radar_origin_x = radar_origin_y = m_radius = XRES(40);
	m_dist_factor = 0.015;

	m_player_index  = gHUD.GetSpriteIndex( "radar_player" ); 
	m_flag_index  = gHUD.GetSpriteIndex( "radar_flag" ); 
	m_origin_index = gHUD.GetSpriteIndex( "radar_origin" );
	m_radar_index  = gHUD.GetSpriteIndex( "radar_screen" );
	m_divider_index = gHUD.GetSpriteIndex( "divider" );
	

	m_radar_sprite = gHUD.GetSprite(m_radar_index);
	m_player_sprite = gHUD.GetSprite(m_player_index);
	m_flag_sprite = gHUD.GetSprite(m_flag_index);
	m_origin_sprite = gHUD.GetSprite(m_origin_index);
	m_divider_sprite = gHUD.GetSprite( m_divider_index );

	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

void CHudRadar::InitHUDData( void )
{	
	// delete flag lit here
	m_flag_list.ClearNodes();
	m_player_list.ClearNodes();
	m_bRadarOn = false;
}

void CHudRadar::Reset( void )
{

}

void CHudRadar::ShowRadar()
{
	m_bRadarOn = true;
	if(cl_radar->value!=0 && g_TeamplayClient)
	{
		m_iFlags |= HUD_ACTIVE;
		m_fUpdateDelay = 0; // update now
	}
}
void CHudRadar::HideRadar()
{
	m_bRadarOn = false;
	m_iFlags &= ~HUD_ACTIVE;
}

bool CHudRadar::RadarOn()
{
	return m_bRadarOn;
}

int CHudRadar::DrawPlayer(int nIndex, int x, int y)
{	
	SPR_Set(gHUD.GetSprite(nIndex), g_hud_color.getRed(), g_hud_color.getGreen(), g_hud_color.getBlue() );
	wrect_t wc = gHUD.GetSpriteRect(nIndex);
	SPR_DrawAdditive( 0,  x-((wc.right-wc.left)>>1), y-((wc.bottom-wc.top)>>1), &wc);
	return 1;
}


int CHudRadar::Draw(float flTime)
{
	static int Y = 0;
	static int X = 0;
	static int r, g, b, a;
	static wrect_t rc;

	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;

	rc.left = 0;
	rc.top = 127;
	rc.bottom = 128;

	r = g_hud_color.getRed();
	g = g_hud_color.getGreen();
	b = g_hud_color.getBlue();
	a = 40;
	ScaleColors(r, g, b, a );


	float r2 = m_radius*m_radius;

	for ( Y = -m_radius; Y < m_radius; Y++ )
	{
		X = sqrt(r2-Y*Y);
		X = m_radius-X;

		rc.right = 2*(m_radius-X);
		if(rc.right ==0)
			continue;
		SPR_Set(m_radar_sprite, r, g, b );
		SPR_DrawAdditive( 0,  X, Y+m_radius, &rc);
	}

	/*if ( gHUD.m_iIntermission  )
	{
		m_iFlags &= ~HUD_ACTIVE;
	}*/

	UpdateRadar(); // update every frame 
	
	rc = gHUD.GetSpriteRect(m_radar_index);
	rc.right = (5);
	SPR_Set(m_radar_sprite, 255, 255, 255 );
	SPR_DrawAdditive( 0,  radar_origin_x-(2), radar_origin_y, &rc);

	rc = gHUD.GetSpriteRect(m_divider_index);
	rc.bottom = (5);
	rc.right--;
	SPR_Set(m_divider_sprite, 255, 255, 255 );
	SPR_DrawAdditive( 0,  radar_origin_x, radar_origin_y - (2), &rc);

	return 1;
}

void CHudRadar::CalcPos(float player_x, float player_y,float* calc_x, float* calc_y)
{
	static float real_angle, raw_angle, x_offset, y_offset, distance;
	static float x, y;
	Vector s;
	x_offset = v_origin[0] - player_x;
	y_offset = v_origin[1] - player_y;
	distance = sqrt(x_offset*x_offset + y_offset*y_offset);

	if(x_offset == 0)	x_offset = 1;
	if(y_offset == 0)	y_offset = 1;
					
	raw_angle = acos(y_offset / distance);
					
	if(x_offset >=0)
		raw_angle *= -1;

	if(raw_angle < -PI_HALF && raw_angle > -PI)
		raw_angle = raw_angle + THREE_PI_HALF;
	else 
		raw_angle -=PI_HALF;
					
	real_angle= raw_angle - (PI_180)*  v_angles[1];

	if(real_angle <-PI)
		real_angle += TWO_PI;
	else if(real_angle > PI)
	{
		real_angle -= TWO_PI;
	}
									
	if(distance > 2667.0f) // hard coded for every resolution
	{
		distance = 40.0f;
	}
	else
		distance *=m_dist_factor;
					
	x = (sin(real_angle) * distance);
	y = (cos(real_angle) * distance);

	*calc_x =  XRES(x);
	*calc_y =  XRES(y);
}

void CHudRadar::UpdateRadar()
{
	

	static float res_x, res_y;


	CRadarNode* pPlr = m_player_list.m_pHead;
	while(pPlr)
	{
		CalcPos(pPlr->m_fPosX, pPlr->m_fPosY, &res_x, &res_y);
		DrawPlayer(m_player_index, radar_origin_x-res_x+1, radar_origin_y-res_y+1);					
		
		CRadarNode* pNode = m_flag_list.m_pHead;
		while(pNode)
		{
			if(pNode->m_nPlayer == pPlr->m_nID)
			{
				DrawFlag(&pNode, radar_origin_x-res_x+1, radar_origin_y-res_y+1);
				break;
			}
			pNode = pNode->m_pNext;
		}					
		pPlr = pPlr->m_pNext;
	}

	CRadarNode* pFlag = m_flag_list.m_pHead;
	while(pFlag)
	{
		if(pFlag->m_nPlayer == 0)
		{
			CalcPos(pFlag->m_fPosX, pFlag->m_fPosY, &res_x, &res_y);
			if(DrawFlag(&pFlag, radar_origin_x-res_x+1, radar_origin_y-res_y+1))			
				pFlag = pFlag->m_pNext;	
		}
		else
			pFlag = pFlag->m_pNext;	
	}	
}


int CHudRadar::DrawFlag(CRadarNode** pFlag, float x, float y)
{
	int r = iFlagColors[(*pFlag)->m_nColor % iNumberOfTeamColors][0];
	int g = iFlagColors[(*pFlag)->m_nColor % iNumberOfTeamColors][1];
	int b = iFlagColors[(*pFlag)->m_nColor % iNumberOfTeamColors][2];

	if((*pFlag)->m_nIntensity < 255)
	{
		if((*pFlag)->m_nIntensity <= 0)
		{	
			CRadarNode *pTemp = (*pFlag)->m_pNext;
			m_flag_list.RemoveNode(*pFlag);
			*pFlag = pTemp;
			return 0;				
		}
		ScaleColors(r, g, b, (*pFlag)->m_nIntensity );	
		(*pFlag)->m_nIntensity -= gHUD.m_flTimeDelta * 85.0f;
	}

	SPR_Set(gHUD.GetSprite(m_flag_index), r, g, b);
	wrect_t wc = gHUD.GetSpriteRect(m_flag_index);
	SPR_DrawAdditive( 0,  x-((wc.right-wc.left)>>1), y-((wc.bottom-wc.top)>>1), &wc);
	return 1;
}

int CHudRadar::MsgFunc_Radar(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	m_nEntIndexOrigin = READ_SHORT(); // which player are we ( note that spectates will have a different origin so the client may not always be the center)
	int nEvent = READ_BYTE(); 

	switch(nEvent)
	{
	case 1: 
		ShowRadar();
		break;
	case 0: 
		HideRadar();
	}
 	return 1;
}

int CHudRadar::MsgFunc_Flag(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int flag = READ_SHORT();
	int player = READ_SHORT();
	float x	  = READ_SHORT();
	float y	  = READ_SHORT();
	int  color = READ_BYTE();
	int action = READ_BYTE();

	if(action==3){}
		//m_flag_list.AddFlag(flag, x, y, color, player);			
	
	else if(action==2)
		m_flag_list.RemoveNode(m_flag_list.FindNode(flag));
	
	else if(action == 1)  // temp flag only
	{
		m_flag_list.AddNode(flag, x, y, color, 0)->m_nIntensity-=1;	// 0 is a temp flag, start the fade right away
	}

	else
		m_flag_list.AddNode(flag, x, y, color, player);			

 	return 1;
}


int CHudRadar::MsgFunc_Location(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int player = READ_SHORT();
	int action = READ_BYTE();
	float x	  = READ_SHORT();
	float y	  = READ_SHORT();
	
	CRadarNode* pNode = m_player_list.FindNode(player);

	if(action==0)

		m_player_list.RemoveNode(pNode);

	else if (action == 1)
	{
		if(pNode)
		{
			pNode->m_fPosX = x;
			pNode->m_fPosY = y;
		}
		else
		{
			m_player_list.AddNode(player, x, y, 0, 0);
		}
	}
	else if (action == 2)
	{
		m_player_list.ClearNodes();
	}
	return 1;
}