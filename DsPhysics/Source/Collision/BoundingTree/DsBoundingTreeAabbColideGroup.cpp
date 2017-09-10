#include "DsPhysicsPch.h"
#ifndef __DS_BOUNDING_TREE_AABB_COLIDE_GROUP__
#include "Collision/BoundingTree/DsBoundingTreeAabbColideGroup.h"
#endif
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef _DS_AABB_H_
#include "Math/DsAabb.h"
#endif

using namespace DsLib;
using namespace DsPhysics;

static const int CHILD_NUM = 8;

DsBoundingTreeAabbColideGroup::DsBoundingTreeAabbColideGroup( const DsAabb& aabb)
	: DsBoundingTreeBase()
	, m_aabb(aabb)
	, m_groupMap()
	, m_pBuf(NULL)
	, m_useBufSize(0)
{
	m_child.reserve(CHILD_NUM);
}

DsBoundingTreeAabbColideGroup::~DsBoundingTreeAabbColideGroup()
{
	delete[] m_pBuf; m_pBuf = NULL;
	m_useBufSize = 0;
}

void DsBoundingTreeAabbColideGroup::Initialize()
{
	m_pBuf = new char[sizeof(DsBoundingTreeAabbColideGroup)*CHILD_NUM];
}

//virtual 
void DsBoundingTreeAabbColideGroup::ConstructTree()
{
	const DsVec3d halfSize = m_aabb.GetMax()*0.5;
	const DsVec3d offsetPos[] =
	{
		m_aabb.GetPos() + DsVec3d(halfSize.x, halfSize.y, halfSize.z),
		m_aabb.GetPos() + DsVec3d(halfSize.x, halfSize.y, -halfSize.z),
		m_aabb.GetPos() + DsVec3d(-halfSize.x, halfSize.y, -halfSize.z),
		m_aabb.GetPos() + DsVec3d(-halfSize.x, halfSize.y, halfSize.z),

		m_aabb.GetPos() + DsVec3d(halfSize.x, -halfSize.y, halfSize.z),
		m_aabb.GetPos() + DsVec3d(halfSize.x, -halfSize.y, -halfSize.z),
		m_aabb.GetPos() + DsVec3d(-halfSize.x, -halfSize.y, -halfSize.z),
		m_aabb.GetPos() + DsVec3d(-halfSize.x, -halfSize.y, halfSize.z),
	};
	static_assert(sizeof(offsetPos) / sizeof(offsetPos[0]) == CHILD_NUM, "サイズ不一致");

	DsAabb box;
	box.Setup(halfSize.x, halfSize.y, halfSize.z, DsVec3d::Zero());
	for (int ci = 0; ci < CHILD_NUM; ++ci){
		box.SetPos(offsetPos[ci]);
		DsBoundingTreeAabbColideGroup* pChild = new(m_pBuf + m_useBufSize)DsBoundingTreeAabbColideGroup(box);
		m_child.push_back(pChild);
		m_useBufSize += sizeof(DsBoundingTreeAabbColideGroup);
	}
}

//virtual 
bool DsBoundingTreeAabbColideGroup::IsContain(const DsBoundingTreeAabbColideGroup& cmp) const
{
	return true;//使いません
}

void DsBoundingTreeAabbColideGroup::Update(const DsActor*const* pActors, int actorNum)
{
	DsVec3d center = DsVec3d::Zero();
	DsVec3d maxSize = DsVec3d::Zero();
	for (int i = 0; i < actorNum; ++i){
		center += pActors[i]->GetPosition();
	}
	center /= (double)actorNum;
	for (int i = 0; i < actorNum; ++i){
		const DsVec3d& size = pActors[i]->RefAabb().GetMax();
		maxSize.x = max(maxSize.x, size.x + fabs(pActors[i]->GetPosition().x-center.x));
		maxSize.y = max(maxSize.y, size.y + fabs(pActors[i]->GetPosition().y-center.y));
		maxSize.z = max(maxSize.z, size.z + fabs(pActors[i]->GetPosition().z-center.z));
	}
	m_aabb.Setup(maxSize.x, maxSize.y, maxSize.z, center);

	m_child.clear();
	m_useBufSize = 0;
	ConstructTree();

	m_groupMap.clear();
	const int childNum = static_cast<int>(m_child.size());
	for (int actIdx = 0; actIdx < actorNum; ++actIdx){
		GroupMask mask = 0;
		//１つだけの領域内に入っている場合はそのまま振り分けてもいいけど、
		//跨いでいる場合は、跨いでるもの同士でやってしまうと２重で当たり判定してしまう。
		//一度判定したもの同士をやらないようにするには誰と誰が判定済みか覚えておかないといけないので難しい。
		//Actor側にGroupMaskを持たせて、一致したものだけやるってのは？AABBとマスク処理あんまり負荷違わない気がするけど。振り分けられるならそれが一番いいけど
		for (int childIdx = 0; childIdx < childNum; ++childIdx){
			const unsigned int isContain = DsAabb::IsContain(pActors[actIdx]->RefAabb(), m_child[childIdx]->m_aabb);
			mask |= (isContain << childIdx);
			const_cast<DsActor*>(pActors[actIdx])->m_testColMask = mask;
		}
		m_groupMap[0].push_back(pActors[actIdx]);
	}
}

int DsBoundingTreeAabbColideGroup::GetTotalBoxNum() const
{
	return CHILD_NUM;
}

//virtual 
void DsBoundingTreeAabbColideGroup::Draw(DsDrawCommand& com) const
{
	m_aabb.Draw(com);
	for each(DsBoundingTreeAabbColideGroup* pChild in m_child){
		pChild->Draw(com);
	}
}
