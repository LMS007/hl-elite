//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef SCOREPANEL_H
#define SCOREPANEL_H

#include<VGUI_Panel.h>
#include<VGUI_TablePanel.h>
#include<VGUI_HeaderPanel.h>
#include<VGUI_TextGrid.h>
#include<VGUI_Label.h>
#include<VGUI_TextImage.h>
#include "..\game_shared\vgui_listbox.h"

#include <ctype.h>

#define MAX_SCORES					10
#define MAX_SCOREBOARD_TEAMS		5

// Scoreboard cells
#define COLUMN_TRACKER	0
#define COLUMN_NAME		1
#define COLUMN_WONID	2
#define COLUMN_AUX1		3
#define COLUMN_AUX0		4
#define COLUMN_KILLS	5
#define COLUMN_DEATHS	6
#define COLUMN_LATENCY	7
#define COLUMN_VOICE	8
#define COLUMN_NULL		9

#define NUM_COLUMNS		10
#define NUM_ROWS		(MAX_PLAYERS + (MAX_SCOREBOARD_TEAMS * 2))

using namespace vgui;


//SBColumnInfo g_ColumnInfo[NUM_COLUMNS] =
//{
//	{NULL,			24,			Label::a_east},
//	{NULL,			120,		Label::a_east},	
///	{"#CAPTURES",	/*140*/40,	Label::a_west},		// name
//	{"#SAVES",		/*56*/40,	Label::a_east},		// class
//	{"#SCORE",		40,			Label::a_east},
//	{"#DEATHS",		40,			Label::a_east},
//	{"#LATENCY",	40,			Label::a_east},
//	{"#VOICE",		40,			Label::a_east},
//	{NULL,			24,			Label::a_east},
//		// blank column to take up the slack
//};

#define TEAM_NO				0
#define TEAM_YES			1
#define TEAM_SPECTATORS		2
#define TEAM_BLANK			3


// Column sizes
class SBColumnInfo
{
public:
	SBColumnInfo()
	{
		*m_pTitle = NULL;
		m_Width = NULL;
		m_Alignment = Label::a_west;
	}
	void SetColumn(char* title, int width, Label::Alignment position)
	{
		if(title)
			strncpy(m_pTitle, title, 31);
		m_Width = width;
		m_Alignment = position;
	}
	void SetTitle(char* title)
	{	
		if(title)
			strncpy(m_pTitle, title, 31);
	}
	void SetWidth(int width)
	{	
		m_Width = width;
	}
	void SetPosistion(Label::Alignment position)
	{	
		m_Alignment = position;
	}
	char				m_pTitle[32];		// If null, ignore, if starts with #, it's localized, otherwise use the string directly.
	int					m_Width;		// Based on 640 width. Scaled to fit other resolutions.
	Label::Alignment	m_Alignment;	
};

class CColumnStructure
{
public:

	static int				m_iNumTeams;

	static int				m_iRows;
	static int				m_iSortedRows[NUM_ROWS];
	static int				m_iIsATeam[NUM_ROWS];
	static bool				m_bHasBeenSorted[MAX_PLAYERS];

	CColumnStructure()
	{
		SetColumnSize();
	}
	virtual void SortTeams(){};
	virtual void SortPlayers(int team_type, int team_number){};

	virtual void SetColumnSize()
	{
		m_ColumnInfo[0].SetColumn(NULL,	     	16,		Label::a_east);
		m_ColumnInfo[1].SetColumn(NULL,			118,	Label::a_west);
		m_ColumnInfo[2].SetColumn(NULL,			53,		Label::a_east);
		m_ColumnInfo[3].SetColumn(NULL,			44,		Label::a_east);
		m_ColumnInfo[4].SetColumn(NULL,			44,		Label::a_east);
		m_ColumnInfo[5].SetColumn(NULL,			44,		Label::a_east);
		m_ColumnInfo[6].SetColumn(NULL,			44,		Label::a_east);
		m_ColumnInfo[7].SetColumn(NULL,			44,		Label::a_east);
		m_ColumnInfo[8].SetColumn(NULL,			44,		Label::a_east);
		m_ColumnInfo[9].SetColumn(NULL,			16,		Label::a_east);
	}
	SBColumnInfo& operator[] (int nColumn)
	{
		return m_ColumnInfo[nColumn];
	}
	SBColumnInfo m_ColumnInfo[NUM_COLUMNS];
};

class CColumnFFA : public CColumnStructure
{
public:
	CColumnFFA() : CColumnStructure()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle(NULL);
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
		
