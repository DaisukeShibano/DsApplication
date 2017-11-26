#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_EXECUTER__
#include "Collision/DsCollisionListener.h"
#endif

#include "Collision/DsCollisionGroup.h"
#include "Collision/DsCollisionExecuter.h"
#include "Collision/DsCollisionResult.h"
#include "Actor/DsActor.h"
#include "Constraint/DsCollisionConstraint.h"
#include "DsPhysicsWorld.h"
#ifndef __DS_BOUNDING_OCTREE__
#include "Collision/BoundingTree/DsBoundingOctree.h"
#endif
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif

using namespace DsPhysics;

static const int ITERATION_NUM = 4;


DsCollisionListener::DsCollisionListener( DsPhysicsWorld& world )
:m_world(world)
,m_pBoundingTree(NULL)
{
	m_pBoundingTree = new DsBoundingOctree();
	DS_ASSERT(m_pBoundingTree, "�������m�ێ��s");
	m_pBoundingTree->CreateTree(64);
}

DsCollisionListener::~DsCollisionListener()
{
	delete m_pBoundingTree; m_pBoundingTree = NULL;
}

void DsCollisionListener::Collide( DsCollisionGroup& group )
{
	//group.SortActor();//�ςݏd�Ȃ����Ƃ��Ɉ��肷��悤�ɁBiteration���₵���ق����}�V����
	const int totalActTNum = group.GetActorNumber();
	DsActor** pActors = group.GetActors();
	DsConstraintSolver* pSolver = m_world.GetConstraintSolver();
	DsCollisionExecuter executer(m_world);

	if (_IsUseBoundingGroup(totalActTNum))
	{
		DsVec3d minPos = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
		DsVec3d maxPos = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
		for (int i = 0; i < totalActTNum; ++i){
			const DsVec3d aabbMin = pActors[i]->RefAabb().GetMin();
			const DsVec3d aabbMax = pActors[i]->RefAabb().GetMax();
			minPos.x = min(minPos.x, aabbMin.x);
			minPos.y = min(minPos.y, aabbMin.y);
			minPos.z = min(minPos.z, aabbMin.z);
			maxPos.x = max(maxPos.x, aabbMax.x);
			maxPos.y = max(maxPos.y, aabbMax.y);
			maxPos.z = max(maxPos.z, aabbMax.z);
		}
		//actor�̏�����ԍX�V
		m_pBoundingTree->Update(pActors, totalActTNum, minPos, maxPos);

		//������₷�����
		//http://marupeke296.com/COL_2D_No8_QuadTree.html
		std::vector < const DsActor* > colideStack;
		colideStack.reserve(totalActTNum);

		//�p�t�H�[�}���X�̂��ߍċA�g�킸�Ƀc���[�����񂷂�
		const DsBdOctreeNode* pNode = m_pBoundingTree->GetRootNode();
		bool isBack = false;
		while (pNode){
			if (isBack){//�q������߂��Ă����Ƃ��͔���ς݂Ȃ̂Ŏq�Ƃ̏Փ˔���͂��ׂďI����Ă���
				//����Ԃɂ��鎩�����������X�g����͂����A�Z��i���Ȃ���ΐe��)
				const DsActor* pActor = pNode->actor;
				while (pActor){
					colideStack.pop_back();
					//����Ԃ̎���actor��
					pActor = pActor->GetOctreeNodeNext();
				}
			}else{
				{//�e�����Ƃ̏Փ˔���
					const DsActor* pActor = pNode->actor;
					while (pActor){
						const int stackNum = static_cast<int>(colideStack.size());
						for (int i = 0; i < stackNum; ++i) {
							const DsActor* pParent = colideStack[i];
							const bool isSkip = (((pActor->IsRest()) && (pParent->IsRest())) ||
								((pActor->IsRest()) && (pParent->RefOption().isStatic)) ||
								((pActor->RefOption().isStatic) && (pParent->IsRest())) ||
								((pActor->RefOption().isStatic) && (pParent->RefOption().isStatic))
								);
							if (!isSkip) {
								const DsCollisionResult& result = executer.Exe(*pActor, *pParent);
								if (0 < result.GetColNum()) {
									pSolver->AddCollision(result);
								}
							}
							//����Ԃ̎���actor��
						}
						pActor = pActor->GetOctreeNodeNext();
					}
				}

				{//���̋�Ԃɓo�^����Ă�����̓��m�œ����蔻��
					const DsActor* pActor = pNode->actor;
					while (pActor){
						const DsActor* pNextActor = pActor->GetOctreeNodeNext();
						while (pNextActor){
							const bool isSkip = (((pActor->IsRest()) && (pNextActor->IsRest())) ||
								((pActor->IsRest()) && (pNextActor->RefOption().isStatic)) ||
								((pActor->RefOption().isStatic) && (pNextActor->IsRest())) ||
								((pActor->RefOption().isStatic) && (pNextActor->RefOption().isStatic))
								);
							if (!isSkip){
								const DsCollisionResult& result = executer.Exe(*pActor, *pNextActor);
								if (0 < result.GetColNum()){
									pSolver->AddCollision(result);
								}
							}
							pNextActor = pNextActor->GetOctreeNodeNext();
						}

						//�q��������Ȃ�A���������ƏՓ˔��肵�Ă��炤���߂Ƀ��X�g�ɒǉ�
						if (pNode->child){
							colideStack.push_back(pActor);
						}
						//����Ԃ̎���actor��
						pActor = pActor->GetOctreeNodeNext();
					}
				}
			}

			//�[���D��Ńc���[��H��
			if (pNode->child && (!isBack)){
				pNode = pNode->child;
			}else if(pNode->brother){
				pNode = pNode->brother;
				isBack = false;
			}else {
				pNode = pNode->parent;
				isBack = true;
			}
		}
	}
	else{//��ԕ������g��Ȃ������蔻��B�������Ȃ��Ƃ��͏�����Ԃ̊���U��E�T�����Ȃ���������̕��������i���ꂭ�炢������������������Ă������̂����j
		for (int actIdx_1 = 0; actIdx_1 < totalActTNum - 1; ++actIdx_1)
		{
			for (int actIdx_2 = actIdx_1 + 1; actIdx_2 < totalActTNum; ++actIdx_2)
			{
				bool isSkip = (((pActors[actIdx_1]->IsRest()) && (pActors[actIdx_2]->IsRest())) ||
					((pActors[actIdx_1]->IsRest()) && (pActors[actIdx_2]->RefOption().isStatic)) ||
					((pActors[actIdx_1]->RefOption().isStatic) && (pActors[actIdx_2]->IsRest())) ||
					((pActors[actIdx_1]->RefOption().isStatic) && (pActors[actIdx_2]->RefOption().isStatic))
					);
	
				if (!isSkip)
				{
					const DsCollisionResult& result = executer.Exe(*pActors[actIdx_1], *pActors[actIdx_2]);
					if (0 < result.GetColNum()){
						pSolver->AddCollision(result);
					}
					
				}
			}
		}
	}
}

