#include "DsPhysicsPch.h"
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#include "Constraint/DsCollisionConstraint.h"
#ifndef __ICONSTRAINT__
#include "Constraint/IConstraint.h"
#endif
#ifndef _DS_POSITION_DEPTH_SOLVER_
#include "Constraint/ConstraintSolver/DsPositionDepthSolver.h"
#endif

using namespace DsPhysics;

static const double s_colBufMinMaxTime = 30.0;
static const int s_defaultColNum = 4096;
static const int s_matSolveIteNum = 10;

DsConstraintSolver::DsConstraintSolver(const DsPhysicsWorld& world)
	: m_world(world)
	, m_constraints()
	, m_colConstraintBuff(NULL)
	, m_colConstraintBuffSize(0)
	, m_colConstraintBuffSizeMin(0)
	, m_colConstraintBuffUseSize(0)
	, m_colBufMinTimer(0)
	, m_eqs()
	, m_currentIteration(0)
	, m_pPosDepthSolver(NULL)
{	
	m_colConstraintBuffSize = sizeof(DsCollisionConstraint) * s_defaultColNum;
	m_colConstraintBuffSizeMin = m_colConstraintBuffSize;

	//�Փ˃R���X�g�����g�̃������m��
	//m_colConstraintBuff = new char[m_colConstraintBuffSize];
	DsCollisionConstraint* tmp = new DsCollisionConstraint[s_defaultColNum];
	DS_ASSERT(tmp, "�������m�ێ��s");
	m_colConstraintBuff = reinterpret_cast<ds_uint8*>(tmp);

	m_pPosDepthSolver = new DsPositionDepthSolver(world);
	DS_ASSERT(m_pPosDepthSolver, "�������m�ێ��s");
}

//virtual 
DsConstraintSolver::~DsConstraintSolver()
{
	delete[] reinterpret_cast<DsCollisionConstraint*>(m_colConstraintBuff);
	m_colConstraintBuff = NULL;

	delete m_pPosDepthSolver; m_pPosDepthSolver = NULL;
}

void DsConstraintSolver::AddCollision(const DsCollisionResult& colResult)
{
	m_pPosDepthSolver->AddDepth(colResult);

	const int colNum = colResult.GetColNum();

	//����Ȃ������烁��������
	const size_t currSize = sizeof(DsCollisionConstraint)*colNum + m_colConstraintBuffUseSize;
	if (m_colConstraintBuffSize < currSize){
		const size_t tmpSize = m_colConstraintBuffSize;
		m_colConstraintBuffSize = currSize;
		//����
		const unsigned int tmpNum = static_cast<unsigned int>( m_colConstraintBuffSize / sizeof(DsCollisionConstraint));
		DsCollisionConstraint* newBuf = new DsCollisionConstraint[tmpNum];
		DS_ASSERT(newBuf, "�������m�ێ��s");
		ds_uint8* tmpBuf = reinterpret_cast<ds_uint8*>(newBuf);
		//���e�R�s�[
		memcpy(tmpBuf, m_colConstraintBuff, tmpSize);
		delete[] reinterpret_cast<DsCollisionConstraint*>(m_colConstraintBuff);
		m_colConstraintBuff = tmpBuf;
	}
	
	for (int i = 0; i < colNum; ++i){
		//DsCollisionConstraint* c = new(m_colConstraintBuff + m_colConstraintBuffUseSize)
		//	DsCollisionConstraint(colResult.RefOwnerId1()[i], colResult.RefOwnerId2()[i], &m_world, colResult.RefPos()[i], colResult.RefNormal()[i], m_world.GetDt(), colResult.RefDepth()[i]);
		DsCollisionConstraint* c = reinterpret_cast<DsCollisionConstraint*>(m_colConstraintBuff + m_colConstraintBuffUseSize);
		c->Initialize(colResult.GetOwnerId1()[i], colResult.GetOwnerId2()[i], &m_world, colResult.GetPos()[i], colResult.GetNormal()[i], m_world.GetDt(), colResult.GetDepth()[i]);
		m_colConstraintBuffUseSize += sizeof(DsCollisionConstraint);
	}
}

void DsConstraintSolver::AddConstraint(IConstraint* pConstraint)
{
	m_constraints.push_back(pConstraint);
}

void DsConstraintSolver::RemoveConstraint(IConstraint* pConstraint)
{
	m_constraints.remove(pConstraint);
}

