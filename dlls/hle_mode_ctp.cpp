#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"hle_gamemodes.h"
#include	"game.h"
#include	"items.h"
#include	"voice_gamemgr.h"
#include    "hle_vote.h"
#include    "client.h"
#include    "hle_utility.h"
#include	"voice_gamemgr.h"
#include    "hle_mode_ctp.h"
#include	"hle_flag.h"
#include	"player.h"
//#include	<fstream.h>



// gmsg messages
extern int gmsgReset;
extern int gmsgColor;
extern int gmsgTeamInfo;
extern int gmsgScoreInfo;
extern int gmsgGameMode;
extern int gmsgCtpEvent;
extern int gmsgCtpTime;
extern int gmsgCap;
extern int gmsgHudStatus;
extern int gmsgReset;
extern int gmsgFlag;
extern int gmsgRadar;


extern CVoiceGameMgr	g_VoiceGameMgr;
extern CVoteStructure* g_pVote;
//
ofstream CGameModeCTP::g_OutCtpFile;
int CGameModeCTP::g_nCtpFlags[7];

CGameModeCTP::CGameModeCTP() : CGameModeCTF()
{
	PRECACHE_MODEL( "models/flag.mdl" ); // hack
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
	m_nNumber = 0;

	for(int i = 1; i<7;i++)
	{
		m_nScores[i]=0;
		g_nCtpFlags[i]=0;
		m_nTeamCount[i]=0;
	}

	m_dScore.AddTime(1);
	m_bInitFlags = false;

}


/*
CGameModeCTP::~CGameModeCTP() 
{	
}
*/


void CGameModeCTP::InitHUD( CBasePlayer *pPlayer )
{

	CHalfLifeTeamplay::InitHUD( pPlayer );

	CBaseEntity* pEntity = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
	while (pEntity)
	{
		CFlagStatic* pFlag = (CFlagStatic*)pEntity;

		MESSAGE_BEGIN( MSG_ONE, gmsgFlag, NULL, pPlayer->edict() );
			WRITE_SHORT(pFlag->entindex());
			WRITE_SHORT(0);					// flag holder (player)
			WRITE_SHORT(pFlag->pev->origin[0]);
			WRITE_SHORT(pFlag->pev->origin[1]);
			WRITE_BYTE(pFlag->m_nFlagColor);
			WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
		MESSAGE_END();
						
        pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
	}
}

BOOL CGameModeCTP::ClientCommand(CBasePlayer *pPlayer, const char *pcmd )
{
	if ( g_VoiceGameMgr.ClientCommand( pPlayer, pcmd ) )
		return TRUE;

	else if(FStrEq(pcmd, "add_flag"))
	{
		PlaceFlag(pPlayer);
		return TRUE;
	}
	return CHalfLifeTeamplay::ClientCommand(pPlayer, pcmd);
}

void CGameModeCTP::PlaceFlag(CBasePlayer *pPlayer) // client command: "add_flag"
{
	
	if(g_OutCtpFile.is_open()) 
	{
		g_OutCtpFile
			<<  pPlayer->pev->origin[0]  << " " 
			<< pPlayer->pev->origin[1] << " " 
			<< pPlayer->pev->origin[2] << " " 
			<< pPlayer->pev->angles[0]  << " " 
			<< pPlayer->pev->angles[1] << " " 
			<< pPlayer->pev->angles[2] << "\n";

		CBaseEntity* pEnt = CBaseEntity::Create( "info_flag_ctp", pPlayer->pev->origin , pPlayer->pev->angles, pPlayer->edict() );
		pEnt->pev->origin[2]+=50;
		pEnt->pev->angles.x = 0;
	}
	else
	{
		ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "You must first open a file \"create_ctp_file\" (This is a server command only)\n");
	}
}

