#include "DsPch.h"
#include "Error/DsError.h"


using namespace DsLib;

void DsLib::DsAssertDummyFunc()
{
	printf("assert");
	int* a = 0;
	(*a) = 0;
	//throw "assert";
}