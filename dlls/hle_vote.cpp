#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"gamerules.h"


#include	"skill.h"
#include	"game.h"
#include	"items.h"
#include     "hle_vote.h"
#include    "hle_gamemodes.h"
#include	"hle_utility.h"
#include    "hle_maploader.h"

extern COutput debug_file;

extern int g_nMatchStart;
extern int g_teamplay;
extern int g_nMapMode;
extern CVoteStructure* g_pVote;

#define VOTE_AUTHORIZED "\0"

char CVoteStructure::g_newmap[32];


CVoteStructure::CVoteStructure(void) // ctor
{

    m_nVotedPlayers = 0;
    m_pVote = NULL;
    m_bVoteInProgress = FALSE;
    m_nNoVotes = 0;
    m_nYesVotes = 0;
    m_nMajority = 0; 

	// load the vote server instructions
	
      
    m_hudtInvalid.x            = 0.45f;
    m_hudtInvalid.y            = 0.1f;
    m_hudtInvalid.a1           = 0.0f;
    m_hudtInvalid.a2           = 0.0f;
    m_hudtInvalid.b1           = 0.0f;
    m_hudtInvalid.b2           = 255.0f;
    m_hudtInvalid.g1           = 255.0f;
    m_hudtInvalid.g2           = 255.0f;
    m_hudtInvalid.r1           = 255.0f;
    m_hudtInvalid.r2            = 255.0f;
    m_hudtInvalid.channel      = 4;
    m_hudtInvalid.effect       = EFFECT;
    m_hudtInvalid.fxTime       = FX_TIME;
    m_hudtInvalid.fadeinTime   = FADE_TIME_IN;
    m_hudtInvalid.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtInvalid.holdTime     = 2;    

	m_hudtDisplayed.x            = 0.02f;
    m_hudtDisplayed.y            = 0.38f;
    m_hudtDisplayed.a1           = 0.0f;
    m_hudtDisplayed.a2           = 0.0f;
    m_hudtDisplayed.b1           = 255.0f;
    m_hudtDisplayed.b2           = 0.0f;
    m_hudtDisplayed.g1           = 120.0f;
    m_hudtDisplayed.g2           = 0.0f;
    m_hudtDisplayed.r1           = 150.0f;
    m_hudtDisplayed.r2            = 0.0f;
    m_hudtDisplayed.channel      = PASS_FAIL_CHANNEL;
    m_hudtDisplayed.effect       = EFFECT;
    m_hudtDisplayed.fxTime       = FX_TIME;
    m_hudtDisplayed.fadeinTime   = FADE_TIME_IN;
    m_hudtDisplayed.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtDisplayed.holdTime     = VOTE_TIME_HOLD;

	ifstream validVoteFile;
	validVoteFile.open("hle/vote.txt");
		
	if(validVoteFile.is_open()) // overwrite if we have a file
	{
		char line[255];
		
		while(!validVoteFile.eof())
		{
				validVoteFile.getline(line,255 );

				char* command = strtok(line, " ");
				if(!command)
				{
					continue;
				}
				if(!strncmp(command, "//", 2))
				{
					continue;
				}

				m_ValidVotes.addSmart(command);				
			}
		
	}
	validVoteFile.close();
   
}

void CVoteStructure::InitializeVoteStructure()
{
	m_hudtDisplayed.holdTime     = VOTE_TIME_HOLD;
	m_fVoteTime = gpGlobals->time + VOTE_TIME; 
	m_bVoteInProgress = TRUE;
	m_nNoVotes          = 0;
	m_nYesVotes         = 0;
	m_nMajority = GetMajority();
}

CBaseVote* CVoteStructure::GetVote(CBasePlayer* pCreator, char* pVote, char* pParameter, char* aux0, char* aux1)
{
	bool bValid=false;

	if(!m_ValidVotes.locateName(pVote))
		return NULL;

	if(FStrEq(pVote, "mode")) // special case
		return new CVoteMode(pCreator, pParameter);

	else if(FStrEq(pVote, "changelevel")) // special case
		return new CVoteMap(pCreator,pParameter);

	else if(FStrEq(pVote, "matchstart")) // special case
		return new CVoteMatch(pCreator,pParameter);

	else if(FStrEq(pVote, "addtime")) // special case
		return new CVoteAddTime(pCreator,pParameter);

	else if(FStrEq(pVote, "pausegame")) // special case
		return new CVotePause(pCreator);

	else if(FStrEq(pVote, "emit")) // special case
		return new CVoteSpawnPlayer(pCreator,pParameter, aux0);
//	else if(FStrEq(pVote, "changeteam")) // special case
	//	return new CVoteChangeTeam(pCreator, pVote, pParameter);
	else if(FStrEq(pVote, "eject"))
			return new CVoteRemovePlayer(pCreator, pParameter);

	else if(CVAR_GET_POINTER( pVote )!=NULL) // generic case for a cvar
	{
		if(pParameter && *pParameter)
			return new CVoteCvar(pCreator, pVote, pParameter);
		else
			return new CVoteBinomialCvar(pCreator, pVote); // no parameter
	}
	else // generic case for a non cvar console command 
	{
		if(pParameter)
			return new CVoteCommand(pCreator, pVote, pParameter, aux0, aux1);
		else
			return new CVoteNullCommand(pCreator, pVote); // no parameter
	}

	return NULL;
}

