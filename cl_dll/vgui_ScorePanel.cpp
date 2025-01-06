//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: VGUI scoreboard
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================


#include<VGUI_LineBorder.h>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ScorePanel.h"
#include "..\game_shared\vgui_helpers.h"
#include "..\game_shared\vgui_loadtga.h"
#include "vgui_SpectatorPanel.h"



hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];   // additional player info sent directly to the client dll
team_info_t			 g_TeamInfo[MAX_TEAMS+1];
int					 g_IsSpectator[MAX_PLAYERS+1];
int					 g_TeamOrder[MAX_TEAMS];

int				CColumnStructure::m_iNumTeams;
int				CColumnStructure::m_iRows;
int				CColumnStructure::m_iSortedRows[NUM_ROWS];
int				CColumnStructure::m_iIsATeam[NUM_ROWS];
bool			CColumnStructure::m_bHasBeenSorted[MAX_PLAYERS];

extern "C" int g_TeamplayClient;
extern CHudColor g_hud_color;
;
int HUD_IsGame( const char *game );
int EV_TFC_IsAllyTeam( int iTeam1, int iTeam2 );

// Scoreboard dimensions
#define SBOARD_TITLE_SIZE_Y			YRES(22)

#define X_BORDER					XRES(4)


enum {

	NONE=0,
	FRAGS,
	WINS,
	CAPTURES,
	RECORD,
	TIME,
	TLMS,
	TDUEL,
};


// grid size is marked out for 640x480 screen

//SBColumnInfo m_pColumnInfo[NUM_COLUMNS] =
//{
//	{NULL,			24,			Label::a_east},
//	{NULL,			120,		Label::a_east},	
//	{"#CAPTURES",	/*140*/40,	Label::a_west},		// name
//	{"#SAVES",		/*56*/40,	Label::a_east},		// class
//	{"#SCORE",		40,			Label::a_east},
//	{"#DEATHS",		40,			Label::a_east},
//	{"#LATENCY",	40,			Label::a_east},
//	{"#VOICE",		40,			Label::a_east},
//	{NULL,			24,			Label::a_east},
		// blank column to take up the slack
//};





//-----------------------------------------------------------------------------
// ScorePanel::HitTestPanel.
//-----------------------------------------------------------------------------

