#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_MESH_MESH_H_
#include "Collision/EachGeom/DsCollisionMeshMesh.h"
#endif
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

using namespace DsPhysics;


namespace
{
	//ODE�̓����蔻����Q�l
	struct LineContactSet
	{
		enum{MAX_POINTS = 8};
		DsVec3d Points[MAX_POINTS];
		int Count;
	};

	/*
	�O�p�`�̃G�b�Ws1-s0�ƎO�p�`�@��normal�̐����ʂ����߂�
	*/
	void BuildEdgePlane(const DsVec3d s0, const DsVec3d s1, const DsVec3d normal, DsVec3d& plane_normal, double& plane_dist)
	{
		const DsVec3d e0= s1 - s0;
		plane_normal = DsVec3d::Normalize(DsVec3d::Cross(e0, normal));
		plane_dist = DsVec3d::Dot(plane_normal, s0);
	}


	/*
	���_s1��s2���A��N�ƌ�������_��Ԃ�
	*/
	DsVec3d PlaneClipSegment(const DsVec3d s1, const DsVec3d s2, const DsVec3d N, double C)
	{
		const double dis1 = DsVec3d::Dot(s1, N) - C;
		DsVec3d clipped =s2 -s1;
		const double dis2 = DsVec3d::Dot(clipped, N);
		clipped = clipped * (-dis1 / dis2);//�����}�C�i�X�Ȃ́Afabs���g�킸�Ƃ��K���v���X�ɂȂ邩��
		clipped = clipped + s1;
		return clipped;
	}

	/*
	Contacts���N�Ő؂���B��̓I�ɂ͎��̇@�A���s��
	�@Contacts�����N�̓����ɂ��镨�����c��
	�A�ʂƌ������Ă���_������Βǉ�����
	*/
	void ClipConvexPolygonAgainstPlane(const DsVec3d N, double C, LineContactSet& Contacts)
	{
		int  i, vi, prevclassif = 32000, classif;
		/*
		classif 0 : back, 1 : front
		*/

		double d;
		DsVec3d clipped[8];
		int clippedcount = 0;

		if (Contacts.Count == 0)
		{
			return;
		}
		for (i = 0; i <= Contacts.Count; i++)
		{
			vi = i%Contacts.Count;

			//���_�̖ʂ���̍���d
			d = DsVec3d::Dot(N, Contacts.Points[vi]) - C;
			////classify point
			if (d > (1.0e-8))	classif = 1;	//�O�p�`�̊O���̊O���̒��_
			else  classif = 0;					//�O�p�`�̊O���̓����̒��_

			if (classif == 0)//back
			{
				if (i>0)
				{
					//prevclassif�Ŕ��肵�����̂́A����Ώے��_���A�O����������ցA��������O���ցA�s�����̂��A�܂�ђʂ������𔻒肵����
					//�����̒��_��clipped�ɂ������܂߂邪�A�O���ƌ������Ă�����W��clipped�Ɋ܂߂���
					if (prevclassif == 1)///in front
					{

						///add clipped point
						if (clippedcount<8)
						{
							//�O�p�`�̊O�����ђʂ�����W�������Ƃ��ďW�߂�
							//Contacts.Points[i - 1] ���� Contacts.Points[i] �܂ł̃x�N�g���ŁAN�@���̖ʂ̊ђʍ��W��clipped[clippedcount]�ɕۑ�
							clipped[clippedcount] =PlaneClipSegment(Contacts.Points[i - 1], Contacts.Points[vi], N, C);
							clippedcount++;
						}
					}
				}
				///add point
				if ( (clippedcount<8) && (i<Contacts.Count) )
				{
					clipped[clippedcount] = Contacts.Points[vi];
					clippedcount++;
				}
			}
			else
			{

				if (i>0)
				{
					if (prevclassif == 0)
					{
						///add point
						if (clippedcount<8)
						{
							//�O�p�`�̊O�����ђʂ�����W�������Ƃ��ďW�߂�
							clipped[clippedcount] = PlaneClipSegment(Contacts.Points[i - 1], Contacts.Points[vi], N, C);
							clippedcount++;
						}
					}
				}
			}

			prevclassif = classif;
		}

		if (clippedcount == 0)
		{
			Contacts.Count = 0;
			return;
		}
		Contacts.Count = clippedcount;
		memcpy(Contacts.Points, clipped, clippedcount * sizeof(DsVec3d));
		return;
	}

