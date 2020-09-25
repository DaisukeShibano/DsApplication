#include "DsPhysicsPch.h"
#ifndef __DS_GJK_H__
#include "Collision/GJK/DsGJK.h"
#endif

#include "DsFace.h"
#include "DsLine.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionGeometry.h"


using namespace DsPhysics;

namespace
{
	struct GJK_POINT
	{
		DsVec3d p;
		double dot;
		int idx;
	};

	struct GJK_POINT_PAIR
	{
		int pair1;
		int pair2;

		bool operator == (const GJK_POINT_PAIR& r) const {
			return (r.pair1 == pair1) && (r.pair2 == pair2);
		}
	};

	//!<d�����̃~���R�t�X�L�[���̒��_���擾
	static bool _NextPoint(DsVec3d& dstPos, const DsVec3d* v1, int v1Num, const DsVec3d* v2, int v2Num, const DsVec3d& d, std::vector<GJK_POINT_PAIR>& useIdx)
	{
		std::vector<GJK_POINT> list1;
		std::vector<GJK_POINT> list2;

		list1.reserve(v1Num);
		list2.reserve(v2Num);

		{//�T�|�[�g�ʑ�
		 //���_�f�[�^��O��Ƃ��Ă��邪����V�����_�[�Ȃǌ`�󂪒��_�ȊO�Ŏ�����`��̏ꍇ�̓T�|�[�g�ʑ��̍ő���W����������΂���
		 //���̏ꍇ�g�p�ςݒ��_���ǂ����𒸓_index�Ŕ���ł��Ȃ��̂ŁA�O�p��(�V���v���b�N�X)�ɕω����Ȃ���ΏI���Ƃ��ɂ���΂����邩��(�P�ɍŐV�̒��_=�ω��Ȃ��ł���������)
			for (int idx1 = 0; idx1 < v1Num; ++idx1) {
				double max1 = -FLT_MAX;
				double dot1 = DsVec3d::Dot(v1[idx1], d);
				GJK_POINT p;
				p.p = v1[idx1];
				p.dot = dot1;
				p.idx = idx1;
				list1.push_back(p);
			}

			for (int idx2 = 0; idx2 < v2Num; ++idx2) {
				double max2 = -FLT_MAX;
				double dot2 = DsVec3d::Dot(v2[idx2], -d);//B�͔��]���Ă���
				GJK_POINT p;
				p.p = v2[idx2];
				p.dot = dot2;
				p.idx = idx2;
				list2.push_back(p);
			}
		}

		auto cmp = [](const GJK_POINT& r, const GJK_POINT& l) {
			return r.dot < l.dot;
		};
		std::sort(list1.begin(), list1.end(), cmp);
		std::sort(list2.begin(), list2.end(), cmp);


		//�܂��g�������Ƃ̂Ȃ��g�ݍ��킹�ŃT�|�[�g�ʑ����ő�ɂȂ�悤�ȑg��T��
		while (true) {
			//�I�ׂ��₪�Ȃ��Ȃ����̂Œ��~
			if (list1.empty() || list2.empty()) {
				break;
			}

			//��ԑ傫���g�ݍ��킹���璲��
			const GJK_POINT& pa = list1.back();
			const GJK_POINT& pb = list2.back();

			GJK_POINT_PAIR pair;
			pair.pair1 = pa.idx;
			pair.pair2 = pb.idx;

			//�g�p�ς݂�����
			bool isUse = false;
			for (const GJK_POINT_PAIR& use : useIdx) {

				if (pair == use) {
					isUse = true;
					break;
				}
			}

			//���g�p�������炱��Ɍ���
			if (!isUse) {
				dstPos = pa.p - pb.p;
				useIdx.push_back(pair);
				return true;
			}

			//�ǂ���̒��_���̂Ă������ő�ɂȂ邩����
			if ((list1.size() == 1) && (2 <= list2.size())) {
				//list1�͂����̂Ă��Ȃ��̂�2���̂Ă�
				list2.pop_back();
			}
			else if ((2 <= list1.size()) && (list2.size() == 1)) {
				//list2�͂����̂Ă��Ȃ��̂�1���̂Ă�
				list1.pop_back();
			}
			else if ((list1.size() == 1) && (list2.size() == 1)) {
				//�ǂ�����̂Ă��Ȃ�
				break;
			}
			else {
				//�ǂ����2�ȏ�c���Ă���

				double max1 = list1[list1.size() - 1].dot + list2[list2.size() - 2].dot;
				double max2 = list1[list1.size() - 2].dot + list2[list2.size() - 1].dot;

				//�ő�ɂȂ�Ȃ����������̂Ă�
				if (max1 < max2) {
					list1.pop_back();
				}
				else {
					list2.pop_back();
				}
			}

		}

		return false;
	};

