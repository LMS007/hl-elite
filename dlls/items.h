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
#ifndef ITEMS_H
#define ITEMS_H


class CItem : public CBaseEntity
{
	float m_fTime;
public:
//	BOOL m_bCanRespawn;
	void	Spawn( void );
	CBaseEntity*	Respawn( void );
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	void	EXPORT Materialize( void );
	void    EXPORT ItemThink( void );
	virtual BOOL MyTouch( CBasePlayer *pPlayer ) { return FALSE; };
};


class CRecharge : public CBaseToggle
{
public:
	void Spawn( );
	void Precache( void );
	void EXPORT Off(void);
	void EXPORT Recharge(void);
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return (CBaseToggle :: ObjectCaps() | FCAP_CONTINUOUS_USE) & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	float m_flNextCharge; 
	int		m_iReactivate ; // DeathMatch Delay until reactvated
	int		m_iJuice;
	int		m_iOn;			// 0 = off, 1 = startup, 2 = going
	float   m_flSoundTime;
};


class CWallHealth : public CBaseToggle
{
public:
	void Spawn( );
	void Precache( void );
	void EXPORT Off(void);
	void EXPORT Recharge(void);
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return (CBaseToggle :: ObjectCaps() | FCAP_CONTINUOUS_USE) & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	float m_flNextCharge; 
	int		m_iReactivate ; // DeathMatch Delay until reactvated
	int		m_iJuice;
	int		m_iOn;			// 0 = off, 1 = startup, 2 = going
	float   m_flSoundTime;
};


class CItemBattery : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
	static CBaseEntity* Drop(CBasePlayer* pPlayer);
};

class CHealthKit : public CItem
{

public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
	static CBaseEntity* Drop(CBasePlayer* pPlayer);

/*
	virtual int		Save( CSave &save ); 
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];
*/

};


class CItemLongJump : public CItem
{
public:
	
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual BOOL MyTouch( CBasePlayer *pPlayer );
	static CBaseEntity* Drop(CBasePlayer* pPlayer);

};

class CLongJumpFollow : public CItemLongJump
{

	
public:
	CBasePlayer* m_pOwner;
	//void Destroy(CBasePlayer* pPlayer);
	static void Create(CBasePlayer* pPlayer);
	void Spawn( void );
	void Precache( void );
	void Remove();
	BOOL MyTouch( CBasePlayer *pPlayer );
	void EXPORT ThinkFollow();
};

class CTeamIndicator : public CItem
{
	
public:

	CBasePlayer* m_pOwner;
	static void Create(CBasePlayer* pPlayer);
	void Spawn( void );
	void Precache( void );
	void Remove();
	void SetTeam(CBasePlayer* pPlayer);
	void EXPORT ThinkId(void);

};

#endif // ITEMS_H
