/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// teamplay_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"hle_gamemodes.h"

#include	"game.h"
#include	"hle_flag.h"
#include	"hle_vote.h"

#define SAME_TEAM 0
#define VALID_TEAM 1
#define INVALID_TEAM -1
#define AUTO_ASSIGN 2

edict_t *EntSelectTeamSpawnPoint( CBaseEntity *pPlayer );
extern COutput debug_file;

char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
int team_scores[MAX_TEAMS];
int	team_order[MAX_TEAMS];
int	color_order[MAX_TEAMS];

int num_teams = 0;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern float g_fTimeOffset;
extern CVoteStructure* g_pVote;

extern int g_nMaxTeams;


extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;
extern int gmsgTeamNames;

extern int gmsgHudStatus;
extern int gmsgScoreInfo;
extern int gmsgColor;
extern int gmsgReset;
extern int gmsgIconInfo;
extern int gmsgRadar;
extern int gmsgLocation;
extern int gmsgTeamOrder;

CHalfLifeTeamplay :: CHalfLifeTeamplay()
{
	m_nNumber = 0;
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	memset( team_names, 0, sizeof(team_names) );
	memset( team_scores, 0, sizeof(team_scores) );
	num_teams = 0;
	if(g_nMaxTeams==0)
		m_nMaxTeams = maxteams.value;
	else
		m_nMaxTeams = g_nMaxTeams;
	if(m_nMaxTeams<2)
		m_nMaxTeams = 2;
	if(m_nMaxTeams > 6)
		m_nMaxTeams = 6;
	
	
	m_szTeamList[0] = NULL;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	
	// hle this is hardcoded!
	for(int i = 0; i < MAX_TEAMS; i++)
	{
		for(int j = 0; j < MAX_TEAMS; j++)
		{
			if(team_order[j]==i)
			{
				color_order[i]=j;
				switch(j)
				{
					case 0:
						strcat(m_szTeamList, "Blue");
						strcat(m_szTeamList, ";");
						break;
					case 1:
						strcat(m_szTeamList, "Red");
						strcat(m_szTeamList, ";");
						break;
					case 2:
						strcat(m_szTeamList, "Green");
						strcat(m_szTeamList, ";");
						break;
					case 3:
						strcat(m_szTeamList, "Yellow");
						strcat(m_szTeamList, ";");
						break;
					case 4:
						strcat(m_szTeamList, "Purple");
						strcat(m_szTeamList, ";");
						break;
					case 5:
						strcat(m_szTeamList, "Orange");
						strcat(m_szTeamList, ";");
						break;
				}
				//team_order[j]=MAX_TEAMS;
				j = MAX_TEAMS;

			}
		}
	
	}
	//strncpy( m_szTeamList, "Blue;Red;Green;Yellow;Purple;Orange", TEAMPLAY_TEAMLISTLENGTH );

	edict_t *pWorld = INDEXENT(0);
	if ( pWorld && pWorld->v.team )
	{
		if ( teamoverride.value )
		{
			const char *pTeamList = STRING(pWorld->v.team);
			if ( pTeamList && strlen(pTeamList) )
			{
				strncpy( m_szTeamList, pTeamList, TEAMPLAY_TEAMLISTLENGTH );
			}
		}
	}
	// Has the server set teams
	if ( strlen( m_szTeamList ) )
		m_teamLimit = TRUE;
	else
		m_teamLimit = FALSE;

	RecountTeams();

#ifdef HLE_DEBUG // DEBUG FILE DUMP ==============
		
		debug_file.OpenFile();
		debug_file << "CHalfLifeTeamplay :: CHalfLifeTeamplay()\n";
		debug_file.CloseFile();

#endif	// END FILE DUMP =========================
}

extern cvar_t timeleft, fragsleft;

#include "voice_gamemgr.h"
extern CVoiceGameMgr	g_VoiceGameMgr;