void  CVoteStructure::CallVote(CBasePlayer *pPlayer, const char *vote_string)
{
	if(allowvotes.value==0)
	{
		InvalidVote(pPlayer, "Voting is not allowed, \"mp_allowvotes 0\"", true);
		return;
	}
	/*if(g_pGameRules->MatchInProgress() && allowmatchvotes.value == 0 )
	{
		InvalidVote(pPlayer, "Voting is not allowed, \"mp_allowmatchvotes 0\"", true);
		return;
	}*/
	if(m_bVoteInProgress == TRUE)
    {
        InvalidVote(pPlayer, "A vote is in progress", true);
        return;
    }
	if(!vote_string)
	{
		InvalidVote(pPlayer, "Invalid vote", true);
		return;
	}

	if(m_pVote)
		ExpungeVote();  

	char szVote1[256];
	char szVote2[256];
	sprintf(szVote2, "%s", strpbrk(vote_string, ",")); 
	sprintf(szVote1, "%s", strtok((char*)vote_string, ","));
	//sprintf(szVote2, "%s", strtok(NULL, " "));
	
	char *vote_message, *vote_parameter, *aux0, *aux1;

	vote_message = strtok(szVote1, " ");
	vote_parameter = strtok(NULL, " ");
	aux0 = strtok(NULL, " ");
	aux1 = strtok(NULL, " ");

	CBaseVote* pVote1 = GetVote(pPlayer, (char*)vote_message, (char*)vote_parameter, (char*)aux0, (char*)aux1);
	
	if(!pVote1)
	{
		delete pVote1; 
		pVote1 = NULL;
	}

	vote_message = strtok(szVote2, " ");
	if(strlen(vote_message)>1)
		vote_message++;
	/*int len = strlen(vote_message);
	for(int i = 0; i < len; i++)
	{
		if(vote_message[i] == ' ')
			vote_message++;
	}*/
	if(strlen(vote_message) == 1)
		vote_message = strtok(NULL, " ");
	vote_parameter = strtok(NULL, " ");
	aux0 = strtok(NULL, " ");
	aux1 = strtok(NULL, " ");

	CBaseVote* pVote2 = GetVote(pPlayer, (char*)vote_message, (char*)vote_parameter, (char*)aux0, (char*)aux1);

	if(!pVote2)
	{
		delete pVote2;
		pVote2 = NULL;
	}
	if(pVote1)
	{
		if(pVote2)
			m_pVote = new CDualVote(pPlayer, pVote1, pVote2);
		else
			m_pVote = pVote1;
	}
	else if(pVote2) // if not 1 then check for 2 and make it the primary
		m_pVote = pVote2;
	else
	{
		InvalidVote(pPlayer, "Invalid votes", true);
		ExpungeVote();
        return; 
	}

	char szAutorize[255];
	*szAutorize=NULL;
	strcpy(szAutorize, m_pVote->Authorize());
	if(FStrEq(szAutorize, VOTE_AUTHORIZED))// check to see if this is a valid structure
	{
		InitializeVoteStructure();
		m_nMajority = GetMajority();
		m_pVote->Amend(); // smart voting enables no paramers nessacary for some votes
		m_pVote->PrintCall(); // print the vote call to the screen for all the players
		PlaceVote(pPlayer, VOTED_YES); // callers automaticly place a yes vote	
	}
	else
	{
		InvalidVote(pPlayer, szAutorize, true);
		ExpungeVote();
        return; 
	}
}
	
//
// Called when a player votes yes or no or types yes/no in the console
//
/*
void CVoteStructure::ResendMessage(CBasePlayer *pPlayer)
{
//	if(m_bVoteInProgress)
//		UTIL_HudMessage(pPlayer, m_hudtVote, m_cOutPutString); 
}*/
void CVoteStructure::PlaceVote(CBasePlayer *pPlayer, int vote)
{
	char buf[63];
	*buf=NULL;
	if(m_pVote)
	{
		if(m_pVote->GetCreator() == pPlayer && vote == VOTED_NO)
			m_nVerdict=-1; // change this lator;
	}
    if(m_bVoteInProgress)
    {
		if(pPlayer->m_iVote == VOTED_NONE)
		{
			pPlayer->m_iVote = vote;
			if(vote == VOTED_YES)
				AddYes();
			else
				AddNo();
			return;
		}	
        else if(pPlayer->m_iVote == vote)
        { 
			strcpy(buf,"You have already voted ");
			if(vote == VOTED_YES)
				strcat(buf, "\"YES\"");
			else
				strcat(buf, "\"NO\"");
            InvalidVote(pPlayer, buf,false); // cant vote twice
            return;
        }
        else if(vote == VOTED_YES)
        {
            if(pPlayer->m_iVote == VOTED_NO) // he allready voted no, so we need to change his votes and recount
            {
                pPlayer->m_iVote = VOTED_YES;
                ChangeVote(VOTED_YES);
            }
            else
			{
               AddYes();
            }
        }
        else if(vote == VOTED_NO)
        {
            if(pPlayer->m_iVote == VOTED_YES) // he allready voted yes, so we need to change his votes and recount
            {
                pPlayer->m_iVote = VOTED_NO;
                ChangeVote(VOTED_NO);
            }
            else
            {
				AddNo();
            }
        }
        else
        {
            InvalidVote(pPlayer,"No votes in progress",true); // no one called a vote
        }

    }
    else
    {
        InvalidVote(pPlayer,"No votes in progress",true); // no one called a vote
    }
}


void CVoteStructure::AddYes(void)
{
	m_nYesVotes ++;
    PrintVoteString();
	UpdateVerdict();
//	m_fVoteTime += 5; // add 5 seconds of time per yes vote
}

void CVoteStructure::AddNo(void)
{
    m_nNoVotes ++;
    PrintVoteString();
    UpdateVerdict();
}

void CVoteStructure::ChangeVote(int newVote)
{
   if(newVote == VOTED_YES)
   {
        m_nNoVotes--;
        m_nYesVotes++;
        PrintVoteString();
        UpdateVerdict();
//      m_fVoteTime += 5;
       
   }
   else if(newVote == VOTED_NO)
   {
        m_nNoVotes++;
        m_nYesVotes--;
        PrintVoteString();
		UpdateVerdict();
   }
}

//citybank
//9pm

int CVoteStructure::GetTotalPossibleVotes(void)
{
    return UTIL_HumansInGame();
}