	//!<����a-b��point���ŒZ�Ō��ԕ���
	static DsVec3d _ClosestDir(const DsVec3d& a, const DsVec3d& b, const DsVec3d& point)
	{
		const DsVec3d toPoint = point - a;
		const DsVec3d n = DsVec3d::Normalize(b - a);
		const DsVec3d nPoint = n * DsVec3d::Dot(n, toPoint);//a-b�ɕ��s�ړ�����
		return DsVec3d::Normalize(toPoint - nPoint);//a-b�ɐ����Ȑ���
	};


	//!<���̂��o�����̂Ō��_���܂ނ�����
	static bool _TestTri(const DsVec3d& a, const DsVec3d& b, const DsVec3d& c, const DsVec3d& d, const DsVec3d& point)
	{
		//��abc�̖@��
		DsVec3d e0 = b - a;
		DsVec3d e1 = c - a;

		DsVec3d normal = DsVec3d::Cross(e0, e1);
		if (normal.IsNearZero()) {
			return false;
		}

		double height = DsVec3d::Dot(normal, d - a);
		if (fabs(height) < 0.000001f) {
			//�̐ς��Ȃ��̂ŕ��ʏ�Ŕ���
			const double dot = DsVec3d::Dot(normal, point - a);
			return fabs(dot) < 0.000001f;
		}
		else {
			//�O�p���̊O�����v���X�ɂȂ�悤�ɏC��
			if (0.0f < height) {
				normal = -normal;
			}
			//point���O�p���̊O���ɂ��邩����
			const double dot = DsVec3d::Dot(normal, point - a);
			if (0.0f < dot) {
				return false;//�O��������
			}
			else {
				return true;//����������
			}
		}
	}
}

//static 
bool DsGJK::Test(const DsVec3d* pVertA, int vertNumA, const DsVec3d* pVertB, int vertNumB)
{
	DsVec3d p[4] = {};
	DsVec3d d;
	std::vector<GJK_POINT_PAIR> useIdx;

	d = DsVec3d::GetY();//�ŏ��͂ǂ�ł������~���R�t�X�L�[���̒��_�����
	if (!_NextPoint(p[0], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	static const DsVec3d origin = DsVec3d::Zero();

	//���_-p[0]�����̎x�_���������Ƃ���
	d = DsVec3d::Normalize(origin - p[0]);
	if (!_NextPoint(p[1], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	//p1 - p0 �����Ԑ��ƌ��_�̍Őڋߕ��������̎x�_���������Ƃ���
	d = _ClosestDir(p[1], p[0], origin);
	if (!_NextPoint(p[2], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	//p2 - p1 �����Ԑ��ƌ��_�̍Őڋߕ��������̎x�_���������Ƃ���
	d = _ClosestDir(p[2], p[1], origin);
	if (!_NextPoint(p[3], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	while (true) {

		//���_���O�p���Ɋ܂܂�邩����
		bool isIn =	_TestTri(p[0], p[1], p[2], p[3], origin)
					&& _TestTri(p[1], p[2], p[3], p[0], origin)
					&& _TestTri(p[2], p[3], p[0], p[1], origin)
					&& _TestTri(p[3], p[0], p[1], p[2], origin);

		if (isIn) {
			return true;//���_�܂�ł����̂ŏI��
		}
		else {
			//�V�������_��T���Ă�����x
			DsVec3d tmp[4] = { p[1], p[2], p[3],{} };//���[�e�[�V����
			d = _ClosestDir(tmp[2], tmp[1], origin);
			//�V�����_��ǉ�
			if (!_NextPoint(tmp[3], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
				return false;//�V�����_�𓾂��Ȃ��Ȃ�����I��
			}
			for (int i = 0; i < 4; ++i) {
				p[i] = tmp[i];//����ւ����f
			}
		}
	}
}