void CHalfLifeTeamplay :: Think ( void )
{
	if(!m_bInitSpawns)
	{
		g_pGameRules->InitSpawns();
		m_bInitSpawns = true;
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
				m_MatchStart.startMatch();
				m_nNumber = 0;
			}
		}
	}

	if(m_dRoundTimer.TimeUp())			
	{
		m_dRoundTimer.ClearTime();
		MatchStart(matchstart.value);
	}

	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	g_VoiceGameMgr.Update(gpGlobals->frametime);

	if ( g_fGameOver )   // someone else quit the game already
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	//float flTimeLimit = timelimit.value * 60;
	float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;
	
	time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);

	if ( flTimeLimit != 0 && gpGlobals->time - g_fTimeOffset >= flTimeLimit )
	{
		GoToIntermission();
		return;
	}

	float flFragLimit = fraglimit.value;
	if ( flFragLimit )
	{
		int bestfrags = 9999;
		int remain;

		// check if any team is over the frag limit
		for ( int i = 0; i < num_teams; i++ )
		{
			if ( team_scores[i] >= flFragLimit )
			{
				GoToIntermission();
				return;
			}

			remain = flFragLimit - team_scores[i];
			if ( remain < bestfrags )
			{
				bestfrags = remain;
			}
		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if ( frags_remaining != last_frags )
	{
		g_engfuncs.pfnCvar_DirectSet( &fragsleft, UTIL_VarArgs( "%i", frags_remaining ) );
	}

	// Updates once per second
	if ( timeleft.value != last_time )
	{
		g_engfuncs.pfnCvar_DirectSet( &timeleft, UTIL_VarArgs( "%i", time_remaining ) );
	}

	last_frags = frags_remaining;
	last_time  = time_remaining;
}

//=========================================================
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
BOOL CHalfLifeTeamplay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if(g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
		return TRUE;

	else if(FStrEq(pcmd, "changeteam"))
	{
		if(!m_MatchStart.isMatch())
		{
			int changed = 0;

			if(CMD_ARGC()==2)
				changed=ChangePlayerTeam(pPlayer, CMD_ARGV(1), TRUE, TRUE);
			else	
				changed=ChangePlayerTeam(pPlayer, "auto", TRUE, TRUE);

			if(changed || pPlayer->m_isSpectator)
			{
				ResumePlayer(pPlayer);
			}
		}
		else
		{
			if(CMD_ARGC()==2)
			{
				char szVoteString[16];
				//sprintf(szVoteString, "changeteam %s", CMD_ARGS());
				sprintf(szVoteString, "emit %i %s", pPlayer->entindex(), CMD_ARGS());
				g_pVote->CallVote(pPlayer, szVoteString);	
			}
		}
		return TRUE;
	}

	return FALSE;
}


void CHalfLifeTeamplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_STRING( gamemode.string );  
		WRITE_STRING( HLE_VERSION );
		WRITE_STRING( (CVAR_GET_POINTER("hostname"))->string );
		WRITE_BYTE(1);
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ONE, gmsgReset, NULL, pPlayer->edict() );
	MESSAGE_END();

}
/*
edict_t *CHalfLifeTeamplay :: GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	// change this to match team spawning
	edict_t *pentSpawnSpot = EntSelectTeamSpawnPoint( pPlayer );

	pPlayer->pev->origin = VARS(pentSpawnSpot)->origin + Vector(0,0,1);
	pPlayer->pev->v_angle  = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = VARS(pentSpawnSpot)->angles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = TRUE;
	
	return pentSpawnSpot;
}*/

void CHalfLifeTeamplay::SetDefaultTeam(CBasePlayer* pPlayer)
{
	char* team = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team" );

	if(CheckValidTeam(pPlayer->m_szTeamName, team) == INVALID_TEAM)
	{
		if(m_MatchStart.isMatch())
		{
			if(pPlayer->m_bInMatch)
			{
				g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict()), "team", pPlayer->m_szTeamName );
			}
		}
		else
		{
			
			pPlayer->ShowVGUIMenu(2);
			*pPlayer->m_szTeamName=NULL; // reset server team since client has been changed
			pPlayer->m_bUnSpawn = true;
		}
	}
}

