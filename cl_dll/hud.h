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
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//


#ifndef HUD_H
#define HUD_H

#define RGB_YELLOWISH 0x0073CCFF//0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FFE613//0x00FF1010 //255,160,0
#define RGB_GREENISH 0x0000A000 //0,160,0

#include "wrect.h"
#include "cl_dll.h"
#include "ammo.h"

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4
#define MIN_ALPHA	 200
#define HLE_FADE 50.0f


#define		HUDELEM_ACTIVE	1

#define MAXCOLORS 1536


typedef struct {
	int x, y;
} POSITION;

enum 
{ 
	MAX_PLAYERS = 64,
	MAX_TEAMS = 64, // change this!
	MAX_TEAM_NAME = 16,
};

class CHudColor
{
	int r, g, b;
	int R, G, B;
	int brightness;
	void UpdateColors();

public:

	bool m_bLockColors;

	static int maxColor(int r, int g, int b);
	static int minColor(int r, int g, int b);
	static void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
	static void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );

	CHudColor();
	CHudColor(int r, int g, int b, int a);
	
	void setInverts(int* r, int* g, int *b);
	int getRed();
	int getGreen();
	int getBlue();
	int getPureRed();
	int getPureGreen();
	int getPureBlue();
	void setRed( int);
	void setGreen( int);
	void setBlue( int);
	void setBrightness(int);
	void setColors(int);
};

extern CHudColor g_hud_color;

enum {

   	FFA=0,
	ACTION,
	DUEL,
	TOURNAMENT,
	CTF,
	TEAM,
	LMS,
	CTP,
	T_LMS,
	PRACTICE
};

typedef struct {
	unsigned char r,g,b,a;
} RGBA;

typedef struct cvar_s cvar_t;


#define HUD_ACTIVE	1
#define HUD_INTERMISSION 2

#define MAX_PLAYER_NAME_LENGTH		32

#define	MAX_MOTD_LENGTH				1536

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	POSITION  m_pos;
	int   m_type;
	int	  m_iFlags; // active, moving, 
	virtual		~CHudBase() {}
	virtual int Init( void ) {return 0;}
	virtual int VidInit( void ) {return 0;}
	virtual int Draw(float flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to

};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};



//
//-----------------------------------------------------
//
#include "..\game_shared\voice_status.h"
#include "hud_spectator.h"


//
//-----------------------------------------------------
//



class CHudAmmo: public CHudBase
{
public:


	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);
	int MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_WeapPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ItemPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ChangedColor( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_HideWeapon( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Zoom( const char *pszName, int iSize, void *pbuf );
	void ChangeXhairColor(  );
	

	void SlotInput( int iSlot );
	void _cdecl UserCmd_Slot1( void );
	void _cdecl UserCmd_Slot2( void );
	void _cdecl UserCmd_Slot3( void );
	void _cdecl UserCmd_Slot4( void );
	void _cdecl UserCmd_Slot5( void );
	void _cdecl UserCmd_Slot6( void );
	void _cdecl UserCmd_Slot7( void );
	void _cdecl UserCmd_Slot8( void );
	void _cdecl UserCmd_Slot9( void );
	void _cdecl UserCmd_Slot10( void );
	void _cdecl UserCmd_Close( void );
	void _cdecl UserCmd_NextWeapon( void );
	void _cdecl UserCmd_PrevWeapon( void );

private:
	float m_fFade;
	RGBA  m_rgba;
	WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

};

//
//-----------------------------------------------------
//

class CHudAmmoSecondary: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	int Draw(float flTime);

	int MsgFunc_SecAmmoVal( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_SecAmmoIcon( const char *pszName, int iSize, void *pbuf );

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};


#include "health.h"


#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Geiger(const char *pszName, int iSize, void *pbuf);
	
private:
	int m_iGeigerRange;

};


//
//-----------------------------------------------------
//
class CHudMapList: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_MapNames(const char *pszName, int iSize, void *pbuf);
	void InitHUDData( void ) ;
	//void Reset();
private:
	bool m_bNoMoreMaps;

};


//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Train(const char *pszName, int iSize, void *pbuf);

