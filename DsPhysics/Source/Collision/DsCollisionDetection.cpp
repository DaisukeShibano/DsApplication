#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
#endif

#include "DsFace.h"
#include "DsLine.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionGeometry.h"


using namespace DsPhysics;


inline double _DsCD_PointFace( const DsVec3d& p, const DsQuad& f, const DsVec3d* fp )
{
	const double dot = DsVec3d::Dot(p - fp[f.index[0]], f.normal);
	return dot;
}

bool DsPhysics::DsCD_PointFace( const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2, DsCollisionResult& info )
{
	bool ret = false;

	const DsActorId& p1Id = pBox1->RefOwnerId();
	const DsVec3d* p1 = pBox1->GetVertex();
	const int p1VetexNum = pBox1->GetVertexNum();
	const DsActorId& p2Id = pBox2->RefOwnerId();
	const DsVec3d* p2 = pBox2->GetVertex();
	const DsQuad* p2Face = pBox2->GetFace();
	const int p2FaceNum = pBox2->GetFaceNum();
	const DsVec3d toDist = pBox2->GetBasePos() - pBox1->GetBasePos();

	if( p1 && p2 && p2Face )
	{
		for( int p1Idx = 0; p1VetexNum > p1Idx; ++p1Idx )
		{
			//�Փ˕����͑��葤�̖ʂ̖@���ɂȂ�
			const DsQuad* nearFace = NULL;
			double maxDot = -100000000000.0;
			for(int p2Idx = 0; p2FaceNum > p2Idx; ++p2Idx )
			{
				//�@���ɑ΂��鑊��̓_�̍��������߂�
				double dot = _DsCD_PointFace(p1[p1Idx], p2Face[p2Idx], p2);
				
				if( dot >= 0.0 )
				{
					if (dot <= 0.0001)
					{
						////�\�ʏ�ɋ������̂̌덷�͈̔͂������̂Ŋ܂߂�
						dot = -0.0001;
					}
					else
					{
						nearFace = NULL;
						break; //�ʂ̕\�ɂ���̂ŏՓ˂Ȃ�
					}
				}
				else
				{
					if( dot > maxDot )
					{
						maxDot = dot;
						nearFace = &p2Face[p2Idx];
					}
				}
			}
			
			if( nearFace )
			{
				//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3f(1, 0, 0)).DrawSphere(p1[p1Idx], 0.05);
				//�����܂ŗ�����p1[p1Idx]�͏Փ˂��Ă�
				//�߂荞�ݗʂ͂߂荞��ł�΃v���X
				info.AddInfo(p1[p1Idx], nearFace->normal, -maxDot, p1Id, p2Id);
				ret = true;
			}
		}
	}

	return ret;
}

bool DsPhysics::DsCD_PointFace(const DsActorId& p1Id, const DsVec3d& p1, const DsCollisionGeometry* pBox2, DsCollisionResult& info)
{
	bool ret = false;

	const DsActorId& p2Id = pBox2->RefOwnerId();
	const DsVec3d* p2 = pBox2->GetVertex();
	const DsQuad* p2Face = pBox2->GetFace();
	const int p2FaceNum = pBox2->GetFaceNum();
	const DsVec3d toDist = DsVec3d::Zero();

	if (p2 && p2Face)
	{
		
		//�Փ˕����͑��葤�̖ʂ̖@���ɂȂ�
		const DsQuad* nearFace = NULL;
		double maxDot = -100000000000.0;
		for (int p2Idx = 0; p2FaceNum > p2Idx; ++p2Idx)
		{
			//�@���ɑ΂��鑊��̓_�̍��������߂�
			const double dot = _DsCD_PointFace(p1, p2Face[p2Idx], p2);

			if (dot >= 0.0)
			{
				nearFace = NULL;
				break; //�ʂ̕\�ɂ���̂ŏՓ˂Ȃ�
			}
			else
			{
				if (dot > maxDot)
				{
					maxDot = dot;
					nearFace = &p2Face[p2Idx];
				}
			}
		}

		if (nearFace)
		{
			//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3f(1,0,0)).DrawSphere(p1, 0.05);
			//�����܂ŗ�����p1[p1Idx]�͏Փ˂��Ă�
			//�߂荞�ݗʂ͂߂荞��ł�΃v���X
			info.AddInfo(p1, nearFace->normal, -maxDot, p1Id, p2Id);
			ret = true;
		}
	}

	return ret;
}