void CVoteStructure::UpdateStatus(void) // call in CGameRules::Think();
{
	if(m_dExecution.TimeUp())
	{
		if(m_pVote->Execute())
		{
#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
		
		debug_file.OpenFile();
		debug_file << "CVoteStructure::UpdateStatus(void); Finished executing vote\n";
		debug_file << m_pVote->GetVote() << endl;
		debug_file << m_pVote->GetParameters() << endl;
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================

			m_dExecution.ClearTime();
			ResetVoteVars();
			ExpungeVote();
		}
	}

	/*if(m_dPause.TimeUp()) // hack for pause
	{
		m_dPause.ClearTime();
		SERVER_COMMAND("pausable 0\n");
		SERVER_EXECUTE();
	}*/
	if(m_bVoteInProgress == TRUE)
	{
		if(m_nVerdict!=0)
			EndVote();		
		
		else if(gpGlobals->time > m_fVoteTime)
			m_nVerdict=-1;
	}
}
void CVoteStructure::TerminateVote()
{
	m_nVerdict = -2;
	EndVote();
}
void CVoteStructure::EndVote()
{
	if(m_nVerdict==VOTED_NO)
	{
		m_pVote->Fail();
		m_pVote->PrintFail();
		ResetVoteVars();
		ExpungeVote();
	}
	else if(m_nVerdict==VOTED_YES)
	{
		m_pVote->Pass();
		m_pVote->PrintPass();
		m_dExecution.AddTime(2);
		m_bVoteInProgress = FALSE;
	}
	else if(m_nVerdict==-2) // end now, dont print!
	{
		m_pVote->Fail();
		m_pVote->PrintFailSafe();	// fixes some crashes with clients leaving servers
		ResetVoteVars();
		ExpungeVote();
	}
	else if(m_nVerdict==-3) // stalemate
	{
		m_pVote->Fail();
		m_pVote->PrintFail();
		ResetVoteVars();
		ExpungeVote();
	}
}

void  CVoteStructure::ExpungeVote(void)
{

	if(m_pVote)
	{
		delete m_pVote;
		m_pVote = NULL;
	}
}
void  CVoteStructure::UpdateVerdict(void)
{
    if(!m_bVoteInProgress)
        return;
	if (m_nYesVotes >= m_nMajority) // let the vote be passed
    {
			m_nVerdict = 1;
	}
	else if(m_nNoVotes >= m_nMajority)  // let the vote be failed
	{
		m_nVerdict = -1;
	}
	else if(m_nNoVotes+m_nYesVotes>=GetTotalPossibleVotes()) //stalemate
	{
		m_nVerdict = -3;
	}
}


void CVoteStructure::InvalidVote(CBasePlayer *pPlayer,  char* pMessage, bool ResetPlayer)
{
    if(ResetPlayer)
        pPlayer->m_iVote = VOTED_NONE;
   
	UTIL_HudMessage(pPlayer, m_hudtInvalid, pMessage );	
}



void  CVoteStructure::ResetVoteVars(void) // globals, whatever, i did code this crappy SDK if i could make a mainGameObject i fucking would.
{

    ClearVotes(); // set each players has voted flag
	*g_newmap			= NULL;
    m_nNoVotes			 = 0;
    m_fVoteClock        = 0;
	m_nYesVotes		 = 0;
    m_nNoVotes          = 0;
    m_nVotedPlayers     = 0;
	m_nVerdict			=0;

	m_hudtDisplayed.holdTime = 0;
	PrintVoteString();
    m_nMajority      = 0;
	m_bVoteInProgress = FALSE;

}


void CVoteStructure::ClearVotes(void)
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *ent = UTIL_PlayerByIndex( i );
		CBaseEntity *plr = NULL;
		if (ent && ent->IsPlayer())
		{
			plr = (CBasePlayer*)ent;
			plr->m_iVote = VOTED_NONE;
		}
	}
}

CVoteStructure::~CVoteStructure()
{
	ResetVoteVars();
	ExpungeVote();
 
}

void CVoteStructure::PrintVoteString()
{
    char cPrintString[128];
	*cPrintString = NULL;
	sprintf(cPrintString, "%i votes for a majority.\nYes: %i\nNo: %i", m_nMajority, m_nYesVotes, m_nNoVotes);
    UTIL_HudMessageAll(this->m_hudtDisplayed, cPrintString);
}

int CVoteStructure::GetMajority(void)
{
	int TotalVotes = GetTotalPossibleVotes();
    return ((TotalVotes / 2) +1);//+ (TotalVotes % 2));
}

CBaseVote* CVoteStructure::ReturnVote()
{
	return m_pVote;
}

void CVoteStructure::PlayerDisconnect(CBasePlayer *pPlayer)
{
	if(m_bVoteInProgress)
	{
		if(m_pVote->FailOnDisconnect(pPlayer))
		{
			TerminateVote();
		}
	}
}

//==================
// CBaseVote
//==================


