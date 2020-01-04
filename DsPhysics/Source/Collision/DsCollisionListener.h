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
			//分かりやすい解説
			//http://marupeke296.com/COL_2D_No8_QuadTree.html
			std::vector < const DsActor* > colideStack;
			colideStack.reserve(reserveActorNum);

			//再帰使わないのはパフォーマンスのため
			const DsBdOctreeNode* pNode = m_pBoundingTree->GetRootNode();
			bool isBack = false;
			while (pNode) {
				if (isBack) {//子供から戻ってきたときは判定済みなので子との衝突判定はすべて終わっている
							 //同空間にいる自分たちをリストからはずし、兄弟（いなければ親へ)
					const DsActor* pActor = pNode->actor;
					while (pActor) {
						colideStack.pop_back();
						//同空間の次のactorへ
						pActor = pActor->GetOctreeNodeNext();
					}
				}
				else {
					{//親たちとの衝突判定
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
								//		//拘束への追加までやってしまっているのでスレッドセーフでない。外に出したい
								//		pSolver->AddCollision(result);
								//	}
								//}

								//同空間の次のactorへ
							}
							pActor = pActor->GetOctreeNodeNext();
						}
					}

					{//この空間に登録されているもの同士で当たり判定
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
								//		//拘束への追加までやってしまっているのでスレッドセーフでない。外に出したい
								//		pSolver->AddCollision(result);
								//	}
								//}

								pNextActor = pNextActor->GetOctreeNodeNext();
							}

							//子供がいるなら、自分たちと衝突判定してもらうためにリストに追加
							if (pNode->child) {
								colideStack.push_back(pActor);
							}
							//同空間の次のactorへ
							pActor = pActor->GetOctreeNodeNext();
						}
					}
				}

				//深さ優先でツリーを辿る
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