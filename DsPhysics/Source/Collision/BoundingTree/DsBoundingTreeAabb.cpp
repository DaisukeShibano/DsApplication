#include "DsPhysicsPch.h"
#ifndef __DS_BOUNDING_TREE_AABB__
#include "Collision/BoundingTree/DsBoundingTreeAabb.h"
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

DsBoundingTreeAabb::DsBoundingTreeAabb(const DsActor& actor, const DsVec3d& pos, const DsVec3d& boxSize, const DsQuad* pFaces, int faceNum, const DsVec3d* pVertex, int vertexNum, const std::vector<int>& containFaceIdxs, int maxDepth)
	: DsBoundingTreeBase()
	, m_pos(pos)
	, m_boxSize(boxSize)
	, m_owner(actor)
	, m_pFace(pFaces)
	, m_faceNum(faceNum)
	, m_pVertex(pVertex)
	, m_vertexNum(vertexNum)
	, m_containFaceIdxs(containFaceIdxs)
	, m_child()
	, m_maxDepth(maxDepth)
{
	m_child.reserve(CHILD_NUM);
}

DsBoundingTreeAabb::~DsBoundingTreeAabb()
{
	for each(DsBoundingTreeAabb* pNode in m_child){
		delete pNode;
	}
}

//virtual 
void DsBoundingTreeAabb::ConstructTree()
{
	for each(DsBoundingTreeAabb* pNode in m_child){
		delete pNode;
	}
	m_child.clear();
	_ConstructTree(0);
}

void DsBoundingTreeAabb::_ConstructTree(int depth)
{
	const bool isTerminal = (m_containFaceIdxs.size() <= CHILD_NUM) //�����������ʂ̐��̕������Ȃ��̂ŁA�ʂ�AABB�𖖒[�Ƃ��Đ����B�����͕ʂɕ���������Ȃ��Ă���������
						|| (depth >= m_maxDepth);
	if (isTerminal){
		//�O�p�`��aabb�𓾂�
		for each(int faceIdx in m_containFaceIdxs){
			const DsQuad& face = m_pFace[faceIdx];
			DsVec3d avePos = DsVec3d::Zero();
			const int vn = face.vn;
			for (int vi = 0; vi < vn; ++vi){
				avePos += m_pVertex[face.index[vi]];
			}
			avePos /= (double)vn;
			DsVec3d maxLen = DsVec3d::Zero();
			for (int vi = 0; vi < vn; ++vi){
				const DsVec3d len = m_pVertex[face.index[vi]] - avePos;
				maxLen.x = max(maxLen.x, fabs(len.x));
				maxLen.y = max(maxLen.y, fabs(len.y));
				maxLen.z = max(maxLen.z, fabs(len.z));
			}
			//�O�p�`�P����aabb�𖖒[�m�[�h�ɂ���
			std::vector<int> tmp;
			tmp.push_back(faceIdx);
			DsBoundingTreeAabb* pChild = new DsBoundingTreeAabb(m_owner, avePos, maxLen, m_pFace, m_faceNum, m_pVertex, m_vertexNum, tmp, m_maxDepth);
			m_child.push_back(pChild);
		}
	}
	else{
		//�������
		const DsVec3d halfSize = m_boxSize*0.5;
		const DsVec3d offsetPos[] =	//�q�m�[�h�̊�ʒu
		{
			m_pos + DsVec3d(halfSize.x, halfSize.y, halfSize.z),	//�������ő�
			m_pos + DsVec3d(halfSize.x, halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, halfSize.y, halfSize.z),

			m_pos + DsVec3d(halfSize.x, -halfSize.y, halfSize.z),
			m_pos + DsVec3d(halfSize.x, -halfSize.y, -halfSize.z),
			m_pos + DsVec3d(-halfSize.x, -halfSize.y, -halfSize.z), //�������ŏ�
			m_pos + DsVec3d(-halfSize.x, -halfSize.y, halfSize.z),
		};
		static_assert(sizeof(offsetPos) / sizeof(offsetPos[0]) == CHILD_NUM, "�T�C�Y�s��v");

		DsAabb box;
		for (int ci = 0; ci < CHILD_NUM; ++ci){
			box.Setup(offsetPos[0], offsetPos[6]);

			std::vector<int> containFaceIdxs;

			//�O�p�`��aabb�𓾂�B�ǂ������̎O�p�`�����Z�o
			for each(int faceIdx in m_containFaceIdxs){
				const DsQuad& face = m_pFace[faceIdx];
				const int vn = face.vn;
				DsVec3d max = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
				DsVec3d min = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
				for (int vi = 0; vi < vn; ++vi){
					const DsVec3d v = m_pVertex[face.index[vi]];
					max.x = max(max.x, v.x);
					max.y = max(max.y, v.y);
					max.z = max(max.z, v.z);
					min.x = min(min.x, v.x);
					min.y = min(min.y, v.y);
					min.z = min(min.z, v.z);
				}
				DsAabb tri;
				tri.Setup(max, min);

				if (DsAabb::IsContain(box, tri)){
					//���̋��E�̒��Ɋ܂܂�Ă�
					containFaceIdxs.push_back(faceIdx);
				}
			}
			DsBoundingTreeAabb* pChild = new DsBoundingTreeAabb(m_owner, offsetPos[ci], halfSize, m_pFace, m_faceNum, m_pVertex, m_vertexNum, containFaceIdxs, m_maxDepth);
			pChild->_ConstructTree(depth+1);
			m_child.push_back(pChild);
		}
	}
}

//virtual 
bool DsBoundingTreeAabb::IsContain(const DsBoundingTreeAabb& cmp) const
{
	DsAabb a;
	a.Setup( m_pos + m_boxSize, m_pos - m_boxSize);

	DsAabb b;
	b.Setup( cmp.m_pos + cmp.m_boxSize, cmp.m_pos - cmp.m_boxSize);

	//�܂��͎�����cmp���܂܂�Ă��邩����
	const bool isContain = DsAabb::IsContain(a, b);
	if (isContain){//�܂܂�Ă��̂ł���ɓ����̎q������
		for each(const DsBoundingTreeAabb* pChild in m_child){
			if (pChild->IsContain(cmp)){
				return true;
			}
		}
	}
	
	return isContain;
}


//virtual
bool DsBoundingTreeAabb::IsContain(const DsAabb& cmp) const
{
	DsAabb a;
	a.Setup(m_pos + m_boxSize, m_pos - m_boxSize);

	DsAabb b = cmp;
	
	//�܂��͎�����cmp���܂܂�Ă��邩����
	const bool isContain = DsAabb::IsContain(a, b);
	if (isContain) {//�܂܂�Ă��̂ł���ɓ����̎q������
		for each(const DsBoundingTreeAabb* pChild in m_child) {
			if (pChild->IsContain(cmp)) {
				return true;
			}
		}
	}

	return isContain;
}

//virtual 
void DsBoundingTreeAabb::Draw(DsDrawCommand& com) const
{
	DsAabb a;
	a.Setup(m_pos + m_boxSize, m_pos - m_boxSize);
	a.Draw(com);

	for each(DsBoundingTreeAabb* pChild in m_child){
		pChild->Draw(com);
	}
}
