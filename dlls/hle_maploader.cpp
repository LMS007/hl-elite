#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

// hle
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
// end hle

//#include    <stdio.h>
#include    <io.h>
#include    <time.h>

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include    "hle_vote.h"
#include	"hle_maploader.h"
#include    "hle_utility.h"


#ifndef CLIENT_DLL

extern int gmsgMapNames;


///SERVER MAPLOADER OBJ
CMapList g_MapsServer;
///////////////////////
#else
///CLIENT MAPLOADER OBJ
CMapList g_MapsClient;
CMapList g_MapsDownloaded;
//////////////////////

#endif

CMapName::CMapName(char* pName)
{

    m_pName = NULL;
	setName(pName);
	m_bLocal = false;
	m_nPreviewIndex = -1;
	m_pNext = NULL;
}

CMapName::CMapName(CMapName& pCopy)
{
	m_pName = NULL;
    *this = pCopy;
}

void CMapName::operator = (CMapName& pCopy)
{
	setName(pCopy.getName());
	m_bLocal = pCopy.getLocal();
	m_nPreviewIndex = pCopy.getPreview();
}

CMapName::CMapName()
{
    m_pName = NULL;
	m_bLocal = false;
}

CMapName::~CMapName()
{
    if(m_pName)
	{
		delete[] m_pName;
	}
}

void CMapName::setLocal( bool bLocal)
{
	m_bLocal = bLocal;
}

bool CMapName::getLocal()
{
	return m_bLocal;
}

void CMapName::setPreview( int bPreview)
{
	m_nPreviewIndex = bPreview;
}

int CMapName::getPreview()
{
	return m_nPreviewIndex;
}



void CMapName::setName(char* cName)
{
	if(cName)
	{
		if(m_pName)
			delete[] m_pName;
		m_pName = new char[strlen(cName)+1];
		strcpy(m_pName, cName);
	}
}

char* CMapName::getName()
{
	if(m_pName)
		return m_pName;
	else
		return " "; // no name
}

bool  CMapName::operator ==(CMapName* pMap)
{
	if(!stricmp(pMap->getName(), m_pName))
		return true;
	return false;
}

bool CMapName::operator < (CMapName& pMap)
{
	int result = stricmp(m_pName, pMap.getName());
	return result < 0 ?  true : false;
}

CMapName* CMapName::createMap(char* pName) // static
{
	if(pName)
	{
		// is up to the user to delete this.
		
		CMapName* pNewMap = new CMapName(pName);
		
		return pNewMap;
	}
	return NULL;
}

void CMapName::destroyMap(CMapName* pMap) // static
{
	if(pMap)
	{
		delete pMap;
	}
}


///////////////////////////////
///////class CMapList//////////
///////////////////////////////


CMapList::CMapList()
{
	initList();
}

CMapList::~CMapList()
{
	clear();
}

