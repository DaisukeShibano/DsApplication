#include "DsAppPch.h"
#include "World/Field/Action/DsChrStateDefine.h"
//‘¼‚Ìƒwƒbƒ_
#include "World/Field/Action/DsChrState.h"
#include "World/Field/Action/Player/DsPlayerState.h"
#include "World/Field/Action/Enemy/DsEnemyState.h"

using namespace DsApp;
using namespace DsChrStateDefine;


void DsChrStateDefine::InitializeState()
{
	static bool isInit = false;
	if (!isInit) {
		DsPlayerState::Initialize();
		isInit = true;
	}
}