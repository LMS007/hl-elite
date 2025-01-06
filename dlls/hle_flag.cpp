#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "animation.h"
#include "hle_flag.h"
#include "weapons.h"
#include "hle_gamemodes.h"

extern int gmsgItemPickup;
//extern int gmsgCtfFlag;
extern int gmsgCtfEvent;
extern int gmsgScoreInfo;
extern int gmsgCtpEvent;
extern int gmsgIconInfo;
extern int gmsgCap;
extern int gmsgFlag;
//extern int CGameModeCTP::g_nRedFlags;
//extern int CGameModeCTP::g_nBlueFlags;
//extern int CGameModeCTP::g_nYellowFlags;
//extern int CGameModeCTP::g_nGreenFlags;

enum Flag_Animations
{
	IDLE = 1,
	CARIED,
};


CFlag::CFlag()
{
	m_nFlagColor			  = -1;
	m_hudMessage.x            = 0.7f;
    m_hudMessage.y            = 0.3f;
    m_hudMessage.a1           = 0.0f;
    m_hudMessage.a2           = 0.0f;
	m_hudMessage.r1           = 255.0f;
	m_hudMessage.g1           = 50.0f;
    m_hudMessage.b1           = 50.0f;
	m_hudMessage.r2           = 255.0f;
    m_hudMessage.g2           = 255.0f;
    m_hudMessage.b2           = 255.0f;
    m_hudMessage.channel      = 4;
    m_hudMessage.effect       = 2;
    m_hudMessage.fxTime       = 0.5;
    m_hudMessage.fadeinTime   = 0.025;
    m_hudMessage.fadeoutTime  = 2;    
    m_hudMessage.holdTime     = 4;
}
void CFlag::Precache( )
{
	PRECACHE_MODEL( "models/flag.mdl" );
}


void CFlag::Spawn ( void )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/flag.mdl");

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(-20, -20, 0), Vector(20, 20, 64));

	SetThink( &CFlag::ThinkFlag );
	//SetTouch( TouchFlag );

	m_bInIt = false;
	pev->sequence = IDLE;
	pev->framerate = 1.0;

	//pev->renderfx = kRenderFxGlowShell;
	//pev->renderamt = 20;  // glow shell distance from entity	

	pev->nextthink = gpGlobals->time + 0.2;
}


void CFlag::Materialize( void )
{
	pev->effects &= ~EF_NODRAW;
	SetTouch( &CFlag::TouchFlag );
	SetThink( &CFlag::ThinkFlag );

	MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
		WRITE_SHORT(entindex());	
		WRITE_SHORT(0);					// flag holder (player)
		WRITE_SHORT(pev->origin[0]);
		WRITE_SHORT(pev->origin[1]);
		WRITE_BYTE(m_nFlagColor);
		WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
	MESSAGE_END();
}


void CFlag::TouchFlag( CBaseEntity *pOther )
{

	if ( !pOther->IsPlayer() )
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;
	
	if (MyTouchFlag( pPlayer )) 
	{
		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		SetTouch( NULL );
		SetThink( NULL );
		pev->effects |= EF_NODRAW;
		
		MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
		WRITE_SHORT(entindex());	
		WRITE_SHORT(0);					// flag holder (player)
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_BYTE(0);
		WRITE_BYTE(2);				
		MESSAGE_END();

		if(FStrEq(STRING(pev->classname),"info_flag_ctf_dropped"))
		{
			UTIL_Remove( this );
		}
	}
}

void CFlag::Reset()
{
	if(m_bFallenFlag)
	{
		ReturnFlag(m_nFlagColor);
		UTIL_Remove( this );
		pev->nextthink = 0;
	}
}

// edit the function later so only dropped flags can create saves