const char *CHalfLifeTeamplay::SetDefaultPlayerTeam( CBasePlayer *pPlayer )
{
	RecountTeams();
	SetDefaultTeam(pPlayer);		
	CBaseEntity* pEntity = NULL;
	return pPlayer->TeamID();
}


//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD( CBasePlayer *pPlayer )
{
	int i;
	pPlayer->m_dRadar.AddTime(0.1);
	
	*pPlayer->m_szTeamName  = NULL;
	CHalfLifeMultiplay::InitHUD( pPlayer );
	
	if(!pPlayer->m_bInMatch)
	{
		if(!pPlayer->m_isSpectator)
		{
			g_pGameRules->GetPlayerSpawnSpot( pPlayer );
			pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, 3);
		}
		pPlayer->ShowVGUIMenu(2); // show team menu	
	}

	MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 0 );
		WRITE_BYTE(100);
		WRITE_BYTE( 8 );
	MESSAGE_END();

	
	// Send down the team names
	MESSAGE_BEGIN( MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict() );  
		WRITE_BYTE( num_teams );
		for ( i = 0; i < num_teams; i++ )
		{
			WRITE_STRING( team_names[ i ] );
		}
	MESSAGE_END();

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );
		if ( plr && IsValidTeam( plr->TeamID() ) )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict() );
				WRITE_BYTE( plr->entindex() );
				WRITE_STRING( plr->TeamID() );
			MESSAGE_END();
		}
	}

	for(i = 0; i < MAX_TEAMS; i++)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgTeamOrder, NULL, pPlayer->edict());
		WRITE_BYTE (i); 
		WRITE_BYTE (team_order[i]);	
		MESSAGE_END();
	}

//	RecountTeams();
		
}

int CHalfLifeTeamplay::CheckValidTeam(char* pCurrentTeam, char* pNewTeam)
{
	if(FStrEq(pNewTeam, "auto"))
	{
		return AUTO_ASSIGN;
	}
	for(int i = 0; i < num_teams; i++)
	{
		if(!stricmp(pNewTeam, team_names[i]))
		{
			if(!stricmp(pCurrentTeam, pNewTeam))
				return SAME_TEAM;
			return VALID_TEAM;
		}
	}
	return INVALID_TEAM;
}