	virtual void SortPlayers(int team_type, int team_number)
	{
		int is_spectator = 1;
		bool bCreatedTeam = false;

		while ( 1 )
		{
			// Find the top ranking player

			int highest_score = -99999;	int lowest_deaths = 99999;
			int best_player;
	//		int best_team;
			
			best_player = 0;

			for ( int i = 1; i < MAX_PLAYERS; i++ )
			{
				if ( m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_score /*&& g_PlayerExtraInfo[i].spectator == is_spectator*/)
				{
					cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

					//if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
					if ( ent && team_number ==   g_PlayerExtraInfo[i].teamnumber && (team_number || team_type==TEAM_SPECTATORS || team_type==TEAM_NO))
					{
						extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
										
						if ( pl_info->frags > highest_score || pl_info->deaths < lowest_deaths )
						{
							best_player = i;
							lowest_deaths = pl_info->deaths;
							highest_score = pl_info->frags;
						}
					}
				}
			}
		
		
			if ( !best_player )
			{
			/*	if(is_spectator==1)
				{
					is_spectator=0;
					continue;
				}
				else*/
					break;
			}

			// If we haven't created the Team yet, do it first
			if (!bCreatedTeam && team_type)
			{
				m_iIsATeam[ m_iRows ] = team_type;
				m_iRows++;
				bCreatedTeam = true;
			}

			// Put this player in the sorted list
			m_iSortedRows[ m_iRows ] = best_player;
			m_bHasBeenSorted[ best_player ] = true;
			m_iRows++;
		}
		if (team_number )
		{
			m_iIsATeam[m_iRows++] = TEAM_BLANK;
		}
	}
};

class CColumnACTION : public CColumnFFA
{
public:	
	CColumnACTION()
	{

		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle(NULL);
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
};

class CColumnPRACTICE : public CColumnStructure
{

public:	

	CColumnPRACTICE()
	{

		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle(NULL);
		m_ColumnInfo[5].SetTitle(NULL);
		m_ColumnInfo[6].SetTitle(NULL);
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
};

class CColumnDUEL : public CColumnStructure
{
public:	
	CColumnDUEL()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle("#WINS");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}

	virtual void SortPlayers(int team_type, int team_number)
	{
		int is_spectator = 1;
		bool bCreatedTeam = false;

		while ( 1 )
		{
			// Find the top ranking player
			int highest_score = -99999;	int lowest_deaths = 99999;
			int best_player;
			
			best_player = 0;

			for ( int i = 1; i < MAX_PLAYERS; i++ )
			{
				if ( m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].aux0 >= highest_score && g_PlayerExtraInfo[i].spectator == is_spectator)
				{
					cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

					//if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
					if ( ent && team_number ==   g_PlayerExtraInfo[i].teamnumber && (team_number || team_type==TEAM_SPECTATORS || team_type==TEAM_NO))
					{
						extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
										
						if ( pl_info->aux0 > highest_score || pl_info->deaths < lowest_deaths )
						{
							best_player = i;
							lowest_deaths = pl_info->deaths;
							highest_score = pl_info->aux0;
						}
					}
				}
			}
		
		
			if ( !best_player )
			{
				if(is_spectator==1)
				{
					is_spectator=0;
					continue;
				}
				else
					break;
			}

			// If we haven't created the Team yet, do it first
			if (!bCreatedTeam && team_type)
			{
				m_iIsATeam[ m_iRows ] = team_type;
				m_iRows++;
					bCreatedTeam = true;
			}

			// Put this player in the sorted list
			m_iSortedRows[ m_iRows ] = best_player;
			m_bHasBeenSorted[ best_player ] = true;
			m_iRows++;
		}
		if (team_number)
		{
			m_iIsATeam[m_iRows++] = TEAM_BLANK;
		}
	}
};

class CColumnTOURNAMENT : public CColumnFFA
{
public:
	CColumnTOURNAMENT()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle("#RECORD");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
};

class CColumnTEAM : public CColumnFFA
{
public:	
	CColumnTEAM()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle(NULL);
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
	virtual void SortTeams()
	{
				while ( 1 )
		{
			int hightest_score = -99999;
			int lowest_deaths	= 99999;
		
			int best_team = 0;

			for (int i = 1; i <= m_iNumTeams; i++ )
			{  
				if ( g_TeamInfo[i].players < 1 )
					continue;
				if ( !g_TeamInfo[i].already_drawn && g_TeamInfo[i].frags >= hightest_score )
				{
					if ( g_TeamInfo[i].frags > hightest_score || g_TeamInfo[i].deaths < lowest_deaths )
					{
						best_team = i;
						lowest_deaths = g_TeamInfo[i].deaths;
						hightest_score = g_TeamInfo[i].frags;
						}
				}
			}
	
			if ( !best_team )
				break;


			m_iSortedRows[ m_iRows ] = best_team;
			m_iIsATeam[ m_iRows ] = TEAM_YES;
			g_TeamInfo[best_team].already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get sorted again
			m_iRows++;
			// Now sort all the players on this team
			SortPlayers(TEAM_NO, g_TeamInfo[best_team].teamnumber );
		}

	}
};

