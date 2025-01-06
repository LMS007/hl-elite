#ifndef _HLE_FLAG_H
#define _HLE_FLAG_H

#define BLUE 1
#define RED 2
#define GREEN 3
#define YELLOW 4
#define PURPLE 5
#define ORANGE 6

void RemoveFlag();

class CFlag : public CBaseEntity
{

public:
	CFlag();
	void Reset();
	virtual void Spawn( void );
	void Precache( void );
	static void ReturnFlag( int );
	void Materialize( void );
	void EXPORT TouchFlag( CBaseEntity *pOther );
	BOOL EXPORT MyTouchFlag( CBasePlayer *pPlayer );
	void EXPORT ThinkFlag( void );
	void KeyValue( KeyValueData *pkvd );
	
	bool m_bFallenFlag;
	int  m_nFlagColor;
	CDelayTime m_dDropTime;
	
protected:
	
	CDelayTime m_dFallTime;
	bool m_bInIt;
	void SendCapturedMessage(CBasePlayer* pCaptor);
	void SendScoredMessage(CBasePlayer *pCaptor);

	hudtextparms_t m_hudMessage;

};

class CFlagFollow : public CFlag
{
	public:
	virtual void Spawn( void );
	void Precache( void );
	void EXPORT ThinkFollow( void );
	CBasePlayer *m_pOwner;
	bool m_bHasFlag;
	void Reset();
	void Remove();
	private:
	CDelayTime m_dSendLocation;
	//bool m_bInIt;
	
	
};

class CFlagStatic : public CFlag
{
	CBasePlayer* m_pOwner;
	//bool m_bInIt;
	CDelayTime m_dDelay;
	

public:
	
	CFlagStatic();
	void Reset();
	void Spawn( void );
	void Precache( void );
	void Materialize( void );
	void EXPORT TouchFlag( CBaseEntity *pOther );
	void EXPORT ThinkFlag();
	int  m_nFlagColor;
	

};


/*
class CFlagStatic : public CFlag
{

public:
	CFlag();
	void Reset();
	virtual void Spawn( void );
	void Precache( void );
	static void ReturnFlag( int );
	void Materialize( void );
	void EXPORT TouchFlag( CBaseEntity *pOther );
	BOOL EXPORT MyTouchFlag( CBasePlayer *pPlayer );
	void EXPORT ThinkFlag( void );
	void KeyValue( KeyValueData *pkvd );
	
	int  m_nFlagColor;
	
private:
	
	bool m_bInIt;
	void SendCapturedMessage(CBasePlayer* pCaptor);
	void SendScoredMessage(CBasePlayer *pCaptor);
	hudtextparms_t m_hudMessage;

};
*/

class CFlagReturnZone : public CBaseEntity
{
	bool bActive;

public:
	CFlagReturnZone();
	virtual void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT TouchZone( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd );

private:
	
	hudtextparms_t m_hudMessage;
}; 


#endif