int CHalfLifeTeamplay::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
//	char text[512];
	int clientIndex = pPlayer->entindex();
	int mTeamResult = CheckValidTeam(pPlayer->m_szTeamName, (char*)pTeamName);

	//if(mTeamResult==VALID_TEAM)
	if(mTeamResult==VALID_TEAM)
	{
		
		int damageFlags = DMG_GENERIC;		
		damageFlags |= DMG_ALWAYSGIB;
		int OldTakeDamage = pPlayer->pev->takedamage;

		// kill the player,  remove a death,  and let them start on the new team
		if(bKill)
		{
			pPlayer->pev->takedamage = DAMAGE_YES;
			m_DisableDeathMessages = TRUE;
			m_DisableDeathPenalty = TRUE;
			entvars_t *pevWorld = VARS( INDEXENT(0) );
			pPlayer->TakeDamage( pevWorld, pevWorld, 900, damageFlags );
			m_DisableDeathMessages = FALSE;
			m_DisableDeathPenalty = FALSE;
		}

		pPlayer->pev->takedamage = OldTakeDamage;

		// copy out the team name from the team
		strncpy( pPlayer->m_szTeamName, pTeamName, 16 );
		if(pPlayer->m_szTeamName[0] > 'Z')
		{
			pPlayer->m_szTeamName[0] -= 0x20;
		}
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );		

		ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, UTIL_VarArgs( "\"%s\" is your new CTF team.\n", pPlayer->m_szTeamName ) );
			
		// notify everyone's HUD of the team change
		MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
			WRITE_BYTE( clientIndex );
			WRITE_STRING( pPlayer->m_szTeamName );
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_ALL, gmsgLocation );
			WRITE_SHORT( pPlayer->entindex() );
			WRITE_BYTE(0);	// 1 = add player to list, 0 = remove player from list
			WRITE_SHORT(0);
			WRITE_SHORT(0);
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_ONE, gmsgLocation, NULL, pPlayer->edict() );
		WRITE_SHORT( 0 );
		WRITE_BYTE( 2 );	// 1 = add player to list, 0 = remove player from list, 2 = cleaer list.
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		MESSAGE_END();

		SendScoreInfo(pPlayer);

		ResetTeamColors(pPlayer, GetTeamIndex(pPlayer) +1);

			char text[1024];
		*text = NULL;
		sprintf( text, "* %s has changed to team \'%s\'\n", STRING(pPlayer->pev->netname), pPlayer->m_szTeamName );
		UTIL_SayTextAll( text, pPlayer );

		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n", 
				STRING(pPlayer->pev->netname),
				GETPLAYERUSERID( pPlayer->edict() ),
				GETPLAYERAUTHID( pPlayer->edict() ),
				pPlayer->m_szTeamName,
				pPlayer->m_szTeamName );

		RecountTeams();
		return 1;
	}
	else if(mTeamResult == INVALID_TEAM)
	{
		// this needs to be fixed.
		if(CheckValidTeam(pPlayer->m_szTeamName, pPlayer->m_szTeamName) == VALID_TEAM)
		{
			// set the client info back to what it was
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );
			return 0;
		}
		else 
		{
			SetDefaultTeam(pPlayer);
			//strcpy(pPlayer->m_szTeamName, pTeamName); // default team
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );
			return 1;
		}
	}
	else if(mTeamResult == AUTO_ASSIGN)
	{
		ChangePlayerTeam(pPlayer, team_names[TeamWithFewestPlayers()], false, false); // test this
		return 1;
	}
	return 0;

}


void CHalfLifeTeamplay::ResetTeamColors(CBasePlayer *pPlayer, int nTeam )
{
	int clientIndex = pPlayer->entindex();
	switch(nTeam)
	{
	case RED:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "0" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "0" );
			pPlayer->m_nGaussColor = 2;
			
			//MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			//WRITE_BYTE( 2 );
			//MESSAGE_END();

 			MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 0 );
			WRITE_BYTE(50);
			WRITE_BYTE( 2 );
			MESSAGE_END();
			break;
			
	case BLUE:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "150" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "150" );
			pPlayer->m_nGaussColor = 5;
			
			
			//MESSAGE_BEGIN( MSG_ONE, gmsgGaColor, NULL, pPlayer->edict() );  
			//WRITE_BYTE( 5 );
			//MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 145 );
			WRITE_BYTE( 50 );
			WRITE_BYTE( 5 );
			MESSAGE_END();
			break;
		
	case YELLOW:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "40" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "40" );
			pPlayer->m_nGaussColor = 0;
			
			
			//MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			//WRITE_BYTE( 0 );
			//MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 40 );
			WRITE_BYTE( 50 );
			WRITE_BYTE( 0 );
			MESSAGE_END();
			break;
		
	case GREEN:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "80" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "80" );
			pPlayer->m_nGaussColor = 7;
			
			
			//MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			//WRITE_BYTE( 7 );
			//MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 80 );
			WRITE_BYTE( 50 );
			WRITE_BYTE( 7 );
			MESSAGE_END();
			break;
		case PURPLE:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "215" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "215" );
			pPlayer->m_nGaussColor = 4;
			
			
			/*MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 4 );
			MESSAGE_END();*/
			
			MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 215 );
			WRITE_BYTE( 50 );
			WRITE_BYTE( 4 );
			MESSAGE_END();
			break;
		case ORANGE:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "20" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "20" );
			pPlayer->m_nGaussColor = 1;
			
			
			/*MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 1 );
			MESSAGE_END();*/
			
			MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 20 );
			WRITE_BYTE( 50 );
			WRITE_BYTE( 1 );
			MESSAGE_END();
			break;
	}
}