class CColumnCTF : public CColumnStructure
{
public:
	CColumnCTF()
	{
		
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle("#CAPTURES");
		m_ColumnInfo[4].SetTitle("#SAVES");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);		
	}
	virtual void SortTeams()
	{
		while ( 1 )
		{
			int hightest_score = -99999;
			int lowest_deaths	= 99999;
		
			int best_team = 0;

			for (int i = 1; i <= m_iNumTeams; i++ )
			{
				if ( g_TeamInfo[i].players < 1 )
					continue;
				if ( !g_TeamInfo[i].already_drawn && g_TeamInfo[i].aux1 >= hightest_score )
				{
					if ( g_TeamInfo[i].aux1 > hightest_score || g_TeamInfo[i].deaths < lowest_deaths )
					{
						best_team = i;
						lowest_deaths = g_TeamInfo[i].deaths;
						hightest_score = g_TeamInfo[i].aux1;
						}
				}
			}
	
			if ( !best_team )
				break;


			m_iSortedRows[ m_iRows ] = best_team;
			m_iIsATeam[ m_iRows ] = TEAM_YES;
			g_TeamInfo[best_team].already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get sorted again
			m_iRows++;
			// Now sort all the players on this team
			SortPlayers(TEAM_NO, g_TeamInfo[best_team].teamnumber );
		}
	}
	virtual void SortPlayers(int team_type, int team_number)
	{
		int is_spectator = 1;
		bool bCreatedTeam = false;

		while ( 1 )
		{
			// Find the top ranking player
			int highest_score = -99999;	int lowest_deaths = 99999;
			int best_player;
			
			best_player = 0;

			for ( int i = 1; i < MAX_PLAYERS; i++ )
			{
				if ( m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].aux1 >= highest_score && g_PlayerExtraInfo[i].spectator == is_spectator)
				{
					cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

					//if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
					if ( ent && team_number ==   g_PlayerExtraInfo[i].teamnumber && (team_number || team_type==TEAM_SPECTATORS))
					{
						extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
										
						if ( pl_info->aux1 > highest_score || pl_info->deaths < lowest_deaths )
						{
							best_player = i;
							lowest_deaths = pl_info->deaths;
							highest_score = pl_info->aux1;
						}
					}
				}
			}
		
		
			if ( !best_player )
			{
				if(is_spectator==1)
				{
					is_spectator=0;
					continue;
				}
				else
					break;
			}

			// If we haven't created the Team yet, do it first
			if (!bCreatedTeam && team_type)
			{
				m_iIsATeam[ m_iRows ] = team_type;
				m_iRows++;
					bCreatedTeam = true;
			}

			// Put this player in the sorted list
			m_iSortedRows[ m_iRows ] = best_player;
			m_bHasBeenSorted[ best_player ] = true;
			m_iRows++;
		}
		if (team_number)
		{
			m_iIsATeam[m_iRows++] = TEAM_BLANK;
		}
	}
};

class CColumnCTP : public CColumnDUEL
{
public:
	CColumnCTP()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle("#TIME");
		m_ColumnInfo[4].SetTitle("#CAPTURES");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);		
	}
	virtual void SortTeams()
	{
		while ( 1 )
		{
			int hightest_score = -99999;
			int lowest_deaths	= 99999;
		
			int best_team = 0;

			for (int i = 1; i <= m_iNumTeams; i++ )
			{
				if ( g_TeamInfo[i].players < 1 )
					continue;	//

				if(!g_TeamInfo[i].already_drawn && CHudCtp::g_CtpScores[g_TeamInfo[i].teamnumber-1] >= hightest_score)
				{

					if(CHudCtp::g_CtpScores[g_TeamInfo[i].teamnumber-1]>hightest_score ) // && i < 5
					{
						best_team = i;
						hightest_score = CHudCtp::g_CtpScores[g_TeamInfo[i].teamnumber-1];
					}
				}
			}
	
			if ( !best_team )
				break;


			m_iSortedRows[ m_iRows ] = best_team;
			m_iIsATeam[ m_iRows ] = TEAM_YES;
			g_TeamInfo[best_team].already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get sorted again
			m_iRows++;
			// Now sort all the players on this team
			SortPlayers(TEAM_NO, g_TeamInfo[best_team].teamnumber );
		}
	}
};