//�ʒup1����Ƃ���pConvex��normal�����̈�Ԑ[���[�x�����߂�
double DsPhysics::DsCD_NormalFaceDepthMax(const DsVec3d& p1, const DsVec3d& normal, const DsCollisionGeometry* pConvex)
{
	double ret = 0.0;

	const DsVec3d* p2 = pConvex->GetVertex();
	const int p2VertexNum = pConvex->GetVertexNum();

	if (p2)
	{
		for (int p2Idx = 0; p2VertexNum > p2Idx; ++p2Idx)
		{
			const double dot = DsVec3d::Dot( p2[p2Idx] - p1, normal);
			ret = min(dot, ret);
		}
	}
	return fabs(ret);
}

bool _DsCD_LineFace(const DsActorId& p1Id, const DsVec3d& lineV0, const DsVec3d& lineV1, const DsActorId& p2Id, const DsVec3d* p2, const DsQuad& face, const DsVec3d& toDist, DsCollisionResult& info)
{
	//�Փ˕����͑��葤�̖ʂ̖@��

	//-----------------�ϐ��錾----------------------
	const int faceVnMax = 16;
	const int faceVn = face.vn;
	DS_ASSERT(faceVn <= faceVnMax, "�z��O�̒��_��");

	const DsVec3d faceNormal = face.normal;
	DsVec3d faceV[faceVnMax];

	DsVec3d edges[faceVnMax];
	int edgeIdx;

	double d1;
	double d2;

	double fd1;
	double fd2;

	double depth;

	double internalDivision;

	DsVec3d lineVec;
	DsVec3d colVec;
	DsVec3d colPos;
	
	int endIdx;

	int faceIdx;

	DsVec3d edge;

	DsVec3d cross;

	double retDepth;
	DsVec3d retNormal;
	DsVec3d lineCol;
	DsVec3d lineFlat;
	double intersectionRate;
	DsVec3d intersection;
	//----------------------------------------------

	for (int i = 0; i < faceVn; ++i) {
		faceV[i] = p2[face.index[i]];
	}


	//���ʂ܂ł̍��������߂�
	d1 = DsVec3d::Dot(lineV0 - faceV[0], faceNormal);
	d2 = DsVec3d::Dot(lineV1 - faceV[0], faceNormal);
	//const double d1 = DsVec3d::Dot( p1[line.index[0]] - faceV[0], faceNormal );
	//const double d2 = DsVec3d::Dot( p1[line.index[1]] - faceV[0], faceNormal );

	//�Փ˂��ĂȂ�
	if( (0.0 >= d1) && (0.0 >= d2) )
	{
		return false;
	}
	if( (0.0 <= d1 ) && (0.0 <= d2) )
	{
		return false;
	}

	fd1 = fabs(d1);
	fd2 = fabs(d2);

	if( 0.000001 > (fd1+fd2) )
	{
		//���Ɩʂ͂قڐ���
		return false;
	}

	//�߂荞��ł���̐[�x�����߂Ă����B�}�C�i�X�̕����߂荞��ł��
	//������߂荞�ݗʂɂ����Ⴄ�ƒ����ӂ͂������߂荞�ނ��ƂɂȂ�
	depth = (0.0 > d1) ? d1 : d2;


	//�Փ˓_�܂ł̔䗦�����߂�
	internalDivision =	fd1/(fd1+fd2);
	//�Փ˓_�܂ł̃x�N�g�������߂�
	lineVec = lineV1 - lineV0;
	colVec = lineVec * internalDivision;
	//�Փ˓_�̈ʒu�����߂�
	colPos = lineV0 + colVec;

	//�Փ˓_���S�p�`�̒��ɓ����Ă��邩���ׂ�
	endIdx = faceVn - 1;
	edgeIdx = 0;
	for (faceIdx = 0; faceIdx < faceVn; ++faceIdx)
	{
		//�l�p�`���\�������
		edge = (endIdx != faceIdx) ? (faceV[faceIdx + 1] - faceV[faceIdx]) : (faceV[0] - faceV[faceIdx]);
		edges[edgeIdx]=edge;//��Ŏg���̂łƂ��Ă���
		++edgeIdx;

		//�O�ς����A�P�ł��ʂƔ��Ε����Ȃ�ʂɏՓ˓_�͊܂܂�Ȃ�
		cross = DsVec3d::Cross( edge, colPos - faceV[faceIdx] );
		if( DsVec3d::Dot(cross, faceNormal) <= -0.00001 )
		{
			return false;
		}
	}

	retDepth = depth;
	retNormal = faceNormal;
	//�ʃG�b�W�Ɗђʐ��̌�_�����߁A���̒����̐��̍������߂荞�ݗʂƂ���B�����͖ʂ̕���
	lineCol = (0.0 > d1) ? (colPos - lineV0) : (colPos - lineV1);//�߂荞��ł钸�_����Փ˓_�܂ł̃x�N�g��
	lineFlat = lineCol - (faceNormal*DsVec3d::Dot(lineCol, faceNormal));//�ʂ̕��ʂɓ��e
	for (faceIdx = 0; faceIdx < faceVn; ++faceIdx)
	{
		if (DsVec3d::GetIntersection(colPos, lineFlat, faceV[faceIdx], edges[faceIdx], intersection))
		{
			//��_��lineCol�̂ǂ̊����̉ӏ��ɂ��邩���߂�
			intersectionRate = ( (colPos - intersection).Length() / lineFlat.Length() );
			//�Փː��̂߂荞��ł���̍����Ɋ������|����Ό����_�̒����̓_�܂ł̍����ɂȂ�
			retDepth *= intersectionRate;
			break;
		}
	}
	retDepth = -retDepth;

	//�ʃG�b�W�Ɗђʓ_�̍ł��߂��������߂荞�ݗʁB�����͖ʂ̒��S����G�b�W�Ɍ���������
	//retDepth = 0.0;
	//for (int faceIdx = 0; faceIdx < faceVn; ++faceIdx){
	//	const DsVec3d tmp = p2[face.index[faceIdx]] - colPos;
	//	const DsVec3d edge = (faceIdx != (faceVn - 1)) ? (faceV[faceIdx+1] - faceV[faceIdx]) : (faceV[0] - faceV[faceIdx]);
	//	DsVec3d normal = DsVec3d::Normalize( DsVec3d::Cross(edge, faceNormal) );
	//	const double dot = DsVec3d::Dot(tmp, normal);
	//	if (dot < 0.0){
	//		normal *= -1.0;//�Փ˓_�Ɍ����������Ȃ̂ŋt
	//	}
	//	const double dotAbs = fabs(dot);
	//	if (retDepth < dotAbs){
	//		retDepth = dotAbs;
	//		retNormal = normal;
	//	}
	//	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3f(1, 0, 0)).DrawLine(faceV[faceIdx], faceV[faceIdx]+retNormal);
	//}

	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3f(1, 0, 0)).DrawSphere(colPos, 0.05);
	//�Փ˓_�m��
	//�߂荞�ݗʂ͂߂荞��ł�΃v���X
	info.AddInfo(colPos, retNormal, retDepth, p1Id, p2Id);
	return true;
}

