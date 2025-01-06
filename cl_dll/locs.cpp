#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <fstream>
using namespace std;


/*{
	vec3_t m_loc;
	CLocNode* m_pNext
}
class CLocs : CBaseHud
{
private:
	vec3_t m_real_loc;
	CLocNode* m_pHead;
public:*/
	// client commands  "%l"
	// client commands  "%h"
	// client commands  "%a"
	//const char *level = gEngfuncs.pfnGetLevelName();

CLocNode::CLocNode()
{
	*m_szLocation = NULL;
	m_pNext = NULL;
}

int CLocs::Init()
{
	m_iFlags &= ~HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

void CLocs::InitHUDData( void )
{
	ClearList();
}

void CLocs::Reset( void )
{
}

CLocs::~CLocs()
{
	ClearList();
}
void CLocs::ClearList()
{
	CLocNode *pNode = m_pHead;
	CLocNode *pParnet = NULL;
	while(pNode)
	{	
		pParnet = pNode->m_pNext;
		delete pNode;
		pNode = pParnet;
	}
	m_pHead = NULL;
}
char* CLocs::GetClosestLoc(vec3_t* loc)
{
	CLocNode* pNode = m_pHead;
	vec3_t sum;
	float dist = 999999.0f;
	char* szLoc = NULL;
	while(pNode)
	{
		sum = *loc - pNode->m_loc;
		float length = sum.Length();
		if(length < dist)
		{
			dist = length;
			szLoc = pNode->m_szLocation;
		}
		pNode = pNode->m_pNext;
		
	}
	if(!szLoc)
		return "%l"; // return original string
	return szLoc;
}
void CLocs::LoadClientLocs()
{
	ClearList();
	ifstream LocFile;
	char map[256];
	char path[256];
	char sz[256];
	*path=NULL;
	*sz=NULL;

	const char* level = gEngfuncs.pfnGetLevelName();
	strcpy( sz, level );
	char *ch = strchr( sz, '.' );
	*ch = '\0';
	strcat( sz, ".loc" );

	// pull out the map name
	strcpy( map, level );
	ch = strchr( map, '.' );
	if ( ch )
	{
		*ch = 0;
	}
	ch = strchr( map, '/' );
	if ( ch )
	{
		// move the string back over the '/'
		memmove( map, ch+1, strlen(ch)+1 );
	}
	sprintf(path, "hle/locs/%s.loc",map);
//	LocFile.open(path, /ios::nocreate);
	if(!LocFile.is_open()) 
	{	
		LocFile.clear();
		sprintf(path, "ag/locs/%s.loc",map); // eat me bullit
//		LocFile.open(path, ios::nocreate);
	}
	if(LocFile.is_open()) 
	{
		CLocNode *pNode = NULL;
		char line[1024];
		*line=NULL;
		while(!LocFile.eof())
		{
			LocFile.getline(line,1023);
			char* command = strtok(line, "#");
			while(command)
			{
				if(!pNode)
				{
					m_pHead = new CLocNode();
					pNode = m_pHead;
				}
				else
				{
					pNode->m_pNext = new CLocNode();
					pNode = pNode->m_pNext;
				}
				strcpy(pNode->m_szLocation, command);
				command = strtok(NULL, "#");
				if(!command)
					break;
				pNode->m_loc[0] = (float)atof(command);
				command = strtok(NULL, "#");
				if(!command)
					break;
				pNode->m_loc[1] = (float)atof(command);
				command = strtok(NULL, "#");
				if(!command)
					break;
				pNode->m_loc[2] = (float)atof(command);
				command = strtok(NULL, "#");
			}
			}

		LocFile.close();
	}	
}