class CColumnLMS : public CColumnDUEL
{
public:	
	CColumnLMS()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle("#WINS");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
};

class CColumnTLMS : public CColumnTEAM
{
public:	
	CColumnTLMS()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle("#WINS");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
	virtual void SortTeams()
	{
		while ( 1 )
		{
			int hightest_score = -99999;
			int lowest_deaths	= 99999;
		
			int best_team = 0;

			for (int i = 1; i <= m_iNumTeams; i++ )
			{
				if ( g_TeamInfo[i].players < 1 )
					continue;

				if(!g_TeamInfo[i].already_drawn && CHudTLms::g_TeamScores[g_TeamInfo[i].teamnumber-1][0] >= hightest_score)
				{

					if(CHudTLms::g_TeamScores[g_TeamInfo[i].teamnumber-1][0]>hightest_score ) // && i < 5
					{
						best_team = i;
						hightest_score = CHudTLms::g_TeamScores[g_TeamInfo[i].teamnumber-1][0];
					}
				}
			}
	
			if ( !best_team )
				break;


			m_iSortedRows[ m_iRows ] = best_team;
			m_iIsATeam[ m_iRows ] = TEAM_YES;
			g_TeamInfo[best_team].already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get sorted again
			m_iRows++;
			// Now sort all the players on this team
			SortPlayers(TEAM_NO, g_TeamInfo[best_team].teamnumber );
		}
	}
};

class CColumnTDuel : public CColumnTLMS
{
public:	
	CColumnTDuel()
	{
		m_ColumnInfo[0].SetTitle(NULL);
		m_ColumnInfo[1].SetTitle(NULL);
		m_ColumnInfo[2].SetTitle("#ID");
		m_ColumnInfo[3].SetTitle(NULL);
		m_ColumnInfo[4].SetTitle("#RECORD");
		m_ColumnInfo[5].SetTitle("#SCORE");
		m_ColumnInfo[6].SetTitle("#DEATHS");
		m_ColumnInfo[7].SetTitle("#LATENCY");
		m_ColumnInfo[8].SetTitle("#VOICE");
		m_ColumnInfo[9].SetTitle( NULL);
	}
};



class CTextImage2 : public Image
{
public:
	CTextImage2()
	{
		_image[0] = new TextImage("");
		_image[1] = new TextImage("");
	}

	~CTextImage2()
	{
		delete _image[0];
		delete _image[1];
	}

	TextImage *GetImage(int image)
	{
		return _image[image];
	}

	void getSize(int &wide, int &tall)
	{
		int w1, w2, t1, t2;
		_image[0]->getTextSize(w1, t1);
		_image[1]->getTextSize(w2, t2);

		wide = w1 + w2;
		tall = max(t1, t2);
		setSize(wide, tall);
	}

	void doPaint(Panel *panel)
	{
		_image[0]->doPaint(panel);
		_image[1]->doPaint(panel);
	}

	void setPos(int x, int y)
	{
		_image[0]->setPos(x, y);
		
		int swide, stall;
		_image[0]->getSize(swide, stall);

		int wide, tall;
		_image[1]->getSize(wide, tall);
		_image[1]->setPos(x + wide, y + (stall * 0.9) - tall);
	}

	void setColor(Color color)
	{
		_image[0]->setColor(color);
	}

	void setColor2(Color color)
	{
		_image[1]->setColor(color);
	}

private:
	TextImage *_image[2];

};

//-----------------------------------------------------------------------------
// Purpose: Custom label for cells in the Scoreboard's Table Header
//-----------------------------------------------------------------------------
class CLabelHeader : public Label
{
public:
	CLabelHeader() : Label("")
	{
		_dualImage = new CTextImage2();
		_dualImage->setColor2(Color(255, 170, 0, 0));
		_row = -2;
		_useFgColorAsImageColor = true;
		_offset[0] = 0;
		_offset[1] = 0;
	}

	~CLabelHeader()
	{
		delete _dualImage;
	}

	void setRow(int row)
	{
		_row = row;
	}

	void setFgColorAsImageColor(bool state)
	{
		_useFgColorAsImageColor = state;
	}

	virtual void setText(int textBufferLen, const char* text)
	{
		_dualImage->GetImage(0)->setText(text);

		// calculate the text size
		Font *font = _dualImage->GetImage(0)->getFont();
		_gap = 0;
		for (const char *ch = text; *ch != 0; ch++)
		{
			int a, b, c;
			font->getCharABCwide(*ch, a, b, c);
			_gap += (a + b + c);
		}

		_gap += XRES(5);
	}

