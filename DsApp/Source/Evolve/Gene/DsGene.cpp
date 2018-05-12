#include "DsAppPch.h"
#ifndef _DS_GENE_H_
#include "Evolve/Gene/DsGene.h"
#endif

#ifndef _DS_DECISION_TREE_NODE_H_
#include "Evolve/Gene/DsDecisionTreeNode.h"
#endif
#ifndef _DS_DECISION_TREE_OPERATOR_H_
#include "Evolve/Gene/DsDecisionTreeOperator.h"
#endif
#ifndef _DS_FIELD_CHR_H_
#include "World/Field/DsFieldChr.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_EVO_DEFINE_H_
#include "Evolve/DsEvoDefine.h"
#endif

using namespace DsLib;
using namespace DsApp;


DsGene::DsGene()
	: m_pRoot(NULL)
	, m_pData(NULL)
	, m_score(0)
	, m_curTime(0)
	, m_curTrial(0)
	, m_pChr(NULL)
{

}

DsGene::~DsGene()
{
	delete m_pData;
	m_pData = NULL;
}

void DsGene::Initialize(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world)
{
	m_pChr = new DsFieldChr(sys, world);

	DsHitRes::Shape shape;
	shape.sizeX=0.5;
	shape.sizeY=1.5;
	shape.sizeZ=0.5;
	shape.weight=100;
	shape.boundCoef=0.2;
	DsHitRes hitRes(shape);

	DsFieldInitInfo initInfo;
	initInfo.name = "GPƒLƒƒƒ‰";
	initInfo.pos = DsVec3d::Zero();
	initInfo.ang = DsVec3d::Zero();
	initInfo.hitName = EVO_CHR_HIT_RES_PATH;
	initInfo.animName = EVO_CHR_ANIM_RES_PATH;
	initInfo.physicsType = DS_MAP_OBJ_TYPE::DYNAMIC;
	m_pChr->Initialize(initInfo);
}

bool DsGene::Evaluation(double dt, double evalTime, int trialNum)
{
	bool ret = false;
	if (m_curTime < evalTime){

		ret = false;
	}
	else{
		++m_curTrial;
		if (trialNum <= m_curTrial){
			ret = true;
		}
		else{
			ret = false;
		}
	}
	m_curTime += dt;
	return ret;
}