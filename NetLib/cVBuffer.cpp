#include "precompile.h"
#include "cvbuffer.h"
#include "CommonDef.h"

IMPLEMENT_SINGLETON( cVBuffer );

cVBuffer::cVBuffer( int nMaxBufSize )
{
	m_pszVBuffer = new char[ nMaxBufSize ];
	m_nMaxBufSize = nMaxBufSize;
	Init();
}

cVBuffer::~cVBuffer(void)
{
	if( NULL != m_pszVBuffer )
		delete [] m_pszVBuffer;
}

void cVBuffer::Init()
{
	//PACKET_SIZE_LENGTH는 저절로 들어가게 만든다.
	m_pCurMark = m_pszVBuffer + PACKET_SIZE_LENGTH;	
	m_nCurBufSize = PACKET_SIZE_LENGTH;				
}

void cVBuffer::GetChar( char& cCh )
{
	cCh = (unsigned char)*m_pCurMark;
	m_pCurMark += 1;
	m_nCurBufSize += 1;
}

void cVBuffer::GetShort( short& sNum )
{
	sNum = (unsigned char)*m_pCurMark + 
		(((unsigned char)*(m_pCurMark + 1) )<< 8);
	m_pCurMark += 2;
	m_nCurBufSize += 2;
}

void cVBuffer::GetInteger( int& nNum )
{
	nNum = (unsigned char)m_pCurMark[0] + 
		((unsigned char)m_pCurMark[1] << 8) + 
		((unsigned char)m_pCurMark[2] << 16) + 
		((unsigned char)m_pCurMark[3] << 24);
	m_pCurMark += 4;
	m_nCurBufSize += 4;
}

//문자열말고 다른 byte stream을 읽을때 쓰인다
void cVBuffer::GetStream( char* pszBuffer, short sLen)	
{
	if( sLen < 0 || sLen > MAX_PBUFSIZE )
		return;
	CopyMemory( pszBuffer , m_pCurMark , sLen );
	m_pCurMark += sLen;
	m_nCurBufSize += sLen;
}

void cVBuffer::GetString( char* pszBuffer )
{
	short sLength;
	GetShort( sLength );
	if( sLength < 0 || sLength > MAX_PBUFSIZE )
		return;
	strncpy( pszBuffer, m_pCurMark, sLength);
	*(pszBuffer + sLength) = NULL;
	m_pCurMark += sLength;
	m_nCurBufSize += sLength;
}

void cVBuffer::SetInteger( int nI )
{
	*m_pCurMark++ = nI;
	*m_pCurMark++ = nI >> 8;
	*m_pCurMark++ = nI >> 16;
	*m_pCurMark++ = nI >> 24;

	m_nCurBufSize += 4;
}
	
void cVBuffer::SetShort( short sShort )
{
	*m_pCurMark++ = sShort;
	*m_pCurMark++ = sShort >> 8;
	m_nCurBufSize += 2;
}

void cVBuffer::SetChar( char cCh)
{
	*m_pCurMark++ = cCh;
	m_nCurBufSize += 1;
}

//문자열말고 다른 byte stream을 넣을때 쓰인다
void cVBuffer::SetStream( char* pszBuffer , short sLen )
{
	CopyMemory( m_pCurMark , pszBuffer , sLen );
	m_pCurMark += sLen;
	m_nCurBufSize += sLen;
}

void cVBuffer::SetString( char* pszBuffer )
{
	short sLen = (short)strlen( pszBuffer );
	if( sLen < 0 || sLen > MAX_PBUFSIZE )
		return;
    SetShort( sLen );
	
	CopyMemory( m_pCurMark , pszBuffer , sLen );
	m_pCurMark += sLen;
	m_nCurBufSize += sLen;
}

bool cVBuffer::CopyBuffer( char* pDestBuffer )
{
	CopyMemory( m_pszVBuffer , (char*)&m_nCurBufSize 
		, PACKET_SIZE_LENGTH );
	CopyMemory( pDestBuffer , m_pszVBuffer , m_nCurBufSize );
	return true;
}

void cVBuffer::SetBuffer( char* pVBuffer )
{
	m_pCurMark = pVBuffer ;
	m_nCurBufSize = 0;
}