void ScorePanel::HitTestPanel::internalMousePressed(MouseCode code)
{
	for(int i=0;i<_inputSignalDar.getCount();i++)
	{
		_inputSignalDar[i]->mousePressed(code,this);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Create the ScoreBoard panel
//-----------------------------------------------------------------------------
ScorePanel::ScorePanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("VGUI_MAIN");//("Scoreboard Title Text");
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Scoreboard Small Text");

	Font *tfont = pSchemes->getFont(hTitleScheme);
	Font *smallfont = pSchemes->getFont(hSmallScheme);

	m_pColumnInfo = new CColumnStructure(); // hle
	
	m_nSortType = FRAGS; // this chagnges depending on teh game mode

	setBgColor(0, 0, 0, 96);
	m_pCurrentHighlightLabel = NULL;
	m_iHighlightRow = -1;

	// Initialize the top title.
	m_TitleLabel.setFont(tfont);
	m_TitleLabel.setText("");
	m_TitleLabel.setBgColor( 0, 0, 0, 255 );
	m_TitleLabel.setFgColor( 255, 255, 255, 0 );
	m_TitleLabel.setContentAlignment( vgui::Label::a_west );

	LineBorder *border = new LineBorder(Color(60, 60, 60, 128));
	setBorder(border);
	setPaintBorderEnabled(true);

	int xpos = (*m_pColumnInfo)[0].m_Width + 3;
	if (ScreenWidth >= 640)
	{
		// only expand column size for res greater than 640
		xpos = XRES(xpos);
	}
	m_TitleLabel.setBounds(xpos, 4, wide, SBOARD_TITLE_SIZE_Y);
	m_TitleLabel.setContentFitted(false);
	m_TitleLabel.setParent(this);

	// Setup the header (labels like "name", "class", etc..).
	m_HeaderGrid.SetDimensions(NUM_COLUMNS, 1);
	m_HeaderGrid.SetSpacing(0, 0);
	
	for(int i=0; i < NUM_COLUMNS; i++)
	{
		if ((*m_pColumnInfo)[i].m_pTitle && (*m_pColumnInfo)[i].m_pTitle[0] == '#')
			m_HeaderLabels[i].setText(CHudTextMessage::BufferedLocaliseTextString((*m_pColumnInfo)[i].m_pTitle));
		else if((*m_pColumnInfo)[i].m_pTitle)
			m_HeaderLabels[i].setText((*m_pColumnInfo)[i].m_pTitle);
		
		

		int xwide = (*m_pColumnInfo)[i].m_Width;
		if (ScreenWidth >= 640)
		{
			xwide = XRES(xwide);
		}
		else if (ScreenWidth == 400)
		{
			// hack to make 400x300 resolution scoreboard fit
			if (i == 1)
			{
				// reduces size of player name cell
				xwide -= 28;
			}
			else if (i == 0)
			{
				xwide -= 8;
			}
		}
		
		m_HeaderGrid.SetColumnWidth(i, xwide);
		m_HeaderGrid.SetEntry(i, 0, &m_HeaderLabels[i]);

		m_HeaderLabels[i].setBgColor(0,0,0,255);
		m_HeaderLabels[i].setFgColor( 255, 255, 255, 0 );
		m_HeaderLabels[i].setFont(smallfont);
		
//		CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
//		SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle( "VGUI_MAPS" );
//		Font* pFontButtons = pSchemes->getFont(hButtonScheme);
//		m_HeaderLabels[i].setFont(pFontButtons);

//		m_HeaderLabels[i].setContentAlignment((*m_pColumnInfo)[i].m_Alignment);

		int yres = 12;
		if (ScreenHeight >= 480)
		{
			yres = YRES(yres);
		}
		m_HeaderLabels[i].setSize(50, yres);
	}

	// Set the width of the last column to be the remaining space.
	int ex, ey, ew, eh;
	m_HeaderGrid.GetEntryBox(NUM_COLUMNS - 2, 0, ex, ey, ew, eh);
	m_HeaderGrid.SetColumnWidth(NUM_COLUMNS - 1, (wide - X_BORDER) - (ex + ew));

	m_HeaderGrid.AutoSetRowHeights();
	m_HeaderGrid.setBounds(X_BORDER, SBOARD_TITLE_SIZE_Y, wide - X_BORDER*2, m_HeaderGrid.GetRowHeight(0));
	m_HeaderGrid.setParent(this);
	m_HeaderGrid.setBgColor(0,0,0,255);


	// Now setup the listbox with the actual player data in it.
	int headerX, headerY, headerWidth, headerHeight;
	m_HeaderGrid.getBounds(headerX, headerY, headerWidth, headerHeight);
	m_PlayerList.setBounds(headerX, headerY+headerHeight, headerWidth, tall - headerY - headerHeight - 6);
	m_PlayerList.setBgColor(0,0,0,255);
	m_PlayerList.setParent(this);

	for(int row=0; row < NUM_ROWS; row++)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];

		pGridRow->SetDimensions(NUM_COLUMNS, 1);
		
		for(int col=0; col < NUM_COLUMNS; col++)
		{
			m_PlayerEntries[col][row].setContentFitted(false);
			m_PlayerEntries[col][row].setRow(row);
			m_PlayerEntries[col][row].addInputSignal(this);
			pGridRow->SetEntry(col, 0, &m_PlayerEntries[col][row]);
		}

		pGridRow->setBgColor(0,0,0,255);
//		pGridRow->SetSpacing(2, 0);
		pGridRow->SetSpacing(0, 0);
		pGridRow->CopyColumnWidths(&m_HeaderGrid);
		pGridRow->AutoSetRowHeights();
		pGridRow->setSize(PanelWidth(pGridRow), pGridRow->CalcDrawHeight());
		pGridRow->RepositionContents();

		m_PlayerList.AddItem(pGridRow);
	}


	// Add the hit test panel. It is invisible and traps mouse clicks so we can go into squelch mode.
	m_HitTestPanel.setBgColor(0,0,0,255);
	m_HitTestPanel.setParent(this);
	m_HitTestPanel.setBounds(0, 0, wide, tall);
	m_HitTestPanel.addInputSignal(this);

	m_pCloseButton = new CommandButton( "x", wide-XRES(12 + 4), YRES(2), XRES( 12 ) , YRES( 12 ) );
	m_pCloseButton->setParent( this );
	m_pCloseButton->addActionSignal( new CMenuHandler_StringCommandWatch( "-showscores", true ) );
	m_pCloseButton->setBgColor(0,0,0,255);
	m_pCloseButton->setFgColor( 255, 255, 255, 0 );
	m_pCloseButton->setFont(tfont);
	m_pCloseButton->setBoundKey( (char)255 );
	m_pCloseButton->setContentAlignment(Label::a_center);

	pTGAEye = LoadTGAForRes("eye");
	pTGARemoved = LoadTGAForRes("removed");
	pTGAFlag = LoadTGAForRes("flag");


	//
}


//-----------------------------------------------------------------------------
// Purpose: Called each time a new level is started.
//-----------------------------------------------------------------------------
void ScorePanel::Initialize( void )
{
	// Clear out scoreboard data
	m_iLastKilledBy = 0;
	m_fLastKillTime = 0;
	m_iPlayerNum = 0;
	m_pColumnInfo->m_iNumTeams = 0;
	memset( g_PlayerExtraInfo, 0, sizeof g_PlayerExtraInfo );
	memset( g_TeamInfo, 0, sizeof g_TeamInfo );
 	
}

bool HACK_GetPlayerUniqueID( int iPlayer, char playerID[16] )
{
	return !!gEngfuncs.GetPlayerUniqueID( iPlayer, playerID );
}
		

