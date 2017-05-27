#include "DsPhysicsPch.h"
#ifndef __DS_GJK_H__
#include "Collision/GJK/DsGJK.h"
#endif

#include "DsFace.h"
#include "DsLine.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionContext.h"


using namespace DsPhysics;


bool DsGJK::CalcColide(const DsCollisionContext& convex1, const DsCollisionContext& convex2)
{
	//���_
	const DsVec3d origine(0, 0, 0);

	//�T�|�[�g�ʑ��̕���
	const DsVec3d supportN = DsVec3d::Normalize( convex1.GetBasePos() - origine );

	//�T�|�[�g�ʑ�1
	double s1 = DBL_MAX;
	int s1Idx = 0;
	const DsVec3d *v1 = convex1.GetVertex();//�O���[�o�����W�łȂ��Ƃ��߂Ȃ͂�
	const int vn1 = convex1.GetVertexNum();
	for (int vidx = 0; vidx < vn1; ++vidx){
		const double dot = DsVec3d::Dot(v1[vidx], supportN);
		if( fabs(dot) < fabs(s1)){
			s1 = dot;
			s1Idx = vidx;
		}
	}

	//�T�|�[�g�ʑ�2
	double s2 = DBL_MAX;
	int s2Idx = 0;
	const DsVec3d *v2 = convex2.GetVertex();//�O���[�o�����W�łȂ��Ƃ��߂Ȃ͂�
	const int vn2 = convex2.GetVertexNum();
	for (int vidx = 0; vidx < vn2; ++vidx){
		const double dot = DsVec3d::Dot(-v2[vidx], supportN);
		if (fabs(dot) < fabs(s1)){
			s2 = dot;
			s2Idx = vidx;
		}
	}
	s2 = -s2;
	
	



	return true;
}