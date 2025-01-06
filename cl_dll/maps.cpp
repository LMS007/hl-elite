
//
// maps.cpp
//
/*#include    <stdio.h>
#include    <io.h>
#include    <time.h>*/

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "hle_maploader.h"
//#include <fstream>
//using namespace std;

void LocalizeList(CMapList* mClient, CMapList* mServer);
//extern ofstream g_iMapfile;
bool IsLocal(char* cMap);
//int g_NoMoreMaps = false;


DECLARE_MESSAGE(m_MapList, MapNames )


///////////////////////////////
///////class CHudMapList///////
///////////////////////////////

int CHudMapList::Init(void)
{
 
    HOOK_MESSAGE( MapNames );
    gHUD.AddHudElem(this);
	return 1;
};


void CHudMapList::InitHUDData( void ) 
{
	g_MapsDownloaded.clear();
}
int CHudMapList::MsgFunc_MapNames(const char *pszName,  int iSize, void *pbuf)
{
	//if(m_bNoMoreMaps)
	//	return 1;
    BEGIN_READ( pbuf, iSize );
	CMapName* temp = NULL;
	g_MapsDownloaded.addMapBack(temp = CMapName::createMap(READ_STRING()));
	if(temp)
	{
		if(!strcmp(temp->getName(), "%")) // this is the last map sent
		{
			g_MapsDownloaded.removeMap(temp);
			//m_bNoMoreMaps = true;
			g_MapsDownloaded.m_bGotMaps = true;
			LocalizeList(&g_MapsClient, &g_MapsDownloaded);
			//g_MapsClient.clear(); // we dont this memory anymore
		}
	}
	if(g_MapsDownloaded.size()>=1024)
	{
		g_MapsDownloaded.removeMap(temp);
		//m_bNoMoreMaps = true;
		g_MapsDownloaded.m_bGotMaps = true;
		LocalizeList(&g_MapsClient, &g_MapsDownloaded);
		//g_MapsClient.clear(); // we dont this memory anymore
	}
	return 1;
}

int CHudMapList::VidInit(void)
{
//	m_hSprite = 0;

	return 1;
};

int CHudMapList::Draw(float fTime)
{
	return 1;
}