void ScorePanel::InstallColumns(char* pszGameMode, char* pszVersion, char* pszServerName)
{
	if(pszGameMode)
	{
		g_hud_color.m_bLockColors = false;
		if(m_pColumnInfo)
			delete m_pColumnInfo;
		if(!strcmp(pszGameMode, "ffa"))
		{
			m_nSortType = FRAGS;
			m_pColumnInfo = new CColumnFFA();
		}
		else if(!strcmp(pszGameMode, "action"))
		{
			m_nSortType = FRAGS;
			m_pColumnInfo = new CColumnACTION();
		}
		else if(!strcmp(pszGameMode, "practice"))
		{
			m_nSortType = NONE;	
			m_pColumnInfo = new CColumnPRACTICE();
		}
		
		else if(!strcmp(pszGameMode, "duel"))
		{
			m_nSortType = WINS;
			m_pColumnInfo = new CColumnDUEL();
		}
		else if(!strcmp(pszGameMode, "teamaction"))
		{
			m_nSortType = FRAGS;
			m_pColumnInfo = new CColumnACTION();
		}
		else if(!strcmp(pszGameMode, "tournament"))
		{
			m_nSortType = RECORD;
			m_pColumnInfo = new CColumnTOURNAMENT();
		}
		else if(!strcmp(pszGameMode, "teamplay"))
		{
			m_nSortType = FRAGS;
			g_hud_color.m_bLockColors = true;
			m_pColumnInfo = new CColumnTEAM();
		}
		else if(!strcmp(pszGameMode, "ctf"))
		{
			m_nSortType = CAPTURES;
			g_hud_color.m_bLockColors = true;
			m_pColumnInfo = new CColumnCTF();
		}
		else if(!strcmp(pszGameMode, "ctp"))
		{
			m_nSortType = TIME;
			g_hud_color.m_bLockColors = true;
			m_pColumnInfo = new CColumnCTP();
		}
		else if(!strcmp(pszGameMode, "lms"))
		{
			m_nSortType = WINS;
			m_pColumnInfo = new CColumnLMS();
		}
		else if(!strcmp(pszGameMode, "lts"))
		{
			m_nSortType = TLMS;
			m_pColumnInfo = new CColumnTLMS();
		}
		else if(!strcmp(pszGameMode, "teamduel"))
		{
			m_nSortType = TDUEL;
			m_pColumnInfo = new CColumnTDuel();
		}
		
		for(int i=0; i < NUM_COLUMNS; i++)
		{
			if ((*m_pColumnInfo)[i].m_pTitle && (*m_pColumnInfo)[i].m_pTitle[0] == '#')
				m_HeaderLabels[i].setText(CHudTextMessage::BufferedLocaliseTextString((*m_pColumnInfo)[i].m_pTitle));
			else if((*m_pColumnInfo)[i].m_pTitle)
				m_HeaderLabels[i].setText((*m_pColumnInfo)[i].m_pTitle);
		}
		char title[128];
		sprintf(title, "Mode: %s  |  Version: %s  |  Host: %s", pszGameMode, pszVersion,pszServerName);
		m_TitleLabel.setText(title);
		
	}
}
//-----------------------------------------------------------------------------
// Purpose: Recalculate the internal scoreboard data
//-----------------------------------------------------------------------------
void ScorePanel::Update()
{
	// Set the title

	m_pColumnInfo->m_iRows = 0;
	gViewPort->GetAllPlayersInfo();

	// Clear out sorts
	for (int i = 0; i < NUM_ROWS; i++)
	{
		m_pColumnInfo->m_iSortedRows[i] = 0;
		m_pColumnInfo->m_iIsATeam[i] = TEAM_NO;
		m_pColumnInfo->m_bHasBeenSorted[i] = false;
	}

	// If it's not teamplay, sort all the players. Otherwise, sort the teams.
	//if ( gHUD.m_iGameMode == CTF || gHUD.m_iGameMode == TEAM )
	if(g_TeamplayClient == 1)
		SortTeams();
	else
		m_pColumnInfo->SortPlayers(TEAM_NO, 0 );
		

	// set scrollbar range
	m_PlayerList.SetScrollRange(m_pColumnInfo->m_iRows);

	FillGrid();

	if ( gViewPort->m_pSpectatorPanel->m_menuVisible )
	{
		 m_pCloseButton->setVisible ( true );
	}
	else 
	{
		 m_pCloseButton->setVisible ( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sort all the teams
//-----------------------------------------------------------------------------
void ScorePanel::SortTeams()
{
	// clear out team scores
	int i = 1;
	for (i = 1; i <= m_pColumnInfo->m_iNumTeams; i++ )
	{
		if ( !g_TeamInfo[i].scores_overriden )
		{
			g_TeamInfo[i].frags = 0;
			g_TeamInfo[i].deaths = 0;
			g_TeamInfo[i].aux0 = 0; // hle
			g_TeamInfo[i].aux1 = 0; // hle
		}
		g_TeamInfo[i].ping = g_TeamInfo[i].packetloss = 0;
	}

	
	// recalc the team scores, then draw them
	for ( i = 1; i < MAX_PLAYERS; i++ )
	{
		if ( g_PlayerInfoList[i].name == NULL )
			continue; // empty player slot, skip

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// find what team this player is in
		int j;
		for (j = 1; j <= m_pColumnInfo->m_iNumTeams; j++ )
		{
			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		}
		if ( j > m_pColumnInfo->m_iNumTeams )  // player is not in a team, skip to the next guy
			continue;

		if ( !g_TeamInfo[j].scores_overriden )
		{
			g_TeamInfo[j].frags += g_PlayerExtraInfo[i].frags;
			g_TeamInfo[j].deaths += g_PlayerExtraInfo[i].deaths;
			g_TeamInfo[j].aux0 += g_PlayerExtraInfo[i].aux0;
			g_TeamInfo[j].aux1 += g_PlayerExtraInfo[i].aux1;
		}

		g_TeamInfo[j].ping += g_PlayerInfoList[i].ping;
		g_TeamInfo[j].packetloss += g_PlayerInfoList[i].packetloss;

		if ( g_PlayerInfoList[i].thisplayer )
			g_TeamInfo[j].ownteam = TRUE;
		else
			g_TeamInfo[j].ownteam = FALSE;

		// Set the team's number (used for team colors)
		g_TeamInfo[j].teamnumber = g_PlayerExtraInfo[i].teamnumber;
	}

	// find team ping/packetloss averages
	for ( i = 1; i <= m_pColumnInfo->m_iNumTeams; i++ )
	{
		g_TeamInfo[i].already_drawn = FALSE;

		if ( g_TeamInfo[i].players > 0 )
		{
			g_TeamInfo[i].ping /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
			g_TeamInfo[i].packetloss /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
		}
	}

	if(m_nSortType)
	{
		m_pColumnInfo->SortTeams();
	// Add all the players who aren't in a team yet into spectators
	m_pColumnInfo->SortPlayers( TEAM_SPECTATORS, NULL );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sort a list of players
//-----------------------------------------------------------------------------
/*void ScorePanel::SortPlayers( int iTeam, char *team )
{
	bool bCreatedTeam = false;

	// draw the players, in order,  and restricted to team if set
	//if(m_nSortType)
	{
	while ( 1 )
	{
		// Find the top ranking player

		int highest_score = -99999;	int lowest_deaths = 99999;
		int best_player;
//		int best_team;
		bool is_spectator = true;
		best_player = 0;
		
		
/*
		if(m_nSortType == FRAGS || m_nSortType == RECORD || m_nSortType == TLMS || m_nSortType == 0)
		{
			for ( int i = 1; i < MAX_PLAYERS; i++ )
			{
				if ( m_pColumnInfo->m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_score )
				{
					cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

					if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
					{
						extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
										
						if ( pl_info->frags > highest_score || pl_info->deaths < lowest_deaths )
						{
							if(is_spectator == true)
							{
								if(g_PlayerExtraInfo[i].spectator!=0)
								{
									is_spectator = false;
								}
							}
							else
							{
								if(g_PlayerExtraInfo[i].spectator!=0)
								{
									continue;
								}
							}

							best_player = i;
							lowest_deaths = pl_info->deaths;
							highest_score = pl_info->frags;
						}
					}
				}
			}
		}
		*/
		/*
		else if(m_nSortType == WINS /*|| m_nSortType == RECORD*//* || m_nSortType == TIME)
		{
			for ( int i = 1; i < MAX_PLAYERS; i++ )
			{
				if ( m_pColumnInfo->m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].aux0 >= highest_score )
				{
					cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

					if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
					{
						extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
										
						if ( pl_info->aux0 > highest_score || pl_info->deaths < lowest_deaths )
						{
							if(is_spectator == true)
							{
								if(g_PlayerExtraInfo[i].spectator!=0)
								{
									is_spectator = false;
								}
							}
							else
							{
								if(g_PlayerExtraInfo[i].spectator==0)
								{
									continue;
								}
							}

							best_player = i;
							lowest_deaths = pl_info->deaths;
							highest_score = pl_info->aux0;
						}
					}
				}
			}
		}
		else if(m_nSortType == CAPTURES)
		{
			for ( int i = 1; i < MAX_PLAYERS; i++ )
			{
				if ( m_pColumnInfo->m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].aux1 >= highest_score )
				{
					cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

					if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
					{
						extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
										
						if ( pl_info->aux1 > highest_score || pl_info->deaths < lowest_deaths )
						{
							if(is_spectator == true)
							{
								if(g_PlayerExtraInfo[i].spectator!=0)
								{
									is_spectator = false;
								}
							}
							else
							{
								if(g_PlayerExtraInfo[i].spectator==0)
								{
									continue;
								}
							}

							best_player = i;
							lowest_deaths = pl_info->deaths;
							highest_score = pl_info->aux1;
						}
					}
				}
			}

		}

		if ( !best_player )
			break;

		// If we haven't created the Team yet, do it first
		if (!bCreatedTeam && iTeam)
		{
			m_pColumnInfo->m_iIsATeam[ m_pColumnInfo->m_iRows ] = iTeam;
			m_pColumnInfo->m_iRows++;

			bCreatedTeam = true;
		}

		// Put this player in the sorted list
		m_pColumnInfo->m_iSortedRows[ m_pColumnInfo->m_iRows ] = best_player;
		m_pColumnInfo->m_bHasBeenSorted[ best_player ] = true;
		m_pColumnInfo->m_iRows++;
	}
	}*/
/*
	if (team)
	{
		m_pColumnInfo->m_iIsATeam[m_pColumnInfo->m_iRows++] = TEAM_BLANK;
	}
}*/

//-----------------------------------------------------------------------------
// Purpose: Recalculate the existing teams in the match
//-----------------------------------------------------------------------------
void ScorePanel::RebuildTeams()
{
	// clear out player counts from teams
	int i;
	for (i = 1; i <= m_pColumnInfo->m_iNumTeams; i++ )
	{
		g_TeamInfo[i].players = 0;
	}

	// rebuild the team list
	gViewPort->GetAllPlayersInfo();
	m_pColumnInfo->m_iNumTeams = 0;
	for ( i = 1; i < MAX_PLAYERS; i++ )
	{
		if ( g_PlayerInfoList[i].name == NULL )
			continue;

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// is this player in an existing team?
		int j;
		for (j = 1; j <= m_pColumnInfo->m_iNumTeams; j++ )
		{
			if ( g_TeamInfo[j].name[0] == '\0' )
				break;

			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		}

		if ( j > m_pColumnInfo->m_iNumTeams )
		{ // they aren't in a listed team, so make a new one
			// search through for an empty team slot
			for ( int j = 1; j <= m_pColumnInfo->m_iNumTeams; j++ )
			{
				if ( g_TeamInfo[j].name[0] == '\0' )
					break;
			}
			m_pColumnInfo->m_iNumTeams = max( j, m_pColumnInfo->m_iNumTeams );

			strncpy( g_TeamInfo[j].name, g_PlayerExtraInfo[i].teamname, MAX_TEAM_NAME );
			g_TeamInfo[j].players = 0;
		}

		g_TeamInfo[j].players++;
	}

	// clear out any empty teams
	for ( i = 1; i <= m_pColumnInfo->m_iNumTeams; i++ )
	{
		if ( g_TeamInfo[i].players < 1 )
			memset( &g_TeamInfo[i], 0, sizeof(team_info_t) );
	}

	// Update the scoreboard
	Update();
}


void ScorePanel::FillGrid()
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hScheme = pSchemes->getSchemeHandle("Scoreboard Text");
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Scoreboard Title Text");
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Scoreboard Small Text");

	Font *sfont = pSchemes->getFont(hScheme);
	Font *tfont = pSchemes->getFont(hTitleScheme);
	Font *smallfont = pSchemes->getFont(hSmallScheme);

	// update highlight position
	int x, y;

	getApp()->getCursorPos(x, y);
	cursorMoved(x, y, this);

	// remove highlight row if we're not in squelch mode
	if (!GetClientVoiceMgr()->IsInSquelchMode())
	{
		m_iHighlightRow = -1;
	}

	bool bNextRowIsGap = false;
	int row = 0;
	for(row=0; row < NUM_ROWS; row++)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];
		pGridRow->SetRowUnderline(0, false, 0, 0, 0, 0, 0);

		if(row >= m_pColumnInfo->m_iRows)
		{
			for(int col=0; col < NUM_COLUMNS; col++)
				m_PlayerEntries[col][row].setVisible(false);
		
			continue;
		}

		bool bRowIsGap = false;
		if (bNextRowIsGap)
		{
			bNextRowIsGap = false;
			bRowIsGap = true;
		}

		for(int col=0; col < NUM_COLUMNS; col++)
		{
			CLabelHeader *pLabel = &m_PlayerEntries[col][row];

			pLabel->setVisible(true);
			pLabel->setText2("");
			pLabel->setImage(NULL);
			pLabel->setFont(sfont);
			pLabel->setTextOffset(0, 0);
			
			
			int rowheight = 13;
			if (ScreenHeight > 480)
			{
				rowheight = YRES(rowheight);
			}
			else
			{
				// more tweaking, make sure icons fit at low res
				rowheight = 15;
			}
			pLabel->setSize(pLabel->getWide(), rowheight);
			pLabel->setBgColor(0, 0, 0, 255);
			
			char sz[128];
			hud_player_info_t *pl_info = NULL;
			team_info_t *team_info = NULL;

			if (m_pColumnInfo->m_iIsATeam[row] == TEAM_BLANK)
			{
				pLabel->setText(" ");
				continue;
			}
			else if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
			{
				// Get the team's data
				team_info = &g_TeamInfo[ m_pColumnInfo->m_iSortedRows[row] ];

				// team color text for team names
				pLabel->setFgColor(	iTeamColors[team_info->teamnumber % iNumberOfTeamColors][0],
									iTeamColors[team_info->teamnumber % iNumberOfTeamColors][1],
									iTeamColors[team_info->teamnumber % iNumberOfTeamColors][2],
									0 );

				// different height for team header rows
				rowheight = 20;
				if (ScreenHeight >= 480)
				{
					rowheight = YRES(rowheight);
				}
				pLabel->setSize(pLabel->getWide(), rowheight);
				pLabel->setFont(tfont);

				pGridRow->SetRowUnderline(	0,
											true,
											YRES(3),	
											iTeamColors[team_info->teamnumber % iNumberOfTeamColors][0],
											iTeamColors[team_info->teamnumber % iNumberOfTeamColors][1],
											iTeamColors[team_info->teamnumber % iNumberOfTeamColors][2],
											0 );
			}
			else if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_SPECTATORS )
			{
				// grey text for spectators
				pLabel->setFgColor(175, 175, 175, 0);

				// different height for team header rows
				rowheight = 20;
				if (ScreenHeight >= 480)
				{
					rowheight = YRES(rowheight);
				}
				pLabel->setSize(pLabel->getWide(), rowheight);
				pLabel->setFont(tfont);

				pGridRow->SetRowUnderline(0, true, YRES(3), 175, 175, 175, 0);
			}
			else
			{
				// team color text for player names
				if(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].spectator==0)
				{
					pLabel->setFgColor(	iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].teamnumber % iNumberOfTeamColors ][0],
										iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].teamnumber % iNumberOfTeamColors ][1],
										iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].teamnumber % iNumberOfTeamColors ][2],
										0 );
				}
				else
				{
					pLabel->setFgColor(175, 175, 175, 0);
				}
				//pLabel->setFgColor(150, 185, 225, 0);

				// Get the player's data
				pl_info = &g_PlayerInfoList[ m_pColumnInfo->m_iSortedRows[row] ];

				// Set background color
				if ( pl_info->thisplayer ) // if it is their name, draw it a different color
				{
					// Highlight this player
					if(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].spectator==0)
					{
						pLabel->setFgColor(Scheme::sc_white);
						pLabel->setBgColor(	iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].teamnumber % iNumberOfTeamColors ][0],
							iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].teamnumber % iNumberOfTeamColors ][1],
							iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].teamnumber % iNumberOfTeamColors ][2],
							170 );
					}
					else
					{
						pLabel->setBgColor(175, 175, 175, 170);
					}

				}
				else if ( m_pColumnInfo->m_iSortedRows[row] == m_iLastKilledBy && m_fLastKillTime && m_fLastKillTime > gHUD.m_flTime )
				{
					// Killer's name
					pLabel->setBgColor( 255,0,0, 255 - ((float)15 * (float)(m_fLastKillTime - gHUD.m_flTime)) );
				}
			}				

			// Align 
			if (col == COLUMN_NAME /*|| col == AUX1 || col == AUX0*/)
			{
				pLabel->setContentAlignment( vgui::Label::a_west );
			}
			else if (col == COLUMN_TRACKER)
			{
				pLabel->setContentAlignment( vgui::Label::a_center );
			}
			else
			{
				pLabel->setContentAlignment( vgui::Label::a_east );
			}

			// Fill out with the correct data
			strcpy(sz, "");
			if ( m_pColumnInfo->m_iIsATeam[row] )
			{
				char sz2[128];
				*sz2 = NULL;

				switch (col)
				{
				case COLUMN_NAME:

					if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_SPECTATORS )
					{
						sprintf( sz2, CHudTextMessage::BufferedLocaliseTextString( "#Spectators" ) );
					}
					else
					{
						sprintf( sz2, gViewPort->GetTeamName(g_TeamOrder[team_info->teamnumber]) ); 
					}

					strcpy(sz, sz2);

					// Append the number of players
					if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
					{
						if (team_info->players == 1)
						{
							sprintf(sz2, "(%d %s)", team_info->players, CHudTextMessage::BufferedLocaliseTextString( "#Player" ) );
						}
						else
						{
							sprintf(sz2, "(%d %s)", team_info->players, CHudTextMessage::BufferedLocaliseTextString( "#Player_plural" ) );
						}

						pLabel->setText2(sz2);
						pLabel->setFont2(smallfont);
					}
					break;
				case COLUMN_VOICE:
					break;
				case COLUMN_WONID:
					
					break;
				case COLUMN_AUX0:
					
					if(m_nSortType == TLMS)
					{
						if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
							sprintf(sz, "%d",  CHudTLms::g_TeamScores[team_info->teamnumber-1][0]); // hle
					}
					else if(m_nSortType==CAPTURES || m_nSortType==WINS || m_nSortType == TIME) 
					{
						if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES ) // hle
							sprintf(sz, "%d",  team_info->aux0 ); // hle
					}
					else if(m_nSortType==RECORD)
					{
						if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES ) // hle
							sprintf(sz, "%d/%d",  team_info->aux0, team_info->aux1 ); // hle
					}
					else if(m_nSortType==TDUEL) 
					{
						if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
							sprintf(sz, "%d/%d",  CHudTLms::g_TeamScores[team_info->teamnumber-1][0],CHudTLms::g_TeamScores[team_info->teamnumber-1][1] ); // hle
					}
					else 
						sz[0]=NULL;
					break;
				case COLUMN_AUX1: 
					
					if(m_nSortType == TIME)
					{	
						if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES ) 
						{
							char szTime[16];
							*szTime = NULL;
							if(team_info->teamnumber < 7)
							{
								int seconds = CHudCtp::g_CtpScores[team_info->teamnumber-1] % 60;
								int minutes = (CHudCtp::g_CtpScores[team_info->teamnumber-1] / 60) % 60;
								int hours = (CHudCtp::g_CtpScores[team_info->teamnumber-1] / 3600) % 24;						
								char szSeconds[3];
								char szMinutes[3];
								char szHours[3];
								sprintf(szSeconds, "%i", seconds);
								sprintf(szMinutes, "%i", minutes);
								sprintf(szHours, "%i", hours);
		
								if(hours)
								{
									strcat(szTime, szHours);
									strcat(szTime, ":");
								}
								if(minutes)
								{
									if(minutes <10 && hours)
										strcat(szTime, "0");								
									strcat(szTime, szMinutes);
									strcat(szTime, ":");
								}
								else if(hours)
								{
									strcat(szTime, "00:");
								}
								if(seconds)
								{

									if(seconds <10 && minutes)
										strcat(szTime, "0");								
									strcat(szTime, szSeconds);

								}
								else if(minutes || hours)
								{
									strcat(szTime, "00");
								}
								sprintf(sz, "%s", szTime ); 
							}
						}
					}
					else if(m_nSortType==CAPTURES ) 
					{
						if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES ) // hle
							sprintf(sz, "%d",  team_info->aux1 ); // hle
					}
					else 
						sz[0]=NULL;
					
	
					break;
				case COLUMN_KILLS:
					sz[0]=NULL;
					if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
						sprintf(sz, "%d",  team_info->frags );
					break;
				case COLUMN_DEATHS:
					if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
						sprintf(sz, "%d",  team_info->deaths );
					break;
				case COLUMN_LATENCY:
					if ( m_pColumnInfo->m_iIsATeam[row] == TEAM_YES )
						sprintf(sz, "%d", team_info->ping );
					break;
				default:
					break;
				}
			}
			else
			{
				bool bShowClass = false;

				switch (col)
				{
				case COLUMN_NAME:
					
					//if(gHUD.m_iGameMode == TOURNAMENT || gHUD.m_iGameMode == DUEL || LMS)
				/*	if(g_TeamplayClient == 0)
					{
						if(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row]].aux2==1) // spectators && dead players
						{
							if (!pl_info->thisplayer ) // if it is their name, draw it a different color
							{
								pLabel->setFgColor(100, 100, 100, 0);
							}
						}
					}*/

					sprintf(sz, "%s  ", pl_info->name);
					break;
				case COLUMN_VOICE:
					sz[0] = 0;
					// in HLTV mode allow spectator to turn on/off commentator voice
					if (!pl_info->thisplayer || gEngfuncs.IsSpectateOnly() )
					{
						GetClientVoiceMgr()->UpdateSpeakerImage(pLabel, m_pColumnInfo->m_iSortedRows[row]);
					}
					break;
				case COLUMN_WONID:
					if(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].playerID!=0)
					{
						if(!strcmp(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].playerID, ""))
							sprintf(sz, "Local");
						else
							sprintf(sz, "%s",  g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].playerID );
					}
					else
						sprintf(sz, "Local");
					break;

				case COLUMN_AUX0:

					sz[0] = 0; // saves/wins
					if(m_nSortType==CAPTURES || m_nSortType==WINS || m_nSortType==TIME) 
					{
						sprintf(sz, "%d",  g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].aux0 );
					}
					else if(m_nSortType==RECORD)
					{
						sprintf(sz, "%d/%d",  g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].aux0, g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].aux1);
					}
					break;
				case COLUMN_AUX1:

					sz[0] = 0; // captures
					if(m_nSortType==CAPTURES)
					{
						sprintf(sz, "%d",  g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].aux1 );
					}
					break;

				case COLUMN_TRACKER:
					{
						if(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].aux2)
						{

							switch(g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].aux2)
							{
							case 0:
								pLabel->setImage( NULL );	
								break;
							case 1:
								pLabel->setImage( pTGAEye );	
								break;
							case 2:
								pLabel->setImage( pTGARemoved );	
								break;
							case 3:
								pLabel->setFgColor(	iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].flag_color  ][0],
									iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].flag_color ][1],
									iTeamColors[ g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].flag_color ][2],
									0 );

								pLabel->setImage( pTGAFlag );	

								break;
							
							}
						}										
						break;
					}
				case COLUMN_KILLS:
					if(m_nSortType)
						sprintf(sz, "%d",  g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].frags );
					break;
				case COLUMN_DEATHS:
					if(m_nSortType)
						sprintf(sz, "%d",  g_PlayerExtraInfo[ m_pColumnInfo->m_iSortedRows[row] ].deaths );
					break;
				case COLUMN_LATENCY:
					
					sprintf(sz, "%d", g_PlayerInfoList[ m_pColumnInfo->m_iSortedRows[row] ].ping );
					break;
				default:
					break;
				}
			}
			
			pLabel->setText(sz);
		}
	}
	for(row=0; row < NUM_ROWS; row++)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];

		pGridRow->AutoSetRowHeights();
		pGridRow->setSize(PanelWidth(pGridRow), pGridRow->CalcDrawHeight());
		pGridRow->RepositionContents();
	}

	// hack, for the thing to resize
	m_PlayerList.getSize(x, y);
	m_PlayerList.setSize(x, y);
}


