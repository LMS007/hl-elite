/*  
 * Half-Life client-side generic hook engine.
 * Copyright (c) Vasily Pupkin (clientbot@narod.ru) 2000-2001
 * Copyright (c) Tim Yunaev (tim@lviv.gu.net) 2000-2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * NOTE:
 * GNU license doesn't cover Engine directory.
 * Content of Engine directory is copyrighted (c) 1999, 2000, by Valve LLC
 * and it is licensed under Valve's proprietary license (see original HL SDK).
 *
 * $Id: menu.h,v 1.2 2001/07/16 11:23:10 vasily Exp $
 */

//#include "..\client.h"

//#include "hook.h"
//#include "superhud.h"

#include "hud.h"
#include "cl_util.h"
//#include "cl_entity.h"

#include <windows.h>
#include "winamp.h"

//#include "util.h"
#include <time.h>

using namespace mp3;

CWinamp gWinamp;

//DECLARE_COMMAND_2(gWinamp, Winamp);

bool CWinamp::Pre_HUD_Init (void)
{
	//HOOK_COMMAND_2("bwinamp", Winamp);
	return true;
}

void CWinamp::Post_HUD_Init (void)
{
}

bool CWinamp::Pre_Hook_SayText (const char *pszString)
{
	return true;
}

void CWinamp::Post_Hook_SayText (const char *pszString)
{
	/*const char *p;
	if(strlen(pszString) > 5)
	{
		p = pszString + strlen(pszString) - 5;
		if(!strcmp(p, "!wut\n"))
		{*/
			char buf[4096];
			HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
			if(hwWinamp)
			{
				if(fLastTriggerTime > gEngfuncs.GetClientTime())
					fLastTriggerTime = gEngfuncs.GetClientTime();
				if((gEngfuncs.GetClientTime() - fLastTriggerTime) > 2)
				{
					fLastTriggerTime = gEngfuncs.GetClientTime();
					sprintf(buf, "say \"[sparky] %s\";", GetCurrentTrack(hwWinamp));
					gEngfuncs.pfnClientCmd(buf);
				}
			}
		//}
	//}
}

char* CWinamp::Cmd_Winamp(int nCommand)
{
	HWND hwWinamp = FindWindow("Winamp v1.x",NULL);
	if(!hwWinamp)
	{
		//gEngfuncs.pfnConsolePrint("[sparky] winamp is not currently running\n");
		return NULL;
	}

	/*if(gEngfuncs.Cmd_Argc() < 2)
	{
		char buf[4096];
		gEngfuncs.pfnConsolePrint("[sparky] valid bwinamp commands: play stop pause next prev\n");
		sprintf(buf, "[sparky] currently playing: %s\n", GetCurrentTrack(hwWinamp));
		gEngfuncs.pfnConsolePrint(buf);
		return;
	}*/

	//if(!strcmp(gEngfuncs.Cmd_Argv(1), "play"))
	switch(nCommand)
	{
	case VOLUMEUP:

		SendMessage(hwWinamp, WM_COMMAND, WINAMP_VOLUMEUP, 0);
		break;

	case VOLUMEDOWN:

		SendMessage(hwWinamp, WM_COMMAND, WINAMP_VOLUMEDOWN, 0);
		break;

	case PLAY:

		SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON2, 0);
		break;

	case STOP:
		SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON4, 0);
		break;

	case PAUSE:
		SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON3, 0);
		break;
	
	case PREV:
		SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON1, 0);
		break;

	case NEXT:
		SendMessage(hwWinamp, WM_COMMAND, WINAMP_BUTTON5, 0);
		break;
	case SHUFFLE_ON:
		SendMessage(hwWinamp,WM_WA_IPC,1,IPC_SET_SHUFFLE);
		break;
	case SHUFFLE_OFF:
		SendMessage(hwWinamp,WM_WA_IPC,0,IPC_SET_SHUFFLE);
		break;

	case REPEAT_ON:
		SendMessage(hwWinamp,WM_WA_IPC,1,IPC_SET_REPEAT);
		break;
	case REPEAT_OFF:
		SendMessage(hwWinamp,WM_WA_IPC,0,IPC_SET_REPEAT);
		break;
	case FAST_FORWARD:
		SendMessage(hwWinamp, WM_COMMAND,FAST_FORWARD,0);
		break;
	case REWIND:
		SendMessage(hwWinamp, WM_COMMAND,WINAMP_REW5S,0);
		break;
		
	default:
		//char buf[4096];
		//gEngfuncs.pfnConsolePrint("[sparky] valid bwinamp commands: play stop pause next prev\n");
		//sprintf(buf, "[sparky] currently playing: %s\n", GetCurrentTrack(hwWinamp));
		//gEngfuncs.pfnConsolePrint(buf);
		break;

	}
	return GetCurrentTrack(hwWinamp);
}

char *CWinamp::GetCurrentTrack(HWND hwWinamp)
{
	static char buf[4096];
	char this_title[2048];
	char *p;
	GetWindowText(hwWinamp,this_title,sizeof(this_title));
	p = this_title+strlen(this_title)-8;
	while (p >= this_title)
	{
		if (!strnicmp(p,"- Winamp",8)) break;
		p--;
	}
	if (p >= this_title) p--;
	while (p >= this_title && *p == ' ') p--;
	*++p=0;

	sprintf(buf, " %s", this_title);
	return buf;
}
	
CWinamp::CWinamp(void)
{
	fLastTriggerTime = 0;
}




