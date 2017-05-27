#include "DsPch.h"
#include "Error/DsError.h"
#include < locale.h >
using namespace DsLib;


DsError::DsError()
{
}

DsError::~DsError()
{
}

//static
void DsError::Panic(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	abort();
}

//static
void DsError::Asset(const char* format, ...)
{
	va_list args; 
	va_start(args, format);
	vprintf(format, args);
	va_end(args); 
	assert(false);
}