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
	DS_ASSERT(m_pBoundingTree, "メモリ確保失敗");
	m_pBoundingTree->CreateTree(64);
}

DsCollisionListener::~DsCollisionListener()
{
	delete m_pBoundingTree; m_pBoundingTree = NULL;
}

void DsCollisionListener::Collide( DsCollisionGroup& group )
{
	group.SortActor();//積み重なったときに安定するように。iteration増やしたほうがマシかも？
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
		//actorの所属空間更新
		m_pBoundingTree->Update(pActors, totalActTNum, minPos, maxPos);

#if 1
		auto func = [&executer, pSolver](const DsActor& a, const DsActor& b) {
			const bool isSkip = (((a.IsRest()) && (b.IsRest())) ||
				((a.IsRest()) && (b.RefOption().isStatic)) ||
				((a.RefOption().isStatic) && (b.IsRest())) ||
				((a.RefOption().isStatic) && (b.RefOption().isStatic))
				);
			if (!isSkip) {
				const DsCollisionResult& result = executer.Exe(a, b);
				if (0 < result.GetColNum()) {
					//拘束への追加までやってしまっているのでスレッドセーフでない。外に出したい
					pSolver->AddCollision(result);
				}
			}
		};
		_Query(func, totalActTNum);

#else //動作が安定したら消す
		//分かりやすい解説
		//http://marupeke296.com/COL_2D_No8_QuadTree.html
		std::vector < const DsActor* > colideStack;
		colideStack.reserve(totalActTNum);

		//パフォーマンスのため再帰使わずにツリーを巡回する
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
								const DsCollisionResult& result = executer.Exe(*pActor, *pParent);
								if (0 < result.GetColNum()) {
									//拘束への追加までやってしまっているのでスレッドセーフでない。外に出したい
									pSolver->AddCollision(result);
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
								const DsCollisionResult& result = executer.Exe(*pActor, *pNextActor);
								if (0 < result.GetColNum()){
									//拘束への追加までやってしまっているのでスレッドセーフでない。外に出したい
									pSolver->AddCollision(result);
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
			}else if(pNode->right){
				pNode = pNode->right;
				isBack = false;
			}else {
				pNode = pNode->parent;
				isBack = true;
			}
		}
#endif

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
					const DsCollisionResult& result = executer.Exe(*pActors[actIdx_1], *pActors[actIdx_2]);
					if (0 < result.GetColNum()){
						pSolver->AddCollision(result);
					}
					
				}
			}
		}
	}
}

void DsCollisionListener::OneColide(DsConstraintSolver& outSolver, const DsActorId& actor, const DsCollisionGroup& group)
{
	DsVec3d ret = DsVec3d::Zero();
	const int totalActTNum = group.GetActorNumber();
	DsCollisionExecuter executer(m_world);
	
	const DsActor& a = *actor.GetActor();
	if (_IsUseBoundingGroup(totalActTNum))
	{
		std::vector < const DsActor* > targetActors;
		m_pBoundingTree->GetContainAreaActors(a, targetActors);
		for (const DsActor* pActor : targetActors) {

			const DsActor& b = *pActor;
			const bool isSkip = (((a.IsRest()) && (b.IsRest())) ||
				((a.IsRest()) && (b.RefOption().isStatic)) ||
				((a.RefOption().isStatic) && (b.IsRest())) ||
				((a.RefOption().isStatic) && (b.RefOption().isStatic))
				);
			if (!isSkip) {
				const DsCollisionResult& result = executer.Exe(*pActor, a);
				if (result.GetColNum() > 0) {
					outSolver.AddCollision(result);
				}
			}
		}
	}
	else
	{
		const DsActor*const* pActors = group.GetActors();
		for (int i = 0; i < totalActTNum; ++i) {

			const DsActor& b = *pActors[i];
			const bool isSkip = (((a.IsRest()) && (b.IsRest())) ||
				((a.IsRest()) && (b.RefOption().isStatic)) ||
				((a.RefOption().isStatic) && (b.IsRest())) ||
				((a.RefOption().isStatic) && (b.RefOption().isStatic))
				);
			if (!isSkip) {
				const DsCollisionResult& result = executer.Exe(*pActors[i], a);
				if (result.GetColNum() > 0) {
					outSolver.AddCollision(result);
				}
			}
		}
	}
}

//拘束に登録せずその場で判定して結果を即返す
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
bool DsCollisionListener::Cast(const DsActor& actor, const DsCollisionGroup& group) const
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
				return true;
			}
		}
	}
	else
	{
		const DsActor*const* pActors = group.GetActors();
		for (int i = 0; i < totalActTNum; ++i) {
			const DsCollisionResult& result = executer.Exe(*pActors[i], actor);
			if (result.GetColNum() > 0) {
				return true;
			}
		}
	}
	return false;
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