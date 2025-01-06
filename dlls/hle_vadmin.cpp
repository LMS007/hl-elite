#include    <stdio.h>
#include    <io.h>
#include    <time.h>
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include    "hle_vote.h"
#include	"hle_vadmin.h"
#include    "hle_utility.h"
#include	"gamerules.h"

extern int g_teamplay;
hudtextparms_t CVirtualAdmin::m_hudtInfo;

// ====================================
// CVirtualAdminPermissions // ========
// ====================================

CVirtualAdmin g_vAdmin;

extern COutput debug_file;


CVirtualAdminPermissions::CVirtualAdminPermissions()
{
	m_bBlockAll = true;
	m_bMembersAll = false;
	*m_szPassword = NULL;
	m_bIsActive = false;
	m_nGroupNumber = 0;
}

void CVirtualAdminPermissions::SetName(char* pszName)
{
	if(pszName)
	{
		strcpy(m_szGroupName, pszName);
	}
}

char* CVirtualAdminPermissions::GetName()
{
	return m_szGroupName;
}

void CVirtualAdminPermissions::AddPermission(char* pszPermission)
{
	m_Permissions.addSmart(pszPermission);
}

void CVirtualAdminPermissions::SetPassword(char* pszPass)
{
	strcpy(m_szPassword, pszPass);
}
bool CVirtualAdminPermissions::IsPassword(char* pszPass)
{
	if(!*m_szPassword)
		return false;
	if(FStrEq(pszPass, m_szPassword))
		return true;
	return false;
}

char* CVirtualAdminPermissions::GetPassword()
{
	return m_szPassword;
}


bool CVirtualAdminPermissions::IsPassword()
{
	if(!*m_szPassword)
		return false;
	return true;
}

void CVirtualAdminPermissions::AddExclusion(char* pszExclusion)
{
	m_Exclusions.addSmart(pszExclusion);
}

void CVirtualAdminPermissions::RemovePermission(char* pszPermission)
{
	m_Permissions.removeName(pszPermission);
}

void CVirtualAdminPermissions::RemoveExclusion(char* pszExclusion)
{
	m_Exclusions.removeName(pszExclusion);
}

bool CVirtualAdminPermissions::IsExclusion(char* pszExclusion)
{
	return m_Exclusions.locateName(pszExclusion);
}


bool CVirtualAdminPermissions::IsPermission(char* pszExclusion)
{
	return m_Permissions.locateName(pszExclusion);
}

void CVirtualAdminPermissions::AddPlayer( char* steamID)
{
	if(IsAuthorized(steamID))
		return;
	m_AuthorizedPlayers.addSmart(steamID);
}

void CVirtualAdminPermissions::RemovePlayer(const char* steamID)
{
	m_AuthorizedPlayers.removeName((char*)steamID);
}

bool CVirtualAdminPermissions::IsAuthorized(const char* steamID)
{
	if(m_bMembersAll==true)
		return true;
	else
		return m_AuthorizedPlayers.locateName((char*)steamID);
}

bool CVirtualAdminPermissions::IsPermitted(char* pszCommand)
{
	if(!pszCommand || *pszCommand==NULL)
 		return false;
	if(m_Exclusions.locateString(pszCommand))
		return false;
	if(m_Permissions.locateString(pszCommand))
		return true;
	if(m_bBlockAll)
		return false;
	else 
		return true;
}


void CVirtualAdminPermissions::SetBlockAll(bool bBlock)
{
	m_bBlockAll = bBlock;
}

bool CVirtualAdminPermissions::GetBlockAll(void)
{
	return m_bBlockAll;
}

void CVirtualAdminPermissions::SetMembersAll(bool bMembers)
{
	m_bMembersAll = bMembers;
}

bool CVirtualAdminPermissions::GetMembersAll(void)
{
	return m_bMembersAll;
}

CName* CVirtualAdminPermissions::GetHeadPermission()
{
	return (CName*)m_Permissions.getHead();
}

CName* CVirtualAdminPermissions::GetHeadExclusion()
{
	return (CName*)m_Exclusions.getHead();
}

CName* CVirtualAdminPermissions::GetHeadAdmin()
{
	return (CName*)m_AuthorizedPlayers.getHead();
}



// ====================================
// CVirtualAdmin // ===================
// ====================================

CVirtualAdmin::CVirtualAdmin()
{
	m_hudtInfo.x            = 0.45f;
    m_hudtInfo.y            = 0.1f;
    m_hudtInfo.a1           = 0.0f;
    m_hudtInfo.a2           = 0.0f;
    m_hudtInfo.b1           = 0.0f;
    m_hudtInfo.b2           = 0.0f;
    m_hudtInfo.g1           = 100.0f;
    m_hudtInfo.g2           = 0.0f;
    m_hudtInfo.r1           = 255.0f;
    m_hudtInfo.r2            = 0.0f;
    m_hudtInfo.channel      = 4;
    m_hudtInfo.effect       = EFFECT;
    m_hudtInfo.fxTime       = FX_TIME;
    m_hudtInfo.fadeinTime   = FADE_TIME_IN;
    m_hudtInfo.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtInfo.holdTime     = 3;    	
	m_bLoaded = false;	
	m_pHead = NULL;
}



void CVirtualAdmin::SetPermission(CBasePlayer* pPlayer)
{
	*pPlayer->m_szVAdminGroup = NULL;

	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(pTemp->m_pGroup->IsAuthorized(GetID(pPlayer)))
		{
			sprintf(pPlayer->m_szVAdminGroup, pTemp->m_pGroup->GetName());
		}
		else if(pTemp->m_pGroup->IsPassword(pPlayer->m_szVAdminPassword))
		{
			sprintf(pPlayer->m_szVAdminGroup, pTemp->m_pGroup->GetName());
		}
		pTemp = pTemp->m_pNext;
	}
}
CVirtualAdminPermissions* CVirtualAdmin::GetPlayerGroup(CBasePlayer* pPlayer)
{
	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pPlayer->m_szVAdminGroup, pTemp->m_pGroup->GetName()))
			return pTemp->m_pGroup;
		pTemp = pTemp->m_pNext;
	}
	return NULL;
}

bool CVirtualAdmin::HasPermission(CBasePlayer* pPlayer, char* pCommand)
{
	if(!pCommand)
		return false;
	CVirtualAdminPermissions *pGroup = GetPlayerGroup(pPlayer);
	if(pGroup)
	{
		return pGroup->IsPermitted(pCommand);
	}
	return false;
}


