#pragma once
#include "cNpc.h"
class cDetectNpc : public cNpc
{
public:
	cDetectNpc(void);
	virtual ~cDetectNpc(void);

	virtual void	OnProcess();    //처리 함수
	virtual void	Init();         //초기화 함수 
	void			InitDetect();   //초기화 함수
	void			DoDetect();     //탐색 함수
	void			DoDisComport(); //불쾌 상태일때 불리는 함수 
private:
	
	BYTE	m_byDisComportIndex;	//불쾌 지수 , 지수가 5이상이면 화냄으로 바뀜
};
