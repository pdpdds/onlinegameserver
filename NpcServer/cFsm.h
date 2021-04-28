#pragma once

#define MAX_DISCOMPORT_INDEX 5
enum eState
{
	NPC_NONE		= 0,    //없음
	NPC_NORMAL		= 1,	//일반			
	NPC_DISCOMPORT	= 2,	//불쾌
    NPC_ANGRY		= 3,	//화남

	NPC_STATE_CNT			//상태 개수
};

enum eEvent
{
	EVENT_NONE	=	0,			//없음
	EVENT_PLAYER_APPEAR	=	1,  //플레이어가 등장
	EVENT_PLAYER_ATTACK	=	2,  //플레이어가 공격함
	EVENT_PLAYER_RUNAWAY=	3,  //플레이어가 도망감
	EVENT_DISCOMPORT_INDEX = 4, //플레이어가 계속 불쾌하게함
};

static char strtableState[][20] = 
{
	"NPC_NONE",
	"NPC_NORMAL",
	"NPC_DISCOMPORT",
	"NPC_ANGRY"
};

class cFsm
{
public:
	cFsm(void);
	~cFsm(void);
	class cFsmClass;
	/////////////////////////////////////////////////////////////////////////
	// 상태와 이벤트를 관리하는 객체
	class cFsmState
	{
		friend class cFsmClass;
	private:
		typedef pair< eEvent , eState >  STATE_PAIR;
		typedef map< eEvent , eState >	 STATE_MAP;
		typedef STATE_MAP::iterator	STATE_IT;
		
		eState				m_eState;
		STATE_MAP			m_mapState;
	
		eState	GetState(){ return m_eState; }
		void	InsertState( eEvent inputEvent, eState inputState );
		void	RemoveState( eEvent inputEvent );
		eState	GetNextState( eEvent inputEvent );
		DWORD	GetStateCount();

	private:
		cFsmState(eState inputState );
		virtual ~cFsmState();
	};
	/////////////////////////////////////////////////////////////////////////
	// 상태객체와 상태를 관리하는 객체
	class cFsmClass
	{
	private:
		typedef pair< eState , cFsmState* >  FSMSTATE_PAIR;
		typedef map< eState , cFsmState* >	 FSMSTATE_MAP;
		typedef FSMSTATE_MAP::iterator	FSMSTATE_IT;

		cFsmState*					m_pCurFsmState;
		FSMSTATE_MAP				m_mapFsmState;
		eEvent						m_eCurEvent;
		
	public:
		void	InsertFsmState( eState inputState, eEvent inputEvent, eState inputNextState  );
		void	RemoveFsmState( eState inputState, eEvent inputEvent );
	
		void	SetCurFsmState( eState inputState );
		eState	GetCurState();
		eState	GetNextState( eEvent inputEvent );
		void	TranslateState( eEvent inputEvent );
		inline	eEvent	GetCurEvent() { return m_eCurEvent; }
		
	public:
		cFsmClass();
		virtual ~cFsmClass();
	};
};