void DsConstraintSolver::Solve(const int maxIteration, double dt)
{
	for (IConstraint* pConstraint : m_constraints) {//�P�S�����ƂɂP��
		pConstraint->SetUp();//�Z�b�g�A�b�v�͋t�s�񋁂߂���d���̂ōŏ��̂P�񂾂�
	}
	for (int i = 0; i < maxIteration; ++i) {
		for each(IConstraint* pConstraint in m_constraints)//�P�S�����ƂɂP��
		{
			pConstraint->UpdateExForce();
		}
		for each(IConstraint* pConstraint in m_constraints)//�P�S�����ƂɂP��
		{
			pConstraint->CalclateConstraintForce();
		}
		for each(IConstraint* pConstraint in m_constraints)//�P�S�����ƂɂP��
		{
			pConstraint->ApplyConstraintForce();
		}
	}

	const int useColNum = static_cast<int>(m_colConstraintBuffUseSize / sizeof(DsCollisionConstraint));
	DsCollisionConstraint* cols = reinterpret_cast<DsCollisionConstraint*>(m_colConstraintBuff);
//	double ms = 0;
	for (int i = 0; i < maxIteration; ++i){
		for (int colIdx = 0; colIdx < useColNum; ++colIdx){
			
			cols[colIdx].SetUp();
			
//			DsPerf::RefPerfDefault().Begin();
			cols[colIdx].CalclateConstraintForce();
//			DsPerf::RefPerfDefault().End();

			
			//�����ŁA���̏Փ˓_��CalclateConstraintForce������O�ɁA���̂ɗ͂�������B��������ΏՓ˓_���Ƃɔ{�����Ȃ��B
			//�J��Ԃ��̒P�ʂ́A�J��Ԃ��̒��r���[�ȂƂ���Ŏ��̍S���ɍs���Ă��܂��Ƃ܂����̂ŁA�P�Փ˓_���ŌJ��Ԃ��I����B
			DsCollisionConstraintSolver cSolver;
			cSolver.Solve(cols[colIdx], s_matSolveIteNum);
			cols[colIdx].ApplyConstraintForce();

//			ms += DsPerf::RefPerfDefault().GetMsec();
		}
	}
//	DS_LOG("==================  %f%%", ms/0.16);

	
	//��x�ł��f�t�H���g�T�C�Y�ȏ�Ȃ�J�E���g�N���A
	if (m_colConstraintBuffSizeMin < m_colConstraintBuffUseSize){
		m_colBufMinTimer = 0;
	}
	
	//�]���Ɋm�ۂ�������߂�
	m_colBufMinTimer += dt;
	if (s_colBufMinMaxTime < m_colBufMinTimer){
		if (m_colConstraintBuffSizeMin < m_colConstraintBuffSize){
			delete[] reinterpret_cast<DsCollisionConstraint*>(m_colConstraintBuff);
			m_colConstraintBuff = new ds_uint8[m_colConstraintBuffSizeMin];
		}
		m_colBufMinTimer = 0;
	}
	
	//�Փ˂͂P�t������Ȃ̂ō폜
	m_colConstraintBuffUseSize = 0;
}


//id��actor�ɂ͗͂��������A�߂�l�Ƃ��ĕԂ��B����ȊO�͍S���͂�������
DsVec3d DsConstraintSolver::SolveCollision(const DsActorId id, const int maxIteration, double dt)
{
	DsVec3d ret = DsVec3d::Zero();
	const int useColNum = static_cast<int>(m_colConstraintBuffUseSize / sizeof(DsCollisionConstraint));
	DsCollisionConstraint* cols = reinterpret_cast<DsCollisionConstraint*>(m_colConstraintBuff);
	for (int i = 0; i < maxIteration; ++i) {
		for (int colIdx = 0; colIdx < useColNum; ++colIdx) {
			cols[colIdx].SetUp();
			cols[colIdx].CalclateConstraintForce();
			DsCollisionConstraintSolver cSolver;
			cSolver.Solve(cols[colIdx], s_matSolveIteNum);
			ret += cols[colIdx].ApplyConstraintForceOne(id);
		}
	}

	//�Փ˂͂P�t������Ȃ̂ō폜
	m_colConstraintBuffUseSize = 0;

	return ret;
}

int DsConstraintSolver::GetIterationNum() const
{
	return 10;
}