//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	//char text[1024];

	// prevent skin/color/model changes

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
			g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );
	}
	else
		strcpy(pPlayer->m_szModelName, mdls);

	if(pPlayer->m_isSpectator)
	{
		g_pGameRules->StartObserver(pPlayer);
//		pPlayer->pev->team = 0;
		/*
		MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
		WRITE_STRING( "" );
		MESSAGE_END();*/
	}
}

extern int gmsgDeathMsg;

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeTeamplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	if ( m_DisableDeathMessages )
		return;
	
	if ( pVictim && pKiller && pKiller->flags & FL_CLIENT )
	{
		CBasePlayer *pk = (CBasePlayer*) CBaseEntity::Instance( pKiller );

		if ( pk )
		{
			if ( (pk != pVictim) && (PlayerRelationship( pVictim, pk ) == GR_TEAMMATE) )
			{
				MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
					WRITE_BYTE( ENTINDEX(ENT(pKiller)) );		// the killer
					WRITE_BYTE( ENTINDEX(pVictim->edict()) );	// the victim
					WRITE_STRING( "teammate" );		// flag this as a teammate kill
				MESSAGE_END();
				return;
			}
		}
	}

	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

//=========================================================
//=========================================================
void CHalfLifeTeamplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
		RecountTeams();
	}
}


//=========================================================
// IsTeamplay
//=========================================================
BOOL CHalfLifeTeamplay::IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeTeamplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	if ( pAttacker && PlayerRelationship( pPlayer, pAttacker ) == GR_TEAMMATE )
	{
		// my teammate hit me.
		if ( (friendlyfire.value == 0) && (pAttacker != pPlayer) )
		{
			// friendly fire is off, and this hit came from someone other than myself,  then don't get hurt
			return FALSE;
		}
	}

	return CHalfLifeMultiplay::FPlayerCanTakeDamage( pPlayer, pAttacker );
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeTeamplay::ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
{
	// always autoaim, unless target is a teammate
	CBaseEntity *pTgt = CBaseEntity::Instance( target );
	if ( pTgt && pTgt->IsPlayer() )
	{
		if ( PlayerRelationship( pPlayer, pTgt ) == GR_TEAMMATE )
			return FALSE; // don't autoaim at teammates
	}

	return CHalfLifeMultiplay::ShouldAutoAim( pPlayer, target );
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	return 1;
}

//=========================================================
//=========================================================
const char *CHalfLifeTeamplay::GetTeamID( CBaseEntity *pEntity )
{
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";

	// return their team name
	return pEntity->TeamID();
}

int CHalfLifeTeamplay::GetTeamIndex( CBasePlayer* pPlayer )
{
	if(pPlayer->m_isSpectator)
		return -1;
	const char* pTeamName = pPlayer->m_szTeamName;
	if ( pTeamName && *pTeamName != 0 )
	{
		for(int i = 0; i < num_teams; i++) 
		{
			if(!stricmp(pTeamName, team_names[i]))
				return color_order[i];
		}
	}	
	return -1;	// No match
}
int CHalfLifeTeamplay::GetTeamIndex2( const char* pTeamName )
{
	if ( pTeamName && *pTeamName != 0 )
	{
		for(int i = 0; i < num_teams; i++) 
		{
			if(!stricmp(pTeamName, team_names[i]))
				return color_order[i];
		}
	}	
	return -1;	// No match
}

const char *CHalfLifeTeamplay::GetIndexedTeamName( int teamIndex )
{
	if ( teamIndex < 0 || teamIndex >= num_teams )
		return "";

	return team_names[ teamIndex ];
}


BOOL CHalfLifeTeamplay::IsValidTeam( const char *pTeamName ) 
{
	if ( !m_teamLimit )	// Any team is valid if the teamlist isn't set
		return TRUE;

	return ( GetTeamIndex2( pTeamName ) != -1 ) ? TRUE : FALSE;
}