//�ʂƐ������s�ɐڐG�����Ƃ�
inline bool _DsCD_LineFaceParallel(const DsActorId& p1Id, const DsVec3d& lineV0, const DsVec3d& lineV1, const DsActorId& p2Id, const DsVec3d* p2, const DsQuad& face, const DsVec3d& toDist, DsCollisionResult& info)
{
	const DsVec3d facePlane = DsVec3d::Normalize( p2[face.index[0]] - p2[face.index[1]] );
	const DsVec3d linePlane = DsVec3d::Normalize(lineV1 - lineV0);

}

bool DsPhysics::DsCD_LineFace( const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2, DsCollisionResult& info )
{
	bool ret = false;

	const DsActorId& p1Id = pBox1->RefOwnerId();
	const DsVec3d* p1 = pBox1->GetVertex();
	const DsLine* p1Line = pBox1->GetLine();
	const int p1LineNum = pBox1->GetLineNum();
	const DsActorId& p2Id = pBox2->RefOwnerId();
	const DsVec3d* p2 = pBox2->GetVertex();
	const DsQuad* p2Face = pBox2->GetFace();
	const int p2FaceNum = pBox2->GetFaceNum();
	const DsVec3d toDist = pBox2->GetBasePos() - pBox1->GetBasePos();

	const DsAabb aabb1 = *pBox1->GetAabb();
	DsAabb aabb2 = *pBox2->GetAabb();

	if (DsAabb::IsContain(aabb1, aabb2)) {
		if (p1 && p1Line && p2 && p2Face)
		{
			for (int lineIdx = 0; p1LineNum > lineIdx; ++lineIdx)
			{
				for (int faceIdx = 0; p2FaceNum > faceIdx; ++faceIdx)
				{
					const DsLine& line = p1Line[lineIdx];
					ret |= _DsCD_LineFace(p1Id, p1[line.index[0]], p1[line.index[1]], p2Id, p2, p2Face[faceIdx], toDist, info);
				}
			}
		}
	}
	return ret;
}