BOOL CFlag::MyTouchFlag( CBasePlayer *pPlayer )
{
	//char text[256];

	if ( ( pPlayer->pev->weapons & (1<< WEAPON_SUIT) ) )
	{

			int test = g_pGameRules->GetTeamIndex( pPlayer )+1;
 		if(m_nFlagColor == g_pGameRules->GetTeamIndex( pPlayer ) + 1 )
		{
			if(m_bFallenFlag) // save!
			{
				
				for ( int j = 1; j <= gpGlobals->maxClients; j++ )
				{
					CBaseEntity *ent = UTIL_PlayerByIndex( j );
					CBaseEntity *plr = NULL;
					if (ent && ent->IsPlayer())
					{
						plr = (CBasePlayer*)ent;
						EMIT_SOUND(ENT(plr->pev), CHAN_STATIC, "flag_saved.wav", 1, ATTN_NORM); // need precashing
					}
				}
				
				
				MESSAGE_BEGIN( MSG_ALL, gmsgCtfEvent );
					WRITE_BYTE(FLAG_RETURNED);
					WRITE_BYTE(0);
					WRITE_BYTE(m_nFlagColor);
				MESSAGE_END();
				

				pPlayer->m_iSaves++;
				//FillPlayer(pPlayer);
				//LoadPlayer(pPlayer);
				ReturnFlag(m_nFlagColor);
				UTIL_Remove( this );

				g_pGameRules->SendScoreInfo(pPlayer);
			
				return TRUE;
			}
			else if(pPlayer->m_bHasFlag) // touchdown!
			{
				
				SendScoredMessage(pPlayer);
				ReturnFlag(pPlayer->m_nCtfFlagInHand);
				// delete the flag on the back
				CBaseEntity* pEntity = NULL;
				while (pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow"))
				{
					CFlagFollow* pFl = (CFlagFollow*)pEntity;
					if(pFl->m_pOwner == pPlayer)
					{
						//MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
						pFl->Remove();
						break;
					}
				}
				pPlayer->m_bHasFlag = false;
				
				MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
						WRITE_BYTE( pPlayer->entindex() );
						WRITE_BYTE( g_pGameRules->GetTeamIndex(pPlayer)+1 );
						WRITE_BYTE( pPlayer->m_isSpectator);
						WRITE_BYTE(	pPlayer->m_bIsRemoved);
						WRITE_BYTE(	pPlayer->m_nCtfFlagInHand );
				MESSAGE_END();

				return FALSE;
			}
			else
				return FALSE; // do nada!			
		}
		
		// you are taking the enemey flag

		if(pPlayer->m_bHasFlag) // cant take 2 flags
			return FALSE;

		for ( int j = 1; j <= gpGlobals->maxClients; j++ )
		{
			CBaseEntity *ent = UTIL_PlayerByIndex( j );
			CBaseEntity *plr = NULL;
			if (ent && ent->IsPlayer())
			{
				plr = (CBasePlayer*)ent;
				EMIT_SOUND(ENT(pev), CHAN_STATIC, "flag_stolen.wav", 1, ATTN_NONE);
			}
		}
		pPlayer->m_bHasFlag = true;
		pPlayer->m_nCtfFlagInHand = m_nFlagColor;
		CBaseEntity *pEnt = CBaseEntity::Create( "flag_follow", pev->origin,
		pev->angles, pPlayer->edict() );
		CFlagFollow *pFollow = (CFlagFollow *)pEnt;
		
		pFollow->m_pOwner = pPlayer;		
		pFollow->m_nFlagColor = m_nFlagColor;

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();

		CEntityNode* pClient = pPlayer->m_lObservers.head();
		while(pClient)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pClient->m_pEntity );
			WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			pClient = pClient->m_pNext;
		}

		MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
			WRITE_BYTE( pPlayer->entindex() );
			WRITE_BYTE( g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)+1 );
			WRITE_BYTE( pPlayer->m_isSpectator);
			WRITE_BYTE(	pPlayer->m_bIsRemoved);
			WRITE_BYTE(	pPlayer->m_nCtfFlagInHand );
		MESSAGE_END();

		SendCapturedMessage(pPlayer);
		//UTIL_Remove( this );
		return TRUE;
	}

	return FALSE;
}		


