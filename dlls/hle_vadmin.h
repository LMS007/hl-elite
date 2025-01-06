#ifndef _VADMIN_H
#define _VADMIN_

//#define VADMIN_MAXGROUPS 6
//#define VADMIN_MAXPLAYERS_GROUP 32
#define LOCAL_ID 1

class CVirtualAdminPermissions 
{
private:

	
	bool m_bBlockAll;
	bool m_bMembersAll;
	char m_szPassword[32];
	char m_szGroupName[32];
	int m_nGroupNumber;
	CNameList m_AuthorizedPlayers;//[VADMIN_MAXPLAYERS_GROUP];

	CNameList m_Permissions;
	CNameList m_Exclusions;

public:

	bool m_bIsActive;
	CVirtualAdminPermissions();
	void AddPermission(char*);
	void AddExclusion(char*);
	void RemovePermission(char*);
	void RemoveExclusion(char*);
	void AddPlayer(char* steamID);
	void RemovePlayer(const char* steamID);
	bool IsPermitted(char*);
	bool IsAuthorized(const char* steamID);
	void SetBlockAll(bool);
	bool GetBlockAll(void);
	void SetMembersAll(bool);
	bool GetMembersAll(void);
	CNameList* GetPlayerList(){return &m_AuthorizedPlayers;};
	CName* GetHeadPermission();
	CName* GetHeadExclusion();
	CName* GetHeadAdmin();
	// GetPlayerID(int);

	bool IsPassword(char*);
	bool IsPassword();
	void SetPassword(char*);
	//bool IsInGroup(char* steamID);
	char* GetPassword();
	//void SetGroup(int nNumber);
	bool IsExclusion(char* pszExclusion);
	bool IsPermission(char* pszExclusion);
	long GetAdminID(int nIndex);

	void SetName(char*);
	char* GetName();
	
};

class CVAdminPermissionsNode
{
public:

	CVAdminPermissionsNode()
	{
		m_pGroup = NULL;
		m_pNext = NULL;
		m_pGroup = new CVirtualAdminPermissions();
	};
	~CVAdminPermissionsNode()
	{
		delete m_pGroup;
	}
	CVirtualAdminPermissions *m_pGroup;
	CVAdminPermissionsNode* m_pNext;
};

class CVirtualAdmin
{
	char* m_pszCommand;
	char* m_pszLine;
	int m_nIndex;
	ifstream vAdminFile;

	CVAdminPermissionsNode *m_pHead;//m_PermissionGroups[VADMIN_MAXGROUPS];
	CVAdminPermissionsNode* m_pCurrent;

	static hudtextparms_t m_hudtInfo;
	static void (*ptAdd)(void);
	static void (*ptRemove)(void);

	bool RemoveQuotes(char**);
	char* GetNextCommand();
	bool GetNextLine();
	int RemoveLeadingSpacesTabs(char* String);

	static void AddMember(char*, CVAdminPermissionsNode*);
	static void CVirtualAdmin::AddPremission(char*, CVAdminPermissionsNode*);
	static void CVirtualAdmin::AddRestriction(char*, CVAdminPermissionsNode*);

	int LoadList(void(*)(char*, CVAdminPermissionsNode*));

public:
	
	
	static CBasePlayer* g_bClient;
	bool m_bLoaded;
	CVirtualAdmin();
	~CVirtualAdmin();

	CVirtualAdminPermissions* GetPlayerGroup(CBasePlayer* pPlayer);
	CVAdminPermissionsNode* GetHeadGroup(){return m_pHead; }
	int LoadPermissionGroups();
	bool HasPermission(CBasePlayer* pPlayer, char* pCommand);
	void SetPermission(CBasePlayer* pPlayer);
	void AddPlayerToGroup(CBasePlayer* pPlayer, char* pzsGroup);
	void RemovePlayerFromGroup(CBasePlayer* pPlayer, char* pzsGroup);
	void AddPermissionToGroup(char* pzsGroup, char* pszCommand);
	void RemovePermissionFromGroup(char* pzsGroup, char* pszCommand);
	void AddRestrictionToGroup(char* pzsGroup, char* pszCommand);
	void RemoveRestrictionFromGroup(char* pzsGroup, char* pszCommand);
	void WriteToDisk(char*);
	
	void PrintGroupCommands(CBasePlayer *pAdmin, CVirtualAdminPermissions* pGroup);
	void PrintGroupAdministration(CBasePlayer* pAdmin, CVirtualAdminPermissions* pGroup);
	CVirtualAdminPermissions* GetGroup(char* pzsGroup);
	void DisplayHelp(CBasePlayer*){};

	static void Add();
	static void Remove();
	static int AdminCommand(CBasePlayer* pAdmin);
	static const char* GetID(CBasePlayer* pPlayer);
	static const char* ConvertID(const char* str);


};

#endif