#include "DsPch.h"
#ifndef _DS_ANIM_CONTROLLER_H_
#include "Animation/ActionRequest/DsAnimController.h"
#endif

using namespace DsLib;


DsAnimController::DsAnimController()
{

}

DsAnimController::~DsAnimController()
{

}

//virtual
void DsAnimController::Update(double dt)
{

}

//virtual 
bool DsAnimController::IsMove() const
{
	return false;
}