void CFlag::ThinkFlag( void )
{
	if(!m_bInIt)
	{
		m_bInIt = true;
		
		m_dFallTime.AddTime(0.75f);
		//SetTouch(NULL);

		switch(m_nFlagColor)
		{
		case RED:
			pev->skin = 2;
			//pev->rendercolor.x = 255;
			//pev->rendercolor.y = 0;
			//pev->rendercolor.z = 0;
			break;

		case BLUE:
		
			pev->skin = 1;
			//pev->rendercolor.x = 0;
			//pev->rendercolor.y = 0;
			//pev->rendercolor.z = 255;
			break;
		case GREEN:
		
			pev->skin = 3;
			//pev->rendercolor.x = 0;
			//pev->rendercolor.y = 255;
			//pev->rendercolor.z = 0;
			break;
		case YELLOW:
		
			pev->skin = 4;
			//pev->rendercolor.x = 255;
			//pev->rendercolor.y = 255;
			//pev->rendercolor.z = 0;
			break;
		case PURPLE:
		
			pev->skin = 5;
			//pev->rendercolor.x = 200;
			//pev->rendercolor.y = 0;
			//pev->rendercolor.z = 255;
			break;
		case ORANGE:
		
			pev->skin = 6;
			//pev->rendercolor.x = 255;
			//pev->rendercolor.y = 100;
			//pev->rendercolor.z = 0;
			break;
		}
		
	}

	else if(m_dFallTime.TimeUp()) 
	{
		m_dFallTime.ClearTime();
		SetTouch( &CFlag::TouchFlag );
	}

	if(m_bFallenFlag)
	{
		if(m_dDropTime.TimeUp()) // treat like a save
		{
			m_dDropTime.ClearTime();
			
			MESSAGE_BEGIN( MSG_ALL, gmsgCtfEvent );
			WRITE_BYTE(FLAG_RETURNED);
			WRITE_BYTE(0);
			WRITE_BYTE(m_nFlagColor);
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
				WRITE_SHORT(entindex());	
				WRITE_SHORT(0);
				WRITE_SHORT(0);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_BYTE(2);				
			MESSAGE_END();

			for ( int j = 1; j <= gpGlobals->maxClients; j++ )
			{
				CBaseEntity *ent = UTIL_PlayerByIndex( j );
				CBaseEntity *plr = NULL;
				if (ent && ent->IsPlayer())
				{
					plr = (CBasePlayer*)ent;
					EMIT_SOUND(ENT(plr->pev), CHAN_STATIC, "flag_saved.wav", 1, ATTN_NORM); // need precashing
				}
			}
			ReturnFlag(this->m_nFlagColor);
			UTIL_Remove( this );
			// (EDIT) message that flag has returned here   
		}
	}

	pev->nextthink = gpGlobals->time + 0.2;
}


void CFlag::ReturnFlag(int nColor )
{

	CBaseEntity *pEntity = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf");
	while ( pEntity )
	{
		CFlag *pFlag = (CFlag *)pEntity;
		if(pFlag->m_nFlagColor == nColor)
		{
			pFlag->Materialize( );
			return;
		}
		pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf");
	}
}