int CHalfLifeTeamplay::TeamWithFewestPlayers( void )
{
	int i;
	int minPlayers = MAX_TEAMS;
	int teamCount[ MAX_TEAMS ];
	char *pTeamName = NULL;
	int nTeam = 0;

	memset( teamCount, 0, MAX_TEAMS * sizeof(int) );
	
	// loop through all clients, count number of players on each team

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr && plr->IsPlayer())
		{
			
			int team = GetTeamIndex( (CBasePlayer*)plr );
			if ( team >= 0 )
				teamCount[team] ++;
		}
	}

	// Find team with least players
	for ( i = 0; i < num_teams; i++ )
	{
		if ( teamCount[i] < minPlayers )
		{
			minPlayers = teamCount[i];
			pTeamName = team_names[i];
			nTeam = i;
		}
	}

	//return pTeamName;
	return nTeam;
}


//=========================================================
//=========================================================
void CHalfLifeTeamplay::RecountTeams( bool bResendInfo )
{
	char	*pName;
	char	teamlist[TEAMPLAY_TEAMLISTLENGTH];
	*teamlist=NULL;

	// loop through all teams, recounting everything
	num_teams = 0;

	// Copy all of the teams from the teamlist
	// make a copy because strtok is destructive
	strcpy( teamlist, m_szTeamList );
	pName = teamlist;
	pName = strtok( pName, ";" );
	while ( pName != NULL && *pName && num_teams < m_nMaxTeams)
	{
		if ( GetTeamIndex2( pName ) < 0 )
		{
			strcpy( team_names[num_teams], pName );
			num_teams++;
		}
		pName = strtok( NULL, ";" );
	}

	if ( num_teams < 2 )
	{
		num_teams = 0;
		m_teamLimit = FALSE;
	}

	// Sanity check
	memset( team_scores, 0, sizeof(team_scores) );

	// loop through all clients
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			const char *pTeamName = plr->TeamID();
			// try add to existing team
			int tm = GetTeamIndex2( pTeamName );
			
			if ( tm < 0 ) // no team match found
			{ 
				if ( !m_teamLimit )
				{
					// add to new team
					tm = num_teams;
					num_teams++;
					team_scores[tm] = 0;
					strncpy( team_names[tm], pTeamName, MAX_TEAMNAME_LENGTH );
				}
			}

			if ( tm >= 0 )
			{
				team_scores[tm] += plr->pev->frags;
			}

			if ( bResendInfo ) //Someone's info changed, let's send the team info again.
			{
				if ( plr && IsValidTeam( plr->TeamID() ) )
				{
					MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo, NULL );
						WRITE_BYTE( plr->entindex() );
						WRITE_STRING( plr->TeamID() );
					MESSAGE_END();
				}
			}
		}
	}
}


void CHalfLifeTeamplay::CapturedFlag(CBasePlayer *pPlayer)
{
}
BOOL CHalfLifeTeamplay::RestrictObserver( void )
{
	return TRUE;
}



void CHalfLifeTeamplay::PlayerSpawn(CBasePlayer *pPlayer)
{
	if(m_MatchStart.isMatch())
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}

	if(radar.value)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, pPlayer->edict() );   // try to show the radar
			WRITE_SHORT( pPlayer->entindex() );
			WRITE_BYTE( 1 );
		MESSAGE_END();
	}

	CTeamIndicator::Create(pPlayer);
	CHalfLifeMultiplay::PlayerSpawn(pPlayer);
}

BOOL CHalfLifeTeamplay::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return CHalfLifeMultiplay::ClientConnected(pEntity,pszName,pszAddress,szRejectReason);
}

BOOL CHalfLifeTeamplay::CanStopObserver(CBasePlayer *pPlayer)
{
	if(m_MatchStart.isMatch())
		return FALSE;
	else if(CheckValidTeam(pPlayer->m_szTeamName, pPlayer->m_szTeamName) == INVALID_TEAM)
	{
		pPlayer->ShowVGUIMenu(2);
		*pPlayer->m_szTeamName=NULL; 
		return FALSE;
	}
	return TRUE;
}