//-----------------------------------------------------------------------------
// Purpose: Setup highlights for player names in scoreboard
//-----------------------------------------------------------------------------
void ScorePanel::DeathMsg( int killer, int victim )
{
	// if we were the one killed,  or the world killed us, set the scoreboard to indicate suicide
	if ( victim == m_iPlayerNum || killer == 0 )
	{
		m_iLastKilledBy = killer ? killer : m_iPlayerNum;
		m_fLastKillTime = gHUD.m_flTime + 10;	// display who we were killed by for 10 seconds

		if ( killer == m_iPlayerNum )
			m_iLastKilledBy = m_iPlayerNum;
	}
}


void ScorePanel::Open( void )
{
	RebuildTeams();
	setVisible(true);
	m_HitTestPanel.setVisible(true);
}


void ScorePanel::mousePressed(MouseCode code, Panel* panel)
{
	if(gHUD.m_iIntermission)
		return;

	if (!GetClientVoiceMgr()->IsInSquelchMode())
	{
		GetClientVoiceMgr()->StartSquelchMode();
		m_HitTestPanel.setVisible(false);
	}
	else if (m_iHighlightRow >= 0)
	{
		// mouse has been pressed, toggle mute state
		int iPlayer = m_pColumnInfo->m_iSortedRows[m_iHighlightRow];
		if (iPlayer > 0)
		{
			// print text message
			hud_player_info_t *pl_info = &g_PlayerInfoList[iPlayer];

			if (pl_info && pl_info->name && pl_info->name[0])
			{
				char string[256];
				if (GetClientVoiceMgr()->IsPlayerBlocked(iPlayer))
				{
					char string1[1024];

					// remove mute
					GetClientVoiceMgr()->SetPlayerBlockedState(iPlayer, false);

					sprintf( string1, CHudTextMessage::BufferedLocaliseTextString( "#Unmuted" ), pl_info->name );
					sprintf( string, "%c** %s\n", HUD_PRINTTALK, string1 );

					gHUD.m_TextMessage.MsgFunc_TextMsg(NULL, strlen(string)+1, string );
				}
				else
				{
					char string1[1024];
					char string2[1024];

					// mute the player
					GetClientVoiceMgr()->SetPlayerBlockedState(iPlayer, true);

					sprintf( string1, CHudTextMessage::BufferedLocaliseTextString( "#Muted" ), pl_info->name );
					sprintf( string2, CHudTextMessage::BufferedLocaliseTextString( "#No_longer_hear_that_player" ) );
					sprintf( string, "%c** %s %s\n", HUD_PRINTTALK, string1, string2 );

					gHUD.m_TextMessage.MsgFunc_TextMsg(NULL, strlen(string)+1, string );
				}
			}
		}
	}
}