void CFlag::KeyValue( KeyValueData *pkvd )
{
	int nColor;
	if (FStrEq(pkvd->szKeyName, "ctf_flag"))
	{
		nColor = atoi(pkvd->szValue);

		switch(nColor)
		{
		case 1:
			pev->skin = 2;
			m_nFlagColor = RED;
			break;

		case 2:
			pev->skin = 1;
			m_nFlagColor = BLUE;
			break;
		case 3:
			pev->skin = 3;
			m_nFlagColor = GREEN;
			break;
		case 4:
			pev->skin = 4;
			m_nFlagColor = YELLOW;			
			break;
		case 5:
			pev->skin = 5;
			m_nFlagColor = PURPLE;		
			break;
		case 6:
			pev->skin = 6;
			m_nFlagColor = ORANGE;
			break;
		}
	
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}


LINK_ENTITY_TO_CLASS( info_flag_ctf, CFlag );
LINK_ENTITY_TO_CLASS( info_flag_ctf_dropped, CFlag );


//////////////////////
//// Flag Follow /////
//////////////////////

void CFlagFollow::Precache( )
{
	PRECACHE_MODEL ("models/flag.mdl");
}


void CFlagFollow::Spawn( )
{
	m_nFlagColor=NULL;
	Precache( );
	UTIL_SetOrigin( pev, pev->origin );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->effects |= EF_NODRAW;
	pev->sequence = 2;
	pev->framerate = 1.0;
	SET_MODEL(ENT(pev), "models/flag.mdl");
	m_bInIt=false;

	SetThink( &CFlagFollow::ThinkFollow );
	pev->nextthink = gpGlobals->time + 0.2;
}

void CFlagFollow::Reset()
{
	if(m_pOwner)
		m_pOwner->RemoveFlag();
	Remove();
}

void CFlagFollow::Remove( )
{

	//SetThink(NULL);
	//SetThink(SUB_Remove);
	//pev->nextthink = gpGlobals->time + .1;//HLE
	MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
		WRITE_SHORT(entindex());	
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_BYTE(0);
		WRITE_BYTE(2);				
	MESSAGE_END();

	SetThink(NULL);
	SetTouch(NULL);
	UTIL_Remove( this );
	m_pOwner = NULL;
	//UTIL_Remove( this );
}

void CFlagFollow::ThinkFollow( )
{
	if(!m_bInIt)
	{
		m_bInIt = true;
		pev->effects &= ~EF_NODRAW;
		pev->aiment = ENT(m_pOwner->pev);
		pev->movetype = MOVETYPE_FOLLOW;
		//pev->renderfx = kRenderFxGlowShell;
		//pev->renderamt = 20;  // glow shell distance from entity
		m_dSendLocation.AddTime(4);

		if(m_pOwner)
		{
			switch(m_pOwner->m_nCtfFlagInHand)
			{
			case RED:
				pev->skin = 2;
				//pev->rendercolor.x = 255;
				//pev->rendercolor.y = 0;
				//pev->rendercolor.z = 0;
				break;

			case BLUE:
			
				pev->skin = 1;
				//pev->rendercolor.x = 0;
				//pev->rendercolor.y = 0;
				//pev->rendercolor.z = 255;
				break;
			case GREEN:
			
				pev->skin = 3;
				//pev->rendercolor.x = 0;
				//pev->rendercolor.y = 255;
				//pev->rendercolor.z = 0;
				break;
			case YELLOW:
			
				pev->skin = 4;
				//pev->rendercolor.x = 255;
				//pev->rendercolor.y = 255;
				//pev->rendercolor.z = 0;
				break;
			case PURPLE:
			
				pev->skin = 5;
				//pev->rendercolor.x = 200;
				//pev->rendercolor.y = 0;
				//pev->rendercolor.z = 255;
				break;
			case ORANGE:
			
				pev->skin = 6;
				//pev->rendercolor.x = 255;
				//pev->rendercolor.y = 100;
				//pev->rendercolor.z = 0;
				break;
			}
			
			MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
			WRITE_SHORT(entindex());	
			WRITE_SHORT(m_pOwner->entindex());					// flag holder (player)
			WRITE_SHORT(pev->origin[0]);
			WRITE_SHORT(pev->origin[1]);
			WRITE_BYTE(m_nFlagColor);
			WRITE_BYTE(0);						// 0 = stay, 1 = fade away, 2 = disapear, 3 = show only if you are on my team
			MESSAGE_END();			
		}
	}
	else
	{
		pev->sequence = 2;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Sends a flag message which fades away after a short delay//////////////////
	// This message only gets sent to players NOT on the team of the flag holder//
	// and spectators not spectating a player not on the team of the flag holder//
	//////////////////////////////////////////////////////////////////////////////
	if(m_dSendLocation.TimeUp())
	{
		m_dSendLocation.AddTime(4);
		int holder_team = g_pGameRules->GetTeamIndex(m_pOwner)+1;

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *plr = UTIL_PlayerByIndex( i );

			if ( plr && plr->IsPlayer())
			{
				CBasePlayer* pPlayer = (CBasePlayer*)plr;
				if(holder_team != g_pGameRules->GetTeamIndex( pPlayer )+1 && !pPlayer->m_isSpectator)
				{
					MESSAGE_BEGIN( MSG_ONE, gmsgFlag, NULL, pPlayer->edict() );
						WRITE_SHORT(entindex());
						WRITE_SHORT(0);					// flag holder (player)
						WRITE_SHORT(pev->origin[0]);
						WRITE_SHORT(pev->origin[1]);
						WRITE_BYTE(m_nFlagColor);		
						WRITE_BYTE(1);					// 0 = stay, 1 = fade away, 2 = dissapear
					MESSAGE_END();	

					CEntityNode* pClient = pPlayer->m_lObservers.head();
					while(pClient)
					{			
						MESSAGE_BEGIN( MSG_ONE, gmsgFlag, NULL, pClient->m_pEntity );
							WRITE_SHORT(entindex());
							WRITE_SHORT(0);					// flag holder (player)
							WRITE_SHORT(pev->origin[0]);
							WRITE_SHORT(pev->origin[1]);
							WRITE_BYTE(m_nFlagColor);		
							WRITE_BYTE(1);					// 0 = stay, 1 = fade away, 2 = dissapear
						MESSAGE_END();	
						pClient = pClient->m_pNext;
					}
				}
				
			}	
		}
	}
	pev->nextthink = gpGlobals->time + 0.2;
}


LINK_ENTITY_TO_CLASS( flag_follow, CFlagFollow );


void CFlag::SendScoredMessage(CBasePlayer *pCaptor)
{
	g_pGameRules->CapturedFlag(pCaptor);

}

void CFlag::SendCapturedMessage(CBasePlayer *pCaptor)
{
	char cText[512];
	int nFlagColor=0;
	*cText=NULL;



	//int nTeam = g_pGameRules->GetTeamIndex( pCaptor ) + 1;

	MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pCaptor->pev );
		WRITE_BYTE(5);
		WRITE_BYTE(0);
		WRITE_BYTE(pCaptor->m_nCtfFlagInHand);
	MESSAGE_END();

	CEntityNode* pClient = pCaptor->m_lObservers.head();
	while(pClient)
	{			
		MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pClient->m_pEntity );
			WRITE_BYTE(5);
			WRITE_BYTE(0);
			WRITE_BYTE(pCaptor->m_nCtfFlagInHand);
		MESSAGE_END();

		pClient = pClient->m_pNext;
	}
		
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr && plr->IsPlayer())
		{
			if(((CBasePlayer*)plr) != pCaptor)
			{
			
				MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, plr->pev );
					WRITE_BYTE(FLAG_CAPTURED);
					WRITE_BYTE(0);
					WRITE_BYTE(pCaptor->m_nCtfFlagInHand);
				MESSAGE_END();

				CEntityNode* pClient = ((CBasePlayer*)plr)->m_lObservers.head();
				while(pClient)
				{			
					MESSAGE_BEGIN( MSG_ONE, gmsgCtfEvent, NULL, pClient->m_pEntity );
						WRITE_BYTE(FLAG_CAPTURED);
						WRITE_BYTE(0);
						WRITE_BYTE(pCaptor->m_nCtfFlagInHand);
					MESSAGE_END();
					pClient = pClient->m_pNext;
				}
			}
		}

	}

	/*MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pCaptor->pev );
	WRITE_BYTE(pCaptor->m_nCtfFlagInHand);
	MESSAGE_END();

	CEntityNode* pClient = pCaptor->m_lObservers.head();
	while(pClient)
	{			
		MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pClient->m_pEntity );
		WRITE_BYTE(pCaptor->m_nCtfFlagInHand);
		MESSAGE_END();
		pClient = pClient->m_pNext;
	}*/

	//m_hudMessage.r1 = 255;
	//m_hudMessage.g1 = 60;
	//m_hudMessage.b1 = 50;
	//nFlagColor = 2; // red team captued blue falg
	
