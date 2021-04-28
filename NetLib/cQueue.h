#pragma once

template < class TYPE >
class cQueue : public cMonitor
{
public:
	cQueue( int nMaxSize = MAX_QUEUESIZE );
	~cQueue(void);

	bool	PushQueue( TYPE typeQueueItem );		//queue에 데이타를 넣는다.
	void	PopQueue();								//queue에서 데이타를 지운다.

	bool	IsEmptyQueue();							//queue가 비었는지 알려준다.
	TYPE	GetFrontQueue();						//queue에서 데이타를 얻어온다.
	int		GetQueueSize();							//queue에 현재 들어있는 item의 갯수를 알려준다.
	int		GetQueueMaxSize() { return m_nQueueMaxSize; }  //최대 queue사이즈를 알려준다
	void	SetQueueMaxSize( int nMaxSize ) { m_nQueueMaxSize = nMaxSize; }   //최대 queue size를 정해 준다
	void	ClearQueue();						//queue에 있는 모든 item을 삭제한다.

private:

	TYPE*			m_arrQueue;
	int				m_nQueueMaxSize;				//큐에대한 사이즈 제한
	cMonitor		m_csQueue;
	
	int				m_nCurSize;
	int				m_nEndMark;
	int				m_nBeginMark;
	
};
template <class TYPE>
cQueue< TYPE >::cQueue( int nMaxSize )
{
	m_arrQueue = new TYPE[ nMaxSize ];
	m_nQueueMaxSize = nMaxSize;
	ClearQueue();
	
}

template <class TYPE>
cQueue< TYPE >::~cQueue(void)
{
	delete [] m_arrQueue;
}

template <class TYPE>
bool cQueue< TYPE >::PushQueue( TYPE typeQueueItem )
{
	cMonitor::Owner lock( m_csQueue );
	{
		
		//정해놓은 사이즈를 넘었다면 더이상 큐에 넣지 않는다.
		if( m_nCurSize >= m_nQueueMaxSize )
			return false;
		
		m_nCurSize++;
		if( m_nEndMark == m_nQueueMaxSize )
			m_nEndMark = 0;
		m_arrQueue[ m_nEndMark++ ] = typeQueueItem;
		
	}
	return true;


}

template <class TYPE>
TYPE cQueue< TYPE >::GetFrontQueue()	
{
	TYPE typeQueueItem;
	cMonitor::Owner lock( m_csQueue );
	{
		if( m_nCurSize <= 0 )
			return NULL;
		if( m_nBeginMark == m_nQueueMaxSize )
			m_nBeginMark = 0;
		typeQueueItem = m_arrQueue[ m_nBeginMark++ ];
		
	}
	return typeQueueItem;

}

template <class TYPE>
void cQueue< TYPE >::PopQueue()
{
	cMonitor::Owner lock( m_csQueue );
	{
		m_nCurSize--;
	}
}

template <class TYPE>
bool cQueue< TYPE >::IsEmptyQueue()
{
	bool bFlag = false;
	cMonitor::Owner lock( m_csQueue );
	{
		bFlag = ( m_nCurSize > 0 ) ? true : false;
	}

	return flag;
}

template <class TYPE>
int cQueue< TYPE >::GetQueueSize()
{
	int nSize;
	cMonitor::Owner lock( m_csQueue );
	{
		nSize = m_nCurSize;
	}
	
	return nSize;
}

template <class TYPE>
void cQueue< TYPE >::ClearQueue()
{
	cMonitor::Owner lock( m_csQueue );
	{
		m_nCurSize = 0;
		m_nEndMark = 0;
		m_nBeginMark = 0;
	}
}