void CGameModeCTP::SummonFlags()
{
	char buf[64];
	sprintf(buf, "hle/modes/ctp/%s",STRING(gpGlobals->mapname) );
	
	m_InCtpFile.open(buf/*ios::nocreate*/);
	if(m_InCtpFile.is_open()) 
	{
		m_bHasCtpFile = true;
		char line[255];		
		while(!m_InCtpFile.eof())
		{
			m_InCtpFile.getline(line,255 );
			char* command = strtok(line, " ");

			if(!command)
			{
				continue;
			}
//			415 771 5519
			vec3_t origin;
			vec3_t angle;
			origin[0] = atof(command);
			origin[1] = atof(strtok(NULL, " "));
			origin[2] = atof(strtok(NULL, " "));

			angle[0] = atof(strtok(NULL, " "));
			angle[1] = atof(strtok(NULL, " "));
			angle[2] = atof(strtok(NULL, " "));

			CBaseEntity *pEnt;

			//UTIL_MakeVectors( pev->v_angle );
			pEnt = CBaseEntity::Create( "info_flag_ctp", origin ,angle );
			pEnt->pev->origin[2]+=50;
			//UTIL_SetOrigin( pEnt->pev, pEnt->pev->origin + gpGlobals->v_up * 20);

			//pEnt->pev->angles = UTIL_VecToAngles (pEnt->pev->velocity);
			pEnt->pev->angles.x = 0;
			
		}
	}
	else
	{
		m_bHasCtpFile = false;
	//	UTIL_HudMessageAll(m_hudtInfo, "No \"*.ctp\" file found for this level\nThis game mode will not function correctly\n" );
	}
	m_InCtpFile.close();
}

void CGameModeCTP::UpKeep(CBasePlayer* pPlayer)
{
	int nTeam = GetTeamIndex( pPlayer )+1;	
	if(pPlayer->IsAlive())
	{
		float hev = ((float)g_nCtpFlags[nTeam]/2000.0f)*regeneration.value;
		float health = hev;
		if(pPlayer->pev->armorvalue < pPlayer->max_hev)
		{
			pPlayer->pev->armorvalue += hev;
			if(pPlayer->pev->armorvalue > pPlayer->max_hev)
				pPlayer->pev->armorvalue = pPlayer->max_hev;
		}	

		if(pPlayer->pev->health < pPlayer->pev->max_health)
		{
			pPlayer->pev->health += health;
			if(pPlayer->pev->health > pPlayer->pev->max_health)
				pPlayer->pev->health = pPlayer->pev->max_health;
		}
	}
}


void CGameModeCTP::Think()
{
	if(!m_bInitFlags)
	{
		SummonFlags();
		m_bInitFlags = true;
	}

	if(m_MatchStart.isMatch())
	{
		if(m_MatchStart.m_dDelay.TimeUp())
		{
			m_MatchStart.m_dDelay.AddTime(1);// add 1 second for the next count
			SendCountDown(MAX_SPRITE_NUMBER - m_nNumber);
			m_nNumber++;

			if(m_nNumber > MAX_SPRITE_NUMBER)
			{

				MESSAGE_BEGIN( MSG_ALL, gmsgCtpEvent);
					WRITE_BYTE(0); 
					WRITE_BYTE(0);
					WRITE_BYTE(0);
					WRITE_BYTE(0);
					WRITE_BYTE(0);
					WRITE_BYTE(0);
					WRITE_BYTE(0);
				MESSAGE_END();
				// reset the scores
				for(int i = 1; i<7;i++)
				{
					m_nScores[i]=0;
					g_nCtpFlags[i]=0;
				}
				//reset the scores

				// send a score message here

				m_MatchStart.startMatch();
				m_nNumber = 0;
				ResetFlags();
			}
		}
	}
	if(m_dScore.TimeUp()) // server side scoring (fake clientside scoring will be impleneted for each client)
	{
		for(int i = 1; i<7;i++)
		{
			m_nScores[i] += g_nCtpFlags[i];
		}
		m_dScore.AddTime(1);
	}
	for(int k = 1; k < 7; k++)
	{
		if(m_nScores[k] >= winlimit.value && winlimit.value != 0)
		{
			
			MESSAGE_BEGIN( MSG_ALL, gmsgCtpEvent); // stop counting
				WRITE_BYTE(-1);  
				WRITE_BYTE(-1); 
				WRITE_BYTE(-1); 
				WRITE_BYTE(-1);
				WRITE_BYTE(-1);
				WRITE_BYTE(-1);
				WRITE_BYTE(-1);
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_ALL, gmsgCtpTime, NULL);  
				WRITE_SHORT( winlimit.value );		// this fixes the time so the limit is not exceeded in the client
				WRITE_BYTE( k );
			MESSAGE_END();

			GoToIntermission();
			break;

		}
	}
	CHalfLifeMultiplay::Think();
}