void CVirtualAdmin::AddPlayerToGroup(CBasePlayer* pPlayer, char* pzsGroup)
{
	if(!pzsGroup)
		return;
	CVAdminPermissionsNode *pTemp = m_pHead;	

	while(pTemp)
	{
		if(FStrEq(pzsGroup, pTemp->m_pGroup->GetName()))
		{
			pTemp->m_pGroup->AddPlayer((char*)GetID(pPlayer));
			return;
		}
		pTemp = pTemp->m_pNext;
	}
}

void CVirtualAdmin::RemovePlayerFromGroup(CBasePlayer* pPlayer, char* pzsGroup)
{
	if(!pzsGroup)
		return;
	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pzsGroup, pTemp->m_pGroup->GetName()))
		{
			pTemp->m_pGroup->RemovePlayer(GetID(pPlayer));
			return;
		}
		pTemp = pTemp->m_pNext;
	}
}


void CVirtualAdmin::AddPermissionToGroup(char* pzsGroup, char* pszCommand)
{
	if(!pzsGroup)
		return;
	else if(!pszCommand)
		return;
	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pzsGroup, pTemp->m_pGroup->GetName()))
		{
			pTemp->m_pGroup->AddPermission(pszCommand);
			return;
		}
		pTemp = pTemp->m_pNext;
	}
}


void CVirtualAdmin::RemovePermissionFromGroup(char* pzsGroup, char* pszCommand)
{
	if(!pzsGroup)
		return;
	else if(!pszCommand)
		return;

	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pzsGroup, pTemp->m_pGroup->GetName()))
		{
			pTemp->m_pGroup->RemovePermission(pszCommand);
			return;
		}
		pTemp = pTemp->m_pNext;
	}
}

void CVirtualAdmin::AddRestrictionToGroup(char* pzsGroup, char* pszCommand)
{
	if(!pzsGroup)
		return;
	else if(!pszCommand)
		return;

	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pzsGroup, pTemp->m_pGroup->GetName()))
		{
			pTemp->m_pGroup->AddExclusion(pszCommand);
			return;
		}
		pTemp = pTemp->m_pNext;
	}
}

void CVirtualAdmin::RemoveRestrictionFromGroup(char* pzsGroup, char* pszCommand)
{
	if(!pzsGroup)
		return;
	else if(!pszCommand)
		return;

	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pzsGroup, pTemp->m_pGroup->GetName()))
		{
			pTemp->m_pGroup->RemoveExclusion(pszCommand);
			return;
		}
		pTemp = pTemp->m_pNext;
	}
}



void CVirtualAdmin::WriteToDisk(char* pFile)
{
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
		
		debug_file.OpenFile();
		debug_file << "CVirtualAdmin::WriteToDisk(); BEGIN\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	ofstream vAdminFile;
	vAdminFile.open(pFile);	

	CVirtualAdminPermissions* pActiveGroup = NULL;
	bool bBracketOpen = false;
	
	if(vAdminFile.is_open()) 
	{

		m_pCurrent = m_pHead;
		while(m_pCurrent)
		{
			
			vAdminFile << "GROUP ( NAME \"" <<  m_pCurrent->m_pGroup->GetName() << "\"";
			if(*m_pCurrent->m_pGroup->GetPassword())
			{
				vAdminFile << ", PASSWORD \"" << m_pCurrent->m_pGroup->GetPassword() << "\"";
			}
			vAdminFile << " )" << endl << "{" << endl;

			// MEMBERS
			if(m_pCurrent->m_pGroup->GetMembersAll())
			{
				vAdminFile << "\t" << "MEMBERS ( ALL )";
				if(!m_pCurrent->m_pGroup->GetHeadAdmin())
					vAdminFile << " " << "{ }" << endl  << endl;
				else 
					vAdminFile  << endl;
			}
			else if(m_pCurrent->m_pGroup->GetHeadAdmin())
			{
				vAdminFile << "\t" << "MEMBERS" << endl;
			}	
			if(m_pCurrent->m_pGroup->GetHeadAdmin())
			{
				CName* pTemp = m_pCurrent->m_pGroup->GetHeadAdmin();
				vAdminFile << "\t" << "{" << endl;
				while(pTemp)
				{
					vAdminFile << "\t\t" << "\"" << pTemp->getName() << "\"" << endl;
					pTemp = (CName*)pTemp->m_pNext;
				}
				vAdminFile << "\t" << "}" << endl << endl;
			}
			// PERMISSIONS
			if(!m_pCurrent->m_pGroup->GetBlockAll())
			{
				vAdminFile << "\t" << "PERMISSIONS ( ALL )";
				if(!m_pCurrent->m_pGroup->GetHeadPermission())
					vAdminFile << " " << "{ }" << endl << endl;
				else 
					vAdminFile  << endl;
			}
			else if(m_pCurrent->m_pGroup->GetHeadPermission())
			{
				vAdminFile << "\t" << "PERMISSIONS" << endl;
			}	
			if(m_pCurrent->m_pGroup->GetHeadPermission())
			{
				CName* pTemp = m_pCurrent->m_pGroup->GetHeadPermission();
				vAdminFile << "\t" << "{" << endl;
				while(pTemp)
				{
					vAdminFile << "\t\t" << "\"" << pTemp->getName() << "\"" << endl;
					pTemp = (CName*)pTemp->m_pNext;
				}
				vAdminFile << "\t" << "}" << endl << endl;
			}
			// EXCLUSIONS
			if(m_pCurrent->m_pGroup->GetBlockAll())
			{
				vAdminFile << "\t" << "RESTRICTIONS ( ALL )";
				if(!m_pCurrent->m_pGroup->GetHeadExclusion())
					vAdminFile << " " << "{ }" << endl  << endl;
				else 
					vAdminFile  << endl;
			}
			else if(m_pCurrent->m_pGroup->GetHeadExclusion())
			{
				vAdminFile << "\t" << "RESTRICTIONS" << endl;
			}	
			if(m_pCurrent->m_pGroup->GetHeadExclusion())
			{
				CName* pTemp = m_pCurrent->m_pGroup->GetHeadExclusion();
				vAdminFile << "\t" << "{" << endl;
				while(pTemp)
				{
					vAdminFile << "\t\t" << "\"" << pTemp->getName() << "\"" << endl;
					pTemp = (CName*)pTemp->m_pNext;
				}
				vAdminFile << "\t" << "}" << endl << endl;
			}
			m_pCurrent = m_pCurrent->m_pNext;
			
			vAdminFile << "}" << endl << endl;

			
		}
		vAdminFile.close();
	}

#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
		
		debug_file.OpenFile();
		debug_file << "CVirtualAdmin::WriteToDisk(); END\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================
}