/*
	bool oOverwrite = false;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer*)UTIL_PlayerByIndex( i );

		if ( plr )
		{
			const int nTeamName =  g_pGameRules->GetTeamIndex( plr ) + 1;
			// try add to existing team
			if(plr->edict() == pCaptor->edict())
			{
				sprintf(cText,"You have stolen the enemy flag!\nReturn it to your flag to score!");
				oOverwrite = true;
			}
			else if(nTeamName == RED)
			{
				if(nFlagColor == 2)
					sprintf(cText, "%s has stole the enemy flag!", STRING(pCaptor->pev->netname));
				else
					sprintf(cText, "%s has stole your flag!", STRING(pCaptor->pev->netname));
			}
			else
			{
				if(nFlagColor == 1)
					sprintf(cText,"%s has stole the enemy flag!" ,STRING(pCaptor->pev->netname));
				else
					sprintf(cText, "%s has stole your flag!", STRING(pCaptor->pev->netname));
			}

			UTIL_HudMessage( plr, m_hudMessage, cText );
		}
	}*/

}

//=================================
//CFlagStatic
//=================================


CFlagStatic::CFlagStatic()
{
	m_bInIt = false;
}
void CFlagStatic::Reset()
{
	m_nFlagColor = 0;
	if(pev)
		pev->skin = 0;
	pev->rendercolor.x = 0;
	pev->rendercolor.y = 0;
	pev->rendercolor.z = 0;
	pev->renderfx = kRenderFxNone;

	MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
		WRITE_SHORT(entindex());	
		WRITE_SHORT(0);					// flag holder (player)
		WRITE_SHORT(pev->origin[0]);
		WRITE_SHORT(pev->origin[1]);
		WRITE_BYTE(0);
		WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
	MESSAGE_END();
}
void CFlagStatic::Spawn( void )
{
	m_nFlagColor = 0;
	Precache( );
	m_bInIt = false;
	SET_MODEL(ENT(pev), "models/flag.mdl");
	pev->movetype = MOVETYPE_TOSS; 
	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(-20, -20, 0), Vector(20, 20, 64));

	SetThink( &CFlagStatic::ThinkFlag );
	SetTouch( &CFlagStatic::TouchFlag );

	pev->sequence = IDLE;
	pev->framerate = 1.0;

	pev->renderfx = kRenderFxNone;
	pev->renderamt = 20;  // glow shell distance from entity
	pev->nextthink = gpGlobals->time + 0.1;
	m_pOwner = NULL;
	m_dDelay.AddTime(0.1);

}
void CFlagStatic::ThinkFlag()
{
	if(!m_bInIt)
	{
		pev->rendercolor.x = 0;
		pev->rendercolor.y = 0;
		pev->rendercolor.z = 0;
		m_nFlagColor = 0;
		pev->skin = 0;
		m_bInIt = true;
	}

	if(pev->renderfx == kRenderFxGlowShell)
	{
		pev->rendercolor.x-=gpGlobals->frametime*200;		
		pev->rendercolor.y-=gpGlobals->frametime*200;
		pev->rendercolor.z-=gpGlobals->frametime*200;
		pev->renderamt +=gpGlobals->frametime*150;
		int check = 0;
		if(pev->rendercolor.x <0)
		{
			pev->rendercolor.x = 0;
			check++;
		}
		if(pev->rendercolor.y <0)
		{
			pev->rendercolor.y = 0;
			check++;
		}
		if(pev->rendercolor.z <0)
		{
			pev->rendercolor.z = 0;
			check++;
		}
		if(check==3)
		{
			pev->renderfx = kRenderFxNone;
		}
	}

	
	pev->nextthink = 0.1;///////gpGlobals->time + 0;
	
}
void CFlagStatic::Precache( void )
{
	PRECACHE_MODEL( "models/flag.mdl" );
}
void CFlagStatic::Materialize( void )
{
	pev->effects &= ~EF_NODRAW;
	SetTouch( &CFlagStatic::TouchFlag );
	SetThink( &CFlagStatic::ThinkFlag );

	MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
		WRITE_SHORT(entindex());	
		WRITE_SHORT(0);					// flag holder (player)
		WRITE_SHORT(pev->origin[0]);
		WRITE_SHORT(pev->origin[1]);
		WRITE_BYTE(m_nFlagColor);
		WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
	MESSAGE_END();
}
void CFlagStatic::TouchFlag( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
		return;

	if(!m_dDelay.TimeUp())
		return;
	
	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	if ( ( pPlayer->pev->weapons & (1<< WEAPON_SUIT) ) )
	{
		int nTeam = g_pGameRules->GetTeamIndex( pPlayer ) + 1;
 		if(m_nFlagColor != nTeam )
		{
			m_dDelay.AddTime(1);
			for ( int j = 1; j <= gpGlobals->maxClients; j++ )
			{
				CBaseEntity *ent = UTIL_PlayerByIndex( j );
				CBaseEntity *plr = NULL;
				if (ent && ent->IsPlayer())
				{
					plr = (CBasePlayer*)ent;
					EMIT_SOUND(ENT(pev), CHAN_STATIC, "flag_dropped.wav", 0.8, ATTN_NONE);
				}
			}
			pev->renderfx = kRenderFxGlowShell;
			pev->renderamt = 0;

			pPlayer->m_iSaves++; // this is used for flag count, edit lator
			
			CGameModeCTP::g_nCtpFlags[m_nFlagColor]--;

			int nOldColor = m_nFlagColor;

			switch(nTeam)
			{
			case BLUE: // blue

				CGameModeCTP::g_nCtpFlags[BLUE]++;
				pev->skin = 1;
				m_nFlagColor = BLUE;
				pev->rendercolor.x = 0;
				pev->rendercolor.y = 0;
				pev->rendercolor.z = 255;
				break;
			case RED: // red

				CGameModeCTP::g_nCtpFlags[RED]++;
				pev->skin = 2;
				m_nFlagColor = RED;
				pev->rendercolor.x = 255;
				pev->rendercolor.y = 0;
				pev->rendercolor.z = 0;
				break;
			case YELLOW: // yellow

				CGameModeCTP::g_nCtpFlags[YELLOW]++;
				pev->skin = 4;
				m_nFlagColor = YELLOW;
				pev->rendercolor.x = 255;
				pev->rendercolor.y = 255;
				pev->rendercolor.z = 0;
				break;
			case GREEN: // green

				CGameModeCTP::g_nCtpFlags[GREEN]++; 
				pev->skin = 3;
				m_nFlagColor = GREEN;
				pev->rendercolor.x = 0;
				pev->rendercolor.y = 255;
				pev->rendercolor.z = 0;
				break;
			case PURPLE: // purple

				CGameModeCTP::g_nCtpFlags[PURPLE]++;
				pev->skin = 5;
				m_nFlagColor = PURPLE;
				pev->rendercolor.x = 255;
				pev->rendercolor.y = 0;
				pev->rendercolor.z = 255;
				break;
			case ORANGE: // orange

				CGameModeCTP::g_nCtpFlags[ORANGE]++; 
				pev->skin = 6;
				m_nFlagColor = ORANGE;
				pev->rendercolor.x = 255;
				pev->rendercolor.y = 100;
				pev->rendercolor.z = 0;
				break;
			}

			MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
				WRITE_SHORT(entindex());	
				WRITE_SHORT(0);					// flag holder (player)
				WRITE_SHORT(pev->origin[0]);
				WRITE_SHORT(pev->origin[1]);
				WRITE_BYTE(m_nFlagColor);
				WRITE_BYTE(0);					// 0 = stay, 1 = fade away, 2 = dissapear
			MESSAGE_END();

			for (int k = 1; k <= gpGlobals->maxClients; k++ )
			{
				CBaseEntity *pEnt = UTIL_PlayerByIndex( k );

				if ( pEnt && pEnt->IsPlayer( ) )
				{
				
					CBasePlayer *pPlr = (CBasePlayer *)pEnt;

					int nTeam = g_pGameRules->GetTeamIndex( pPlr)+1;
					
					if(!pPlr->m_isSpectator)
					{
						MESSAGE_BEGIN( MSG_ONE, gmsgCtpEvent, NULL, pPlr->edict() );
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[BLUE]);  
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[RED]); 
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[GREEN]);
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[YELLOW]); 
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[PURPLE]);
						WRITE_BYTE(CGameModeCTP::g_nCtpFlags[ORANGE]);
						WRITE_BYTE(nTeam);
						MESSAGE_END();

						CEntityNode* pClient = pPlr->m_lObservers.head();
						while(pClient)
						{
							MESSAGE_BEGIN( MSG_ONE, gmsgCtpEvent, NULL, pClient->m_pEntity);
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
					
					/*
						if(nTeam == nOldColor)// send a lossed capacticy message here
						{
							if(pPlr->max_hev == 100)
								break; // its already as low as possible
							int max_capacity = 100 + (CGameModeCTP::g_nCtpFlags[nTeam] * 10);
							if(max_capacity>200)
								max_capacity = 200;
							pPlr->max_hev =  max_capacity;
							pPlr->pev->max_health = max_capacity;
						
							MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pPlr->edict() );  
							WRITE_SHORT(max_capacity-100);
							WRITE_SHORT(max_capacity-100);
							MESSAGE_END();

							CEntityNode* pClient = pPlr->m_lObservers.head();
							while(pClient)
							{
								MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pClient->m_pEntity );
									WRITE_SHORT(max_capacity-100);
									WRITE_SHORT(max_capacity-100);
								MESSAGE_END();
								pClient = pClient->m_pNext;
							}
							
						}
						else if(nTeam == m_nFlagColor)// send a gain capacticy message here
						{
							if(pPlr->max_hev >= 200)
								break;
							int max_capacity = 100 + (CGameModeCTP::g_nCtpFlags[nTeam] * 10);
							if(max_capacity>200)
								max_capacity = 200;
							pPlr->max_hev =  max_capacity;
							pPlr->pev->max_health = max_capacity;

							MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pPlr->edict() );  
							WRITE_SHORT(max_capacity-100);
							WRITE_SHORT(max_capacity-100);
							MESSAGE_END();

							CEntityNode* pClient = pPlr->m_lObservers.head();
							while(pClient)
							{
								MESSAGE_BEGIN( MSG_ONE, gmsgCap, NULL, pClient->m_pEntity );
									WRITE_SHORT(max_capacity-100);
									WRITE_SHORT(max_capacity-100);
								MESSAGE_END();
								pClient = pClient->m_pNext;
							}
							
						}*/
					}
				}
			}	

			g_pGameRules->CapturedFlag(pPlayer);
		}
	}
}

