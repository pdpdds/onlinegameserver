#include "precompile.h"
#include "cringbuffer.h"
cRingBuffer::cRingBuffer(void)
{
	m_pRingBuffer = NULL;
	m_pBeginMark = NULL;
	m_pEndMark = NULL;
	m_pCurrentMark = NULL;
	m_pGettedBufferMark = NULL;
	m_pLastMoveMark = NULL;
	m_nUsedBufferSize = 0;
	m_uiAllUserBufSize = 0;
}

cRingBuffer::~cRingBuffer(void)
{
	if( NULL != m_pBeginMark )
		delete [] m_pBeginMark;

}

bool cRingBuffer::Initialize()
{
	cMonitor::Owner lock( m_csRingBuffer );
	{
		m_nUsedBufferSize = 0;
		m_pCurrentMark = m_pBeginMark;
		m_pGettedBufferMark = m_pBeginMark;
		m_pLastMoveMark = m_pEndMark;
		m_uiAllUserBufSize = 0;

	}
	return true;
}

bool cRingBuffer::Create( int nBufferSize )
{
	if( NULL != m_pBeginMark )
		delete [] m_pBeginMark;

	m_pBeginMark = new char[ nBufferSize ];
	
	if( NULL == m_pBeginMark )
		return false;

	m_pEndMark = m_pBeginMark + nBufferSize - 1;
	m_nBufferSize = nBufferSize;

	Initialize();
	return true;
}

char* cRingBuffer::ForwardMark( int nForwardLength )
{
	char* pPreCurrentMark = NULL;
	cMonitor::Owner lock( m_csRingBuffer );
	{
		//링버퍼 오버플로 체크
		if( m_nUsedBufferSize + nForwardLength > m_nBufferSize )
			return NULL;
		if( ( m_pEndMark - m_pCurrentMark ) >= nForwardLength )
		{
			pPreCurrentMark = m_pCurrentMark;
			m_pCurrentMark += nForwardLength;
		}
		else
		{
			//순환 되기 전 마지막 좌표를 저장
			m_pLastMoveMark = m_pCurrentMark;
			m_pCurrentMark = m_pBeginMark + nForwardLength;
			pPreCurrentMark = m_pBeginMark;
		}
		
	}
	return pPreCurrentMark;
}

char* cRingBuffer::ForwardMark( int nForwardLength 
							   , int nNextLength 
							   , DWORD dwRemainLength )
{
	cMonitor::Owner lock( m_csRingBuffer );
	{	
		//링버퍼 오버플로 체크
		if( m_nUsedBufferSize + nForwardLength + nNextLength > m_nBufferSize )
			return NULL;
		
		if( ( m_pEndMark - m_pCurrentMark ) > ( nNextLength + nForwardLength ) )
			m_pCurrentMark += nForwardLength;

		//남은 버퍼의 길이보다 앞으로 받을 메세지양 크면 현재 메세지를 처음으로 
		//복사한다음 순환 된다.
		else
		{
			//순환 되기 전 마지막 좌표를 저장
			m_pLastMoveMark = m_pCurrentMark;
			CopyMemory( m_pBeginMark , 
				m_pCurrentMark - ( dwRemainLength - nForwardLength ) , 
				dwRemainLength );
			m_pCurrentMark = m_pBeginMark + dwRemainLength;
		}
	}
	return m_pCurrentMark;
}

void cRingBuffer::BackwardMark( int nBackwardLength )
{
	cMonitor::Owner lock( m_csRingBuffer );
	{
		//nBackwardLength양만큼 현재 버퍼포인터를 뒤로 보낸다.
		m_nUsedBufferSize -= nBackwardLength;
		m_pCurrentMark -= nBackwardLength;
	}
}

void cRingBuffer::ReleaseBuffer( int nReleaseSize )
{
	cMonitor::Owner lock( m_csRingBuffer );
	{
		m_nUsedBufferSize -= nReleaseSize;
	}
}

//사용된 버퍼양 설정(이것을 하는 이유는 SendPost()함수가 멀티 쓰레드에서 돌아가기때문에
//PrepareSendPacket()에서(ForwardMark()에서) 사용된 양을 늘려버리면 PrepareSendPacket한다음에 데이터를
//채워 넣기전에 바로 다른 쓰레드에서 SendPost()가 불린다면 엉뚱한 쓰레기 데이터가 갈 수 있다.
//그걸 방지하기 위해 데이터를 다 채운 상태에서만 사용된 버퍼 사이즈를 설정할 수 있어야한다.
//이함수는 sendpost함수에서 불리게 된다.
void cRingBuffer::SetUsedBufferSize( int nUsedBufferSize )
{
	cMonitor::Owner lock( m_csRingBuffer );
	{
		m_nUsedBufferSize += nUsedBufferSize;
		m_uiAllUserBufSize += nUsedBufferSize;
	}
}

char* cRingBuffer::GetBuffer( int nReadSize , int* pReadSize )
{
	char* pRet = NULL;
	cMonitor::Owner lock( m_csRingBuffer );
	{
		//마지막까지 다 읽었다면 그 읽어들일 버퍼의 포인터는 맨앞으로 옮긴다.
		if( m_pLastMoveMark == m_pGettedBufferMark )
		{
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark = m_pEndMark;
		}

		//현재 버퍼에 있는 size가 읽어들일 size보다 크다면
		if( m_nUsedBufferSize > nReadSize )
		{
			//링버퍼의 끝인지 판단.
			if( ( m_pLastMoveMark - m_pGettedBufferMark ) >= nReadSize )
			{
				*pReadSize = nReadSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += nReadSize;
			}
			else
			{
				*pReadSize = (int)( m_pLastMoveMark - m_pGettedBufferMark );
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
		else if( m_nUsedBufferSize > 0 )
		{
			//링버퍼의 끝인지 판단.
			if( ( m_pLastMoveMark - m_pGettedBufferMark ) >= m_nUsedBufferSize )
			{
				*pReadSize = m_nUsedBufferSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += m_nUsedBufferSize;
			}
			else
			{
				*pReadSize = (int)( m_pLastMoveMark - m_pGettedBufferMark );
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
	}
	return pRet;
}