CVirtualAdmin::~CVirtualAdmin()
{
	WriteToDisk("hle/vadmin.txt");
	if(m_pszCommand)
		delete[] m_pszCommand;
	m_pszCommand = NULL;
	if(m_pszLine)
		delete[] m_pszLine;
	m_pszLine = NULL;
	
	CVAdminPermissionsNode* pTemp = this->m_pHead;
	while(pTemp)
	{
		m_pHead = pTemp->m_pNext;
		delete pTemp;
		pTemp = m_pHead;
	}
	m_pHead = NULL;
}

bool CVirtualAdmin::RemoveQuotes(char** String)
{
	if(*String)
	{
		int len = strlen(*String);
		if(len>2)
		{
			if((*String)[0]=='\"' && (*String)[len-1]=='\"')
			{
				(*String)[len-1]=NULL; // cut off tail quote
				*String = *String+1; // cut off head quote
				return true;
			}
		}
	}
	return false;
}


int CVirtualAdmin::RemoveLeadingSpacesTabs(char* String)
{
	int j = 0;
	if(String)
	{
		int len = strlen(String);
		int i = 0;
		
		while(i < len)
		{	
			if((String)[i]==' ' || (String)[i]=='\t')
				j++;
			else 
				break;
			i++;
		}
	}
	return j;

}

char* CVirtualAdmin::GetNextCommand()
{
	if(m_pszCommand)
		delete[] m_pszCommand;
	m_pszCommand = new char[256];
	int len1 = strlen(m_pszCommand);
	for(int j=0;j<256;j++)
		m_pszCommand[j]=NULL;
	int len2 = strlen(m_pszCommand);
	
	int leading_spaces = 0; 
	bool breakout = false;
	bool seperation_char = false;
	bool text = false;
	bool quotes = false;
	int i = m_nIndex;
	for(i = m_nIndex; i < 255; i++ )
	{
		if(m_pszLine==NULL)
			break;
		
		else if(m_pszLine[i]==NULL)
			break;
		
		switch((int)m_pszLine[i])
		{
		case '"':
			quotes = !quotes;
			break;
		case '{':
			if(text) breakout = true;
			else if(seperation_char) breakout = true;
			seperation_char = true;
			break;
		case '}':
			if(text) breakout = true;
			else if(seperation_char) breakout = true;
			seperation_char = true;
			break;
		case ')':
			if(text) breakout = true;
			else if(seperation_char) breakout = true;
			seperation_char = true;
			break;
		case '(':
			if(text) breakout = true;
			else if(seperation_char) breakout = true;
			seperation_char = true;
			break;
		case ',':
			if(text) breakout = true;
			else if(seperation_char) breakout = true;
			seperation_char = true;
			break;
		case ' ':
			if(!quotes)
			{
				if(text || seperation_char)
				breakout = true;
			}
			break;
		case '\t':
			if(!quotes)
			{
				if(text || seperation_char)
				breakout = true;
			}
			break;
		default:
			if(seperation_char) breakout = true;
			text=true;
			break;
		}
		if(breakout)
			break;
		
	}
	//if(m_pszLine)
	//{	
		strncpy(m_pszCommand, m_pszLine+m_nIndex, i-m_nIndex);
	//}
	if(m_pszCommand && *m_pszCommand)
	{
		m_nIndex = i;
	
		int spaces = RemoveLeadingSpacesTabs(m_pszCommand);

		if(!FStrEq(m_pszCommand+spaces, ""))
		{
			return m_pszCommand+spaces;
		}
	}
	if(GetNextLine()==false)
		return NULL;
	else
		return GetNextCommand();
}

bool CVirtualAdmin::GetNextLine()
{
	if(m_pszLine)
		delete[] m_pszLine;
	m_pszLine = new char[256];
	//m_pszLine = NULL;


	if(!vAdminFile.is_open()) 
	{
		return false;
	}

	do
	{
		if(vAdminFile.eof())
			return false; //no more lines
		vAdminFile.getline(m_pszLine,256 );
	}

	while(!m_pszLine);
	m_nIndex = 0;
	return true;
}

void CVirtualAdmin::AddMember(char* player, CVAdminPermissionsNode* pNode)
{
	pNode->m_pGroup->AddPlayer(player);
}

void CVirtualAdmin::AddPremission(char* premission, CVAdminPermissionsNode* pNode)
{
	pNode->m_pGroup->AddPermission(premission);
}
void CVirtualAdmin::AddRestriction(char* restriction, CVAdminPermissionsNode* pNode)
{
	pNode->m_pGroup->AddExclusion(restriction);
}


int CVirtualAdmin::LoadList(void(*ptFunction)(char*, CVAdminPermissionsNode*))
{	
	char* command;
	while(1)
	{	
		command = GetNextCommand();
		if(command && RemoveQuotes(&command))
		{
			ptFunction(command, m_pCurrent);
			continue;
		}
		else if(FStrEq(command, "}"))
		{
			return 1;
		}	
		return 0;
	}

	return 1;
}