//�S���ɓo�^�������̏�Ŕ��肵�Č��ʂ𑦕Ԃ�
void DsCollisionListener::Cast( const DsActor& actor, const DsCollisionGroup& group, std::vector<DsCollisionResult>& resultVec ) const
{
	const int totalActTNum = group.GetActorNumber();
	DsCollisionExecuter executer(m_world);

	if (_IsUseBoundingGroup(totalActTNum))
	{
		std::vector < const DsActor* > targetActors;
		m_pBoundingTree->GetContainAreaActors(actor, targetActors);
		for (const DsActor* pActor : targetActors) {
			//((DsActor*)(pActor))->Draw(DsDbgSys::GetIns().RefDrawCom());
			const DsCollisionResult& result = executer.Exe(*pActor, actor);
			if (result.GetColNum() > 0) {
				resultVec.push_back(result);
			}
		}
	}
	else
	{
		const DsActor*const* pActors = group.GetActors();
		for (int i = 0; i < totalActTNum; ++i) {
			const DsCollisionResult& result = executer.Exe(*pActors[i], actor);
			if (result.GetColNum() > 0) {
				resultVec.push_back(result);
			}
		}
	}
}

bool DsCollisionListener::_IsUseBoundingGroup(int actorNum) const
{
	//�������邱�ƂŃp�t�H�[�}���X���オ�����߂邩����

	//�f�̌v�Z��
	const unsigned long int totalNum = (actorNum*actorNum - actorNum) / 2;

	//������̌v�Z��
	const  unsigned long int bNum = m_pBoundingTree->GetResolutionNum();//������
	const  unsigned long int dividedNum = bNum*actorNum * 2;//�o�^����Ƃ��ƌ�������Ƃ�

	return dividedNum < totalNum;
}