LINK_ENTITY_TO_CLASS( info_flag_ctp, CFlagStatic );

//=================================
//CFlagReturnZone
//=================================




CFlagReturnZone::CFlagReturnZone()
{
	m_hudMessage.x            = 0.5f;
    m_hudMessage.y            = 0.4f;
    m_hudMessage.a1           = 0.0f;
    m_hudMessage.a2           = 0.0f;
	m_hudMessage.r1           = 255.0f;
	m_hudMessage.g1           = 50.0f;
    m_hudMessage.b1           = 50.0f;
	m_hudMessage.r2           = 0.0f;
    m_hudMessage.g2           = 0.0f;
    m_hudMessage.b2           = 0.0f;
    m_hudMessage.channel      = 4;
    m_hudMessage.effect       = 2;
    m_hudMessage.fxTime       = 0.5;
    m_hudMessage.fadeinTime   = 0.025;
    m_hudMessage.fadeoutTime  = 2;    
    m_hudMessage.holdTime     = 4;
}

void CFlagReturnZone::Spawn ( void )
{
	bActive = true;
	//SetThink( Think );
	SetTouch( &CFlagReturnZone::TouchZone );
	if (pev->angles != g_vecZero)
	SetMovedir(pev);
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	if ( CVAR_GET_FLOAT("showtriggers") == 0 )
		SetBits( pev->effects, EF_NODRAW );
UTIL_SetOrigin( pev, pev->origin );	

	pev->nextthink = gpGlobals->time + 0.2;
}