void CGameModeCTP::SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget)
{
	CHalfLifeTeamplay::SetSpectatorHud(pPlayer, pTarget);

	int nTeam = g_pGameRules->GetTeamIndex( pTarget )+1;

	MESSAGE_BEGIN( MSG_ONE, gmsgCtpEvent, NULL, pPlayer->edict() );
		WRITE_BYTE(CGameModeCTP::g_nCtpFlags[BLUE]);  
		WRITE_BYTE(CGameModeCTP::g_nCtpFlags[RED]); 
		WRITE_BYTE(CGameModeCTP::g_nCtpFlags[YELLOW]); 
		WRITE_BYTE(CGameModeCTP::g_nCtpFlags[GREEN]);
		WRITE_BYTE(CGameModeCTP::g_nCtpFlags[PURPLE]);
		WRITE_BYTE(CGameModeCTP::g_nCtpFlags[ORANGE]);
		WRITE_BYTE(nTeam);
	MESSAGE_END();
}
/*
edict_t* CGameModeCTP::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	return CHalfLifeMultiplay::GetPlayerSpawnSpot(pPlayer);
}*/

void CGameModeCTP :: ClientDisconnected( edict_t *pClient )
{
	if ( pClient )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );

		if ( pPlayer )
		{
			CleanUpTeam(g_pGameRules->GetTeamIndex( pPlayer )+1  );
		}
	}
	 
	CHalfLifeTeamplay:: ClientDisconnected( pClient );
}
BOOL CGameModeCTP::CanStartObserver(CBasePlayer *pPlayer)
{
	CleanUpTeam(g_pGameRules->GetTeamIndex( pPlayer)+1);
	if(m_MatchStart.isMatch())
	{
		pPlayer->m_bInMatch = false;
		m_MatchStart.m_Players.RemovePlayer(pPlayer->edict());
	}
	

	return TRUE;
}

void CGameModeCTP::CleanUpTeam(int team)
{
	if(team <= 0 || team > 6)
		return;
	
	bool playersleft = false;

	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *plr = (CBasePlayer *)pEnt;
	
			int nTeam = g_pGameRules->GetTeamIndex( plr)+1;
					
			if(nTeam == team) // are there any players left of this players old team?
			{
				playersleft = true;  
				break;
			}
		}
	}
	if(!playersleft)
	{
		CGameModeCTP::g_nCtpFlags[team]=0;  // clear out this teams score

		// clear all this teams flags
		CBaseEntity* pEntity = NULL;
		pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
		while (pEntity)
		{
			if(((CFlagStatic*)pEntity)->m_nFlagColor == team)
			{
				((CFlagStatic*)pEntity)->Reset();
				
				MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
					WRITE_SHORT(pEntity->entindex());	
					WRITE_SHORT(0);					// flag holder (player)
					WRITE_SHORT(pEntity->pev->origin[0]);
					WRITE_SHORT(pEntity->pev->origin[1]);
					WRITE_BYTE(0);
					WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
				MESSAGE_END();
			}
			pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctp");
		}
	}
}
int CGameModeCTP::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
	int oldteam = g_pGameRules->GetTeamIndex( pPlayer)+1;
	bool playersleft = false;

	int result = CHalfLifeTeamplay::ChangePlayerTeam( pPlayer, pTeamName, bKill, bGib );

	CleanUpTeam(oldteam);

	return result;

}

void CGameModeCTP::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	int clientIndex = pPlayer->entindex();
	char *mdls = g_engfuncs.pfnInfoKeyValue( infobuffer, "model" );
	char *team = g_engfuncs.pfnInfoKeyValue( infobuffer, "team" );

	if(m_MatchStart.isMatch())	
	{
		if(pPlayer->m_isSpectator)
			return;

		if(stricmp(mdls, pPlayer->m_szModelName))
		{
			if(CheckValidModel(pPlayer->m_szModelName, pPlayer->m_szModelName)<0)
			{
				if(CheckValidModel(mdls, mdls)>=0)
				{
					ChangePlayerModel(pPlayer, mdls, false);	
				}
				else
				{
					SetDefaultModel(pPlayer);
				}
			}
			else
			{
				g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );
				ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, "You may not change your model during a match\n" );
			}
		}
		return;
	}

	if(CheckValidModel(pPlayer->m_szModelName, mdls) ==-1)
	{
		if(CheckValidModel(pPlayer->m_szModelName, pPlayer->m_szModelName) ==-1)
			SetDefaultModel(pPlayer);

		else
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );
	}
	else
		strcpy(pPlayer->m_szModelName, mdls);
	
	
	if(stricmp(pPlayer->TeamID(), team))
		ChangePlayerTeam( pPlayer, team, TRUE, TRUE );

	ResetTeamColors(pPlayer, GetTeamIndex(pPlayer) +1);
}

