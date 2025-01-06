#ifndef HLE_UTILITY_H
#define HLE_UTILITY_H


#define MATCHSTART_NONE 0
#define MATCHSTART_EMPTY 1
#define MATCHSTART_FULL 2

#define MAX_SPRITE_NUMBER 5
#define COUNTER_TIME 1

#define MIN_DROP_DISTANCE 75

#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>


/*
typedef struct CBasePlayerNode // this is prolly recoded 100 times 
{
    CBasePlayer *pPlayer;
    CBasePlayerNode *next;

}PlayerNode;

*/


/////////////
// hle list//
/////////////
class CMapName;

class CEntityNode
{

public:

	~CEntityNode();
	CEntityNode();

	edict_t* m_pEntity;
	CEntityNode* m_pNext;
};


class CEntityList 
{

private:

	CEntityNode *m_pHead;
	int	m_nSize;

public:
	CEntityList();
	~CEntityList();
	void push_back(edict_t* pNewEntity);
	int lookup(edict_t* pEntity);
	int size();
	void clear(); 
	void remove(int nIndex);
	void remove(edict_t* pEntity);
	void clean();
	CEntityNode* head();
	edict_t* operator [] ( int nIndex );
};




class CDelayTime
{
private:
	float m_fTime;
public:
	CDelayTime(){m_fTime = 0;}
	inline void AddTime(float fTime){m_fTime = gpGlobals->time + fTime;}
	inline void SubtractTime(float fTime) { m_fTime = gpGlobals->time - fTime;}
	inline BOOL TimeUp()
	{
		if(m_fTime==0) return FALSE;
		if(gpGlobals->time > m_fTime ) return TRUE;
		else return FALSE;
	}
	inline BOOL TimeUp(float fExtendedDelay)
	{
		if(m_fTime==0) return FALSE;
		if(gpGlobals->time > m_fTime+fExtendedDelay ) return TRUE;
		else return FALSE;
	}
	inline float GetTimeLeft()
	{ 
		if(m_fTime - gpGlobals->time  <=0) return 0;
		return m_fTime - gpGlobals->time;
	}
	inline void ClearTime() { m_fTime = 0; }
};

class CMapPackage
{
	CDelayTime	m_dPackageDelay;
	int m_nMapsSent;
	void UpdateSending();
	CMapName* m_pStart;
	bool m_bMapsToSend;
	

public:
		
	CMapPackage();
	void Reset();
	void SendPackage(edict_t* pReciever);
	
};


class CPlayerInfo
{
public:
	CPlayerInfo(CBasePlayer*);
	long m_nWonID;
	int m_nWins;
	int m_nDeaths;
	int m_nFrags;
	int m_nSaves;
	int m_nCaptures;
	char m_szTeam[32];
	CPlayerInfo* m_pNext;
	CDelayTime m_dTime;
	void ResetPlayer(CBasePlayer*);
};


class CPlayerRecoverQueue
{
private:
	int m_nSize;
	CPlayerInfo* m_pHead;
public:
	
	CPlayerRecoverQueue();
	~CPlayerRecoverQueue();
	CPlayerInfo* GetHead();
	void AddPlayer(edict_t* pClient);
	void RemovePlayer(edict_t* pClient);
	CPlayerInfo* LocatePlayer(CBasePlayer* Player);
	bool AttemptedToRecover(edict_t* pClient);
	void ClearQueue();
	void UpdateQueue();
};


class CMatchStart /*: public CBaseEntity*/
{
	int m_nMatchStart;
	
	hudtextparms_t m_hudtMessage;
public:
	
	//void EXPORT ThinkSequence();
	~CMatchStart();
	CPlayerRecoverQueue m_Players;
	int m_bInProgess;
	CMatchStart();
	void startMatch(void);
	void setMatch(int nMatch);
	int getMatch();
	int hasStarted();
	void hasStarted(bool bStarted);
	bool isMatch();
	CDelayTime m_dDelay;
	static void callMatch( );
};

class AdminMapCommand
{
public:


};

class DropQueueNode
{
public:
	DropQueueNode()
	{
		m_pNext = NULL;
		m_dDelay.AddTime(30);
	}
	DropQueueNode* m_pNext;
	CDelayTime m_dDelay;
};

class DropQueue
{
	DropQueueNode* m_pHead;
public:
	DropQueue();
	void Add();
	float Full();
	virtual ~DropQueue();
	void Clear();
	void Clear(DropQueueNode*);
};


class SpawnPointsDM
{
protected:
	int m_nSpawnPoints;
	int m_nLastSpawn;
public:
	CBaseDMStart* m_Spawns[64];
	

	void ClearList();
	void CreateSpawnList();
	edict_t* FindRandomSpawnPoint(CBasePlayer* pPlayer	);

};

class SpawnPointsTeam 
{
protected:
	int m_nTeamPoints[7];
	int m_nLastSpawn;
public:
	CBaseDMStart* m_TeamSpawns[7][64];

	void ClearList();
	void CreateSpawnList();
	edict_t* FindRandomSpawnPoint(CBasePlayer* pPlayer	);

};



class CSystemCount  
{
private:
	clock_t m_time;
public:
	static void Delay(long);
	long StopClock();
	void StartClock();
	CSystemCount();
	virtual ~CSystemCount();
};

class COutput  
{
private:
	struct _timeb m_TimeBuff;
	char m_cText[1024];
	char m_szFileName[64];
	ofstream m_oFile;

public:

	void CreateFile();
	void OpenFile();
	void CloseFile();
	CSystemCount SystemClock;
	ostream* m_pOutputFilePointer;
	bool	m_bOutputFile;
	COutput();
	COutput(char* pFileName);
	void SetOutputFileName(char* pFileName);
	ofstream& GetOut() { return m_oFile; }
	~COutput();
	friend ostream& operator << (COutput& Output, char* pText);
	friend ostream& operator << (COutput& Output, int nNum);

};

void FillPlayer(CBasePlayer *pPlayer);
void LoadPlayer(CBasePlayer *pPlayer);
void ArmPlayer(CBasePlayer *pPlayer);
void ArmPlayerRestricted(CBasePlayer *pPlayer);
void SendCountDown(int nNumber);
void SendCountClear();
void HLE_EndGame();
void RemoveAllEntites();
void ShowGameInfo(CBasePlayer* pPlayer, bool All, float time);
void ShowPlayerInfo(CBasePlayer* pPlayer, bool All, float time);
void RespawnAllEntites();
void ResetFlags();
void RemoveFlag();
void AddServerCommands();
void SendMapsToClients();
CBaseEntity* TossItem(CBasePlayer* pPlayer, const char* classname);

#endif