#include "DsAppPch.h"
#include "World/Field/Action/Enemy/DsActionRequestCom1.h"


using namespace DsLib;
using namespace DsApp;

DsActionRequestCom1:: DsActionRequestCom1(const DsSys& sys)
	: m_moveVec(DsVec3d::Zero())
	, m_request(0)
{

}

DsActionRequestCom1::~DsActionRequestCom1()
{

}

//virtual
void DsActionRequestCom1::Update(double dt)
{

}

//virtual
DsVec3d DsActionRequestCom1::GetMoveVec()const
{
	return m_moveVec;
}

//virtual
bool DsActionRequestCom1::IsAction(ACTION_TYPE type)const
{
	return (0ULL != (m_request & (1ULL << static_cast<ds_uint64>(type))));
}

//virtual
bool DsActionRequestCom1::IsMove()const
{
	return 0.0001 < GetMoveVec().LengthSq();
}
//virtual
void DsActionRequestCom1::SetRequest(ACTION_TYPE type)
{

}
//virtual
void DsActionRequestCom1::SetCancel(int type)
{

}
//virtual
void DsActionRequestCom1::SetCancelAll()
{

}
//virtual
void DsActionRequestCom1::SetInput(int type)
{

}