private:
	SpriteHandle_t m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
// REMOVED: Vgui has replaced this.
//
/*
class CHudMOTD : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );

	int MsgFunc_MOTD( const char *pszName, int iSize, void *pbuf );

protected:
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];
	float m_flActiveRemaining;
	int m_iLines;
};
*/

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );
	void ParseStatusString( int line_num );

	int MsgFunc_StatusText( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_StatusValue( const char *pszName, int iSize, void *pbuf );

protected:
	enum { 
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];  // a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	// the constructed bar that is drawn
	int m_iStatusValues[MAX_STATUSBAR_VALUES];  // an array of values for use in the status bar

	int m_bReparseString; // set to TRUE whenever the m_szStatusBar needs to be recalculated

	// an array of colors...one color for each line
	float *m_pflNameColors[MAX_STATUSBAR_LINES];
};

//
//-----------------------------------------------------
//
// REMOVED: Vgui has replaced this.
//
/*
class CHudScoreboard: public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int DrawPlayers( int xoffset, float listslot, int nameoffset = 0, char *team = NULL ); // returns the ypos where it finishes drawing
	void UserCmd_ShowScores( void );
	void UserCmd_HideScores( void );
zz	int MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf );
	void DeathMsg( int killer, int victim );

	int m_iNumTeams;

	int m_iLastKilledBy;
	int m_fLastKillTime;
	int m_iPlayerNum;
	int m_iShowscoresHeld;

	void GetAllPlayersInfo( void );
private:
	struct cvar_s *cl_showpacketloss;

};
*/

struct extra_player_info_t 
{
	
	short aux0;
	short aux1;
	short aux2;
	short flag_color;
	short frags;
	short spectator;
	short deaths;
	short playerclass;
	short teamnumber;
	char playerID[32];
	char teamname[MAX_TEAM_NAME];
};

struct team_info_t 
{
	char name[MAX_TEAM_NAME];
	short frags;
	short deaths;
	short ping;
	short packetloss;
	short ownteam;
	short players;
	int already_drawn;
	int scores_overriden;
	int teamnumber;
	short aux0; 
	short aux1; 
};

extern hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];   // additional player info sent directly to the client dll
extern team_info_t			g_TeamInfo[MAX_TEAMS+1];
extern int					g_IsSpectator[MAX_PLAYERS+1];


//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_d_skull;  // sprite index of skull icon
};

//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	void Reset( void );
	int Draw( float flTime );
	int MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf );

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	int m_fWaitingForMore;
};

//
//-----------------------------------------------------
//
class CHudSayText : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int MsgFunc_SayText( const char *pszName, int iSize, void *pbuf );
	void SayTextPrint( const char *pszBuf, int iBufSize, int clientIndex = -1 );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
	
	friend class CHudSpectator;

	private:

	struct cvar_s *	m_HUD_saytext;
	struct cvar_s *	m_HUD_saytext_time;

};

//
//-----------------------------------------------------
//
class CHudBattery: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf );
	wrect_t *m_prc1;
private:
	SpriteHandle_t m_hSprite1;
	SpriteHandle_t m_hSprite2;
	
	wrect_t *m_prc2;
	int	  m_iBat;	
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Reset( void );
	int MsgFunc_Flashlight(const char *pszName,  int iSize, void *pbuf );
	int MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf );
	
private:
	SpriteHandle_t m_hSprite1;
	SpriteHandle_t m_hSprite2;
	SpriteHandle_t m_hBeam;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	wrect_t *m_prcBeam;
	float m_flBat;	
	int	  m_iBat;	
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage: public CHudBase
{
public:
	int Init( void );
	static char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	static char *BufferedLocaliseTextString( const char *msg );
	char *LookupString( const char *msg_name, int *msg_dest = NULL );
	int MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_HudText(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_GameTitle(const char *pszName, int iSize, void *pbuf);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( const char *pName, float time );
	void MessageAdd(client_textmessage_t * newMessage );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH	24

class CHudStatusIcons: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	int Draw(float flTime);
	int MsgFunc_StatusIcon(const char *pszName, int iSize, void *pbuf);

	enum { 
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

	
	//had to make these public so CHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon( char *pszIconName, unsigned char red, unsigned char green, unsigned char blue );
	void DisableIcon( char *pszIconName );

private:

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		SpriteHandle_t spr;
		wrect_t rc;
		unsigned char r, g, b;
	} icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];

};


