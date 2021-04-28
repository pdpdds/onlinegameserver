#pragma once
#include "cNpc.h"
class cNormalNpc : public cNpc
{
public:
	cNormalNpc(void);
	virtual ~cNormalNpc(void);
	virtual void	OnProcess();
	virtual void	Init();
};