bool DsPhysics::DsCD_LineFaceVelocity(const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2, DsCollisionResult& info)
{
	bool ret = false;

	const DsActorId& p1Id = pBox1->RefOwnerId();
	const DsVec3d* p1 = pBox1->GetVertex();
	const DsVec3d* preP1 = pBox1->GetPreVertex();//�P�t���O�̒��_
	const int p1Vn = pBox1->GetVertexNum();
	const DsActorId& p2Id = pBox2->RefOwnerId();
	const DsVec3d* p2 = pBox2->GetVertex();
	const DsQuad* p2Face = pBox2->GetFace();
	const int p2FaceNum = pBox2->GetFaceNum();
	const DsVec3d toDist = pBox2->GetBasePos() - pBox1->GetBasePos();

	if (p1 && preP1 && p2 && p2Face)
	{
		for (int p1vIdx = 0; p1Vn > p1vIdx; ++p1vIdx)
		{
			for (int faceIdx = 0; p2FaceNum > faceIdx; ++faceIdx)
			{
				const DsVec3d& velLineV0 = p1[p1vIdx];
				const DsVec3d& vrlLineV1 = preP1[p1vIdx];
				ret |= _DsCD_LineFace(p1Id, velLineV0, vrlLineV1, p2Id, p2, p2Face[faceIdx], toDist, info);
			}
		}
	}
	return ret;
}