class CHudCountDown: public CHudBase
{
public:

	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_CountNumber(const char *pszName, int iSize, void *pbuf);
	float m_fHold;

private:

	float m_fTime;
	char m_ccSounds[6][32];
	int m_nIndex;
	float m_nIntensity;
	SpriteHandle_t m_hNumber[6];


};
/*
class CHColor
{
public:
	int r, g, b;
	CHudColor(int r, int g, int b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	CHudColor()
	{
		r = b = g = 0;
	}
};*/

class CHudSprite
{

	SpriteHandle_t sprite;
	int x;
	int y;
	wrect_t source;
	float intensity;
	int r, g, b;
	float time;
	float hold;
	float speed;
	bool active;
	int render_method;

public:

	CHudSprite();
	void SetRenderMethod(int);
	void SetSprite(char* pSprite);
	void SetActive(bool bActive);
	void SetDest(int, int);
	void SetSource(int left, int top, int right, int bottom);
	void StartFade(float speed, float hold=0);
	void ResetAnimation();
	void SetColor(CHudColor* color);
	void GetColor(CHudColor*);
	void GetDest(int*, int*);
	SpriteHandle_t* GetSprite();
	int Draw();
};


class CHudDuel: public CHudBase
{
public:

	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Bonus(const char *pszName, int iSize, void *pbuf);
	

private:

	int x, y, r, g, b;
	float m_fTime;
	float m_fTimeFactor;
	int m_nIndex;
	float m_nIntensity;
	SpriteHandle_t m_hOwned;
	SpriteHandle_t m_hFlawless;
	SpriteHandle_t m_hImmortal;
	SpriteHandle_t m_hPunish;
	SpriteHandle_t m_hLuck;
	SpriteHandle_t m_hAnninilation;
	SpriteHandle_t m_hSpree;
	SpriteHandle_t m_hDomination;
	SpriteHandle_t *m_pSprite;
};


class CHudTLms: public CHudBase
{
public:

	int Init( void );
	int VidInit( void );
	//int Draw(float flTime);
	int MsgFunc_TeamScores(const char *pszName, int iSize, void *pbuf);
	static int g_TeamScores[6][6];

private:
	
	int m_nTeamsLeft;
	//float m_fTime;
	//float m_nIntensity;
};

class CHudLms: public CHudBase
{
public:

	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_MenLeft(const char *pszName, int iSize, void *pbuf);

private:
	
	int x, y;
	int m_nMenLeft;
	float m_fTime;
	float m_nIntensity;
	SpriteHandle_t m_hMenLeft;
};
class CHudTournament: public CHudBase
{
public:

	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_FragsLeft(const char *pszName, int iSize, void *pbuf);

private:

	int x, y;
	int m_nFragsLeft;
	float m_fTime;
	float m_nIntensity;
	SpriteHandle_t m_hFragsLeft;
};


class CHudStatus: public CHudBase
{
public:
	
	bool m_matchInProgress;
	bool m_ShowRemoved;
	bool m_ShowMatch;

	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_HudStatus(const char *pszName, int iSize, void *pbuf);;

	void ResetColors();
	void ToggleBreak();
	void ToggleLock();

private:

	CHudSprite m_hsRemoved;
	CHudSprite m_hsLocked;

};


/*class CHudLms public CHudBase
{
public:
	
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_LmsEvent(const char *pszName, int iSize, void *pbuf);

private:
	
	SpriteHandle_t m_Standing;
	SpriteHandle_t m_Finish;

};*/

class CHudAnounce: public CHudBase
{
public:


	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Anounce(const char *pszName, int iSize, void *pbuf);

private:
	
	
	CHudSprite m_hsTeams[6];

	CHudSprite m_hsTeamWins[6];