	/*
	points���Atri[3]���ӂɂ����O�p���Ő؂��������̂�clipped_points�Ɋi�[
	*/
	void ClipPointsByTri(
		const DsVec3d* points, int pointcount,
		const DsVec3d tri[3],
		const DsVec3d triplanenormal,
		const double triplanedist,
		LineContactSet& clipped_points,
		const bool triplane_clips)
	{
		///build edges planes
		DsVec3d planeN;
		double planeC;

		clipped_points.Count = pointcount;
		memcpy(&clipped_points.Points[0], &points[0], pointcount * sizeof(DsVec3d));
		for (int i = 0; i < 3; i++)
		{//�R�ӕ����[�v

		 //�O�p�`�̕ӂƎO�p�`�̖@���̐����ʁi�O�p�`�̊O���ɂł���ʁj�����
		 //�ʂ̖@���͎O�p�`�̓���
			BuildEdgePlane(tri[i], tri[(i + 1) % 3], triplanenormal, planeN, planeC);
			//clipped_points�ɂ́��ōŏ���points�S���̒��_���R�s�[���Ă�
			//plane�̖@������̒��_��clipped_points����i��B�܂�plane�ƌ������Ă�����W��clipped_points�ɒǉ�
			ClipConvexPolygonAgainstPlane(planeN, planeC, clipped_points);
		}

		//�����܂łŁAtri�̊O�����͂ނ悤�ɂł����O�p�����Ɋ܂܂��points�̒��_������clipped_points�ɏo�͂��ꂽ

		if (triplane_clips){
			ClipConvexPolygonAgainstPlane(triplanenormal, triplanedist, clipped_points);
		}
	}

	/*
	���_s0,s1,s2����ʂ����߂�B�\�ߋ��߂Ă������ق����悳����
	*/
	void BuildPlane(const DsVec3d s0, const DsVec3d s1, const DsVec3d s2, DsVec3d& Normal, double& Dist)
	{
		const DsVec3d e0 = s1 - s0;
		const DsVec3d e1 = s2 - s0;
		Normal = DsVec3d::Normalize(DsVec3d::Cross( e0, e1) );
		Dist = DsVec3d::Dot(Normal, s0);
	}

	/*
	points�̒�����A���plane_normal�@���̔��Α��Ɉ�ԉ������̂�deep_points�Ɋi�[
	*/
	double MostDeepPoints(
		const LineContactSet & points,
		const DsVec3d plane_normal,
		double plane_dist,
		LineContactSet& deep_points)
	{
		int max_candidates[8];
		double maxdeep = -DBL_MAX;
	
		deep_points.Count = 0;
		for (int i = 0; i < points.Count; i++)
		{
			double dist = DsVec3d::Dot(plane_normal, points.Points[i]) - plane_dist;
			dist *= -1.0;
			if (dist > maxdeep)
			{
				maxdeep = dist;
				deep_points.Count = 1;
				max_candidates[deep_points.Count - 1] = i;
			}
			else if (dist + (0.000001) >= maxdeep)
			{
				deep_points.Count++;
				max_candidates[deep_points.Count - 1] = i;
			}
		}

		for (int i = 0; i < deep_points.Count; i++)
		{
			deep_points.Points[i] = points.Points[max_candidates[i]];
		}
		return maxdeep;
	}