CBaseVote::CBaseVote(CBasePlayer *pPlayer, char* pVote, char* pParameter)
{
	m_bCanDual = false;
	if(pVote)
		strncpy(m_pVoteName, pVote, 63);
	else
		*m_pVoteName = NULL;
	if(pParameter)
		strncpy(m_pVoteParameter, pParameter, 63);
	else
		*m_pVoteParameter = NULL;
	
	m_pCreator = pPlayer;

	m_hudtVote.x            =  0.02f;
    m_hudtVote.y            = 0.2f;
    m_hudtVote.a1           = 0.0f;
    m_hudtVote.a2           = 0.0f;
    m_hudtVote.b1           = 255.0f;
    m_hudtVote.b2           = 0.0f;
    m_hudtVote.g1           = 180.0f;
    m_hudtVote.g2           = 0.0f;
    m_hudtVote.r1           = 120.0f;
    m_hudtVote.r2            = 0.0f;
    m_hudtVote.channel      = VOTE_CHANNEL;
    m_hudtVote.effect       = EFFECT;
    m_hudtVote.fxTime       = FX_TIME;
    m_hudtVote.fadeinTime   = FADE_TIME_IN;
    m_hudtVote.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtVote.holdTime     = VOTE_TIME_HOLD;

    // red color for failed votes
    m_hudtFail.x            = 0.02f;
    m_hudtFail.y            = 0.2f;
    m_hudtFail.a1           = 0.0f;
    m_hudtFail.a2           = 0.0f;
    m_hudtFail.b1           = 50.0f;
    m_hudtFail.b2           = 0.0f;
    m_hudtFail.g1           = 50.0f;
    m_hudtFail.g2           = 0.0f;
    m_hudtFail.r1           = 255.0f;
    m_hudtFail.r2            = 0.0f;
    m_hudtFail.channel      = ERROR_CHANNEL;
    m_hudtFail.effect       = EFFECT;
    m_hudtFail.fxTime       = FX_TIME;
    m_hudtFail.fadeinTime   = FADE_TIME_IN;
    m_hudtFail.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtFail.holdTime     = 2;

    // green for passed votes
    m_hudtPass.x            = 0.02f;
    m_hudtPass.y            = 0.2f;
    m_hudtPass.a1           = 0.0f;
    m_hudtPass.a2           = 0.0f;
    m_hudtPass.b1           = 0.0f;
    m_hudtPass.b2           = 0.0f;
    m_hudtPass.g1           = 255.0f;
    m_hudtPass.g2           = 0.0f;
    m_hudtPass.r1           = 0.0f;
    m_hudtPass.r2           = 0.0f;
    m_hudtPass.channel      = ERROR_CHANNEL;
    m_hudtPass.effect       = EFFECT;
    m_hudtPass.fxTime       = FX_TIME;
    m_hudtPass.fadeinTime   = FADE_TIME_IN;
    m_hudtPass.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtPass.holdTime     = 2;

    // yellow for invaild votes votes
    m_hudtInvalid.x            = 0.45f;
    m_hudtInvalid.y            = 0.1f;
    m_hudtInvalid.a1           = 0.0f;
    m_hudtInvalid.a2           = 0.0f;
    m_hudtInvalid.b1           = 0.0f;
    m_hudtInvalid.b2           = 0.0f;
    m_hudtInvalid.g1           = 255.0f;
    m_hudtInvalid.g2           = 0.0f;
    m_hudtInvalid.r1           = 255.0f;
    m_hudtInvalid.r2            = 0.0f;
    m_hudtInvalid.channel      = 4;
    m_hudtInvalid.effect       = EFFECT;
    m_hudtInvalid.fxTime       = FX_TIME;
    m_hudtInvalid.fadeinTime   = FADE_TIME_IN;
    m_hudtInvalid.fadeoutTime  = FADE_TIME_OUT;    
    m_hudtInvalid.holdTime     = 2;    	
}
char* CBaseVote::Authorize()
{
	// this no longer applies since we'll alwayts call this base auth method
	/*if(!m_pVoteName || !*m_pVoteName)
		return "Insufficient vote name";
	if(!m_pVoteParameter || !*m_pVoteParameter)
		return "Insufficient vote parameter ";*/
	if(g_pGameRules->MatchInProgress() && allowmatchvotes.value == 0 )
	{
		if(!strcpy(m_pVoteName, "matchstart") && !strcpy(m_pVoteParameter, "0"))
		{
			return VOTE_AUTHORIZED;
		}
		else 
		{
			return "Only matchstart voting is allowed during a match";
		}
			
	}
	return VOTE_AUTHORIZED;
}
void CBaseVote::Amend()
{
	// for binomial cvar_t's or corrections of that sort
}
void CBaseVote::PrintCall()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "Vote called by %s.\n%s %s.\nOpen the menu to vote.",STRING(m_pCreator->pev->netname ),m_pVoteName,m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtVote, cBuf);

}
void CBaseVote::PrintPass()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has passed\n%s %s is now in effect.", m_pVoteName, m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtPass, cBuf);
}
void CBaseVote::PrintFail()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has failed\n%s %s is not in effect.", m_pVoteName, m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtFail, cBuf);
}
void CBaseVote::PrintFailSafe()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has be terminated due to a client disconnection\n.");
	UTIL_HudMessageAll(m_hudtFail, cBuf);
}

bool CBaseVote::Execute()
{
	return true;
	// undone for abstraction
}

char* CBaseVote::GetParameters()
{
	return m_pVoteParameter;
}
char* CBaseVote::GetVote()
{
	return m_pVoteName;
}

void CBaseVote::Pass()
{
}
void CBaseVote::Fail()
{
}
bool CBaseVote::FailOnDisconnect(CBasePlayer* pPlayer)
{
	if(this->m_pCreator == pPlayer)
	{
		return true;
	}
	return false;
}
//==================
// CVoteCvar
//==================


CVoteCvar::CVoteCvar(CBasePlayer* pPlayer, char* pCvar, char* pParameter) : CBaseVote(pPlayer, pCvar, pParameter)
{
	m_pCvar = CVAR_GET_POINTER(pCvar);
}

char* CVoteCvar::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}

	if(!m_pCvar)
		return "Insufficient cvar name.";
	if(!m_pVoteParameter)
		return "Insufficient cvar parameter.";

	return CBaseVote::Authorize();
}
void CVoteCvar::Amend()
{
	CBaseVote::Amend();
}

	
bool CVoteCvar::Execute()
{
	if(m_pCvar)
	{
		CVAR_SET_STRING(m_pCvar->name, m_pVoteParameter);
	}
	return true;
}


//==================
// CVoteBinomialCvar
//==================

CVoteBinomialCvar::CVoteBinomialCvar(CBasePlayer* pPlayer, char* pCvar) : CVoteCvar(pPlayer, pCvar, NULL)
{
}