void CFlagReturnZone::TouchZone( CBaseEntity *pOther )
{
	// (if) is not a player
	if(!bActive)
		return;
	if ( !pOther->IsPlayer() )
	{
		CBaseEntity *pEntity = NULL;
		pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf_dropped");
		while (pEntity)
		{
			if(pEntity==pOther)
			{
				CFlag* pFlag = (CFlag*)pEntity;

				MESSAGE_BEGIN( MSG_ALL, gmsgCtfEvent );
				WRITE_BYTE(FLAG_RETURNED);
				WRITE_BYTE(0);
				WRITE_BYTE(pFlag->m_nFlagColor);
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ALL, gmsgFlag);
				WRITE_SHORT(pFlag->entindex());	
				WRITE_SHORT(0);
				WRITE_SHORT(0);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_BYTE(2);				
				MESSAGE_END();
			
				CFlag::ReturnFlag(pFlag->m_nFlagColor);
				UTIL_Remove( pOther );
				for ( int j = 1; j <= gpGlobals->maxClients; j++ )
				{
					CBaseEntity *ent = UTIL_PlayerByIndex( j );
					CBaseEntity *plr = NULL;
					if (ent && ent->IsPlayer())
					{
						plr = (CBasePlayer*)ent;
						EMIT_SOUND(ENT(plr->pev), CHAN_STATIC, "flag_saved.wav", 1, ATTN_NORM); // need precashing
					}
				}
				break;
			}
			
			pEntity = UTIL_FindEntityByClassname( pEntity, "info_flag_ctf_dropped");
		}
		return;
	}

	// ( else )is a player

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;
	if(pPlayer->m_bHasFlag == false)
		return;
	CFlag::ReturnFlag(pPlayer->m_nCtfFlagInHand);

	CBaseEntity *pEntity = NULL;
	pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	while (pEntity)
	{
		CFlagFollow* pFlag = (CFlagFollow*)pEntity;
		if(pFlag->m_pOwner == pPlayer)
		{
			pFlag->Remove();
			break;
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "flag_follow");
	}						

	pPlayer->m_bHasFlag = false;

	g_pGameRules->SendScoreInfo(pPlayer);

	MESSAGE_BEGIN( MSG_ALL, gmsgIconInfo );
		WRITE_BYTE( pPlayer->entindex() );
		WRITE_BYTE( g_pGameRules->GetTeamIndex2(pPlayer->m_szTeamName)+1 );
		WRITE_BYTE( pPlayer->m_isSpectator);
		WRITE_BYTE(	pPlayer->m_bIsRemoved);
		WRITE_BYTE(	pPlayer->m_nCtfFlagInHand );
	MESSAGE_END();
		
	

	MESSAGE_BEGIN( MSG_ALL, gmsgCtfEvent );
		WRITE_BYTE(FLAG_RETURNED);
		WRITE_BYTE(0);
		WRITE_BYTE(pPlayer->m_nCtfFlagInHand);
	MESSAGE_END();