inline bool _DsCD_TriangleFace(const DsActorId& meshActorId, const DsQuad& tri, const DsVec3d* triV, const DsActorId& faceActorId, const DsQuad& face, const DsVec3d* faceV, DsCollisionResult& info)
{
	bool ret = false;

	const DsVec3d& faceP = faceV[face.index[0]];
	const double dot1 = DsVec3d::Dot( triV[tri.index[0]] - faceP, face.normal);
	const double dot2 = DsVec3d::Dot( triV[tri.index[1]] - faceP, face.normal);
	const double dot3 = DsVec3d::Dot( triV[tri.index[2]] - faceP, face.normal);
	const double fdot1 = fabs(dot1);
	const double fdot2 = fabs(dot2);
	const double fdot3 = fabs(dot3);
	struct _Edge
	{
		DsVec3d start;//�n�_
		DsVec3d edge;//��
	};
	std::vector<_Edge> edges(4);//face���\������ӂ̃x�N�g���B�����v���
	edges.clear();
	{
		const int endIdx = face.vn - 1;
		for (int fi = 0; fi < face.vn; ++fi)
		{
			const DsVec3d edge = (fi != endIdx) ? (faceV[face.index[fi + 1]] - faceV[face.index[fi]]) : (faceV[face.index[0]] - faceV[face.index[fi]]);
			_Edge tmp = { faceV[face.index[fi]], edge };
			edges.push_back(tmp);
		}
	}

	//�S�Ă̒��_���ʂ̈�����ɂ���Γ������ĂȂ�
	if ((0.0 < dot1) && (0.0 < dot2) && (0.0 < dot3))
	{
		return false;
	}
	if ((0.0 > dot1) && (0.0 > dot2) && (0.0 > dot3))
	{
		return false;
	}

	int crossCount = 0;//�����ʂ��ђʂ�����

	//�ʂ̒��ɓ_���܂܂�Ă��邩�𔻒�
	auto IsInFace = [](const DsVec3d& p, const DsQuad& face, const DsVec3d* faceV, const std::vector<_Edge>& edges)-> bool
	{
		bool ret = false;
		bool firstDepth = true;
		const int endIdx = face.vn - 1;
		
		for (int fi = 0; fi < face.vn; ++fi)
		{
			//�Փ˓_�ƕӂ̊O�ς����A�P�ł��ʂ̖@���Ɠ��������Ȃ�ʂɏՓ˓_�͊܂܂�Ȃ�
			const DsVec3d cross = DsVec3d::Cross(edges[fi].edge, p - faceV[face.index[fi]]);
			if ( 0.0 < DsVec3d::Dot(cross, face.normal) )//�@���Ɠ����Ȃ�ʂ̒�
			{
				ret = true;
			}
			else
			{
				ret = false;
				break;
			}
		}
		return ret;
	};


	struct _CollisionPoint
	{
		DsVec3d hitPos;
		double depth;
		DsVec3d intersctionEnd;//face���ђʂ��Ă���ӂ̗����ɂ�����̒��_�B
	};
	std::vector<_CollisionPoint> colPoints;

	//���_1,2,3�̓��A����̖ʂ��ׂ��ł���g�ݍ��킹���݂���B
	if((dot1*dot2) < 0.0)
	{
		const double innnerRate = (fdot1 / (fdot1+fdot2)); //���ʂɌׂ��ł���͂��Ȃ̂ŕ����̓}�C�i�X�ɂȂ�͂��B-1�����đł�����
		const DsVec3d p = ((triV[tri.index[1]] - triV[tri.index[0]])*innnerRate) + triV[tri.index[0]];
		const bool result = IsInFace(p, face, faceV, edges);
		if (result)//�O�p�n�̓����ɂ�����
		{
			_CollisionPoint cp;
			cp.hitPos = p;
			cp.depth = (dot1 < 0.0) ? (-dot1) :(-dot2);
			cp.intersctionEnd = (dot1 < 0.0) ? (triV[tri.index[0]]) : (triV[tri.index[1]]);
			colPoints.push_back(cp);
		}
		++crossCount;
	}
	
	if((dot1*dot3) < 0.0)
	{
		const double innnerRate = (fdot1 / (fdot1+fdot3));
		const DsVec3d p = ((triV[tri.index[2]] - triV[tri.index[0]])*innnerRate) + triV[tri.index[0]];
		const bool result = IsInFace(p, face, faceV, edges);
		if (result)//�O�p�n�̓����ɂ�����
		{
			_CollisionPoint cp;
			cp.hitPos = p;
			cp.depth = (dot1 < 0.0) ? (-dot1) : (-dot3);
			cp.intersctionEnd = (dot1 < 0.0) ? (triV[tri.index[0]]) : (triV[tri.index[2]]);
			colPoints.push_back(cp);
		}
		++crossCount;
	}
	
	//�O�p�`�̓��A�ђʂ����ӂ��Q�{�����̏ꍇ�͊ђʂ��Ă���ӂ̉\��������̂ł��B
	if (crossCount < 2)
	{
		if ((dot2*dot3) < 0.0)
		{
			const double innnerRate = (fdot2 / (fdot2+fdot3));
			const DsVec3d p = ((triV[tri.index[2]] - triV[tri.index[1]])*innnerRate) + triV[tri.index[1]];
			const bool result = IsInFace(p, face, faceV, edges);
			if (result)//�O�p�n�̓����ɂ�����
			{
				_CollisionPoint cp;
				cp.hitPos = p;
				cp.depth = (dot2 < 0.0) ? (-dot2) : (-dot3);
				cp.intersctionEnd = (dot2 < 0.0) ? (triV[tri.index[1]]) : (triV[tri.index[2]]);
				colPoints.push_back(cp);
			}
		}
	}
	

	ret = (0 < colPoints.size());

	for each(const _CollisionPoint& cp in colPoints)
	{
		double depth = cp.depth;//�������ĂȂ��Ƃ��̃f�t�H�͏I�[

		//�߂荞�ݐ[�x�����߂�Bface�̖@�����_�Ō��āAtri�̊ђʕӂ�face�̍\���ӂ̌�������_�̂Ƃ���ɂ���tri�̊ђʕӂ̍���
		DsVec3d colVec = cp.intersctionEnd - cp.hitPos;
		colVec = colVec - (face.normal*DsVec3d::Dot(colVec, face.normal));//face���ђʂ���ӂ�face��̃x�N�g��
		DsVec3d intersection;;
		for each(const _Edge& edge in edges)
		{
			if (DsVec3d::GetIntersection(cp.hitPos, colVec, edge.start, edge.edge, intersection))
			{
				//���������̂ŁA�������߂荞�ݐ[�x�����߂�_
				//��_��lineCol�̂ǂ̊����̉ӏ��ɂ��邩���߂�
				const double intersectionRate = ((cp.hitPos - intersection).Length() / colVec.Length());
				depth *= intersectionRate;
				break;
			}
		}
		
		//�߂荞�ݗʂ͂߂荞��ł�΃v���X
		info.AddInfo(cp.hitPos, face.normal, depth, meshActorId, faceActorId);
	}

	//return ret;
	return true;//�O�p�`�̒��ɖʂ����S�ɓ����Ă���Ƃ������m�������̂ŁA�O�p�`�Ɩʂ����ւ��邽��true��Ԃ�
}