char* CVoteBinomialCvar::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}

	if(!m_pVoteName || !*m_pVoteName)
		return "Insufficient vote name";
	return VOTE_AUTHORIZED;
}
void CVoteBinomialCvar::Amend()
{
	if(m_pCvar->value ==0)
		strcpy(m_pVoteParameter, "1");
	else
		strcpy(m_pVoteParameter, "0");
}


CVoteMap::CVoteMap(CBasePlayer* pPlayer, char* pMap) : CVoteCvar(pPlayer, "changelevel", pMap)
{
	// we need this in the ctor because we check it right away
	*CVoteStructure::g_newmap=NULL;
}

char* CVoteMap::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}

	if(!g_MapsServer.m_bGotMaps)
    {
		ALERT(at_console, "\n**** ERROR MAP LIST NOT LOADED ****\n");
        return "Map list not loaded on server";
    }    
    for(int i=0; i < g_MapsServer.size();i++)
    {        
		//if(!m_pVoteParameter)
		//	return false;
		CMapName tempMap(m_pVoteParameter);
		if(g_MapsServer.locateMap(&tempMap))
		{
			CVoteCvar::Amend();
			strncpy(CVoteStructure::g_newmap, m_pVoteParameter, 31);
			return VOTE_AUTHORIZED;  
		}
		//if(FStrEq(g_MapsServer[i]->getName(),m_pVoteParameter))
        //{
          // return VOTE_AUTHORIZED;    
        //}
	}
    return "Map not found on server";

}

void CVoteMap::Pass()
{
	m_dPrintPassDelay.AddTime(3);
	m_dIntermissionDelay.AddTime(7);
}

void CVoteMap::PrintPass()
{
	
	char buf[128];
	sprintf(buf, "The vote has passed,\n%s is the new level.", m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtPass, buf);
}

bool CVoteMap::Execute()
{

	if(m_dPrintPassDelay.TimeUp())
	{
		m_dPrintPassDelay.ClearTime();
		UTIL_HudMessageAll(m_hudtPass, "");
		MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
		MESSAGE_END();
//		ShowGameInfo(NULL, true, 5);
	}
	
	if(m_dIntermissionDelay.TimeUp())
	{
		m_dIntermissionDelay.ClearTime();
		char szCommand[128];
		*szCommand = NULL;
		sprintf(szCommand, "changelevel %s\n", m_pVoteParameter);
		SERVER_COMMAND(szCommand);
		ALERT(at_console, "CVoteMap::Execute()\n");
		return true;
	}
	return false;
}

//==================
// CVoteMode
//==================


CVoteMode::CVoteMode(CBasePlayer* pPlayer, char* pMode) : CVoteCvar(pPlayer, "mode", pMode)
{
}
char* CVoteMode::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}

	char szBannedModes[256];
	*szBannedModes=NULL;
	strcpy(szBannedModes, banmodes.string);
	

	char* pszMode = strtok( szBannedModes, ";" );
	while ( pszMode != NULL && *pszMode )
	{
		if(FStrEq(pszMode, m_pVoteParameter))
		{
			return "This game mode is banned by the server";
		}
		pszMode = strtok( NULL, ";" );
	}
	
	if(FStrEq(gamemode.string, m_pVoteParameter))
		return "This game mode is already enabled";

	if(FStrEq(CVoteStructure::g_newmap, ""))
	{
		if(FStrEq(m_pVoteParameter, "ctf") && g_nMapMode != FORCE_CTF)
			return "CTF mode is only allowed in a CTF map.";

		if(FStrEq(m_pVoteParameter, "practice"))
			return VOTE_AUTHORIZED;
		

		if(g_nMapMode == FORCE_CTF)
		{
			if(FStrEq(m_pVoteParameter, "ctp"))
			{
				return "CTP mode is not allowed in a CTF map.";
			}
			else if(FStrEq(m_pVoteParameter, "teamplay"))
			{
				return "Teamplay mode is not allowed in a CTF map.";
			}
			else if(FStrEq(m_pVoteParameter, "ffa"))
			{
				return "FFA mode is not allowed in a CTF map.";
			}
		}
		if(g_nMapMode == FORCE_DUEL)
		{
			if(!FStrEq(m_pVoteParameter, "duel") 
				&& !FStrEq(m_pVoteParameter, "action") 
				&& !FStrEq(m_pVoteParameter, "lms") 
				&& !FStrEq(m_pVoteParameter, "teamaction") 
				&& !FStrEq(m_pVoteParameter, "teamduel") 
				&& !FStrEq(m_pVoteParameter, "lts")) // only allowed modes
				return "This mode is not allowed in a duel map.";
		}

	}
	else
	{
		if(FStrEq(m_pVoteParameter, "practice"))
			return VOTE_AUTHORIZED;

		if(!strncmp(CVoteStructure::g_newmap, "hleduel", 6))
		{
			if(!FStrEq(m_pVoteParameter, "duel") 
				&& !FStrEq(m_pVoteParameter, "action") 
				&& !FStrEq(m_pVoteParameter, "lms") 
				&& !FStrEq(m_pVoteParameter, "teamaction") 
				&& !FStrEq(m_pVoteParameter, "teamduel") 
				&& !FStrEq(m_pVoteParameter, "lts")) // only allowed modes

				return "This mode is not allowed in a duel map.";
		}
		else if(!strncmp(CVoteStructure::g_newmap, "hlectf", 5))
		{
			if(FStrEq(m_pVoteParameter, "ctp"))
			{
				return "CTP mode is not allowed in a CTF map.";
			}
			else if(FStrEq(m_pVoteParameter, "teamplay"))
			{
				return "Teamplay mode is not allowed in a CTF map.";
			}
			else if(FStrEq(m_pVoteParameter, "ffa"))
			{
				return "FFA mode is not allowed in a CTF map.";
			}
		}
		else
		{
			if(FStrEq(m_pVoteParameter, "ctf"))
			{
				return "CTF mode is only allowed in a CTF map.";
			}
		}
		
	}
	
	return VOTE_AUTHORIZED;
}

