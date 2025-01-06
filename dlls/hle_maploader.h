#ifndef _MAP_LOADER_H
#define _MAP_LOADER_H


#define MAX_MAP_FILE_NAME_SIZE 32 // if a map name is longer hten 64 god help us!
#define MAPS_PER_PACKAGE 4

class CMapName;
class CMapList;

#ifndef CLIENT_DLL
extern CMapList g_MapsServer;
#else
extern CMapList g_MapsClient;
extern CMapList g_MapsDownloaded;
#endif


//#include "cl_entity.h"
//#include "hud.h"
//#include "vgui_TeamFortressViewport.h"

class CMapName
{

protected:

	char*	m_pName;
	bool	m_bLocal;
	int		m_nPreviewIndex;

public:
	
	CMapName(CMapName& cCopy);

	CMapName(char* cName);
	CMapName();
	virtual ~CMapName();
	char* getName();
	bool	getLocal();
	void setLocal(bool bLocal);
	void setName(char* cName);
	void setPreview( int bPreview);
	int getPreview();
	bool operator == (CMapName*);
	void  operator = (CMapName&);
	bool operator < (CMapName&);
	
	static CMapName* createMap(char* pName);
	static void destroyMap(CMapName* pMap);

	CMapName* m_pNext;

};

// generic abstraction
class CName : public CMapName
{
public:

	CName(char* cName) : CMapName(cName) {}

	CName() : CMapName() {}

	bool operator == (CName* pName)
	{
		CMapName* pThis = (CMapName*)this;
		return *pThis == (CMapName*) pName;
	}
	void  operator = (CName& pName)
	{
		CMapName* pThis = (CMapName*)this;
		*pThis = (CMapName&)pName;
	}
	bool operator < (CName& pName)
	{
		CMapName* pThis = (CMapName*)this;
		return *pThis < (CMapName&) pName;
	}

	static CName* createName(char* pName)
	{
		return (CName*)createMap(pName);
	}
	static void destroyName(CName* pName)
	{
		destroyMap(pName);
	}
};

class CMapList 
{

private:

	CMapName *m_pHead;
	int	m_nSize;
	
	void initList();
	
public:

	bool	m_bGotMaps;

	CMapList();
	CMapList(const CMapList* Copy);
	virtual~CMapList();
	void addMapBack(char* pName);
	void addMapBack(CMapName* pMap);
	void addMapFront(char* pName);
	void addMapFront(CMapName* pMap);
	void addMapSmart(CMapName* pMap);
	void addMapSmart(char*);
	CMapName* getHead();
	bool locateMap(CMapName*);
	int getMapOrder(CMapName*);
	int size() { return m_nSize; }
	void clear();
	void removeMap(CMapName* pMap);

	CMapName* operator [] ( int nIndex );
	void operator = (const CMapList* mapAssignment);
	
	static int compareOrder(CMapName* pFirst, CMapName* pSecond);
	static void AddMapsFromFile(CMapList* pList, char* pFileName);
	static void CompileMapList(CMapList* pList);//unfinished

#ifndef CLIENT_DLL
	CMapName* SendMapsToClient( edict_t *client, CMapName* pStart, int nSize );
#endif

	

	
};

// generic abstratction
class CNameList : public CMapList
{
public:
	CNameList() : CMapList() {}
	
	CNameList(const CNameList* pCopy) : CMapList((const CMapList*)pCopy) {}
	
	void addBack(char* pName)
	{
		addMapBack(pName);
	}
	void addBack(CName* pName)
	{
		addMapBack(pName);
	}
	void addFront(char* pName)
	{
		addMapFront(pName);
	}
	void addFront(CMapName* pName)
	{
		addMapFront(pName);
	}
	void addSmart(CName* pName)
	{
		addMapSmart((CMapName*)pName);
	}
	void addSmart(char* pName)
	{
		addMapSmart(pName);
	}
	static void AddNamesFromFile(CNameList* pList, char* pFileName)
	{
		AddMapsFromFile(pList, pFileName);
	}
	void removeName(char* pszName)
	{
		CMapName temp(pszName);
			removeMap(&temp);
	}
	void removeName(CName* pName)
	{
		removeMap(pName);
	}
	bool locateName(CName* pName)
	{
		return locateMap((CMapName*)pName);
	}
	bool locateName(char* pName)
	{
		CMapName temp(pName);
		return locateMap(&temp);
	}
	
	bool locateString(const char* pName)
	{
		
		CMapName* temp = getHead();
		int nChars;
		while(temp)
		{
			nChars = strlen(temp->getName());
			if(!strncmp(pName, temp->getName(), nChars))
				return true;
			temp = temp->m_pNext;
			
		}
		return false;
	}
};

enum
{
	MODE_FFA = 0,
	MODE_TEAMPLAY,
	MODE_ACTION,
	MODE_DUEL,
	MODE_TOURNAMENT,
	MODE_LMS,
	MODE_CTF,
	MODE_CTP,
	MODE_TEAM_ACTION,
	MODE_TEAM_LMS,
	MODE_PRACTICE,
};
#endif