	CHudSprite m_hsLTS;
	CHudSprite m_hsNoLTS;
	CHudSprite m_hsVersus;
	CHudSprite m_hsTie;
	CHudSprite m_hsTimeup;

};

class CHudCtf: public CHudBase
{
public:


	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_CtfEvent(const char *pszName, int iSize, void *pbuf);

private:

	int flash;
	CHudSprite* flags[6];
	int sprite_heigth;
	CHudSprite m_hsIndicator;
	CHudSprite* m_hsIndex;
	CHudSprite m_hsReturned[6];
	CHudSprite m_hsScored[6];
	CHudSprite m_hsFlagMini[6];
	CHudSprite m_hsFlagMiniHand[6];
};

class CHudCap: public CHudBase
{
public:


	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Cap(const char *pszName, int iSize, void *pbuf);

private:

	SpriteHandle_t m_plus;
	
	float m_fIntensity;
	bool m_bActive;
	bool m_bFade;
	int x0;
	int x1;
	int y;
	int r, g, b, a;
	float m_fFade;
	int m_nHevCapacity;
	int m_nHealthCapacity;

};


class CHudCtp: public CHudBase
{
public:


	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_CtpEvent(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_CtpTime(const char *pszName, int iSize, void *pbuf);
	void UpdateScores();
	void CalcRecs();
	static int g_CtpScores[6];


private:

	int nMyScore;
	int x, y;
	float m_fFade;
	SpriteHandle_t m_hsFlag;
	SpriteHandle_t m_hX;
	int m_nOldTime;
	int m_nFlags[6];
	int m_nTeam[6];
	int m_nHeight[6];

	int m_nTotalHeight;
	int m_nWidth;

	int m_nTotalFlags;

};



class CHudFlag: public CHudBase
{
public:

	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_CtfFlag(const char *pszName, int iSize, void *pbuf);

private:
	
	CHudSprite m_hsRedFlag;
	CHudSprite m_hsBlueFlag;
	CHudSprite m_hsYellowFlag;
	CHudSprite m_hsGreenFlag;
	CHudSprite m_hsOrangeFlag;
	CHudSprite m_hsPurpleFlag;

};

class CHudCountDown2: public CHudBase
{
public:

	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void BlitNumber(int);
	void Blit60();

private:

	bool m_bDraw60;
	float m_fTime;
	int m_nIndex;
	float m_nIntensity;
	SpriteHandle_t m_h60Seconds;
	SpriteHandle_t m_hNumber[5];
};

class CHudClock: public CHudBase
{
public:

	virtual void RebuildTime(float);
	virtual int Init( void );
	virtual int VidInit( void );
	virtual int Draw(float flTime);
	virtual void UpdateTime();

	int MsgFunc_TimeLeft(const char *pszName, int iSize, void *pbuf);
	void SixtySeconds();

protected:

	int seconds, minutes, hours, days;
	
	bool m_bTimeLimit;
	SpriteHandle_t m_hColen;
	SpriteHandle_t m_hClock;
	
	int m_nOldTime;
	int m_nTimeLeft;
	int m_nTime;

	float m_nIntensity;
	float m_n60Intensity;
};


class CHudTimer: public CHudClock
{

public:
	
	//int VidInit(void);
	int Init( void );
	int MsgFunc_Timer(const char *pszName, int iSize, void *pbuf);
	void UpdateTime();

private:

};

class CHudMp3: public CHudClock
{
	
	SpriteHandle_t m_hPercent;
	SpriteHandle_t m_hVolume;
	int m_pVolume;
	float m_fDrawTime;
	float m_nIntensity;
public:

	int VidInit(void);
	int Init( void );
	void SetVolume(int volume);
	void UpdateTime();

private:

};

class CRadarNode
{

	
public:

	CRadarNode* m_pNext;
	float	m_fPosX;
	float	m_fPosY;
	int		m_nColor;
	int		m_nIntensity; 
	int		m_nID;
	int		m_nPlayer;
	int		m_nSpriteIndex;

