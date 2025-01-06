#ifndef _MAPS_H
#define _MAPS_H


/*

class CMapName
{
private:

	char*	m_pName;
	bool	m_bLocal;

public:
	
	CMapName(CMapName& cCopy);

	CMapName(char* cName);
	CMapName();
	~CMapName();
	char* getName();
	bool	getLocal();
	void setLocal(bool bLocal);
	void setName(char* cName);
	bool operator == (CMapName*);
	void  operator = (CMapName&);
	bool operator < (CMapName&);
	
	static CMapName* createMap(char* pName);
	static void destroyMap(CMapName* pMap);

	CMapName* m_pNext;

};


class CMapList 
{

private:

	CMapName *m_pHead;
	CMapName *m_pTail;
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
	int size() { return m_nSize; }
	void clear();
	void removeMap(int nIndex);
	void removeMap(CMapName* pMap);
	void removeDubplicates();
	//friend void alphabetizeList(CMapList* pList);
	void alphabetizeList();
	void assignSentinal();

	CMapName* operator [] ( int nIndex );
	void operator = (const CMapList* mapAssignment);
	
	static int compareOrder(CMapName* pFirst, CMapName* pSecond);
	static void AddMapFromFile(CMapList* pList, char* pFileName);
	static void CompileMapList(CMapList* pList);

	
	
	
};
*/

#endif
