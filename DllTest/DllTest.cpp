#include "DllTest.h"

void cMyClass::Sum( int a , int b )
{
	m_nSum = a + b;
}

void cMyClass::OutputSum()
{
	cout<<"Sum : "<<m_nSum;
}