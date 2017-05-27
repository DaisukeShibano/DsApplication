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
using namespace DsPhysics;

static const int ITERATION_NUM = 4;


DsCollisionListener::DsCollisionListener( DsPhysicsWorld& world )
:m_world(world)
,m_pColExecuter(NULL)
,m_pBoundingTree(NULL)
{
	m_pColExecuter = new DsCollisionExecuter();
	m_pBoundingTree = new DsBoundingOctree();
	m_pBoundingTree->CreateTree(64);
}

DsCollisionListener::~DsCollisionListener()
{
	delete m_pColExecuter; m_pColExecuter = NULL;
	delete m_pBoundingTree; m_pBoundingTree = NULL;
}

void DsCollisionListener::Collide( DsCollisionGroup& group )
{
	//group.SortActor();//積み重なったときに安定するように。iteration増やしたほうがマシかも
	const int totalActTNum = group.GetActorNumber();
	DsActor** pActors = group.GetActors();
	DsConstraintSolver& solver = m_world.RefConstraintSolver();

	if (_IsUseBoundingGroup(totalActTNum))
	{
		DsVec3d minPos = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
		DsVec3d maxPos = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
		for (int i = 0; i < totalActTNum; ++i){
			const DsVec3d& tmpPos = pActors[i]->GetPosition();
			const DsVec3d& tmpLen = pActors[i]->RefAabb().GetMax();
			minPos.x = min(minPos.x, tmpPos.x - tmpLen.x);
			minPos.y = min(minPos.y, tmpPos.y - tmpLen.y);
			minPos.z = min(minPos.z, tmpPos.z - tmpLen.z);
			maxPos.x = max(maxPos.x, tmpPos.x + tmpLen.x);
			maxPos.y = max(maxPos.y, tmpPos.y + tmpLen.y);
			maxPos.z = max(maxPos.z, tmpPos.z + tmpLen.z);
		}
		m_pBoundingTree->Update(pActors, totalActTNum, minPos, maxPos);

		//ツリーを巡って当たり判定
		//http://marupeke296.com/COL_2D_No8_QuadTree.html
		//まったく同じではない。参考にしている程度
		std::vector < const DsActor* > colideStack;
		colideStack.reserve(totalActTNum);

		const DsBdOctreeNode* pNode = m_pBoundingTree->GetRootNode();
		bool isBack = false;
		while (pNode){
			if (isBack){//子供から戻ってきたときは判定済みなので子との衝突判定はすべて終わっている
				//同空間にいる自分たちをリストからはずし、兄弟（いなければ親へ)
				const DsActor* pActor = pNode->actor;
				while (pActor){
					colideStack.pop_back();
					//同空間の次のactorへ
					pActor = pActor->GetOctreeNodeNext();
				}
			}else{
					{//親たちとの衝突判定
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
									const DsCollisionResult& result = m_pColExecuter->Exe(*pActor, *pParent);
									if (0 < result.GetColNum()) {
										solver.AddCollision(result);
									}
								}
								//同空間の次のactorへ
							}
							pActor = pActor->GetOctreeNodeNext();
						}
					}

				{//この空間に登録されているもの同士で当たり判定
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
								const DsCollisionResult& result = m_pColExecuter->Exe(*pActor, *pNextActor);
								if (0 < result.GetColNum()){
									solver.AddCollision(result);
								}
							}
							pNextActor = pNextActor->GetOctreeNodeNext();
						}

						//子供がいるなら、自分たちと衝突判定してもらうためにリストに追加
						if (pNode->child){
							colideStack.push_back(pActor);
						}
						//同空間の次のactorへ
						pActor = pActor->GetOctreeNodeNext();
					}
				}
			}

			//深さ優先でツリーを辿る
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
	else{//空間分割を使わない総当り判定。数が少ないときは所属空間の割り振り・探索しない分こちらの方が速い（それくらいだったらもう無視してもいいのかも）
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
					{
						const DsCollisionResult& result = m_pColExecuter->Exe(*pActors[actIdx_1], *pActors[actIdx_2]);
						if (0 < result.GetColNum()){
							solver.AddCollision(result);
						}
					}
				}
			}
		}
	}
}

//拘束に登録せずその場で判定して結果を即返す
void DsCollisionListener::Cast( const DsActor& actor, const DsCollisionGroup& group, std::vector<DsCollisionResult>& resultVec ) const
{
	const int actNum = group.GetActorNumber();
	const DsActor*const* pActors = group.GetActors();

	for(int actIdx = 0; actNum > actIdx; ++actIdx)
	{
		DsCollisionExecuter executor;
		const DsCollisionResult& result =  executor.Exe( *pActors[actIdx], actor );

		if( result.GetColNum() > 0 )
		{
			resultVec.push_back(result);
		}
	}
}

bool DsCollisionListener::_IsUseBoundingGroup(int actorNum) const
{
	//分割することでパフォーマンス向上が見込めるか判定

	//素の計算数
	const unsigned long int totalNum = (actorNum*actorNum - actorNum) / 2;

	//分割後の計算数
	const  unsigned long int bNum = m_pBoundingTree->GetResolutionNum();//分割数
	const  unsigned long int dividedNum = bNum*actorNum * 2;//登録するときと検索するとき

	return dividedNum < totalNum;
}