void CHalfLifeTeamplay::ResetPlayerHud(CBasePlayer* pPlayer)
{
	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( pPlayer->entindex() );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();

	int team = GetTeamIndex( pPlayer ) + 1;

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( pPlayer->entindex() );
		WRITE_BYTE( team );
		WRITE_BYTE( 0 );
		WRITE_BYTE(	0 );
		WRITE_BYTE(	0 );
	MESSAGE_END();


	MESSAGE_BEGIN( MSG_ONE, gmsgReset, NULL, pPlayer->edict() );  
	MESSAGE_END();

	ResetTeamColors(pPlayer, GetTeamIndex(pPlayer) +1);

	SendScoreInfo(pPlayer);
}
void CHalfLifeTeamplay::SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget)
{
	int nTeam = g_pGameRules->GetTeamIndex( pTarget )+1;
	if(nTeam == BLUE) 
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 145 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( -1 );
		MESSAGE_END();
	}
	else if(nTeam == RED)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 0 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( -1 );
		MESSAGE_END();
	}
	else if(nTeam == YELLOW)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 40 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( -1 );
		MESSAGE_END();
	}
	else if(nTeam == GREEN)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 80 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( -1 );
		MESSAGE_END();
	}
	else if(nTeam == PURPLE)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 215 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( -1 );
		MESSAGE_END();
	}
	else if(nTeam == ORANGE)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 20 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( -1 );
		MESSAGE_END();
	}

	if(radar.value)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, pPlayer->edict() );
			WRITE_SHORT( pTarget->entindex() );
			WRITE_BYTE( 1 );
		MESSAGE_END();
	}
}






void CHalfLifeTeamplay::ChangePlayerModel( CBasePlayer *pPlayer, const char *pModelName, BOOL bKill )
{
//	char text[512];
	int clientIndex = pPlayer->entindex();
	int nModelResult = CheckValidModel(pPlayer->m_szModelName, (char*)pModelName);
	if(nModelResult==1)
	{
		
		int damageFlags = DMG_GENERIC;

		damageFlags |= DMG_ALWAYSGIB;

		if(bKill)
		{
			// kill the player,  remove a death,  and let them start on with a new model, this will prevent model changing aliases
			m_DisableDeathMessages = TRUE;
			m_DisableDeathPenalty = TRUE;
			entvars_t *pevWorld = VARS( INDEXENT(0) );
			pPlayer->TakeDamage( pevWorld, pevWorld, 900, damageFlags );
			m_DisableDeathMessages = FALSE;
			m_DisableDeathPenalty = FALSE;
		}

		// copy out the team name from the model
		strncpy( pPlayer->m_szModelName, pModelName, 16 );
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );		
		ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, UTIL_VarArgs( "\"%s\" is your new CTF model.\n", pPlayer->m_szModelName ) );
	}
	else if(nModelResult==-1)
	{
		if(CheckValidModel(pPlayer->m_szModelName, pPlayer->m_szModelName)!=-1)
		{
			// set the client info back to what it was
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );
		}
		else
		{
			SetDefaultModel(pPlayer);
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );

		}

	}

}


void CHalfLifeTeamplay::SetDefaultModel(CBasePlayer *pPlayer)
{
	if(CheckValidModel(pPlayer->m_szModelName, pPlayer->m_szModelName) ==-1)
		ChangePlayerModel(pPlayer, "helmet", FALSE); // dont kill him
}



int CHalfLifeTeamplay::CheckValidModel(char *pCurrentModel, char* pNewModel)
{
 if(strcmp(pNewModel, "gman"))
 {
  if(strcmp(pNewModel, "gina"))
  {
   if(strcmp(pNewModel, "gordon"))
   {
    if(strcmp(pNewModel, "hgrunt"))
	{
     if(strcmp(pNewModel, "helmet"))
	 {
      if(strcmp(pNewModel, "scientist"))
	  {
		if(strcmp(pNewModel, "barney"))
		{
		 if(strcmp(pNewModel, "zombie"))
		 {
		   if(strcmp(pNewModel, "robo"))
		   {
		  return -1;
 }}}}}}}}}

 if(!strcmp(pCurrentModel, pNewModel))
	return 0; // same model do nothing

return 1;
}