int CVirtualAdmin::LoadPermissionGroups()
{
	
	vAdminFile.open("hle/vadmin.txt");
	int nLine = 0;

	m_pszLine = NULL;
	CVirtualAdminPermissions* pActiveGroup = NULL;
	
	CNameList m_lExpectationList;

	int par_open = 0;
	int brace_open = 0;

	m_pCurrent = NULL;

	// clean up old groups
	CVAdminPermissionsNode* pTemp = this->m_pHead;
	while(pTemp)
	{
		m_pHead = pTemp->m_pNext;
		delete pTemp;
		pTemp = m_pHead;
	}
	m_pHead = NULL;
		
	//

	if(vAdminFile.is_open()) 
	{
		m_lExpectationList.addFront("GROUP");

		char* command=NULL;
		do
		{
			command = GetNextCommand();
			if(!command)
				break;
			//======================
			// GROUP
			//======================
			if(FStrEq(command, "GROUP"))
			{
				if(!m_lExpectationList.locateString("GROUP"))
				{
					ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					break;
				}
				
				if(m_pCurrent)
				{
					m_pCurrent->m_pNext = new CVAdminPermissionsNode();
					m_pCurrent = m_pCurrent->m_pNext;
				}
				if(!m_pHead)
				{
					m_pHead = new CVAdminPermissionsNode();
					m_pCurrent = m_pHead;
				}
				
				m_lExpectationList.clear();
				m_lExpectationList.addFront("(");
			}
			//======================
			// NAME
			//======================
			else if(FStrEq(command, "NAME"))
			{
				if(!m_lExpectationList.locateString("NAME"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}

								//if(RemoveQuotes(&m_pszCommand))
				command = GetNextCommand();
				if(command && RemoveQuotes(&command))
				{
					// UNDONE!
					// MAKE SURE NO TWO NAMES ARE THE SAME

					m_pCurrent->m_pGroup->SetName(command);
					m_lExpectationList.clear();
					m_lExpectationList.addFront(",");
					m_lExpectationList.addFront(")");
				}
				else
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
			}
			//======================
			// PASSWORD
			//======================
			else if(FStrEq(command, "PASSWORD"))
			{
				if(!m_lExpectationList.locateString("PASSWORD"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				command = GetNextCommand();
				if(command && RemoveQuotes(&command))
				{
					m_pCurrent->m_pGroup->SetPassword(command);
					m_lExpectationList.clear();
					m_lExpectationList.addFront(",");
					m_lExpectationList.addFront(")");
				}
				else
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
			}
			//======================
			// ,
			//======================
			else if(FStrEq(command, ","))
			{	
				if(!m_lExpectationList.locateString(","))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				m_lExpectationList.clear();
				m_lExpectationList.addFront("NAME");
				m_lExpectationList.addFront("PASSWORD");
			}
			//======================
			// (
			//======================
			else if(FStrEq(command, "("))
			{
				if(!m_lExpectationList.locateString("("))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				par_open++;

				m_lExpectationList.clear();
				m_lExpectationList.addFront("ALL");
				m_lExpectationList.addFront("NAME");
				m_lExpectationList.addFront("PASSWORD");
			}
			//======================
			// )
			//======================
			else if(FStrEq(command, ")"))
			{
				if(!m_lExpectationList.locateString(")"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				par_open--;
				
				m_lExpectationList.clear();
				m_lExpectationList.addFront("{");
			}		
			//======================
			// {
			//======================
			else if(FStrEq(command, "{"))
			{
				if(!m_lExpectationList.locateString("{"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				brace_open++;
				m_lExpectationList.clear();
				m_lExpectationList.addFront("MEMBERS");
				m_lExpectationList.addFront("PERMISSIONS");
				m_lExpectationList.addFront("RESTRICTIONS");
			}
			//======================
			// }
			//======================
			else if(FStrEq(command, "}"))
			{
				if(!m_lExpectationList.locateString("}"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				brace_open--;
				m_lExpectationList.clear();
				m_lExpectationList.addFront("GROUP");
				m_lExpectationList.addFront("PERMISSIONS");
				m_lExpectationList.addFront("MEMBERS");
				m_lExpectationList.addFront("RESTRICTIONS");
				m_lExpectationList.addFront("}");
			}
			//======================
			// MEMBERS
			//======================
			else if(FStrEq(command, "MEMBERS"))
			{
				if(!m_lExpectationList.locateString("MEMBERS"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}
				
				//if(!FStrEq(GetNextCommand(), "{"))					
				//	break;
				command = GetNextCommand();
				if(!FStrEq(command, "{"))					
				{
					if(FStrEq(command, "("))
					{
						if(FStrEq(GetNextCommand(), "ALL"))	
						{
							m_pCurrent->m_pGroup->SetMembersAll(true);
							if(!FStrEq(GetNextCommand(), ")"))	
								continue;
							if(!FStrEq(GetNextCommand(), "{"))	
								continue;
						}
						else if(!FStrEq(GetNextCommand(), ")"))	
							continue;
					}
					
				}
				if(!LoadList(&AddMember))
					continue;

				m_lExpectationList.clear();
				m_lExpectationList.addFront("GROUP");
				m_lExpectationList.addFront("PERMISSIONS");
				m_lExpectationList.addFront("MEMBERS");
				m_lExpectationList.addFront("RESTRICTIONS");
				m_lExpectationList.addFront("}");
				
			}
			//======================
			// PERMISSIONS
			//======================
			else if(FStrEq(command, "PERMISSIONS"))
			{
				if(!m_lExpectationList.locateString("PERMISSIONS"))
				{
					ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}

				command = GetNextCommand();
				if(!FStrEq(command, "{"))					
				{
					if(FStrEq(command, "("))
					{
						if(FStrEq(GetNextCommand(), "ALL"))	
						{
							m_pCurrent->m_pGroup->SetBlockAll(false);
							if(!FStrEq(GetNextCommand(), ")"))	
								continue;
							if(!FStrEq(GetNextCommand(), "{"))	
								continue;
						}
						else if(!FStrEq(GetNextCommand(), ")"))	
							continue;
					}
					
				}
				if(!LoadList(&AddPremission))
					continue;
			
				m_lExpectationList.clear();
				m_lExpectationList.addFront("GROUP");
				m_lExpectationList.addFront("PERMISSIONS");
				m_lExpectationList.addFront("MEMBERS");
				m_lExpectationList.addFront("RESTRICTIONS");
				m_lExpectationList.addFront("}");
			}
			//======================
			// RESTRICTIONS
			//======================
			else if(FStrEq(command, "RESTRICTIONS"))
			{
				if(!m_lExpectationList.locateString("RESTRICTIONS"))
				{
					//ALERT( at_console, "ERROR vadmin.txt line:%n, Invalid expectation\n",nLine);
					continue;
				}

				command = GetNextCommand();
				if(!FStrEq(command, "{"))					
				{
					if(FStrEq(command, "("))
					{
						if(FStrEq(GetNextCommand(), "ALL"))	
						{
							m_pCurrent->m_pGroup->SetBlockAll(true);
							if(!FStrEq(GetNextCommand(), ")"))	
								continue;
							if(!FStrEq(GetNextCommand(), "{"))	
								continue;
						}
						else if(!FStrEq(GetNextCommand(), ")"))	
							continue;
					}

				}
				if(!LoadList(&AddRestriction))
					continue;
				
				m_lExpectationList.clear();
				m_lExpectationList.addFront("GROUP");
				m_lExpectationList.addFront("PERMISSIONS");
				m_lExpectationList.addFront("MEMBERS");
				m_lExpectationList.addFront("RESTRICTIONS");
				m_lExpectationList.addFront("}");
			}
		}
		while(command);

		if(par_open<0)
		{
			ALERT( at_console, "ERROR vadmin.txt, Missing \"(\"\n");
			return 0;
		}
		else if(par_open>0)
		{
			ALERT( at_console, "ERROR vadmin.txt, Missing \")\"\n");
			return 0;
		}
		if(brace_open<0)
		{
			ALERT( at_console, "ERROR vadmin.txt, Missing \"{\"\n");
			return 0;
		}
		else if(brace_open>0)
		{
			ALERT( at_console, "ERROR vadmin.txt, Missing \"}\"\n");
			return 0;
		}
				
		m_bLoaded = true;
		vAdminFile.close();
	}
	else
	{
		ALERT( at_console, "ERROR vadmin.txt, File was not found\n");
		return 0;
	}
	if(m_pszCommand)
		delete[] m_pszCommand;
	m_pszCommand = NULL;
	if(m_pszLine)
		delete[] m_pszLine;
	m_pszLine = NULL;

	m_lExpectationList.clear();

#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
		
		debug_file.OpenFile();
		debug_file << "CVirtualAdmin::LoadPermissionGroups(); END\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

	return 1;
}


CVirtualAdminPermissions* CVirtualAdmin::GetGroup(char* pszGroup)
{
	if(!pszGroup)
		return NULL;

	CVAdminPermissionsNode *pTemp = m_pHead;
	while(pTemp)
	{
		if(FStrEq(pszGroup, pTemp->m_pGroup->GetName()))
		{
			return pTemp->m_pGroup;
		}
		pTemp = pTemp->m_pNext;
	}

	return NULL;
}


void CVirtualAdmin::PrintGroupCommands(CBasePlayer *pAdmin, CVirtualAdminPermissions* pGroup)
{
	if(pGroup)
	{
		char buf[2048];
		*buf = NULL;
		sprintf(buf, "\n========= Virtual Admin Group \"%s\" =========\n\n", pGroup->GetName());
		
		ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE,  buf);
		CName* pCommand = pGroup->GetHeadPermission();
		sprintf(buf, "---Permissions---\n");
		ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, buf);
		if(!pGroup->GetBlockAll())
		{
			sprintf(buf, "All unlisted commands are permitted\n");
			ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
		}
		while(pCommand)
		{
			sprintf(buf, "%s\n", pCommand->getName());
			ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
			pCommand = (CName*)pCommand->m_pNext;
		}
		pCommand = pGroup->GetHeadExclusion();
		sprintf(buf, "\n---Restrictions---\n");
		ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
		if(pGroup->GetBlockAll())
		{
			sprintf(buf, "All unlisted commands are restricted\n");
			ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
		}
		while(pCommand)
		{
			sprintf(buf, "%s\n", pCommand->getName());
			ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
			pCommand = (CName*)pCommand->m_pNext;
		}
		sprintf(buf, "==================\n");
		ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, buf );
	}
}


void CVirtualAdmin::PrintGroupAdministration(CBasePlayer *pAdmin, CVirtualAdminPermissions* pGroup)
{
	
	if(pGroup)
	{
		char buf[2048];
		*buf = NULL;
		sprintf(buf, "\n========= Administration of Group \"%s\" =========\n\n", pGroup->GetName());
		
		
		ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE,  buf);
		
		ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, "---Members of this group---\n");

		CName* pTemp = pGroup->GetHeadAdmin();

		while(pTemp)
		{
			sprintf(buf, "%s", pTemp->getName());
			bool InServer = false;
				
			for (int k = 1; k <= gpGlobals->maxClients; k++ )
			{
				CBaseEntity *pEnt = UTIL_PlayerByIndex( k );
				if ( pEnt && pEnt->IsPlayer( ) )
				{
					CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
				
					if(FStrEq(GetID(pPlayer), pTemp->getName()))
					{
						strcat(buf, " ");
						strcat(buf, STRING(pPlayer->pev->netname));
						strcat(buf, "\n");
						InServer = true;
						break;
					}
				}
			}
			if(!InServer)
			{	
				strcat(buf, " Not Local\n");
			}
			ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
			pTemp = (CName*)pTemp->m_pNext;
		}	
		
		*buf = NULL;
		if(pGroup->GetPassword()!=NULL)
			sprintf(buf, "\nPassword: %s\n",pGroup->GetPassword());
		ClientPrint( pAdmin->pev,HUD_PRINTCONSOLE, buf);
		sprintf(buf, "==================\n");
		ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, buf );
	}
}



int CVirtualAdmin::AdminCommand( CBasePlayer* pAdmin)
{
	const char *pcmd = CMD_ARGV(1);

	if ( FStrEq(pcmd, "add" ) ) 
	{
		if(FStrEq(CMD_ARGV(2), "member")) // add admin group playerID
		{
			const char* pszGroup = CMD_ARGV(3);
			char szPlayer[128];
			sprintf(szPlayer, CMD_ARGV(4));
			for(int u = 5; *CMD_ARGV(u)!=NULL; u++)
			{
				strcat(szPlayer, " ");
				strcat(szPlayer, CMD_ARGV(u));
			}
			//const char* pszPlayer = CMD_ARGV(4);
			int nGroup = 0;
			if((*pszGroup) &&(*szPlayer))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					bool IsPlayer=false;
					for (int i = 1; i <= gpGlobals->maxClients; i++ )
					{
						CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
						if ( pEnt && pEnt->IsPlayer( ) )
						{
							CBasePlayer *pPlayer = (CBasePlayer *)pEnt;					
							const char* pszID =  GetID(pPlayer);
							if ( FStrEq( STRING(pPlayer->pev->netname), szPlayer ) )
							{
							
								g_vAdmin.AddPlayerToGroup(pPlayer, pGroup->GetName());
								g_vAdmin.SetPermission(pPlayer);
								
								char buf[128];
								sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
								UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
								
								ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Player was added\n" );
								IsPlayer =true;
								break;
							}
							else if(FStrEq(pszID, ConvertID(szPlayer))) // maybe its a wonid
							{
								
								g_vAdmin.AddPlayerToGroup(pPlayer, pGroup->GetName());
								g_vAdmin.SetPermission(pPlayer);
								
								char buf[128];
								sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
								UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
								
								ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Player was added\n" );
								IsPlayer =true;
								break;
							}
						}
					}
					if(IsPlayer)
						return 1;
					else
					{
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Player not found\n" );
						return 1;
					}

				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add admin <group> <player>\n" );
				return 1;
			}			
		}
		else if(FStrEq(CMD_ARGV(2), "permission")) // add vcommand group# permission
		{
			const char* pszGroup = CMD_ARGV(3);
			char szPermission[128];
			sprintf(szPermission, CMD_ARGV(4));
			for(int u =5; *CMD_ARGV(u)!=NULL; u++)
			{
				strcat(szPermission, " ");
				strcat(szPermission, CMD_ARGV(u));
			}

			if((*pszGroup) &&(*szPermission))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					pGroup->AddPermission(szPermission);
					//g_vAdmin.AddPermissionToGroup(pGroup->GetName(), (char*)pszPermission);
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Premission was added\n" );
					return 1;	
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add permission <group> <premission>\n" );
				return 1;
			}
			
		}
		else if(FStrEq(CMD_ARGV(2), "restriction")) // add vcommand group# restriction
		{
			const char* pszGroup = CMD_ARGV(3);
			char szRestriction[128];
			sprintf(szRestriction, CMD_ARGV(4));
			for(int u =5; *CMD_ARGV(u)!=NULL; u++)
			{
				strcat(szRestriction, " ");
				strcat(szRestriction, CMD_ARGV(u));
			}
			int nGroup = 0;
			if((*pszGroup) &&(*szRestriction))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					pGroup->AddExclusion(szRestriction);
					//g_vAdmin.AddRestrictionToGroup(pGroup->GetName(), (char*)pszRestriction);
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Restriction was added\n" );
					return 1;	
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add restriction <group> <restriction>\n" );
				return 1;
			}
		}
		else if(FStrEq(CMD_ARGV(2), "password")) // add password group# password
		{
			const char* pszGroup = CMD_ARGV(3);
			const char* pszPassword = CMD_ARGV(4);
			int nGroup = 0;
			if((*pszGroup))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					if(pszPassword && *pszPassword != NULL)
					{
						pGroup->SetPassword((char*)pszPassword);
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Password was set\n" );
						return 1;	
					}
					else
					{
						char buf[128];
						*buf = NULL;
						sprintf(buf, "password: %s\n", pGroup->GetPassword());
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, buf );
						return 1;
					}
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add password <group> <password>\n" );
				return 1;
			}	
		}
		else if(FStrEq(CMD_ARGV(2), "blockall")) // add blockall group#  1/0
		{
			const char* pszGroup = CMD_ARGV(3);
			int nGroup = 0;
			if((*pszGroup))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					pGroup->SetBlockAll(1);
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Block restriction was added\n" );
					return 1;	
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add blockall <group>\n" );
				return 1;
			}
		}
		else
		{
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "\nmember <group> <player name or steam id#>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "permission <group> <permission>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "restriction <group> <restriction>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "password <group> <password>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "blockall <group>\n\n" );
			return 1;
		}

	}

	else if ( FStrEq(pcmd, "remove" ) )
	{
		if(FStrEq(CMD_ARGV(2), "member")) // add admin group# playerID
		{
			const char* pszGroup = CMD_ARGV(3);
			char szPlayer[128];
			sprintf(szPlayer, CMD_ARGV(4));
			for(int u = 5; *CMD_ARGV(u)!=NULL; u++)
			{
				strcat(szPlayer, " ");
				strcat(szPlayer, CMD_ARGV(u));
			}
			int nGroup = 0;
			if((*pszGroup) &&(*szPlayer))
			{
				//unsigned long lPlayerID = (atol((CMD_ARGV(4))));
				const char* pszPlayerID = CMD_ARGV(4);

				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{	
					bool IsPlayer = false;

					// CASE LONG ID
					if(pGroup->IsAuthorized(pszPlayerID))
					{
						pGroup->RemovePlayer(pszPlayerID);
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Player was removed\n" );

						//for (int j = 0; j <= VADMIN_MAXPLAYERS_GROUP; j++ ) // search to see if this ID is a player in the game, 
								//we need to change his permisions right a way
						for (int j = 1; j <= gpGlobals->maxClients; j++ )
						{				
							CBaseEntity *pEnt = UTIL_PlayerByIndex( j );
							if ( pEnt && pEnt->IsPlayer( ) )
							{
								CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
								if(FStrEq(pszPlayerID, GetID(pPlayer)))
								{		
									g_vAdmin.SetPermission(pPlayer);
									
									char buf[128];
									if(pPlayer->m_szVAdminGroup !=NULL)
									{
										sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
										UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
									}
									else
									{
										UTIL_HudMessage(pPlayer, m_hudtInfo, "You are no longer a vadmin\n" );
									}
									break;									
								}

							}
						}
						return 1;
					}
					// CASE PLAYER NAME
					for (int i = 1; i <= gpGlobals->maxClients; i++ )
					{
						
						CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
						if ( pEnt && pEnt->IsPlayer( ) )
						{
							CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
							if(FStrEq(szPlayer, STRING(pPlayer->pev->netname)))
							{
								g_vAdmin.RemovePlayerFromGroup(pPlayer, pPlayer->m_szVAdminGroup);
								g_vAdmin.SetPermission(pPlayer);
								ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Player was removed\n" );

								char buf[128];
								if(pPlayer->m_szVAdminGroup !=NULL)
								{
									sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
									UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
								}
								else
								{
									UTIL_HudMessage(pPlayer, m_hudtInfo, "You are no longer a vadmin\n" );
								}
								return 1;
							}

						}
					}
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Player not found\n" );
					return 1;
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "remove admin <group> <player id>\n" );
				return 1;
			}			
		}
		else if(FStrEq(CMD_ARGV(2), "permission")) // add vcommand group# permission
		{
			const char* pszGroup = CMD_ARGV(3);
			char szPermission[128];
			sprintf(szPermission, CMD_ARGV(4));
			for(int u =5; *CMD_ARGV(u)!=NULL; u++)
			{
				strcat(szPermission, " ");
				strcat(szPermission, CMD_ARGV(u));
			}
			int nGroup = 0;
			if((*pszGroup) &&(*szPermission))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					if(pGroup->IsPermission(szPermission))
					{
						pGroup->RemovePermission(szPermission);
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Permission was removed\n" );
					}
					else
					{
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Permission not found\n" );
					}
					return 1;	
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "remove permission <group> <premission>\n" );
				return 1;
			}
			
		}
		else if(FStrEq(CMD_ARGV(2), "restriction")) // add vcommand group# restriction
		{
			const char* pszGroup = CMD_ARGV(3);
			char szRestriction[128];
			sprintf(szRestriction, CMD_ARGV(4));
			for(int u =5; *CMD_ARGV(u)!=NULL; u++)
			{
				strcat(szRestriction, " ");
				strcat(szRestriction, CMD_ARGV(u));
			}
			int nGroup = 0;
			if((*pszGroup) &&(*szRestriction))
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					if(pGroup->IsExclusion(szRestriction))
					{
						//g_vAdmin.RemoveRestrictionFromGroup(nGroup, (char*)pszRestriction);
						pGroup->RemoveExclusion(szRestriction);
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Restriction was removed\n" );
					}
					else
					{
						ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Restriction not found\n" );
					}
					return 1;	
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "remove restriction <group> <restriction>\n" );
				return 1;
			}
		}
		else if(FStrEq(CMD_ARGV(2), "password")) // add password group# password
		{
			const char* pszGroup = CMD_ARGV(3);
			const char* pszPassword = CMD_ARGV(4);
			int nGroup = 0;
			if((*pszGroup))
			{
				//int nGroup = atoi(pszGroup);
				//if(nGroup >0 && nGroup < VADMIN_MAXGROUPS)
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					pGroup->SetPassword("\0");
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Password was erased\n" );
					
					// reset EVERYONES permissions
					for (int i = 1; i <= gpGlobals->maxClients; i++ )
					{
						CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
						if ( pEnt && pEnt->IsPlayer( ) )
						{
							CBasePlayer* pPlayer = (CBasePlayer*)pEnt;
							g_vAdmin.SetPermission(pPlayer);
						}
					}
					return 1;		
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add password <group> <password>\n" );
				return 1;
			}	
		}
		else if(FStrEq(CMD_ARGV(2), "blockall")) // add blockall group#  1/0
		{
			const char* pszGroup = CMD_ARGV(3);
			int nGroup = 0;
			if((*pszGroup) )
			{
				CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					//CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup(nGroup);
					pGroup->SetBlockAll(0);
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Block restriction was removed\n" );
					return 1;	
				}
				else
				{
					ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "Invalid group\n" );
					return 1;
				}
				 
			}
			else
			{
				ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "add blockall <group>\n" );
				return 1;
			}
		}
		else
		{
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "\nmember <group> <player name or steam id#>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "permission <group> <permission>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "restriction <group> <restriction>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "password <group> <password>\n" );
			ClientPrint( pAdmin->pev, HUD_PRINTCONSOLE, "blockall <group>\n\n" );
			return 1;
		}
	}
	return 0;
}