	virtual void setText(const char* text)
	{
		// strip any non-alnum characters from the end
		char buf[512];
		strcpy(buf, text);

		int len = strlen(buf);
		while (len && isspace(buf[--len]))
		{
			buf[len] = 0;
		}

		CLabelHeader::setText(0, buf);
	}

	void setText2(const char *text)
	{
		_dualImage->GetImage(1)->setText(text);
	}

	void getTextSize(int &wide, int &tall)
	{
		_dualImage->getSize(wide, tall);
	}

	void setFgColor(int r,int g,int b,int a)
	{
		// here
		Label::setFgColor(r,g,b,a);
		Color color(r,g,b,a);
		_dualImage->setColor(color);
		_dualImage->setColor2(color);
		repaint();
	}

	void setFgColor(Scheme::SchemeColor sc)
	{
		int r, g, b, a;
		Label::setFgColor(sc);
		Label::getFgColor( r, g, b, a );

		// Call the r,g,b,a version so it sets the color in the dualImage..
		setFgColor( r, g, b, a );
	}

	void setFont(Font *font)
	{
		_dualImage->GetImage(0)->setFont(font);
	}

	void setFont2(Font *font)
	{
		_dualImage->GetImage(1)->setFont(font);
	}

	// this adjust the absolute position of the text after alignment is calculated
	void setTextOffset(int x, int y)
	{
		_offset[0] = x;
		_offset[1] = y;
	}

	void paint();
	void paintBackground();
	void calcAlignment(int iwide, int itall, int &x, int &y);

private:
	CTextImage2 *_dualImage;
	int _row;
	int _gap;
	int _offset[2];
	bool _useFgColorAsImageColor;
};

class ScoreTablePanel;

#include "..\game_shared\vgui_grid.h"
#include "..\game_shared\vgui_defaultinputsignal.h"

//-----------------------------------------------------------------------------
// Purpose: Scoreboard back panel
//-----------------------------------------------------------------------------
class ScorePanel : public Panel, public vgui::CDefaultInputSignal
{
private:
	// Default panel implementation doesn't forward mouse messages when there is no cursor and we need them.
	
	

	class HitTestPanel : public Panel
	{
	public:
		virtual void	internalMousePressed(MouseCode code);
	};


private:

	BitmapTGA* pTGAFlag;
	BitmapTGA* pTGAEye;
	BitmapTGA* pTGARemoved;

	Label			m_TitleLabel;
	
	// Here is how these controls are arranged hierarchically.
	// m_HeaderGrid
	//     m_HeaderLabels

	// m_PlayerGridScroll
	//     m_PlayerGrid
	//         m_PlayerEntries 

	CGrid			m_HeaderGrid;
	CLabelHeader	m_HeaderLabels[NUM_COLUMNS];			// Labels above the 
	CLabelHeader	*m_pCurrentHighlightLabel;
	int				m_iHighlightRow;
	
	vgui::CListBox	m_PlayerList;
	CGrid			m_PlayerGrids[NUM_ROWS];				// The grid with player and team info. 
	CLabelHeader	m_PlayerEntries[NUM_COLUMNS][NUM_ROWS];	// Labels for the grid entries.

	ScorePanel::HitTestPanel	m_HitTestPanel;
	CommandButton				*m_pCloseButton;		
	CLabelHeader*	GetPlayerEntry(int x, int y)	{return &m_PlayerEntries[x][y];}
	
	int				m_nSortType;

public:
	
	CColumnStructure* m_pColumnInfo;
	//int				m_iNumTeams;
	int				m_iPlayerNum;
	int				m_iShowscoresHeld;

	/*int				m_iRows;
	int				m_iSortedRows[NUM_ROWS];
	int				m_iIsATeam[NUM_ROWS];
	bool			m_bHasBeenSorted[MAX_PLAYERS];*/

	int				m_iLastKilledBy;
	int				m_fLastKillTime;

public:

	virtual ~ScorePanel()
	{
		if(m_pColumnInfo)
			delete m_pColumnInfo;
	}
	ScorePanel(int x,int y,int wide,int tall);

	void InstallColumns(char* pszGameMode, char* pszVersion, char* pszServerName);
	void Update( void );

	void SortTeams( void );
	//void SortPlayers( int iTeam, char *team );
	void RebuildTeams( void );
	
	void FillGrid();

	void DeathMsg( int killer, int victim );

	void Initialize( void );

	void Open( void );

	void MouseOverCell(int row, int col);

// InputSignal overrides.
public:

	virtual void mousePressed(MouseCode code, Panel* panel);
	virtual void cursorMoved(int x, int y, Panel *panel);

	friend CLabelHeader;
};

#endif