void CVoteMode::Pass()
{
	m_dPrintPassDelay.AddTime(3);
	m_dIntermissionDelay.AddTime(7);
}

void CVoteMode::PrintPass()
{
	char buf[127];
	sprintf(buf, "The vote has passed,\n%s is the new mode.", m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtPass, buf);

}
bool CVoteMode::Execute()
{
	if(m_dPrintPassDelay.TimeUp())
	{
		m_dPrintPassDelay.ClearTime();
		UTIL_HudMessageAll(m_hudtPass, "");
		MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
		MESSAGE_END();
		//ShowGameInfo(NULL, true, 5);
	}

	if(m_dIntermissionDelay.TimeUp())
	{
		char szCommand[128];
		m_dIntermissionDelay.ClearTime();
		*szCommand = NULL;
		//strcpy(gamemode.string,m_pVoteParameter);
		CVAR_SET_STRING("mp_gamemode", m_pVoteParameter);
		//gamemode.value = atof(m_pVoteParameter);
		overwrite_nextmap.value = 1;
		sprintf(szCommand, "changelevel %s\n", STRING(gpGlobals->mapname));
		SERVER_COMMAND(szCommand);
		overwrite_nextmap.value = 0;
		ALERT(at_console, "CVoteMode::Execute()\n");
		return true;
	}
	return false;

}


//==================
// CVoteCommand
//==================

CVoteCommand::CVoteCommand(CBasePlayer* pPlayer, char* pCommand, char* pParameter, char* aux0, char* aux1) : CBaseVote(pPlayer, pCommand, pParameter)
{
	m_bCanDual = true;
	*m_szAux0=NULL;
	*m_szAux1=NULL;
	if(aux0 && *aux0)
		strcpy(m_szAux0, aux0);
	if(aux1 && *aux1)
		strcpy(m_szAux1, aux1);
}

char* CVoteCommand::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}

	if(!m_pVoteName || !*m_pVoteName)
		return "Insufficient vote name";
	if(!m_pVoteParameter || !*m_pVoteParameter)
		return "Insufficient vote parameter ";
	return VOTE_AUTHORIZED;
}

void CVoteCommand::Amend()
{
	*m_szParameters=NULL;
	strncpy(m_szParameters,m_pVoteParameter, 255);

	if(m_szAux0 && *m_szAux0)
	{
		strncat(m_szParameters," ", 255);
		strncat(m_szParameters,m_szAux0, 255);
	}
	if(m_szAux1 && *m_szAux1)
	{
		strncat(m_szParameters," ", 255);
		strncat(m_szParameters,m_szAux1, 255);
	}
}
void CVoteCommand::PrintPass()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has passed\n%s %s will now be executed.", m_pVoteName, m_szParameters);
	UTIL_HudMessageAll(m_hudtPass, cBuf);
}
void CVoteCommand::PrintFail()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has failed\n%s %s will not be executed.", m_pVoteName, m_szParameters);
	UTIL_HudMessageAll(m_hudtFail, cBuf);
}
bool CVoteCommand::Execute()
{
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "%s %s\n", m_pVoteName, m_szParameters);
	SERVER_COMMAND(szCommand);
	return true;
}

char* CVoteCommand::GetParameters()
{
	return m_szParameters;
}
char* CVoteCommand::GetVote()
{
	return m_pVoteName;
}

//==================
// CVoteNullCommand
//==================

CVoteNullCommand::CVoteNullCommand(CBasePlayer* pPlayer, char* pCommand) : CVoteCommand(pPlayer, pCommand, NULL)
{
	m_bCanDual = false;
}
char* CVoteNullCommand::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}
	return VOTE_AUTHORIZED;
}

void CVoteNullCommand::PrintCall()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "Vote called by %s.\n%s.\nOpen the menu to vote.",STRING(m_pCreator->pev->netname ),m_pVoteName);
}
void CVoteNullCommand::PrintPass()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has passed\n%s will now be executed.", m_pVoteName);
	UTIL_HudMessageAll(m_hudtPass, cBuf);
}
void CVoteNullCommand::PrintFail()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has failed\n%s will not be executed.", m_pVoteName);
	UTIL_HudMessageAll(m_hudtFail, cBuf);
}
bool CVoteNullCommand::Execute()
{
	char szCommand[128];
	*szCommand = NULL;
	sprintf(szCommand, "%s\n", m_pVoteName);
	SERVER_COMMAND(szCommand);
	return true;
}

//==================
// CVoteMatchmode
//==================


CVoteMatch::CVoteMatch(CBasePlayer* pPlayer, char* pMatchmode) : CVoteCommand(pPlayer, "matchstart", pMatchmode)
{

}
char* CVoteMatch::Authorize()
{
	if(!g_pGameRules->MatchInProgress() && FStrEq(m_pVoteParameter, "0"))
	{
		return "Match not in progress";
	}
		if(!strcpy(m_pVoteName, "matchstart") && !strcpy(m_pVoteParameter, "0"))

	if(!g_pGameRules->MatchMode())
		return "Matchstart only works in match modes";
	if(!m_pVoteParameter)
	{
		strcpy(m_pVoteParameter, "0");
		return VOTE_AUTHORIZED;
	}
	if(!FStrEq(m_pVoteParameter, "1") && (!FStrEq(m_pVoteParameter, "2")) && (!FStrEq(m_pVoteParameter, "0")))
		return "Insufficient matchstart parameter ";
	
	return VOTE_AUTHORIZED;
}

bool CVoteMatch::Execute()
{
	char szBuf[256];
	*szBuf=NULL;
	sprintf(szBuf, "matchstart %s\n", m_pVoteParameter);
	SERVER_COMMAND(szBuf);
	return true;
};

//==================
// CVoteAddTime
//==================


CVoteAddTime::CVoteAddTime(CBasePlayer* pPlayer, char* pTime)  : CVoteCommand(pPlayer, "addtime", pTime)
{
}