void CVirtualAdmin::Add()
{

	if(FStrEq(CMD_ARGV(1), "member")) // add admin group# playerID
	{
		const char* pszGroup = CMD_ARGV(2);
		char szPlayer[128];
		sprintf(szPlayer, CMD_ARGV(3));
		for(int u = 4; *CMD_ARGV(u)!=NULL; u++)
		{
			strcat(szPlayer, " ");
			strcat(szPlayer, CMD_ARGV(u));
		}
				
		if((*pszGroup) &&(*szPlayer))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				for (int i = 1; i <= gpGlobals->maxClients; i++ )
				{
					CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
					if ( pEnt && pEnt->IsPlayer( ) )
					{
						CBasePlayer *pPlayer = (CBasePlayer *)pEnt;						
						//unsigned long wonID =  GetID(pPlayer);
						const char* pszPlayerID = GetID(pPlayer);

						if ( FStrEq( STRING(pPlayer->pev->netname), szPlayer ) )
						{
							g_vAdmin.AddPlayerToGroup(pPlayer, pGroup->GetName());
							g_vAdmin.SetPermission(pPlayer);
							
							char buf[128];
							sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
							UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
						}
						else if(FStrEq(pszPlayerID, szPlayer)) // maybe its a wonid
						{
							
							g_vAdmin.AddPlayerToGroup(pPlayer, pGroup->GetName());
							g_vAdmin.SetPermission(pPlayer);
							
							char buf[128];
							sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
							UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
						}
					}
				}
			}
		}
	}
	else if(FStrEq(CMD_ARGV(1), "permission")) // add vcommand group# permission
	{
		const char* pszGroup = CMD_ARGV(2);
		char szPermission[128];
		sprintf(szPermission, CMD_ARGV(3));
		for(int u = 4; *CMD_ARGV(u)!=NULL; u++)
		{
			strcat(szPermission, " ");
			strcat(szPermission, CMD_ARGV(u));
		}
		int nGroup = 0;
		if((*pszGroup) &&(*szPermission))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
				if(pGroup != NULL)
				{
					pGroup->AddPermission(szPermission);
					//g_vAdmin.AddPermissionToGroup(nGroup, (char*)pszPermission);
			
				}
			}
		
	}
	else if(FStrEq(CMD_ARGV(1), "restriction")) // add vcommand group# restriction
	{
		const char* pszGroup = CMD_ARGV(2);
		char szRestriction[128];
		sprintf(szRestriction, CMD_ARGV(3));
		for(int u = 4; *CMD_ARGV(u)!=NULL; u++)
		{
			strcat(szRestriction, " ");
			strcat(szRestriction, CMD_ARGV(u));
		}
		int nGroup = 0;
		if((*pszGroup) &&(*szRestriction))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				pGroup->AddExclusion(szRestriction);
				//g_vAdmin.AddRestrictionToGroup(nGroup, (char*)pszRestriction);
				
			}
		}
	}
	else if(FStrEq(CMD_ARGV(1), "password")) // add password group# password
	{
		const char* pszGroup = CMD_ARGV(2);
		const char* pszPassword = CMD_ARGV(3);
		int nGroup = 0;
		if((*pszGroup))
		{
			//int nGroup = atoi(pszGroup);
			//if(nGroup >0 && nGroup < VADMIN_MAXGROUPS)
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				if(pszPassword && *pszPassword != NULL)
				{
					//CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup(nGroup);
					pGroup->SetPassword((char*)pszPassword);
					
				}
			}
			 
		}
		
	}
	else if(FStrEq(CMD_ARGV(1), "blockall")) // add blockall group#  1/0
	{
		const char* pszGroup = CMD_ARGV(2);
		int nGroup = 0;
		if((*pszGroup) )
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				//CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup(nGroup);
				pGroup->SetBlockAll(1);
			}
		}

	}
}

