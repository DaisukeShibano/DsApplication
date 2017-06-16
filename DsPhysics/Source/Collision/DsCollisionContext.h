#ifndef __DS_COLLISION_CONTEXT__
#define __DS_COLLISION_CONTEXT__

/*
#ifndef __DS_COLLISION_CONTEXT__
#include "Collision/DsCollisionContext.h"
#endif
*/
#ifndef __DS_FACE__
#include "DsFace.h"
#endif
#ifndef __DS_LINE__
#include "DsLine.h"
#endif

namespace DsPhysics
{
	class DsActorId;
	class DsBoundingTreeBase;
}

namespace DsPhysics
{
	class DsCollisionContext
	{
	public:
		//@各種Actorで初期化できるコンストラクタ用意する@
		//DsCollisionContext(const DsRigidCube& rigid);
		//DsCollisionContext(const DsRigidMesh& mesh);
		

		DsCollisionContext(const DsVec3d* pVertex, const int vertexNum, const DsQuad* pFace, const int faceNum,
			const DsLine* pLine, const int lineNum, const DsActorId& id, const DsVec3d& basePos, const DsVec3d* pPreVertex,
			const DsVec3d& side, const DsBoundingTreeBase* pBoungingTree, const DsAabb* pAabb, const DsMat33d& rot)
			: m_pVertex(pVertex)
			, m_vertexNum(vertexNum)
			, m_pFace(pFace)
			, m_faceNum(faceNum)
			, m_pLine(pLine)
			, m_lineNum(lineNum)
			, m_ownerId(id)
			, m_basePos(basePos)
			, m_pPreVertex(pPreVertex)
			, m_side(side)
			, m_pBoungingTree(pBoungingTree)
			, m_pAabb(pAabb)
			, m_rot(rot)
		{}

	public:
		const DsVec3d* GetVertex() const { return m_pVertex; }
		const int GetVertexNum() const { return m_vertexNum; }
		const DsQuad* GetFace() const { return m_pFace; }
		const int GetFaceNum() const { return m_faceNum; }
		const DsLine* GetLine() const { return m_pLine; }
		const int GetLineNum() const { return m_lineNum; }
		const DsActorId& RefOwnerId() const { return m_ownerId; }
		const DsVec3d& GetBasePos() const { return m_basePos; }
		const DsVec3d* GetPreVertex() const { return m_pPreVertex; }
		const DsVec3d& GetSide() const { return m_side; }
		const DsBoundingTreeBase* GetBoungingTree() const { return m_pBoungingTree; }
		const DsAabb* GetAabb() const { return m_pAabb; }
		const DsMat33d& GetRot() const { return m_rot; }

	private:
		const DsVec3d* m_pVertex;
		const int m_vertexNum;
		const DsQuad* m_pFace;
		const int m_faceNum;
		const DsLine* m_pLine;
		const int m_lineNum;
		const DsActorId& m_ownerId;
		const DsVec3d& m_basePos;
		const DsVec3d* m_pPreVertex;
		const DsVec3d& m_side;
		const DsBoundingTreeBase* m_pBoungingTree;
		const DsAabb* m_pAabb;
		const DsMat33d& m_rot;
	};
}

#endif