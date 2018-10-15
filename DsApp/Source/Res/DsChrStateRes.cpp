#include "DsAppPch.h"
#include "Res/DsChrStateRes.h"
//‘¼‚Ìƒwƒbƒ_

using namespace DsLib;
using namespace DsApp;


DsChrStateRes::DsChrStateRes()
	: DsResItem()
	, m_pData(NULL)
{

}
DsChrStateRes::~DsChrStateRes()
{
	delete m_pData; m_pData = NULL;
}


//virtual
bool DsChrStateRes::Initialize(const char* path, DsResource& resource)
{
	
	return true;
}
