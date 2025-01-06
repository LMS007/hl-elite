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
/*
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"hle_mode_team.h"
#include	"game.h"
#include	"hle_flag.h"
#include	"hle_vote.h"


static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
static int num_teams = 0;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern float g_fTimeOffset;
extern CVoteStructure* g_pVote;



extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;
extern int gmsgTeamNames;
extern int gmsgHudColor;
extern int gmsgHudStatus;
extern int gmsgScoreInfo;
extern int gmsgGaussColor;
extern int gmsgReset;


CHalfLifeTeamplay :: CHalfLifeTeamplay()
{
	m_nNumber = 0;
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	memset( team_names, 0, sizeof(team_names) );
	memset( team_scores, 0, sizeof(team_scores) );
	num_teams = 0;

	// Copy over the team from the server config
	m_szTeamList[0] = 0;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	strncpy( m_szTeamList, teamlist.string, TEAMPLAY_TEAMLISTLENGTH );

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
}

extern cvar_t timeleft, fragsleft;

#include "voice_gamemgr.h"
extern CVoiceGameMgr	g_VoiceGameMgr;

void CHalfLifeTeamplay :: Think ( void )
{
	
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
	if(FStrEq(pcmd, "changeteam"))
	{
		if(!m_MatchStart.isMatch())
		{
			if(*CMD_ARGV(1))
			{
				ChangePlayerTeam(pPlayer, CMD_ARGV(1), TRUE, TRUE);
			}
		}
		else
		{
			if(*CMD_ARGV(1))
			{
				g_pVote->CallVote(pPlayer, "changeteam", CMD_ARGV(1));
			}
		}
		return TRUE;
	}

	return FALSE;
}


void CHalfLifeTeamplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_STRING( "team" );  // game mode teamplay
		WRITE_STRING( HLE_VERSION );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ONE, gmsgReset, NULL, pPlayer->edict() );
	MESSAGE_END();

}

void CHalfLifeTeamplay::SetDefaultTeam(CBasePlayer* pPlayer)
{
	char* team = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team" );
	if(CheckValidTeam(pPlayer->m_szTeamName, team) ==-1)
		ChangePlayerTeam(pPlayer, team_names[TeamWithFewestPlayers()], false, false);
	else if(stricmp(pPlayer->m_szTeamName, team))
		ChangePlayerTeam(pPlayer, team, false, false);

}

const char *CHalfLifeTeamplay::SetDefaultPlayerTeam( CBasePlayer *pPlayer )
{
	RecountTeams();
	SetDefaultTeam(pPlayer);		
	return pPlayer->TeamID();
}


//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD( CBasePlayer *pPlayer )
{
	int i;
	

//0x16b61b85
//0x14fd75a0
	//SetDefaultPlayerTeam( pPlayer );

	CHalfLifeMultiplay::InitHUD( pPlayer );

	bool bRecovered = false;
	if(m_MatchStart.isMatch())
		bRecovered = m_MatchStart.m_Players.AttemptedToRecover(pPlayer->edict());

	if(!bRecovered)
	{
		pPlayer->ShowVGUIMenu(2); // show team menu	
		
		g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", "" );		
		*pPlayer->m_szTeamName  = NULL;
		//*pPlayer->m_szModelName = NULL;
		pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, 3);
		
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 0 );
		WRITE_BYTE(255);
		MESSAGE_END();
	}

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

	if(!bRecovered)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pPlayer->edict() );
			WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
			WRITE_SHORT( 0 );
			WRITE_SHORT( 0 );
			WRITE_SHORT( 0 );
			WRITE_SHORT( GetTeamIndex( pPlayer->m_szTeamName ) + 1 );
			WRITE_SHORT( 0 ); // saves
			WRITE_SHORT( 0 ); // captures
			WRITE_BYTE( 0);
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(pPlayer->edict()) );
			WRITE_SHORT( pPlayer->pev->frags );
			WRITE_SHORT( pPlayer->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( GetTeamIndex( pPlayer->m_szTeamName ) + 1 );
			WRITE_SHORT( 0 ); // saves
			WRITE_SHORT( 0 ); // captures
			WRITE_BYTE( 0);
		MESSAGE_END();
	}

	//UpdateGameMode(pPlayer);

//	RecountTeams();

//	char *team = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team" );
	// update the current player of the team he is joining
/*	char text[1024];
	if ( !strcmp( team, pPlayer->m_szTeamName ) )
	{
		sprintf( text, "* you are on team \'%s\'\n", pPlayer->m_szTeamName );
	}
	else
	{
		sprintf( text, "* assigned to team %s\n", pPlayer->m_szTeamName );
	}*/