	CRadarNode(int id, float x, float y, int color, int player)
	{
		m_nID = id;
		m_fPosX = x;
		m_fPosY = y;
		m_nColor = color;
		m_nIntensity = 255;
		m_nPlayer = player;
		m_pNext = NULL;
	}
};


class CRadarList
{

public:
	
	CRadarList();
	CRadarNode* m_pHead;
	virtual CRadarNode* AddNode(int id, float x, float y, int color, int player);
	virtual void RemoveNode(CRadarNode* node);
	virtual CRadarNode* FindNode(int id);
	void SetFade(int id);
	virtual void ClearNodes();
	~CRadarList();
};


class CHudRadar:  public CHudBase
{
	//int					radius;
	float				m_dist_factor;
	int					m_pos_x, m_pos_y;
	int					radar_origin_x, radar_origin_y;

	float m_fUpdateDelay;
	int   m_radius;

	int		m_player_index;
	int		m_origin_index;
	int		m_radar_index;
	int		m_flag_index;
	int		m_divider_index;

	SpriteHandle_t m_flag_sprite;
	SpriteHandle_t m_player_sprite;
	SpriteHandle_t m_origin_sprite;
	SpriteHandle_t m_radar_sprite;
	SpriteHandle_t m_divider_sprite;

	//SpriteHandle_t	m_radar_sprite;
	//SpriteHandle_t m_hPlayer;
	//SpriteHandle_t m_hFlag;
	//SpriteHandle_t m_hPlayerDistress;
	int     m_nEntIndexOrigin;
	bool	m_bRadarOn;

	int sprite_origin;

	CRadarList	m_flag_list;
	CRadarList	m_player_list;

public:

	CHudRadar();
	void ShowRadar();
	void HideRadar();
	void Reset();
	bool RadarOn();
	void InitHUDData();
	int VidInit( void );
	void CHudRadar::CalcPos(float player_x, float player_y,float* calc_x, float* calc_y);
	int MsgFunc_Radar(const char *pszName, int iSize, void *pbuf);	
	int MsgFunc_Flag(const char *pszName, int iSize, void *pbuf);	
	int MsgFunc_Location(const char *pszName, int iSize, void *pbuf);
	void UpdateRadar();
	int DrawFlag(CRadarNode** pNode, float x, float y);
	int DrawPlayer( int sprite, int x, int y);
	int Init();
	int Draw(float flTime);
};
		



class CHudHleColors : public CHudBase
{
	int m_nOriginalHudColor;
	int m_nOriginalGaussColor;
public:

	//CHudHleColors();
	int Init( void );
	int VidInit( void );
	void InitHUDData( void );
	//void Think();
	int Draw(float flTime);
	//int MsgFunc_GaussColor(const char *pszName, int iSize, void *pbuf);
	//int MsgFunc_HudColor(const char *pszName, int iSize, void *pbuf);	
	int MsgFunc_Color(const char *pszName, int iSize, void *pbuf);	
	int MsgFunc_Reset(const char *pszName, int iSize, void *pbuf);

};

class CHudVgui: public CHudBase
{
public:

	virtual int Init( void );
	virtual int VidInit( void );
	virtual int Draw(float flTime); // VguiThink
	virtual void ActivateVguiThink(bool);
};


class CLocNode
{
public:
	CLocNode();
	vec3_t m_loc;
	char m_szLocation[256];
	CLocNode* m_pNext;
};

class CLocs : CHudBase
{
private:
	vec3_t m_real_loc;
	CLocNode* m_pHead;
public:
	// client commands  "%l"
	// client commands  "%h"
	// client commands  "%a"
	//const char *level = gEngfuncs.pfnGetLevelName();
	int Init();
	void Reset( void );
	void InitHUDData( void );
	int MsgFunc_Locs( const char *pszName, int iSize, void *pbuf );
	char* GetClosestLoc(vec3_t* loc);
	void LoadClientLocs();
	void ClearList();
	~CLocs();

};

class CHud
{
private:
	HUDLIST						*m_pHudList;
	SpriteHandle_t						m_hsprLogo;
	int							m_iLogo;
	client_sprite_t				*m_pSpriteList;
	int							m_iSpriteCount;
	int							m_iSpriteCountAllRes;
	float						m_flMouseSensitivity;
	int							m_iConcussionEffect; 

public:

