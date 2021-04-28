#pragma once

class cTickThread : public cThread
{
public:
	cTickThread(void);
	~cTickThread(void);

	virtual void	OnProcess();

};
