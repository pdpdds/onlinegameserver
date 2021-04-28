#include "Precompile.h"
#include "cmonitor.h"

cMonitor::cMonitor(void)
{
	InitializeCriticalSection(&m_csSyncObject);
}

cMonitor::~cMonitor(void)
{
	DeleteCriticalSection( &m_csSyncObject );
}

#if(_WIN32_WINNT >= 0x0400)
BOOL cMonitor::TryEnter()
{
   return TryEnterCriticalSection(&m_csSyncObject);
}
#endif

void cMonitor::Enter()
{
   ::EnterCriticalSection(&m_csSyncObject);
}

void cMonitor::Leave()
{
   ::LeaveCriticalSection(&m_csSyncObject);
}

cMonitor::Owner::Owner( cMonitor &crit )
   : m_csSyncObject(crit)
{
	m_csSyncObject.Enter();
}

cMonitor::Owner::~Owner()
{
   m_csSyncObject.Leave();
}