//	ChangePlayerTeam( pPlayer, pPlayer->m_szTeamName, FALSE, FALSE );
//	UTIL_SayText( text, pPlayer );
//	int clientIndex = pPlayer->entindex();
//	RecountTeams();
	// update this player with all the other players team info
	// loop through all active players and send their team info to the new client
	/*
}

int CHalfLifeTeamplay::CheckValidTeam(char* pCurrentTeam, char* pNewTeam)
{
	for(int i = 0; i < num_teams; i++)
	{
		if(!stricmp(pNewTeam, team_names[i]))
		{
			if(!stricmp(pCurrentTeam, pNewTeam))
				return 0;
			return 1;
		}
	}
	return -1;
}

void CHalfLifeTeamplay::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
	int mTeamResult = CheckValidTeam(pPlayer->m_szTeamName, (char*)pTeamName);
	if(mTeamResult == 1)
	{
		int damageFlags = DMG_GENERIC;
		int clientIndex = pPlayer->entindex();

		if ( !bGib )
		{
			damageFlags |= DMG_NEVERGIB;
		}
		else
		{
			damageFlags |= DMG_ALWAYSGIB;
		}

		if ( bKill )
		{
			// kill the player,  remove a death,  and let them start on the new team
			m_DisableDeathMessages = TRUE;
			m_DisableDeathPenalty = TRUE;

			entvars_t *pevWorld = VARS( INDEXENT(0) );
			pPlayer->TakeDamage( pevWorld, pevWorld, 900, damageFlags );

			m_DisableDeathMessages = FALSE;
			m_DisableDeathPenalty = FALSE;
		}

		// copy out the team name from the model
		strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );

		//g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName ); hle
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );

		//switch

		// notify everyone's HUD of the team change
		MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
			WRITE_BYTE( clientIndex );
			WRITE_STRING( pPlayer->m_szTeamName );
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( clientIndex );
			WRITE_SHORT( pPlayer->pev->frags );
			WRITE_SHORT( pPlayer->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( g_pGameRules->GetTeamIndex( pPlayer->m_szTeamName ) + 1 );
			WRITE_SHORT( 0 );
			WRITE_SHORT( 0 );
			WRITE_BYTE( 0);
		MESSAGE_END();
	}
	else if(mTeamResult == -1)
	{
		// check the server team now
		if(CheckValidTeam(pPlayer->m_szTeamName, pPlayer->m_szTeamName)!=-1)
		{
			g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );
		}
		else 
		{
			SetDefaultTeam(pPlayer);
			g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );
		}
	}

}

void CHalfLifeTeamplay::ResetTeamColors(CBasePlayer *pPlayer, int nTeam )
{
	int clientIndex = pPlayer->entindex();
	switch(GetTeamIndex(pPlayer->m_szTeamName) +1)
	{
	case RED:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "0" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "0" );
			pPlayer->m_nGaussColor = 2;
			
			MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 2 );
			MESSAGE_END();

 			MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 0 );
			WRITE_BYTE(0);
			MESSAGE_END();
			break;
			
	case BLUE:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "150" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "150" );
			pPlayer->m_nGaussColor = 5;
			
			
			MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 5 );
			MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 145 );
			WRITE_BYTE( 0 );
			MESSAGE_END();
			break;
		
	case YELLOW:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "40" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "40" );
			pPlayer->m_nGaussColor = 0;
			
			
			MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 0 );
			MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 40 );
			WRITE_BYTE( 0 );
			MESSAGE_END();
			break;
		
	case GREEN:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "80" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "80" );
			pPlayer->m_nGaussColor = 7;
			
			
			MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 7 );
			MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 80 );
			WRITE_BYTE( 0 );
			MESSAGE_END();
			break;
		case PURPLE:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "215" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "215" );
			pPlayer->m_nGaussColor = 4;
			
			
			MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 4 );
			MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 215 );
			WRITE_BYTE( 0 );
			MESSAGE_END();
			break;
		case ORANGE:
		
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "topcolor", "20" );
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "bottomcolor", "20" );
			pPlayer->m_nGaussColor = 1;
			
			
			MESSAGE_BEGIN( MSG_ONE, gmsgGaussColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 1 );
			MESSAGE_END();
			
			MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
			WRITE_BYTE( 20 );
			WRITE_BYTE( 0 );
			MESSAGE_END();
			break;
	}
}

//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	char text[1024];

	// prevent skin/color/model changes

	char *mdls = g_engfuncs.pfnInfoKeyValue( infobuffer, "model" );
	char *team = g_engfuncs.pfnInfoKeyValue( infobuffer, "team" );
	
	if(m_MatchStart.isMatch())	
	{
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

		if(stricmp(team, pPlayer->TeamID()))
		{
			if(CheckValidTeam(pPlayer->m_szTeamName, pPlayer->m_szTeamName)<0)
			{
				if(CheckValidTeam(team, team)>=0)
				{
					ChangePlayerTeam(pPlayer, team, false, false);	
				}
				else
				{
					SetDefaultTeam(pPlayer); 
				}
			}
			g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );
			ClientPrint( pPlayer->pev, HUD_PRINTCONSOLE, "You may not change your team during a match\n" );
		}
		ResetTeamColors(pPlayer, GetTeamIndex(pPlayer->m_szTeamName) +1);
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
	
	if(stricmp(pPlayer->TeamID(), team))
	{
		ChangePlayerTeam( pPlayer, team, TRUE, TRUE );
		sprintf( text, "* %s has changed to team \'%s\'\n", STRING(pPlayer->pev->netname), team );
		UTIL_SayTextAll( text, pPlayer );
	}
	
	if ( defaultteam.value )
	{
		int clientIndex = pPlayer->entindex();

		//g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName ); hle
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );
		sprintf( text, "* Not allowed to change teams in this game!\n" ); // was removed hle
		//edict_t *pentSpawnSpot = g_pGameRules->GetPlayerSpawnSpot( pPlayer );
		
		UTIL_SayText( text, pPlayer );
		return;
	}

	if ( defaultteam.value || !IsValidTeam( team ) )
	{
		int clientIndex = pPlayer->entindex();

		//g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName );
		sprintf( text, "* Can't change team to \'%s\'\n", team );
		UTIL_SayText( text, pPlayer );
		sprintf( text, "* Server limits teams to \'%s\'\n", m_szTeamList );
		UTIL_SayText( text, pPlayer );
		return;
	}
	// notify everyone of the team change
	//sprintf( text, "* %s has changed to team \'%s\'\n", STRING(pPlayer->pev->netname), team );
	//UTIL_SayTextAll( text, pPlayer );

	UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n", 
		STRING(pPlayer->pev->netname),
		GETPLAYERUSERID( pPlayer->edict() ),
		GETPLAYERAUTHID( pPlayer->edict() ),
		pPlayer->m_szTeamName,
		team );


	if(stricmp(pPlayer->TeamID(), team))
		ChangePlayerTeam( pPlayer, team, TRUE, TRUE );
	//ChangePlayerTeam( pPlayer, team, TRUE, TRUE );

	int clientIndex = pPlayer->entindex();

	ResetTeamColors(pPlayer, GetTeamIndex(pPlayer->m_szTeamName) +1);
	// recound stuff
	RecountTeams( TRUE );
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


int CHalfLifeTeamplay::GetTeamIndex( const char *pTeamName )
{
	if ( pTeamName && *pTeamName != 0 )
	{
		// try to find existing team
		for ( int tm = 0; tm < num_teams; tm++ )
		{
			if ( !stricmp( team_names[tm], pTeamName ) )
				return tm;
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

	return ( GetTeamIndex( pTeamName ) != -1 ) ? TRUE : FALSE;
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

		if ( plr )
		{
			int team = GetTeamIndex( plr->TeamID() );
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
		/*}
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

	// loop through all teams, recounting everything
	num_teams = 0;

	// Copy all of the teams from the teamlist
	// make a copy because strtok is destructive
	strcpy( teamlist, m_szTeamList );
	pName = teamlist;
	pName = strtok( pName, ";" );
	while ( pName != NULL && *pName )
	{
		if ( GetTeamIndex( pName ) < 0 )
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
			int tm = GetTeamIndex( pTeamName );
			
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

BOOL CHalfLifeTeamplay::CanStartAutoObserver(CBasePlayer* pPlayer)
{
	// no death cam auto spectate
	return FALSE;
}

BOOL CHalfLifeTeamplay::CanStartObserver(CBasePlayer *pPlayer)
{
	if(m_MatchStart.isMatch())
	{
		m_MatchStart.m_Players.RemovePlayer(pPlayer->edict());
	}
	return TRUE;
}
BOOL CHalfLifeTeamplay::CanStopObserver(CBasePlayer *pPlayer)
{
	if(m_MatchStart.isMatch())
		return FALSE;
	return TRUE;
}

void CHalfLifeTeamplay::PlayerThink( CBasePlayer *pPlayer )
{
	if(m_MatchStart.isMatch())
	{
		if(m_MatchStart.m_bInProgess)
		{
			if(!pPlayer->m_bInMatch && !pPlayer->m_isSpectator)
			{
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles); // cant spawn homie
			}
		}
	}
	CHalfLifeMultiplay::PlayerThink(pPlayer);
}


void CHalfLifeTeamplay::PlayerSpawn(CBasePlayer *pPlayer)
{
	if(m_MatchStart.isMatch())
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudStatus, NULL, pPlayer->edict() );  
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	
	CHalfLifeMultiplay::PlayerSpawn(pPlayer);
}

BOOL CHalfLifeTeamplay::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return CHalfLifeMultiplay::ClientConnected(pEntity,pszName,pszAddress,szRejectReason);
}

void CHalfLifeTeamplay::ClientDisconnected( edict_t *pClient )
{
	if(m_MatchStart.isMatch())
		m_MatchStart.m_Players.AddPlayer(pClient);
	CHalfLifeMultiplay::ClientDisconnected(pClient);
}

void CHalfLifeTeamplay::MatchStart(int nType)
{
	m_MatchStart.setMatch(nType);	
}

BOOL CHalfLifeTeamplay::MatchInProgress(void)
{
	return m_MatchStart.isMatch();
}

CHalfLifeTeamplay::~CHalfLifeTeamplay()
{
//	m_MatchStart.setMatch(0);
}

void CHalfLifeTeamplay::ResetPlayerHud(CBasePlayer* pPlayer)
{

	MESSAGE_BEGIN( MSG_ONE, gmsgReset, NULL, pPlayer->edict() );  
	MESSAGE_END();
	ResetTeamColors(pPlayer, GetTeamIndex(pPlayer->m_szTeamName) +1);
}
void CHalfLifeTeamplay::SetSpectatorHud(CBasePlayer* pPlayer, CBasePlayer* pTarget)
{
	int nTeam = g_pGameRules->GetTeamIndex( pTarget->m_szTeamName )+1;
	if(nTeam == BLUE) 
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 145 );
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else if(nTeam == RED)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else if(nTeam == YELLOW)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 40 );
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else if(nTeam == GREEN)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 80 );
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else if(nTeam == PURPLE)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 215 );
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else if(nTeam == ORANGE)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudColor, NULL, pPlayer->edict() );  
		WRITE_BYTE( 20 );
		WRITE_BYTE( 0 );
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
	   if(strcmp(pNewModel, "squad"))
	   {
		if(strcmp(pNewModel, "barney"))
		{
		 if(strcmp(pNewModel, "zombie"))
		 {
		  if(strcmp(pNewModel, "squad"))
		  {
		   if(strcmp(pNewModel, "robo"))
		   {
		  return -1;
 }}}}}}}}}}}

 if(!strcmp(pCurrentModel, pNewModel))
	return 0; // same model do nothing

return 1;
}
*/