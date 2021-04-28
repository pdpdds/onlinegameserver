#ifdef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif
#include <iostream>
using namespace std;

class DLLTEST_API cMyClass
{
private:
	int m_nSum;

public:
	void Sum( int a, int b );

	void OutputSum();
};