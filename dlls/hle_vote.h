
#ifndef VOTE_H
#define VOTE_H

// constants for text messages
#define FADE_TIME_OUT   2
#define HOLD_TIME       4 
#define VOTE_TIME       (FADE_TIME_IN  + VOTE_TIME_HOLD + FADE_TIME_OUT+2) 
#define VOTE_TIME_HOLD  30 
#define FX_TIME         0.5
#define FADE_TIME_IN    0.025
#define EFFECT          2

#define VOTE_CHANNEL 1
#define PASS_FAIL_CHANNEL 2
#define ERROR_CHANNEL 3

#define VOTED_YES 1
#define VOTED_NO -1
#define VOTED_NONE 0

#include    "hle_utility.h"
#include    "hle_maploader.h"

class CBaseVote
{
protected:

	CDelayTime m_dPrintPassDelay;
	CDelayTime m_dIntermissionDelay;
	hudtextparms_t m_hudtVote; // called a vote
    hudtextparms_t m_hudtFail; // vote failed
    hudtextparms_t m_hudtInvalid; // invalid vote try again
    hudtextparms_t m_hudtPass; // vote passed
 
	CBasePlayer* m_pCreator;
	char m_pVoteName[64];
	char m_pVoteParameter[64];

public:

	
	bool m_bCanDual;
	CBaseVote(CBasePlayer* pPlayer, char* pVote=NULL, char* pParameter=NULL);
	CBasePlayer* GetCreator(){return m_pCreator;}
	virtual ~CBaseVote(){}
	virtual char* Authorize();
	virtual void Amend();
	virtual void PrintCall();
	virtual void PrintPass();
	virtual void PrintFail();
	void PrintFailSafe();
	virtual bool Execute();
	virtual char* GetParameters();
	virtual char* GetVote();
	virtual void Pass();
	virtual void Fail();
	virtual bool FailOnDisconnect(CBasePlayer* pPlayer);
};


class CDualVote : public CBaseVote
{
protected:

	bool VoteOneExecute;
	bool VoteTwoExecute;
	CBaseVote* m_pVoteOne;
	CBaseVote* m_pVoteTwo;

public:

	CDualVote(CBasePlayer* pPlayer,CBaseVote*  pVoteOne,CBaseVote*  pVoteTwo);
	virtual ~CDualVote();
	virtual char* Authorize();
	virtual void Amend();
	virtual bool FailOnDisconnect(CBasePlayer*);
	virtual void PrintCall();
	virtual void PrintPass();
	virtual void PrintFail();
	virtual bool Execute();
	virtual void Pass();
	virtual void Fail();
};

class CVoteCvar : public CBaseVote
{
protected:
	cvar_t* m_pCvar;
public:
	CVoteCvar(CBasePlayer* pPlayer, char* pCvar, char* pParameter);

	virtual char* Authorize();
	virtual void Amend();
	virtual bool Execute();
};

class CVoteBinomialCvar : public CVoteCvar
{

public:
	CVoteBinomialCvar(CBasePlayer* pPlayer, char* pCvar);
	virtual char* Authorize();
	virtual void Amend();
};

class CVoteMap : public CVoteCvar
{
public:
	CVoteMap(CBasePlayer* pPlayer, char* pMap);
	
	virtual char* Authorize();
	virtual void PrintPass();
	virtual bool Execute();
	virtual void Pass();
};

class CVoteMode : public CVoteCvar
{
public:
	CVoteMode(CBasePlayer* pPlayer, char* pMode);
	virtual void PrintPass();
	virtual char* Authorize();
	virtual bool Execute();
	virtual void Pass();
};

class CVoteCommand : public CBaseVote
{
private:
	char m_szParameters[256];
protected:
	char m_szAux0[64];
	char m_szAux1[64];
public:
	char* Authorize();
	CVoteCommand(CBasePlayer* pPlayer, char* pCommand, char* pParameter=NULL, char* aux0=NULL, char* aux1=NULL);
	virtual void Amend();
	virtual void PrintPass();
	virtual void PrintFail();
	virtual bool Execute();
	virtual char* GetParameters();
	virtual char* GetVote();
};