/*	MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pPlayer->pev );
	WRITE_BYTE(0);
	MESSAGE_END();*/

	for ( int j = 1; j <= gpGlobals->maxClients; j++ )
	{
		CBaseEntity *ent = UTIL_PlayerByIndex( j );
		CBaseEntity *plr = NULL;
		if (ent && ent->IsPlayer())
		{
			plr = (CBasePlayer*)ent;
			EMIT_SOUND(ENT(plr->pev), CHAN_STATIC, "flag_saved.wav", 1, ATTN_NORM); // need precashing
		}
	}
	/*

	CEntityNode* pClient = pPlayer->m_lObservers.head();
	while(pClient)
	{			
		MESSAGE_BEGIN( MSG_ONE, gmsgCtfFlag, NULL, pClient->m_pEntity );
		WRITE_BYTE(0);
		MESSAGE_END();
		pClient = pClient->m_pNext;
	}*/

}


void CFlagReturnZone::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if(bActive)
		bActive = false;
	else
		bActive = true;
}

void CFlagReturnZone::KeyValue( KeyValueData *pkvd )
{

	if (FStrEq(pkvd->szKeyName, "ctf_return_zone"))	
		pkvd->fHandled = TRUE;
	if (FStrEq(pkvd->szKeyName, ""))	
		pkvd->fHandled = TRUE;

	else
		CBaseEntity::KeyValue( pkvd );
}


LINK_ENTITY_TO_CLASS( ctf_return_zone, CFlagReturnZone );



////////////////////////////////////
////////////////////////////////////
////////////////////////////////////