bool DsPhysics::DsCD_TriangleFace(const DsCollisionGeometry* pMesh, const DsCollisionGeometry* pBox, DsCollisionResult& info)
{
	bool ret = false;

	const DsActorId& mesh = pMesh->RefOwnerId();
	const DsActorId& box = pBox->RefOwnerId();
	const DsVec3d* meshV = pMesh->GetVertex();
	const DsVec3d* faceV = pBox->GetVertex();
	const int triN = pMesh->GetFaceNum();
	const int faceN = pBox->GetFaceNum();

	if (meshV && faceV)
	{
		for (int ti = 0; ti < triN; ++ti)
		{
			for (int fi = 0; fi < faceN; ++fi)
			{
				const bool isHit = _DsCD_TriangleFace(mesh, pMesh->GetFace()[ti], meshV, box, pBox->GetFace()[fi], faceV, info);
				ret |= isHit;


				//�������Ă�̂ł���΁A��������̖ʂ��������Ă�\��������̂œ���ւ���
				if (isHit)
				{
					DsCollisionResult tmp;
					if (pBox->GetFace()[fi].vn == 3)
					{
						//_DsCD_TriangleFace(mesh, pMes��h->GetFace()[ti], meshV, box, pBox->GetFace()[fi], faceV, info);���������]���Ă�̂����₷���悤�ɔ�r
						// _DsCD_TriangleFace(box, pBox->GetFace()[fi], faceV, mesh, pMesh->GetFace()[ti], meshV, info);
						   _DsCD_TriangleFace(mesh, pBox->GetFace()[fi], faceV, box, pMesh->GetFace()[ti], meshV, tmp);
						  //master���̃I�[�i�[��mesh�ɓ���B�S�������Ƃ��ɂPactor�̏Փˌv�Z�ŃI�[�i�[������ւ��ƍ��͌v�Z�̂Ƃ���master���̍��W��master�ł͂Ȃ��Ȃ邽��
					}
					else //box�͂S�p�`�ō\������Ă�̂ŁA�O�p�`�ɕ���
					{
						DsQuad splitF1; 
						splitF1.vn = 3;
						splitF1.index[0] = pBox->GetFace()[fi].index[0];
						splitF1.index[1] = pBox->GetFace()[fi].index[1];
						splitF1.index[2] = pBox->GetFace()[fi].index[2];
						splitF1.normal = pBox->GetFace()[fi].normal;
						//_DsCD_TriangleFace(box, splitF1, faceV, mesh, pMesh->GetFace()[ti], meshV, info);
						  _DsCD_TriangleFace(mesh, splitF1, faceV, box, pMesh->GetFace()[ti], meshV, tmp);//���Ɠ������R�ŃI�[�i�[����ւ�
				
						DsQuad splitF2;
						splitF2.vn = 3;
						splitF2.index[0] = pBox->GetFace()[fi].index[2];
						splitF2.index[1] = pBox->GetFace()[fi].index[3];
						splitF2.index[2] = pBox->GetFace()[fi].index[0];
						splitF2.normal = pBox->GetFace()[fi].normal;
						//_DsCD_TriangleFace(box, splitF2, faceV, mesh, pMesh->GetFace()[ti], meshV, info);
						  _DsCD_TriangleFace(mesh, splitF2, faceV, box, pMesh->GetFace()[ti], meshV, tmp);
					}

					//�S�����������Amesh�����}�X�^�[�Ƃ��ĉ����悤�ɂ��Ă���̂ŁAmesh��mesh�̖@�������ɂ߂荞�݉������Ă��܂��B
					//�@���𔽓]
					const int cn = tmp.GetColNum();
					for (int ci = 0; ci < cn; ++ci)
					{
						tmp.m_colNormal[ci] *= -1.0;
					}
					info.AddInfo(tmp);
				
					//���̎��_��ret��true�m��Ȃ̂ŁA�߂�l�ɔ��f������K�v�͂Ȃ�
				}
			}
		}
	}
	return ret;
}