	SpriteHandle_t						m_hsprCursor;
	float m_flTime;	   // the current client time
	float m_fOldTime;  // the time at which the HUD was last redrawn
	double m_flTimeDelta; // the difference between flTime and fOldTime
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;
	//int		m_Teamplay;
	int		m_iRes;
	cvar_t  *m_pCvarStealMouse;
	cvar_t	*m_pCvarDraw;


	int m_iFontHeight;
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudNumberSmall( int x, int y, int iFlags, int iNumber, int r, int g, int b);
	int DrawHudString(int x, int y, int iMaxX, char *szString, int r, int g, int b );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b );
	int GetNumWidth(int iNumber, int iFlags);


private:
	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	SpriteHandle_t *m_rghSprites;	/*[HUD_SPRITE_COUNT]*/			// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/
	bool m_bWeightSent;
	struct cvar_s *default_fov;
public:
	SpriteHandle_t GetSprite( int index ) 
	{
		return (index < 0) ? 0 : m_rghSprites[index];
	}

	wrect_t& GetSpriteRect( int index )
	{
		return m_rgrcRects[index];
	}

	
	int GetSpriteIndex( const char *SpriteName );	// gets a sprite index, for use in the m_rghSprites[] array

	CHudAmmo		m_Ammo;
	CHudHealth		m_Health;
	CHudSpectator		m_Spectator;
	CHudGeiger		m_Geiger;
	CHudBattery		m_Battery;
	CHudTrain		m_Train;
	CHudFlashlight	m_Flash;
	CHudMessage		m_Message;
	CHudStatusBar   m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText		m_SayText;
	CHudMenu		m_Menu;
	CHudAmmoSecondary	m_AmmoSecondary;
	CHudTextMessage m_TextMessage;
	CHudStatusIcons m_StatusIcons;
	//CHudSpectator   m_Spectator;
    CHudMapList     m_MapList; // hle
	CHudCountDown	m_Count;
	CHudDuel		m_Duel;
	CHudStatus		m_Status;
	CHudTournament	m_Tournament;
	CHudLms			m_Lms;
	CHudTLms		m_Tlms;
	CHudCtf			m_Ctf;
	CHudCap			m_Cap;
	CHudCtp			m_Ctp;
	CHudFlag		m_Flag;
	CHudCountDown2  m_Count2;
	CHudHleColors	m_Color;
	CHudClock		m_Clock;
	CHudTimer		m_Timer;
	CLocs			m_Locs;
	CHudRadar		m_Radar;
	CHudMp3			m_Winamp;
	CHudAnounce		m_Anounce;

	void Init( void );
	void VidInit( void );
	void Think(void);
	int Redraw( float flTime, int intermission );
	int UpdateClientData( client_data_t *cdata, float time );

	CHud() : m_iSpriteCount(0), m_pHudList(NULL) {}  
	~CHud();			// destructor, frees allocated memory

	// user messages
	int _cdecl MsgFunc_Damage(const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf );
//	int _cdecl MsgFunc_Logo(const char *pszName,  int iSize, void *pbuf); // REMOVED BY 007 bcaeuse we need message space
	void _cdecl MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_ResetHUD(const char *pszName,  int iSize, void *pbuf);
	void _cdecl MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf);
//	int _cdecl MsgFunc_Weight(const char *pszName,  int iSize, void *pbuf);
	int  _cdecl MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf );
	int  _cdecl MsgFunc_FixEgon( const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_ChangeXHair( const char *pszName, int iSize, void *pbuf );

	// hle

	// Screen information
	SCREENINFO	m_scrinfo;

	int	m_iWeaponBits;
	int	m_fPlayerDead;
	int m_iIntermission;

	// sprite indexes
	int m_HUD_number_0;


	void AddHudElem(CHudBase *p);

	float GetSensitivity();

};

class TeamFortressViewport;

extern CHud gHUD;
extern TeamFortressViewport *gViewPort;

extern int g_iPlayerClass;
extern int g_iTeamNumber;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;


#endif