char* CVoteAddTime::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}
	if(m_pVoteParameter)
	{
		for(int i = 0; i < (int)strlen(m_pVoteParameter); i++)
		{
			if((m_pVoteParameter[i] < '0' || m_pVoteParameter[i] > '9') && m_pVoteParameter[i] != '.')
				return "Addtime parameter must be a number"; // not going to be a number
		}
	}
	return VOTE_AUTHORIZED;
}

void CVoteAddTime::Amend()
{
	//undone
	/// if < maxtime admend the time...UNDONE
	if(*m_pVoteParameter==NULL)
		strcpy(m_pVoteParameter, "10"); // change this to a cvar value maxaddtime or something
	CVoteCommand::Amend();
}

void CVoteAddTime::PrintPass()
{
	char szBuf[256];
	*szBuf=NULL;
	sprintf(szBuf, "The vote has passed.\n%s minutes will be added to mp_timelimit.", m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtPass, szBuf);
}
void CVoteAddTime::PrintFail()
{
	char szBuf[256];
	*szBuf=NULL;
	sprintf(szBuf, "The vote has failed.\n%s minutes will not added to mp_timelimit.", m_pVoteParameter);
	UTIL_HudMessageAll(m_hudtFail, szBuf);

}
bool CVoteAddTime::Execute()
{
	char szBuf[256];
	*szBuf=NULL;
	sprintf(szBuf, "addtime %f\n", atof(m_pVoteParameter));
	SERVER_COMMAND(szBuf);
	return true;
}

//==================
// CVoteSpawnPlayer
//==================


CVoteSpawnPlayer::CVoteSpawnPlayer(CBasePlayer* pPlayer, char* pPlayerIndex, char* team) : CVoteCommand(pPlayer, "emit", pPlayerIndex, team)
{}

char* CVoteSpawnPlayer::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}
	CBaseEntity *pEnt = UTIL_PlayerByIndex( atoi(m_pVoteParameter));
	if (pEnt &&	pEnt->IsPlayer() )
		m_pPlayer =  (CBasePlayer *)pEnt;	
	else
		m_pPlayer = m_pCreator;
	if(m_szAux0 && *m_szAux0==NULL)
	{
		strcpy(m_szAux0, m_pPlayer->m_szTeamName);
		//return "You must pick a team name.";
	}
	int result = g_pGameRules->CheckValidTeam(m_pPlayer->m_szTeamName, m_szAux0);

	if(result==-1)
		return "This team is not valid.";
	
	if(!m_pPlayer->m_isSpectator && result == 0)
	{
		if(m_pPlayer == m_pCreator)
			return "You are already on this team.";
		else
			return "This player is already on this team.";
	}
	
	return VOTE_AUTHORIZED;
}

char* CVoteSpawnPlayer::GetParameters()
{
	static char buf[256];
	sprintf(buf, "%s", STRING(m_pPlayer->pev->netname));
	if(!FStrEq(m_szAux0, ""))
	{
		strcat(buf, " ");
		strcat(buf, m_szAux0);		
	}
	return buf;
}
void CVoteSpawnPlayer::PrintCall()
{
	char cBuf[512];
	*cBuf=NULL;
	if(m_szAux0)
		sprintf(cBuf, "Vote called by %s.\nAllow %s to join the game on the %s team.\nOpen the menu to vote.",STRING(m_pCreator->pev->netname ),STRING(m_pPlayer->pev->netname), m_szAux0);
	else
		sprintf(cBuf, "Vote called by %s.\nAllow %s to join the game.\nOpen the menu to vote.",STRING(m_pCreator->pev->netname ),STRING(m_pPlayer->pev->netname));
	UTIL_HudMessageAll(m_hudtVote, cBuf);
}
void CVoteSpawnPlayer::PrintPass()
{
	char szBuf[256];
	*szBuf=NULL;
	if(m_szAux0)
		sprintf(szBuf, "The vote has passed.\n%s is allowed to join the game on the %s team.", STRING(m_pPlayer->pev->netname), m_szAux0);
	else
		sprintf(szBuf, "The vote has passed.\n%s is allowed to join the game.", STRING(m_pPlayer->pev->netname));
	UTIL_HudMessageAll(m_hudtPass, szBuf);
}
void CVoteSpawnPlayer::PrintFail()
{
	char szBuf[256];
	*szBuf=NULL;
	if(m_szAux0)
		sprintf(szBuf, "The vote has failed.\n%s is not allowed to join the game under the %s team.", STRING(m_pPlayer->pev->netname), m_szAux0);
	else
		sprintf(szBuf, "The vote has failed.\n%s is not allowed to join the game.", STRING(m_pPlayer->pev->netname));
	UTIL_HudMessageAll(m_hudtFail, szBuf);
}

bool CVoteSpawnPlayer::Execute()
{
	if(m_szAux0)
		g_pGameRules->ChangePlayerTeam(m_pPlayer,  m_szAux0, TRUE, TRUE);
	g_pGameRules->ResumePlayer(m_pPlayer);
	return true;
}

bool CVoteSpawnPlayer::FailOnDisconnect(CBasePlayer* pPlayer)
{
	if(m_pPlayer == pPlayer)
	{
		return true;
	}
	return CBaseVote::FailOnDisconnect(pPlayer);
}

//==================
// CVoteRemovePlayer
//==================


CVoteRemovePlayer::CVoteRemovePlayer(CBasePlayer* pPlayer, char* pPlayerIndex) : CVoteCommand(pPlayer, "eject", pPlayerIndex)
{}

char* CVoteRemovePlayer::Authorize()
{
	char* base_auth = CBaseVote::Authorize();
	if ( base_auth != VOTE_AUTHORIZED)
	{
		return base_auth;
	}

	CBaseEntity *pEnt = UTIL_PlayerByIndex( atoi(m_pVoteParameter));
	if (pEnt &&	pEnt->IsPlayer() )
	{
		m_pPlayer =  (CBasePlayer *)pEnt;	
	}
	else
	{
		m_pPlayer = m_pCreator;
	}
	return VOTE_AUTHORIZED;
}

