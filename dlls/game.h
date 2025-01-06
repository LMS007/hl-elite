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

#ifndef GAME_H
#define GAME_H

extern void GameDLLInit( void );
/*
enum {

   	FFA=0,
	ACTION,
	DUEL,
	TOURNAMENT,
	CTF,
	TEAM,
	LMS
};*/

enum {

   	FORCE_ALL=0,
	FORCE_CTF,
	FORCE_DUEL,
	FORCE_TOURNAMENT
};

#define HLE_VERSION "1.0.6"
#define HLE_DEBUG
#define HLE_DEBUG2


extern cvar_t	displaysoundlist;

extern int g_gamemode; //hle game mode flag
// multiplayer server rules

//extern cvar_t	tourneyspawn;
//extern cvar_t	tourneytime;
//extern cvar_t	lmstime;
//extern cvar_t	dueltime;


extern cvar_t	teamplay;
extern cvar_t	satchelexplode;
extern cvar_t	fraglimit;
extern cvar_t	timelimit;
extern cvar_t	friendlyfire;
extern cvar_t	falldamage;
extern cvar_t	weaponstay;
extern cvar_t	flaghandicap;
extern cvar_t	forcerespawn;
extern cvar_t   loselongjump;
extern cvar_t	flashlight;
extern cvar_t	aimcrosshair;
extern cvar_t	decalfrequency;
extern cvar_t	teamlist;
extern cvar_t	teamoverride;
extern cvar_t	lockteams;
//extern cvar_t	defaultteam;
extern cvar_t	allowmonsters;
extern cvar_t	reset;
extern cvar_t   resetloaded;
//extern cvar_t	allow_spectators;	// hle edit 
//extern cvar_t	allowa3d;
extern cvar_t	maxvotetime;
extern cvar_t	pausegame;
extern cvar_t	corpsequeue;
extern cvar_t	nextmap;
extern cvar_t	banweapons;
extern cvar_t	overwrite_nextmap;
// settings/options
//extern cvar_t	admit;


extern cvar_t	winlimit;
extern cvar_t	spawnlimit;
extern cvar_t	gamemode;
extern cvar_t	modeconfig;
extern cvar_t	resumetime;
extern cvar_t   allowmatchvotes;


extern cvar_t	roundtime;
extern cvar_t	banmodes;
extern cvar_t	spawnform;
extern cvar_t	banentities;
extern cvar_t	radar;
extern cvar_t	showlongjump;

extern cvar_t	votetime;
//extern cvar_t	tossitem;
//extern cvar_t disablea3dperiod;
extern cvar_t	regeneration;
//extern cvar_t	allowop4;
extern cvar_t maxteams;
extern cvar_t	matchstart;
extern cvar_t	tournament;
extern cvar_t	allowvotes;



//extern cvar_t	loadedrestart;

// Engine Cvars
extern cvar_t   *g_maxspeed;
extern cvar_t	*g_psv_gravity;
extern cvar_t	*g_psv_aim;
extern cvar_t	*g_footsteps;


#endif		// GAME_H
