#include "DsAppPch.h"
#ifndef _DS_GP_CONTROLLER_H_
#include "World/Population/Controller/DsGpController.h"
#endif

#ifndef _DS_DECISION_TREE_NODE_H_
#include "Evolve/Gene/DsDecisionTreeNode.h"
#endif

using namespace DsApp;


enum
{
	SENSOR_0,
	SENSOR_1,
	SENSOR_2,
	SENSOR_3,
	SENSOR_4,
	SENSOR_NUM,
};

enum
{
	ACTION_0,
};


DsGpController::DsGpController()
	: m_pGpInputData(NULL)
	, m_pGpOutputData(NULL)
{
	m_pGpInputData = new DsDecisionTreeData[SENSOR_NUM];
	m_pGpOutputData = new DsDecisionTreeData;
}

DsGpController::~DsGpController()
{
	delete[] m_pGpInputData;
	m_pGpInputData = NULL;

	delete m_pGpOutputData;
	m_pGpOutputData = NULL;
}

//virtual 
void DsGpController::Update(double dt)
{

}