void ScorePanel::cursorMoved(int x, int y, Panel *panel)
{
	if (GetClientVoiceMgr()->IsInSquelchMode())
	{
		// look for which cell the mouse is currently over
		for (int i = 0; i < NUM_ROWS; i++)
		{
			int row, col;
			if (m_PlayerGrids[i].getCellAtPoint(x, y, row, col))
			{
				MouseOverCell(i, col);
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles mouse movement over a cell
// Input  : row - 
//			col - 
//-----------------------------------------------------------------------------
void ScorePanel::MouseOverCell(int row, int col)
{
	CLabelHeader *label = &m_PlayerEntries[col][row];

	// clear the previously highlighted label
	if (m_pCurrentHighlightLabel != label)
	{
		m_pCurrentHighlightLabel = NULL;
		m_iHighlightRow = -1;
	}
	if (!label)
		return;

	// don't act on teams
	if (m_pColumnInfo->m_iIsATeam[row] != TEAM_NO)
		return;

	// don't act on disconnected players or ourselves
	hud_player_info_t *pl_info = &g_PlayerInfoList[ m_pColumnInfo->m_iSortedRows[row] ];
	if (!pl_info->name || !pl_info->name[0])
		return;

	if (pl_info->thisplayer && !gEngfuncs.IsSpectateOnly() )
		return;

	// setup the new highlight
	m_pCurrentHighlightLabel = label;
	m_iHighlightRow = row;
}

//-----------------------------------------------------------------------------
// Purpose: Label paint functions - take into account current highligh status
//-----------------------------------------------------------------------------
void CLabelHeader::paintBackground()
{
	Color oldBg;
	getBgColor(oldBg);

	if (gViewPort->GetScoreBoard()->m_iHighlightRow == _row)
	{
		setBgColor(134, 91, 19, 0);
	}

	Panel::paintBackground();

	setBgColor(oldBg);
}
		

//-----------------------------------------------------------------------------
// Purpose: Label paint functions - take into account current highligh status
//-----------------------------------------------------------------------------
void CLabelHeader::paint()
{
	Color oldFg;
	getFgColor(oldFg);

	if (gViewPort->GetScoreBoard()->m_iHighlightRow == _row)
	{
		setFgColor(255, 255, 255, 0);
	}

	// draw text
	int x, y, iwide, itall;
	getTextSize(iwide, itall);
	calcAlignment(iwide, itall, x, y);
	_dualImage->setPos(x, y);

	int x1, y1;
	_dualImage->GetImage(1)->getPos(x1, y1);
	_dualImage->GetImage(1)->setPos(_gap, y1);

	_dualImage->doPaint(this);

	// get size of the panel and the image
	if (_image)
	{
		Color imgColor;
		getFgColor( imgColor );
		if( _useFgColorAsImageColor )
		{
			_image->setColor( imgColor );
		}

		_image->getSize(iwide, itall);
		calcAlignment(iwide, itall, x, y);
		_image->setPos(x, y);
		_image->doPaint(this);
	}

	setFgColor(oldFg[0], oldFg[1], oldFg[2], oldFg[3]);
}


void CLabelHeader::calcAlignment(int iwide, int itall, int &x, int &y)
{
	// calculate alignment ourselves, since vgui is so broken
	int wide, tall;
	getSize(wide, tall);

	x = 0, y = 0;

	// align left/right
	switch (_contentAlignment)
	{
		// left
		case Label::a_northwest:
		case Label::a_west:
		case Label::a_southwest:
		{
			x = 0;
			break;
		}
		
		// center
		case Label::a_north:
		case Label::a_center:
		case Label::a_south:
		{
			x = (wide - iwide) / 2;
			break;
		}
		
		// right
		case Label::a_northeast:
		case Label::a_east:
		case Label::a_southeast:
		{
			x = wide - iwide;
			break;
		}
	}

	// top/down
	switch (_contentAlignment)
	{
		// top
		case Label::a_northwest:
		case Label::a_north:
		case Label::a_northeast:
		{
			y = 0;
			break;
		}
		
		// center
		case Label::a_west:
		case Label::a_center:
		case Label::a_east:
		{
			y = (tall - itall) / 2;
			break;
		}
		
		// south
		case Label::a_southwest:
		case Label::a_south:
		case Label::a_southeast:
		{
			y = tall - itall;
			break;
		}
	}

// don't clip to Y
//	if (y < 0)
//	{
//		y = 0;
//	}
	if (x < 0)
	{
		x = 0;
	}

	x += _offset[0];
	y += _offset[1];
}