void CVoteRemovePlayer::PrintCall()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "Vote called by %s.\nRemove %s from play.\nOpen the menu to vote.",STRING(m_pCreator->pev->netname ),STRING(m_pPlayer->pev->netname));
	UTIL_HudMessageAll(m_hudtVote, cBuf);
}
void CVoteRemovePlayer::PrintPass()
{
	char szBuf[256];
	*szBuf=NULL;
	sprintf(szBuf, "The vote has passed.\n%s is removed from play.", STRING(m_pPlayer->pev->netname));
	UTIL_HudMessageAll(m_hudtPass, szBuf);
}
void CVoteRemovePlayer::PrintFail()
{
	char szBuf[256];
	*szBuf=NULL;
	sprintf(szBuf, "The vote has failed.\n%s is not removed from play.", STRING(m_pPlayer->pev->netname));
	UTIL_HudMessageAll(m_hudtFail, szBuf);
}

bool CVoteRemovePlayer::Execute()
{
	g_pGameRules->RemovePlayer(m_pPlayer);
	ClientPrint( m_pPlayer->pev, HUD_PRINTCONSOLE, "You are now removed from play\n" );
	return true;
}

char* CVoteRemovePlayer::GetParameters()
{
	return (char*)STRING(m_pPlayer->pev->netname);
}

bool CVoteRemovePlayer::FailOnDisconnect(CBasePlayer* pPlayer)
{
	if(m_pPlayer == pPlayer)
	{
		return true;
	}
	return CBaseVote::FailOnDisconnect(pPlayer);
}


//==================
// CVotePause
//==================

CVotePause::CVotePause(CBasePlayer* pPlayer) : CVoteNullCommand(pPlayer, "pausegame")
{
}

void CVotePause::PrintPass()
{
	char szBuf[256];
	*szBuf=NULL;
	if(pausegame.value != 0)
		strcpy(szBuf, "The server will now unpaused."); 
	else
		strcpy(szBuf, "The server will now now paused."); 
	UTIL_HudMessageAll(m_hudtPass, szBuf);
}

bool CVotePause::Execute()
{
	ALERT(at_console, "pause exectued");
	pausegame.value =! pausegame.value;
	SERVER_COMMAND("pausable 1\n");
	SERVER_COMMAND("pause\n");
	SERVER_EXECUTE();

	// hack for pausable cvar_t
	g_pVote->m_dPause.ClearTime();
	g_pVote->m_dPause.AddTime(1);
	return true;
}


CDualVote::CDualVote(CBasePlayer* pPlayer,CBaseVote*  pVoteOne,CBaseVote*  pVoteTwo) : CBaseVote(pPlayer, NULL, NULL)
{	
	m_pVoteOne = pVoteOne;
	m_pVoteTwo = pVoteTwo;	
	VoteOneExecute = false;
	VoteTwoExecute = false;
}

char* CDualVote::Authorize()
{
	if(FStrEq(m_pVoteOne->GetVote(), m_pVoteTwo->GetVote()))
	{
		if(!m_pVoteOne->m_bCanDual)
			return "You may not vote for this twice at once";
	}
	char* pszAuth = m_pVoteOne->Authorize();
	if(!FStrEq(pszAuth, ""))
		return pszAuth;
	else pszAuth = m_pVoteTwo->Authorize();
	if(!FStrEq(pszAuth, ""))
		return pszAuth;
	
		

	return VOTE_AUTHORIZED;
}

void CDualVote::Amend()
{
	m_pVoteOne->Amend();
	m_pVoteTwo->Amend();
}

bool CDualVote::Execute()
{ 
	if(!VoteOneExecute)
		VoteOneExecute = m_pVoteOne->Execute();
	if(!VoteTwoExecute)
		VoteTwoExecute = m_pVoteTwo->Execute();
	return  VoteOneExecute & VoteTwoExecute;
}

void CDualVote::PrintCall()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "Vote called by %s.\n%s %s,\n%s %s\nOpen the menu to vote.",STRING(m_pCreator->pev->netname ),m_pVoteOne->GetVote(), m_pVoteOne->GetParameters(), m_pVoteTwo->GetVote(), m_pVoteTwo->GetParameters());
	UTIL_HudMessageAll(m_hudtVote, cBuf);
}

void CDualVote::PrintFail()
{	
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has failed\n%s %s,\n%s %s are not in effect.", m_pVoteOne->GetVote(), m_pVoteOne->GetParameters(), m_pVoteTwo->GetVote(), m_pVoteTwo->GetParameters());
	UTIL_HudMessageAll(m_hudtFail, cBuf);
}

void CDualVote::PrintPass()
{
	char cBuf[512];
	*cBuf=NULL;
	sprintf(cBuf, "The vote has passed\n%s %s,\n%s %s are now in effect.", m_pVoteOne->GetVote(), m_pVoteOne->GetParameters(), m_pVoteTwo->GetVote(), m_pVoteTwo->GetParameters());
	UTIL_HudMessageAll(m_hudtPass, cBuf);
}

void CDualVote::Pass()
{
	m_pVoteOne->Pass();
	m_pVoteTwo->Pass();
}
void CDualVote::Fail()
{
	m_pVoteOne->Fail();
	m_pVoteTwo->Fail();
}

CDualVote::~CDualVote()
{
	if(m_pVoteOne)
		delete m_pVoteOne;
	if(m_pVoteTwo)
		delete m_pVoteTwo;
}

bool CDualVote::FailOnDisconnect(CBasePlayer* pPlayer)
{
	if(m_pVoteOne->FailOnDisconnect(pPlayer) == false)
	{
		if(m_pVoteTwo->FailOnDisconnect(pPlayer) == false)
			return false;
	}
	return true;
}