void CGameModeCTP::CapturedFlag(CBasePlayer *pPlayer)
{
	SendScoreInfo(pPlayer);
}



void CGameModeCTP::PlayerThink( CBasePlayer *pPlayer )
{
	// removed for now
	//if(pPlayer->m_dUpkeep.TimeUp())
	//{
	//	UpKeep(pPlayer);
	//	pPlayer->m_dUpkeep.AddTime(0.25);
	//}
	CGameModeCTF::PlayerThink(pPlayer);
}
void CGameModeCTP::PlayerSpawn(CBasePlayer *pPlayer)
{	int i;
	if(!m_bHasCtpFile)
		UTIL_HudMessage(pPlayer, m_hudtInfo, "No \"ctp\" file found for this level\nThis game mode will not function correctly\n" );

	for(i = 1; i < 7; i++)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgCtpTime, NULL, pPlayer->edict() );  
			WRITE_SHORT( m_nScores[i] );
			WRITE_BYTE( i );
		MESSAGE_END();
	}

	if(!pPlayer->m_isSpectator)
	{
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
			if ( pEnt && pEnt->IsPlayer( ) )
			{
				CBasePlayer *plr = (CBasePlayer *)pEnt;
				if(!plr->m_isSpectator)
				{
					int nTeam = g_pGameRules->GetTeamIndex( plr)+1;
				
					MESSAGE_BEGIN( MSG_ONE, gmsgCtpEvent, NULL, plr->edict() );
					WRITE_BYTE(CGameModeCTP::g_nCtpFlags[BLUE]);  
					WRITE_BYTE(CGameModeCTP::g_nCtpFlags[RED]); 
					WRITE_BYTE(CGameModeCTP::g_nCtpFlags[GREEN]);
					WRITE_BYTE(CGameModeCTP::g_nCtpFlags[YELLOW]); 
					WRITE_BYTE(CGameModeCTP::g_nCtpFlags[PURPLE]);
					WRITE_BYTE(CGameModeCTP::g_nCtpFlags[ORANGE]);
					WRITE_BYTE(nTeam);
					MESSAGE_END();

					CEntityNode* pClient = pPlayer->m_lObservers.head();
					while(pClient)
					{
						MESSAGE_BEGIN( MSG_ONE, gmsgCtpEvent, NULL, plr->edict() );
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[BLUE]);  
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[RED]); 
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[GREEN]);
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[YELLOW]); 
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[PURPLE]);
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[ORANGE]);
						WRITE_BYTE(nTeam);
						MESSAGE_END();
						pClient = pClient->m_pNext;
					}
				}
			}
		}
	
		/*int max_capacity = 100+ (g_nCtpFlags[GetTeamIndex( pPlayer)+1] * 10);
		if(max_capacity > 200)
			max_capacity = 200;
		pPlayer->max_hev =  max_capacity;
		pPlayer->pev->max_health = max_capacity;
		
		//pPlayer->m_dUpkeep.AddTime(.25);

		MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pPlayer->edict() );  
			WRITE_SHORT(max_capacity-100);
			WRITE_SHORT(max_capacity-100);
		MESSAGE_END();
	

		CEntityNode* pClient = pPlayer->m_lObservers.head();
		while(pClient)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pClient->m_pEntity );
				WRITE_SHORT(max_capacity-100);
				WRITE_SHORT(max_capacity-100);
			MESSAGE_END();
			pClient = pClient->m_pNext;
		}*/
	}

	CGameModeCTF::PlayerSpawn(pPlayer);	
}


void CGameModeCTP::SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever)
{
	if(pReciever==NULL)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iSaves );
			WRITE_SHORT( 0 );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pReciever->edict() );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
 			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( pSender->m_iSaves );
			WRITE_SHORT( 0 );
		MESSAGE_END();
	}
}