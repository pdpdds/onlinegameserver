#pragma once

class NETLIB_API cThread
{
public:
	cThread(void);
	~cThread(void);
	
	bool	CreateThread( DWORD dwWaitTick );
	void	DestroyThread();
	void	Run();
	void	Stop();
	void	TickThread();
	virtual void	OnProcess() = 0;
	
	inline DWORD GetTickCount() { return m_dwTickCount; }
	bool	IsRun() { return m_bIsRun; }

protected:
	HANDLE	m_hThread;
	HANDLE	m_hQuitEvent;
	bool	m_bIsRun;
	DWORD	m_dwWaitTick;
	DWORD	m_dwTickCount;
};
