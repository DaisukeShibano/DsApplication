#include "DsPch.h"
#include "Error/DsError.h"


using namespace DsLib;

void DsLib::DsAssertDummyFunc()
{
	printf("assert");
	throw "assert";
}