void CVirtualAdmin::Remove()
{
	if(FStrEq(CMD_ARGV(1), "member")) // add admin group# playerID
	{
		const char* pszGroup = CMD_ARGV(2);
		char szPlayer[128];
		sprintf(szPlayer, CMD_ARGV(3));
		for(int u = 4; *CMD_ARGV(u)!=NULL; u++)
		{
			strcat(szPlayer, " ");
			strcat(szPlayer, CMD_ARGV(u));
		}

		if((*pszGroup) &&(*szPlayer))
		{
			const char* pszPlayerID = CMD_ARGV(3);
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{	
				bool IsPlayer = false;
					// CASE LONG ID
				if(pGroup->IsAuthorized(pszPlayerID))
				{
					pGroup->RemovePlayer(pszPlayerID);

					for ( int j = 1; j <= gpGlobals->maxClients; j++ )
					{													//we need to change his permisions right a way
						CBaseEntity *pEnt = UTIL_PlayerByIndex( j );
						if ( pEnt && pEnt->IsPlayer( ) )
						{
							CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
							if(FStrEq(pszPlayerID, GetID(pPlayer)))
							{
								g_vAdmin.SetPermission(pPlayer);

								char buf[128];
								if(pPlayer->m_szVAdminGroup !=NULL)
								{
									sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
									UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
								}
								else
								{
									UTIL_HudMessage(pPlayer, m_hudtInfo, "You are no longer a vadmin\n" );
								}
							}
						}
					}
				}
				// CASE PLAYER NAME
				else
				{
					for ( int j = 1; j <= gpGlobals->maxClients; j++ )
					{
						
						CBaseEntity *pEnt = UTIL_PlayerByIndex( j );
						if ( pEnt && pEnt->IsPlayer( ) )
						{
							CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
							if(FStrEq(szPlayer, STRING(pPlayer->pev->netname)))
							{
								pGroup->RemovePlayer(GetID(pPlayer));
								//g_vAdmin.RemovePlayerFromGroup(pPlayer, nGroup);
								g_vAdmin.SetPermission(pPlayer);

								char buf[128];
								if(pPlayer->m_szVAdminGroup !=NULL)
								{
									sprintf(buf, "You are now vadmin of group %s\n", pPlayer->m_szVAdminGroup);
									UTIL_HudMessage(pPlayer, m_hudtInfo, buf );
								}
								else
								{
									UTIL_HudMessage(pPlayer, m_hudtInfo, "You are no longer a vadmin\n" );
								}
							}
						}
					}
				}
			}
		}
	}
	else if(FStrEq(CMD_ARGV(1), "permission")) // add vcommand group# permission
	{
		const char* pszGroup = CMD_ARGV(2);
		char szPermission[128];
		sprintf(szPermission, CMD_ARGV(3));
		for(int u = 4; *CMD_ARGV(u)!=NULL; u++)
		{
			strcat(szPermission, " ");
			strcat(szPermission, CMD_ARGV(u));
		}
		int nGroup = 0;
		if((*pszGroup) &&(*szPermission))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				if(pGroup->IsPermission(szPermission))
				{
					pGroup->RemovePermission(szPermission);
					//g_vAdmin.RemovePermissionFromGroup(nGroup, (char*)pszPermission);
				}
			}
		}
		
	}
	else if(FStrEq(CMD_ARGV(1), "restriction")) // add vcommand group# restriction
	{
		const char* pszGroup = CMD_ARGV(2);
		char szRestriction[128];
		sprintf(szRestriction, CMD_ARGV(3));
		for(int u = 4; *CMD_ARGV(u)!=NULL; u++)
		{
			strcat(szRestriction, " ");
			strcat(szRestriction, CMD_ARGV(u));
		}
		int nGroup = 0;
		if((*pszGroup) &&(*szRestriction))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				if(pGroup->IsExclusion(szRestriction))
				{
					pGroup->RemoveExclusion(szRestriction);
					
				}
			}
		}
	}
	else if(FStrEq(CMD_ARGV(1), "password")) // add password group# password
	{
		const char* pszGroup = CMD_ARGV(2);
		const char* pszPassword = CMD_ARGV(3);
		int nGroup = 0;
		if((*pszGroup))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				pGroup->SetPassword("\0");
				
				// reset EVERYONES permissions
				for (int i = 1; i <= gpGlobals->maxClients; i++ )
				{
					CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
					if ( pEnt && pEnt->IsPlayer( ) )
					{
						CBasePlayer* pPlayer = (CBasePlayer*)pEnt;
						g_vAdmin.SetPermission(pPlayer);
					}
				}
			}
		}
	}
	else if(FStrEq(CMD_ARGV(1), "blockall")) // add blockall group#  1/0
	{
		const char* pszGroup = CMD_ARGV(2);
		int nGroup = 0;
		if((*pszGroup))
		{
			CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup((char*)pszGroup);
			if(pGroup != NULL)
			{
				//CVirtualAdminPermissions* pGroup = g_vAdmin.GetGroup(nGroup);
				pGroup->SetBlockAll(0);
				return;	
			}
		}
	}
}

const char* CVirtualAdmin::GetID(CBasePlayer* pPlayer)
{
	char* pID = (char*)GETPLAYERAUTHID(pPlayer->edict());

	if(pID && *pID)
		sprintf(pPlayer->m_szPlayerID, pID);
	else
		*pPlayer->m_szPlayerID = NULL;
	return ConvertID(pPlayer->m_szPlayerID);	
}

const char* CVirtualAdmin::ConvertID(const char* id)
{
	int len = strlen(id); 
	{
		if(FStrEq(id, ""))
			return "No ID";

		if(FStrEq(id+len-8, "LOOPBACK"))
			return "0"; // return 1 if local
	}
	
	const char* pId = id;
	for (int i = 0; i < len; i++)
	{
		if(id[i]==':')
		{
			pId++;
			return pId;
		}
		pId++;
	}
	return pId;
}