class CVoteNullCommand : public CVoteCommand
{
public:
	CVoteNullCommand(CBasePlayer* pPlayer, char* pCommand);
	virtual char* Authorize();
	virtual void PrintCall();
	virtual void PrintPass();
	virtual void PrintFail();
	virtual bool Execute();
};

class CVoteMatch : public CVoteCommand
{
public:
	CVoteMatch(CBasePlayer* pPlayer, char* pMatchmode);
	virtual char* Authorize();
	virtual bool Execute();
};

class CVoteAddTime : public CVoteCommand
{
public:
	CVoteAddTime(CBasePlayer* pPlayer, char* pTime); 
	virtual char* Authorize();
	virtual void Amend();
	virtual void PrintPass();
	virtual void PrintFail();
	virtual bool Execute();
};

class CVotePause : public CVoteNullCommand
{
public:
	CVotePause(CBasePlayer* pPlayer);
	virtual void PrintPass();
	virtual bool Execute();
};
/*
class CVoteChangeTeam : public CVoteCommand
{
public:
	
	CVoteChangeTeam(CBasePlayer* pPlayer, char* pCommand, char* pParameter=NULL);
	virtual char* Authorize();
	virtual void PrintPass();
	virtual void PrintFail();	
	virtual bool Execute();
};*/

class CVoteSpawnPlayer : public CVoteCommand
{
	CBasePlayer* m_pPlayer;

public:

	CVoteSpawnPlayer(CBasePlayer* pPlayer, char* pPlayerIndex, char* team=NULL);
	virtual char* Authorize();
	virtual void PrintCall();
	virtual bool FailOnDisconnect(CBasePlayer*);
	virtual void PrintPass();
	virtual void PrintFail();
	virtual bool Execute();
	virtual char* GetParameters();

};

class CVoteRemovePlayer : public CVoteCommand
{
	CBasePlayer* m_pPlayer;
public:
	CVoteRemovePlayer(CBasePlayer* pPlayer, char* pPlayerIndex);
	virtual char* Authorize();
	virtual void PrintCall();
	virtual bool FailOnDisconnect(CBasePlayer*);
	virtual void PrintPass();
	virtual void PrintFail();
	virtual bool Execute();
	virtual char* GetParameters();
};

class CVoteStructure
{

private:

	hudtextparms_t m_hudtInvalid; // invalid vote try again
	hudtextparms_t m_hudtDisplayed; // vote string yes/no's

	CBaseVote* m_pVote;
	CBaseVote* m_pVote2;
	CNameList m_ValidVotes;

    int m_nVotedPlayers;
    int m_nMajority;
    int m_nNoVotes;
    int m_nYesVotes;
    int m_nPossibleVotes;     
    
    float m_fVoteTime;
    float m_fVoteClock;


	int m_nVerdict;

public:
	static char g_newmap[32];

	CDelayTime m_dPause;
	CBaseVote* GetVote(CBasePlayer*, char*, char*, char*, char*);
	CBaseVote* ReturnVote();
	BOOL m_bVoteInProgress;

	CDelayTime m_dExecution;

    CVoteStructure(void); 
    ~CVoteStructure(void); 

	void PlayerDisconnect(CBasePlayer* pPlayer);
    void CallVote(CBasePlayer*, const char*);
    void AddYes(void);
    void PlaceVote(CBasePlayer*, int);
    void InvalidVote(CBasePlayer*, char*, bool);
    void InitializeVoteStructure();
    void AddNo(void);
    void UpdateVerdict();
    void ChangeVote(int);
	void EndVote(void);
	void TerminateVote(void);
    void UpdateStatus(void);
    void ResetVoteVars(void);
    void PrintVoteString(void);
    void ClearVotes(void);
	void ExpungeVote();
	BOOL InProgress() {return m_bVoteInProgress;}
    
    static int GetTotalPossibleVotes(void);
    static int GetMajority(void);
};

#endif