	///returns the penetration depth
	double FindTriangleTriangleCollision(
		const DsVec3d tri1[3],
		const DsVec3d tri2[3],
		DsVec3d separating_normal,
		LineContactSet& deep_points)
	{
		double maxdeep = DBL_MAX;
		double dist;
		int mostdir = 0, /*mostface=0,*/ currdir = 0;
		//	dReal vmin1,vmax1,vmin2,vmax2;
		//	dVector3 crossdir, pt1,pt2;
		DsVec3d tri1planeN;
		double tri1planeC;
		DsVec3d tri2planeN;
		double tri2planeC;
		LineContactSet clipped_points1, clipped_points2;
		LineContactSet deep_points1, deep_points2;

		// It is necessary to initialize the count because both conditional statements 
		// might be skipped leading to uninitialized count being used for memcpy in if(mostdir==0)
		deep_points1.Count = 0;

		////find interval face1
		{
			BuildPlane(tri1[0], tri1[1], tri1[2], tri1planeN, tri1planeC);
			clipped_points1.Count = 0;
			//tri2�O�p�` ���Atri1���������O�p�����̓����ɂ�����̂�����clipped_points1�֊i�[
			ClipPointsByTri(
				tri2, 3,
				tri1,
				tri1planeN,
				tri1planeC,
				clipped_points1, false);

			//clipped_points1�̒�����A��Ԗ�tri1plane�ɋ߂��_(�قړ����Ȃ炻����܂�)��deep_points1�ցB
			//maxdeep�͖@�������ɂ����΂����قǃ}�C�i�X
			maxdeep = MostDeepPoints(
				clipped_points1,
				tri1planeN,
				tri1planeC,
				deep_points1);
			separating_normal = tri1planeN;
		}
		currdir++;

		////find interval face2
		{
			BuildPlane(tri2[0], tri2[1], tri2[2], tri2planeN, tri2planeC);
			clipped_points2.Count = 0;
			//tri1�O�p�` ���Atri2���������O�p�����̓����ɂ�����̂�����clipped_points2�֊i�[
			ClipPointsByTri(
				tri1, 3,
				tri2,
				tri2planeN,
				tri2planeC,
				clipped_points2, false);

			dist = MostDeepPoints(
				clipped_points2,
				tri2planeN,
				tri2planeC,
				deep_points2);

			//maxdeep�̕�(tri2)��tri1��[���ђʂ��Ă���
			if (dist < maxdeep)
			{
				maxdeep = dist;
				mostdir = currdir;
				//mostface = 1;
				separating_normal = tri2planeN;
			}
		}
		
		currdir++;

		////check most dir for contacts
		if (mostdir == 0)
		{
			///find most deep points
			deep_points.Count = deep_points1.Count;
			memcpy(
				&deep_points.Points[0],
				&deep_points1.Points[0],
				deep_points1.Count * sizeof(DsVec3d));

			///invert normal for point to tri1
			separating_normal = -separating_normal;
		}
		else if (mostdir == 1)
		{
			deep_points.Count = deep_points2.Count;
			memcpy(
				&deep_points.Points[0],
				&deep_points2.Points[0],
				deep_points2.Count * sizeof(DsVec3d));

		}
		
		return maxdeep;
	}

	///SUPPORT UP TO 8 CONTACTS
	bool TriTriContacts(const DsVec3d tr1[3], const DsVec3d tr2[3], int TriIndex1, int TriIndex2)
	{
		DsVec3d normal;
		double depth;
		///Test Tri Vs Tri
		//	dContactGeom* pcontact;
		int ccount = 0;
		LineContactSet contactpoints;
		contactpoints.Count = 0;

		///find best direction

		depth = FindTriangleTriangleCollision(
			tr1,
			tr2,
			normal,
			contactpoints);

		if (depth < 0.0f) {
			return false;
		}

		ccount = 0;
		while (ccount < contactpoints.Count)
		{
			//PushNewContact(g1, g2, TriIndex1, TriIndex2,
			//	contactpoints.Points[ccount],
			//	normal, depth, Flags, hashcontactset,
			//	Contacts, Stride, contactcount);
			ccount++;
		}
		return true;
	}
}


DsCollisionResult& DsCollisionMeshMesh::Collide()
{
	m_info.Clear();
	return m_info;
}