void CHalfLifeTeamplay::SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever)
{
	if(pReciever==NULL)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( 0 );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pReciever->edict() );
			WRITE_BYTE( pSender->entindex() );
			WRITE_SHORT( pSender->pev->frags );
 			WRITE_SHORT( pSender->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( 0 );
		MESSAGE_END();
	}
}

edict_t *CHalfLifeTeamplay::GetRandomSpawn( CBasePlayer *pPlayer )
{
	return m_pTeamSpawnPoints.FindRandomSpawnPoint(pPlayer);
}

void CHalfLifeTeamplay::InitSpawns()
{
	m_pTeamSpawnPoints.CreateSpawnList();
}

void CHalfLifeTeamplay :: PlayerThink( CBasePlayer *pPlayer )
{

	CHalfLifeMultiplay::PlayerThink(pPlayer);

	// radar update messages;
	if(!pPlayer->m_isSpectator)
	{
		if(pPlayer->m_dRadar.TimeUp())
		{
			if(pPlayer->pev->origin[0] != pPlayer->pev->oldorigin[0] || pPlayer->pev->origin[1] != pPlayer->pev->oldorigin[1])
			{
				int team = GetTeamIndex( (CBasePlayer*)pPlayer);

				for ( int i = 1; i <= gpGlobals->maxClients; i++ )
				{
					CBaseEntity *plr = UTIL_PlayerByIndex( i );

					if ( plr && plr->IsPlayer())
					{
						if((CBasePlayer*)plr != pPlayer)
						{
							if(GetTeamIndex( (CBasePlayer*)plr) == team)
							{
								MESSAGE_BEGIN( MSG_ONE, gmsgLocation, NULL, plr->edict() );
									WRITE_SHORT( pPlayer->entindex() );
									WRITE_BYTE( 1 );	// 1 = add player to list, 0 = remove player from list
									WRITE_SHORT( pPlayer->pev->origin[0] );
									WRITE_SHORT( pPlayer->pev->origin[1] );
								MESSAGE_END();
								
								// relay mesage to spectators
								CEntityNode* pClient = ((CBasePlayer*)plr)->m_lObservers.head();
								while(pClient)
								{
									MESSAGE_BEGIN( MSG_ONE, gmsgLocation, NULL, pClient->m_pEntity );
										WRITE_SHORT( pPlayer->entindex() );
										WRITE_BYTE( 1 );	// 1 = add player to list, 0 = remove player from list
										WRITE_SHORT( pPlayer->pev->origin[0] );
										WRITE_SHORT( pPlayer->pev->origin[1] );
									MESSAGE_END();

									pClient = pClient->m_pNext;
								}
							}
						}
						
					}
				}
			}
			pPlayer->m_dRadar.AddTime(0.2);
		}

	}
}


void CHalfLifeTeamplay::ClientDisconnected( edict_t *pClient )
{
	if ( pClient )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );

		if ( pPlayer )
		{
			int index = pPlayer->entindex();
			if(*pPlayer->m_szTopcolor && *pPlayer->m_szBottomcolor)
			{
				g_engfuncs.pfnSetClientKeyValue( index, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", pPlayer->m_szTopcolor );
				g_engfuncs.pfnSetClientKeyValue( index, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", pPlayer->m_szBottomcolor );
			}
		}
	}
	CHalfLifeMultiplay::ClientDisconnected( pClient );
}


void CHalfLifeTeamplay::StartObserver(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
		WRITE_STRING( "" );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 1 );
		WRITE_BYTE(	(int)pPlayer->m_bIsRemoved);
		WRITE_BYTE(	0 );
	MESSAGE_END();
}

CHalfLifeTeamplay::~CHalfLifeTeamplay()
{
	
}


