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

	//!<d方向のミンコフスキー差の頂点を取得
	static bool _NextPoint(DsVec3d& dstPos, const DsVec3d* v1, int v1Num, const DsVec3d* v2, int v2Num, const DsVec3d& d, std::vector<GJK_POINT_PAIR>& useIdx)
	{
		std::vector<GJK_POINT> list1;
		std::vector<GJK_POINT> list2;

		list1.reserve(v1Num);
		list2.reserve(v2Num);

		{//サポート写像
		 //頂点データを前提としているが球やシリンダーなど形状が頂点以外で示せる形状の場合はサポート写像の最大座標さえ分かればいい
		 //その場合使用済み頂点かどうかを頂点indexで判定できないので、三角錐(シンプレックス)に変化がなければ終了とかにすればいけるかも(単に最新の頂点=変化なしでもいいかも)
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
				double dot2 = DsVec3d::Dot(v2[idx2], -d);//Bは反転している
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


		//まだ使ったことのない組み合わせでサポート写像が最大になるような組を探す
		while (true) {
			//選べる候補がなくなったので中止
			if (list1.empty() || list2.empty()) {
				break;
			}

			//一番大きい組み合わせから調査
			const GJK_POINT& pa = list1.back();
			const GJK_POINT& pb = list2.back();

			GJK_POINT_PAIR pair;
			pair.pair1 = pa.idx;
			pair.pair2 = pb.idx;

			//使用済みか調査
			bool isUse = false;
			for (const GJK_POINT_PAIR& use : useIdx) {

				if (pair == use) {
					isUse = true;
					break;
				}
			}

			//未使用だったらこれに決定
			if (!isUse) {
				dstPos = pa.p - pb.p;
				useIdx.push_back(pair);
				return true;
			}

			//どちらの頂点を捨てた方が最大になるか調査
			if ((list1.size() == 1) && (2 <= list2.size())) {
				//list1はもう捨てられないので2を捨てる
				list2.pop_back();
			}
			else if ((2 <= list1.size()) && (list2.size() == 1)) {
				//list2はもう捨てられないので1を捨てる
				list1.pop_back();
			}
			else if ((list1.size() == 1) && (list2.size() == 1)) {
				//どちらも捨てられない
				break;
			}
			else {
				//どちらも2つ以上残っている

				double max1 = list1[list1.size() - 1].dot + list2[list2.size() - 2].dot;
				double max2 = list1[list1.size() - 2].dot + list2[list2.size() - 1].dot;

				//最大にならなかった方を捨てる
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

	//!<線分a-bとpointを最短で結ぶ方向
	static DsVec3d _ClosestDir(const DsVec3d& a, const DsVec3d& b, const DsVec3d& point)
	{
		const DsVec3d toPoint = point - a;
		const DsVec3d n = DsVec3d::Normalize(b - a);
		const DsVec3d nPoint = n * DsVec3d::Dot(n, toPoint);//a-bに平行移動成分
		return DsVec3d::Normalize(toPoint - nPoint);//a-bに垂直な成分
	};


	//!<立体が出来たので原点を含むか調査
	static bool _TestTri(const DsVec3d& a, const DsVec3d& b, const DsVec3d& c, const DsVec3d& d, const DsVec3d& point)
	{
		//面abcの法線
		DsVec3d e0 = b - a;
		DsVec3d e1 = c - a;

		DsVec3d normal = DsVec3d::Cross(e0, e1);
		if (normal.IsNearZero()) {
			return false;
		}

		double height = DsVec3d::Dot(normal, d - a);
		if (fabs(height) < 0.000001f) {
			//体積がないので平面上で判定
			const double dot = DsVec3d::Dot(normal, point - a);
			return fabs(dot) < 0.000001f;
		}
		else {
			//三角錐の外側がプラスになるように修正
			if (0.0f < height) {
				normal = -normal;
			}
			//pointが三角錐の外側にいるか判定
			const double dot = DsVec3d::Dot(normal, point - a);
			if (0.0f < dot) {
				return false;//外側だった
			}
			else {
				return true;//内側だった
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

	d = DsVec3d::GetY();//最初はどれでもいいミンコフスキー差の頂点を取る
	if (!_NextPoint(p[0], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	static const DsVec3d origin = DsVec3d::Zero();

	//原点-p[0]が次の支点検索方向とする
	d = DsVec3d::Normalize(origin - p[0]);
	if (!_NextPoint(p[1], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	//p1 - p0 を結ぶ線と原点の最接近方向を次の支点検索方向とする
	d = _ClosestDir(p[1], p[0], origin);
	if (!_NextPoint(p[2], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	//p2 - p1 を結ぶ線と原点の最接近方向を次の支点検索方向とする
	d = _ClosestDir(p[2], p[1], origin);
	if (!_NextPoint(p[3], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
		return false;
	}

	while (true) {

		//原点が三角錐に含まれるか調査
		bool isIn =	_TestTri(p[0], p[1], p[2], p[3], origin)
					&& _TestTri(p[1], p[2], p[3], p[0], origin)
					&& _TestTri(p[2], p[3], p[0], p[1], origin)
					&& _TestTri(p[3], p[0], p[1], p[2], origin);

		if (isIn) {
			return true;//原点含んでいたので終了
		}
		else {
			//新しい頂点を探してもう一度
			DsVec3d tmp[4] = { p[1], p[2], p[3],{} };//ローテーション
			d = _ClosestDir(tmp[2], tmp[1], origin);
			//新しい点を追加
			if (!_NextPoint(tmp[3], pVertA, vertNumA, pVertB, vertNumB, d, useIdx)) {
				return false;//新しい点を得られなくなったら終了
			}
			for (int i = 0; i < 4; ++i) {
				p[i] = tmp[i];//入れ替え反映
			}
		}
	}
}