void CMapList::addMapBack( CMapName* pMap )
{
	if(!pMap) 
		return;

	CMapName* pTemp = m_pHead;
	CMapName* pParent = NULL;
	while(pTemp)
	{
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	if(!pParent)
	{
		m_pHead = pMap;
		m_pHead->m_pNext = NULL;
	}
	else
	{
		pParent->m_pNext = pMap;
		pMap->m_pNext = NULL;
	}
	m_nSize++;
}

void CMapList::addMapBack( char* pName )
{
	if(!pName) 
		return;
	addMapBack(CMapName::createMap(pName));
	return;
}

void CMapList::addMapFront( CMapName* pMap )
{
	if(!pMap) 
		return;

	if(!m_pHead)
	{
		m_pHead = pMap;
		m_pHead->m_pNext=NULL;
		m_nSize = 1;
	}
	else
	{
		pMap->m_pNext = m_pHead;
		m_pHead = pMap;
		m_nSize++;
	}
}

void CMapList::addMapFront( char* pName )
{
	if(!pName) 
		return;
	addMapBack(CMapName::createMap(pName));
	return;
}




void CMapList::removeMap(CMapName* pMap)
{
	if(pMap == NULL)
		return;	
	CMapName* pDelete = m_pHead;
	CMapName* pParent = NULL;
	while(pDelete)
	{
		if(*pDelete == pMap)
		{
			CMapName* pTemp = pDelete->m_pNext;
			CMapName::destroyMap(pDelete);
			if(pParent)
				pParent->m_pNext = pTemp;
			else
				m_pHead = pTemp;
			break;
		}
		pParent = pDelete;
		pDelete = pDelete->m_pNext;
	}
	m_nSize--;
	if(!m_nSize)
		clear();
}	


CMapName* CMapList::operator [] ( int nIndex )
{
	if(nIndex >= m_nSize)
		return NULL;

	CMapName* pTemp = m_pHead;
	
	for(int i=0; i < m_nSize; i++)
	{
		if(i == nIndex)
			break;
		pTemp = pTemp->m_pNext;
	}

	return pTemp;
}

void CMapList::addMapSmart(CMapName* pMap)
{
	CMapName* pTemp = m_pHead;
	CMapName* pParent = NULL;
	while(pTemp)
	{
		if(*pMap == pTemp) // duplicate
		{
			return;
		}
		if(*pMap < *pTemp)
		{	
			if(!pParent)
			{
				m_pHead = pMap;
				pMap->m_pNext = pTemp;
			}
			else
			{
				pParent->m_pNext = pMap;
				pMap->m_pNext = pTemp;
			}
			m_nSize++;
			return;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	if(pParent == NULL)
	{
		m_pHead = pMap;
		m_pHead->m_pNext=NULL;
		m_nSize++;
	}
	else
	{
		pParent->m_pNext = pMap;
		pMap->m_pNext = NULL;
		m_nSize++;
	}
}

void CMapList::addMapSmart(char* pszName)
{
	addMapSmart(new CMapName(pszName));
}

bool CMapList::locateMap(CMapName* pMap)
{
 	CMapName* pTemp = m_pHead;
	while(pTemp)
	{
		if(*pTemp == pMap)
			return true;
		else if(*pMap < *pTemp )
			return false;
		else
			pTemp = pTemp->m_pNext;
	}
	return false;
}

int CMapList::getMapOrder(CMapName* pMap)
{
	int order = 0;
	CMapName* pTemp = m_pHead;
	while(pTemp)
	{
		if(*pTemp == pMap)
			return order;
		else if(*pMap < *pTemp )
			return -1;
		else
		{
			pTemp = pTemp->m_pNext;
			order++;
		}
	}
	return -1;
}

CMapName* CMapList::getHead()
{
	return m_pHead;
}

void CMapList::initList()
{
	m_pHead = NULL;
    m_nSize = 0;
	m_bGotMaps = false;
}


void CMapList::clear()
{
	CMapName* pTemp = m_pHead;
	while(pTemp)
	{
		m_pHead = m_pHead->m_pNext;
		CMapName::destroyMap(pTemp);
		pTemp = m_pHead;
	}
    initList();
}


// client side map check compair to determine if he has maps which match the servers.
void CMapList::AddMapsFromFile (CMapList* pList, char* pFileName)
{
	long hFile;
	struct _finddata_t bsp_file;
	
	if( (hFile = _findfirst( pFileName, &bsp_file )) == -1L )
	{
		return;
	}
	else
	{
		// find first bsp name

		strtok(bsp_file.name, ".");
		if(*bsp_file.name!='.')
		{
			char* suffix = strtok(NULL, ".");
			if( (suffix != NULL && stricmp(suffix, "bsp") != 0) ||
				((strncmp(bsp_file.name, "agctf", 5)) &&
				(strncmp(bsp_file.name, "c1a", 3)) &&
				(strncmp(bsp_file.name, "c2a", 3)) &&
				(strncmp(bsp_file.name, "c3a", 3)) &&
				(strncmp(bsp_file.name, "c4a", 3)) &&
				(strncmp(bsp_file.name, "t0a", 3))))
				pList->addMapSmart(bsp_file.name);
		}

        // Find the rest of the bsp files 
        while( _findnext( hFile, &bsp_file ) == 0 )
        {
            strtok(bsp_file.name, "."); // hax to remove .bsp
			if(*bsp_file.name!='.')
			{
				char* suffix = strtok(NULL, ".");
				if( (suffix != NULL && stricmp(suffix, "bsp") != 0) ||
					((strncmp(bsp_file.name, "agctf", 5)) &&
					(strncmp(bsp_file.name, "c1a", 3)) &&
					(strncmp(bsp_file.name, "c2a", 3)) &&
					(strncmp(bsp_file.name, "c3a", 3)) &&
					(strncmp(bsp_file.name, "c4a", 3)) &&
					(strncmp(bsp_file.name, "t0a", 3))))
					pList->addMapSmart(bsp_file.name);
			}
        }
        _findclose( hFile );
	}
}


void LocalizeList(CMapList* mClient, CMapList* mServer)
{
	CMapName* pClient = mClient->getHead();;
	CMapName* pServer = mServer->getHead();;
	int previewInedx = 0;
	while(1)
	{
		if(!pClient)
			return;

		else if(!pServer)
			return;

		if(*pClient==pServer)
		{
			pServer->setLocal(true);
			pServer->setPreview(previewInedx);
			pClient = pClient->m_pNext;
			pServer = pServer->m_pNext;
			previewInedx++;

		}
		else if(*pClient < *pServer)
		{
			pClient = pClient->m_pNext;
			previewInedx++;
		}
		
		else
			pServer = pServer->m_pNext;
	}
	return;
}

#ifndef CLIENT_DLL

CMapName* CMapList::SendMapsToClient( edict_t *client, CMapName* pStart, int nSize )
{
		if(!m_bGotMaps)
			return NULL;
        while(nSize)
	    {
			if(pStart)
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgMapNames, NULL, client );
				WRITE_STRING( pStart->getName());
				MESSAGE_END();   
				pStart = pStart->m_pNext;
				nSize--;
				continue;
			}
			break;
        }
		if(!pStart)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgMapNames, NULL, client );
			WRITE_STRING( "%"); // end char
			MESSAGE_END();   
		}
		return pStart;
}

#endif
#endif


