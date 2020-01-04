#ifndef __DS_COLLISION_LISTENER__
#define __DS_COLLISION_LISTENER__

#ifndef __DS_BOUNDING_OCTREE__
#include "Collision/BoundingTree/DsBoundingOctree.h"
#endif
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

namespace DsPhysics
{
	class DsCollisionGroup;
	class DsCollisionResult;
	class DsActor;
	class DsActorId;
	class DsPhysicsWorld;
	class DsBoundingOctree;
	class DsConstraintSolver;
}

namespace DsPhysics
{
	class DsCollisionListener
	{
	public:
		DsCollisionListener( DsPhysicsWorld& world );
		virtual ~DsCollisionListener();
		void Collide( DsCollisionGroup& group );
		void OneColide(DsConstraintSolver& outSolver, const DsActorId& actor, const DsCollisionGroup& group);

		void Cast( const DsActor& actor, const DsCollisionGroup& group, std::vector<DsCollisionResult>& resultVec) const;
		bool Cast(const DsActor& actor, const DsCollisionGroup& group) const;

	private:
		template<typename FUNC>
		void _Query(FUNC func, size_t reserveActorNum ) const
		{
			//������₷�����
			//http://marupeke296.com/COL_2D_No8_QuadTree.html
			std::vector < const DsActor* > colideStack;
			colideStack.reserve(reserveActorNum);

			//�ċA�g��Ȃ��̂̓p�t�H�[�}���X�̂���
			const DsBdOctreeNode* pNode = m_pBoundingTree->GetRootNode();
			bool isBack = false;
			while (pNode) {
				if (isBack) {//�q������߂��Ă����Ƃ��͔���ς݂Ȃ̂Ŏq�Ƃ̏Փ˔���͂��ׂďI����Ă���
							 //����Ԃɂ��鎩�����������X�g����͂����A�Z��i���Ȃ���ΐe��)
					const DsActor* pActor = pNode->actor;
					while (pActor) {
						colideStack.pop_back();
						//����Ԃ̎���actor��
						pActor = pActor->GetOctreeNodeNext();
					}
				}
				else {
					{//�e�����Ƃ̏Փ˔���
						const int stackNum = static_cast<int>(colideStack.size());
						const DsActor* pActor = pNode->actor;
						while (pActor) {
							for (int i = 0; i < stackNum; ++i) {
								const DsActor* pParent = colideStack[i];
								func(*pActor, *pParent);

								//const bool isSkip = (((pActor->IsRest()) && (pParent->IsRest())) ||
								//	((pActor->IsRest()) && (pParent->RefOption().isStatic)) ||
								//	((pActor->RefOption().isStatic) && (pParent->IsRest())) ||
								//	((pActor->RefOption().isStatic) && (pParent->RefOption().isStatic))
								//	);
								//if (!isSkip) {
								//	const DsCollisionResult& result = executer.Exe(*pActor, *pParent);
								//	if (0 < result.GetColNum()) {
								//		//�S���ւ̒ǉ��܂ł���Ă��܂��Ă���̂ŃX���b�h�Z�[�t�łȂ��B�O�ɏo������
								//		pSolver->AddCollision(result);
								//	}
								//}

								//����Ԃ̎���actor��
							}
							pActor = pActor->GetOctreeNodeNext();
						}
					}

					{//���̋�Ԃɓo�^����Ă�����̓��m�œ����蔻��
						const DsActor* pActor = pNode->actor;
						while (pActor) {
							const DsActor* pNextActor = pActor->GetOctreeNodeNext();
							while (pNextActor) {

								func(*pActor, *pNextActor);

								//const bool isSkip = (((pActor->IsRest()) && (pNextActor->IsRest())) ||
								//	((pActor->IsRest()) && (pNextActor->RefOption().isStatic)) ||
								//	((pActor->RefOption().isStatic) && (pNextActor->IsRest())) ||
								//	((pActor->RefOption().isStatic) && (pNextActor->RefOption().isStatic))
								//	);
								//if (!isSkip) {
								//	const DsCollisionResult& result = executer.Exe(*pActor, *pNextActor);
								//	if (0 < result.GetColNum()) {
								//		//�S���ւ̒ǉ��܂ł���Ă��܂��Ă���̂ŃX���b�h�Z�[�t�łȂ��B�O�ɏo������
								//		pSolver->AddCollision(result);
								//	}
								//}

								pNextActor = pNextActor->GetOctreeNodeNext();
							}

							//�q��������Ȃ�A���������ƏՓ˔��肵�Ă��炤���߂Ƀ��X�g�ɒǉ�
							if (pNode->child) {
								colideStack.push_back(pActor);
							}
							//����Ԃ̎���actor��
							pActor = pActor->GetOctreeNodeNext();
						}
					}
				}

				//�[���D��Ńc���[��H��
				if (pNode->child && (!isBack)) {
					pNode = pNode->child;
				}
				else if (pNode->right) {
					pNode = pNode->right;
					isBack = false;
				}
				else {
					pNode = pNode->parent;
					isBack = true;
				}
			}
		}

	private:
		bool _IsUseBoundingGroup(int actorNum) const;

	private:
		DsPhysicsWorld& m_world;
		DsBoundingOctree* m_pBoundingTree;
	};

}

#endif