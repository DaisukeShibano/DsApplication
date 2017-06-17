#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_BOX_BOX__
#include "Collision/EachGeom/DsCollisionBoxBox.h"
#endif

#include "Actor/DsRigidBox.h"
#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
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
	enum class _N_AXIS : char
	{
		NONE,
		AX,
		AY,
		AZ,
		B_AXIS,
		BX,
		BY,
		BZ,

		CROSS,
		AX_BX,
		AX_BY,
		AX_BZ,
		AY_BX,
		AY_BY,
		AY_BZ,
		AZ_BX,
		AZ_BY,
		AZ_BZ,
	};


	inline double _Dot(const DsVec3d& a, const DsVec3d& b)
	{
		return DsVec3d::Dot(a, b);
	}

	//衝突条件から外れたらtrue
	inline bool _MaxDepth3Axis(_N_AXIS type, double expr1, double expr2, double& depth, double& sign, _N_AXIS& axis){
		double s2 = fabs(expr1) - expr2;
		if (depth < s2){
			depth = s2;
			axis = type;
			sign = (0.0 < expr1) ? (1.0) : (-1.0);
		}
		return (0.0 < s2);//trueで当たってないの確定なので判定を終える
	}

	//衝突条件から外れたらtreu
	inline bool _MaxDeoth9Axis(_N_AXIS type, double expr1, double expr2, const DsVec3d& cross, double& depth, DsVec3d& normal, _N_AXIS& axis){
		double s2 = fabs(expr1) - expr2;
		double len = cross.Length();
		if (0.0 < len){
			s2 /= len;
			if (depth < (s2*1.05)){//ほぼ平行に衝突したときは、３軸の方に引っかかって欲しいので、若干判定厳しくする
				depth = s2;
				double sign = (0.0 < expr1) ? (1.0) : (-1.0);
				normal = cross / (len*sign);
				axis = type;
			}
		}
		return (0.0 < s2);//trueで当たってないの確定なので判定を終える
	}

	inline DsVec3d _GetNormal(_N_AXIS type, const DsVec3d Ra[3], const DsVec3d Rb[3])
	{
		switch (type){
		case _N_AXIS::AX: return Ra[0];
		case _N_AXIS::AY: return Ra[1];
		case _N_AXIS::AZ: return Ra[2];
		case _N_AXIS::BX: return Rb[0];
		case _N_AXIS::BY: return Rb[1];
		case _N_AXIS::BZ: return Rb[2];
		default: DS_ASSERT(false, "実装ミス"); return DsVec3d(0,0,0);
		}
	}

	inline void _GetAxis(_N_AXIS type, const DsVec3d Ra[3], const DsVec3d Rb[3], DsVec3d& dstA, DsVec3d& dstB)
	{
		switch (type){
		case _N_AXIS::AX_BX: dstA = Ra[0]; dstB = Rb[0]; return;
		case _N_AXIS::AX_BY: dstA = Ra[0]; dstB = Rb[1]; return;
		case _N_AXIS::AX_BZ: dstA = Ra[0]; dstB = Rb[2]; return;
		case _N_AXIS::AY_BX: dstA = Ra[1]; dstB = Rb[0]; return;
		case _N_AXIS::AY_BY: dstA = Ra[1]; dstB = Rb[1]; return;
		case _N_AXIS::AY_BZ: dstA = Ra[1]; dstB = Rb[2]; return;
		case _N_AXIS::AZ_BX: dstA = Ra[2]; dstB = Rb[0]; return;
		case _N_AXIS::AZ_BY: dstA = Ra[2]; dstB = Rb[1]; return;
		case _N_AXIS::AZ_BZ: dstA = Ra[2]; dstB = Rb[2]; return;
		default: DS_ASSERT(false, "実装ミス");  return;
		}
	}

	/*
		ODE版重なってる領域判定
		h 面１の四角形の寸法（移動も回転もしてないこと前提）
		p 面２の四角形の各頂点
		ret 交差座標 最大で8点なので、16要素
		@return 交差点数
		@par	やってることがいまいち不明
	*/
	int _intersectRectQuad(const double h[2], const double p[8], double ret[16])
	{
		// q (and r) contain nq (and nr) coordinate points for the current (and
		// chopped) polygons
		int nq = 4, nr;
		double buffer[16];
		const double *q = p;
		double *r = ret;
		for (int dir = 0; dir <= 1; dir++) {
			// direction notation: xy[0] = x axis, xy[1] = y axis
			for (int sign = -1; sign <= 1; sign += 2) {
				// chop q along the line xy[dir] = sign*h[dir]
				const double *pq = q;
				double *pr = r;
				nr = 0;
				for (int i = nq; i > 0; i--) {
					// go through all points in q and all lines between adjacent points
					if (sign*pq[dir] < h[dir]) {
						// this point is inside the chopping line
						pr[0] = pq[0];
						pr[1] = pq[1];
						pr += 2;
						nr++;
						if (nr & 8) {
							q = r;
							goto done;
						}
					}
					const double *nextq = (i > 1) ? pq + 2 : q;
					if ((sign*pq[dir] < h[dir]) ^ (sign*nextq[dir] < h[dir])) {
						// this line crosses the chopping line
						pr[1 - dir] = pq[1 - dir] + (nextq[1 - dir] - pq[1 - dir]) /
							(nextq[dir] - pq[dir]) * (sign*h[dir] - pq[dir]);
						pr[dir] = sign*h[dir];
						pr += 2;
						nr++;
						if (nr & 8) {
							q = r;
							goto done;
						}
					}
					pq += 2;
				}
				q = r;
				r = (q == ret) ? buffer : ret;
				nq = nr;
			}
		}
	done:
		if (q != ret) memcpy(ret, q, nr * 2 * sizeof(double));
		return nr;
	}
}



DsCollisionResult& DsCollisionBoxBox::Collide()
{
	//return m_info;
	DS_ASSERT(m_pBox1, "衝突オーナー1がNULL");
	DS_ASSERT(m_pBox2, "衝突オーナー2がNULL");
	DS_ASSERT(m_pBox1 != m_pBox2, "同じ物体同士で当たり判定");
	
	m_info.Clear();
	const bool isContain = DsAabb::IsContain(*m_pBox1->GetAabb(), *m_pBox2->GetAabb());
	if (isContain){
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pBox1->RefOwnerId().GetActor(), *m_pBox2->RefOwnerId().GetActor())) {
				return m_info;
			}
		}
		return _ColideFinal();
	}

	return m_info;
}

DsCollisionResult& DsCollisionBoxBox::_ColideFinal()
{
	/*
	//ODE(ちなみにbulletも同じ)を参考。
	分離軸を判定し、一番浅い方向へ返す

	１．ねじれの方向の分離軸だった場合
	辺同士の最接近点を求める。平均座標が衝突点(bulletはどっちかの最接近点)
	めり込み量は分離軸の重なり量
	方向は辺の外積の方向

	２．xyzの３軸が分離軸だった場合
	面と面が重なる領域を算出し、めり込んでる領域の頂点を衝突点にする
	めり込み量は衝突点のめり込み量
	方向は分離軸
	*/
	const DsVec3d sideA = m_pBox1->GetSide();
	const DsVec3d sideB = m_pBox2->GetSide();
	const DsVec3d Ra[3] = {
		m_pBox1->GetRot().GetAxisX(),
		m_pBox1->GetRot().GetAxisY(),
		m_pBox1->GetRot().GetAxisZ(),
	};
	const DsVec3d Rb[3] = {
		m_pBox2->GetRot().GetAxisX(),
		m_pBox2->GetRot().GetAxisY(),
		m_pBox2->GetRot().GetAxisZ(),
	};
	const DsVec3d posA = m_pBox1->GetBasePos();
	const DsVec3d posB = m_pBox2->GetBasePos();
	const DsVec3d AtoB = posB - posA;
	const DsMat33d RaT = m_pBox1->GetRot().ToTransposition();
	const DsMat33d RbT = m_pBox2->GetRot().ToTransposition();
	const DsVec3d RaTAtoB = RaT*AtoB;
	const DsVec3d RbTAtoB = RbT*AtoB;


	double depth = -DBL_MAX;
	double sign = 1.0;
	_N_AXIS axType = _N_AXIS::NONE;

	//AとBの軸の内積が何回も行われないように各要素を使いまわすため、それぞれ保存しておく
	double R11, R12, R13, R21, R22, R23, R31, R32, R33;
	double Q11, Q12, Q13, Q21, Q22, Q23, Q31, Q32, Q33;
	R11 = _Dot(Ra[0], Rb[0]); R12 = _Dot(Ra[0], Rb[1]); R13 = _Dot(Ra[0], Rb[2]);
	R21 = _Dot(Ra[1], Rb[0]); R22 = _Dot(Ra[1], Rb[1]); R23 = _Dot(Ra[1], Rb[2]);
	R31 = _Dot(Ra[2], Rb[0]); R32 = _Dot(Ra[2], Rb[1]); R33 = _Dot(Ra[2], Rb[2]);
	Q11 = fabs(R11); Q12 = fabs(R12); Q13 = fabs(R13);
	Q21 = fabs(R21); Q22 = fabs(R22); Q23 = fabs(R23);
	Q31 = fabs(R31); Q32 = fabs(R32); Q33 = fabs(R33);

	//xyz３軸の分離軸
	if (   (_MaxDepth3Axis(_N_AXIS::AX, RaTAtoB.x, sideA.x + (Q11 * sideB.x) + (Q12 * sideB.y) + (Q13 * sideB.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::AY, RaTAtoB.y, sideA.y + (Q21 * sideB.x) + (Q22 * sideB.y) + (Q23 * sideB.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::AZ, RaTAtoB.z, sideA.z + (Q31 * sideB.x) + (Q32 * sideB.y) + (Q33 * sideB.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::BX, RbTAtoB.x, sideB.x + (Q11 * sideA.x) + (Q21 * sideA.y) + (Q31 * sideA.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::BY, RbTAtoB.y, sideB.y + (Q12 * sideA.x) + (Q22 * sideA.y) + (Q32 * sideA.z), depth, sign, axType))
		|| (_MaxDepth3Axis(_N_AXIS::BZ, RbTAtoB.z, sideB.z + (Q13 * sideA.x) + (Q23 * sideA.y) + (Q33 * sideA.z), depth, sign, axType)))
	{
		return m_info;//衝突無し
	}
	DsVec3d normal = _GetNormal(axType, Ra, Rb)*sign;

	//ねじれの分離軸 
	//OBBの辺同士の分離軸計算と似たような感じ。計算結果を使いまわす。
	//分離軸は、例えば、(Aのローカルx軸)×(Aから見たBのx軸) = (1,0,0)×(R11,R21,R31) = (0, -R31, R21) のように求められる。
	//分離軸方向のA-B間の距離、分離軸方向のAとBの寸法の最大長さ、分離軸 を入力。ここはAのローカル座標であるので(0,1,0)*(0, -R31, R21) = -R31 となる
	if (   _MaxDeoth9Axis(_N_AXIS::AX_BX, RaTAtoB.z * R21 - RaTAtoB.y * R31, (sideA.y * Q31 + sideA.z * Q21 + sideB.y * Q13 + sideB.z * Q12), DsVec3d(0, -R31, R21), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AX_BY, RaTAtoB.z * R22 - RaTAtoB.y * R32, (sideA.y * Q32 + sideA.z * Q22 + sideB.x * Q13 + sideB.z * Q11), DsVec3d(0, -R32, R22), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AX_BZ, RaTAtoB.z * R23 - RaTAtoB.y * R33, (sideA.y * Q33 + sideA.z * Q23 + sideB.x * Q12 + sideB.y * Q11), DsVec3d(0, -R33, R23), depth, normal, axType)

		|| _MaxDeoth9Axis(_N_AXIS::AY_BX, RaTAtoB.x * R31 - RaTAtoB.z * R11, (sideA.x * Q31 + sideA.z * Q11 + sideB.y * Q23 + sideB.z * Q22), DsVec3d(R31, 0, -R11), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AY_BY, RaTAtoB.x * R32 - RaTAtoB.z * R12, (sideA.x * Q32 + sideA.z * Q12 + sideB.x * Q23 + sideB.z * Q21), DsVec3d(R32, 0, -R12), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AY_BZ, RaTAtoB.x * R33 - RaTAtoB.z * R13, (sideA.x * Q33 + sideA.z * Q13 + sideB.x * Q22 + sideB.y * Q21), DsVec3d(R33, 0, -R13), depth, normal, axType)

		|| _MaxDeoth9Axis(_N_AXIS::AZ_BX, RaTAtoB.y * R11 - RaTAtoB.x * R21, (sideA.x * Q21 + sideA.y * Q11 + sideB.y * Q33 + sideB.z * Q32), DsVec3d(-R21, R11, 0), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AZ_BY, RaTAtoB.y * R12 - RaTAtoB.x * R22, (sideA.x * Q22 + sideA.y * Q12 + sideB.x * Q33 + sideB.z * Q31), DsVec3d(-R22, R12, 0), depth, normal, axType)
		|| _MaxDeoth9Axis(_N_AXIS::AZ_BZ, RaTAtoB.y * R13 - RaTAtoB.x * R23, (sideA.x * Q23 + sideA.y * Q13 + sideB.x * Q32 + sideB.y * Q31), DsVec3d(-R23, R13, 0), depth, normal, axType))
	{
		return m_info;//衝突無し
	}

	if (axType == _N_AXIS::NONE){
		DS_ASSERT(false, "ここには来ないはず");
		return m_info;
	}

	/*
	ODEと違うところ
	ODE:法線方向は、AからBへと向かう方向がプラスになるベクトル
	ここではAが跳ね返る方向を返さねばならないので逆向きになる
	*/
	//normal *= -1.0;//この先ODEとの整合性がとれなくなるので最後で反転させる

	//ねしれの分離軸が一番浅かった
	if (_N_AXIS::CROSS < axType){
		normal = m_pBox1->GetRot()*normal;
		DsVec3d pa = posA;//交差した辺の始点を求めたい
		for (int i = 0; i < 3; ++i){
			const double _sign = (_Dot(normal, Ra[i]) > 0.0) ? (1.0) : (-1.0);
			pa += Ra[i] * (sideA.v[i] * _sign);
		}

		DsVec3d pb = posB;//交差した辺の始点を求めたい
		for (int i = 0; i < 3; ++i){
			const double _sign = (_Dot(normal, Rb[i]) > 0.0) ? (-1.0) : (1.0);
			pb += Rb[i] * (sideB.v[i] * _sign);
		}

		DsVec3d ua;
		DsVec3d ub;
		_GetAxis(axType, Ra, Rb, ua, ub);
		double alpha;
		double beta;
		CalcNearestPoint(pa, ua, pb, ub, alpha, beta);//OBBで当たっていることは確定なのではみ出ることはないはず
		//alphaとbetaは１を超えても問題ない。辺の長さを超えるのはまずいけど、それはありえないはず
		pa = pa + ua*alpha;
		pb = pb + ub*beta;

		const DsVec3d colPos = (pa + pb)*0.5;
		m_info.AddInfo(colPos, -normal, -depth, m_pBox1->RefOwnerId(), m_pBox2->RefOwnerId());
		//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 0)).DrawSphere(colPos, 0.1);
		//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 1)).DrawLine(colPos, colPos +(normal*10.0));
		return m_info;
	}


	//ここから３軸が分離軸だった場合の計算
	//やり方は全部ODE

	//Aの軸かBの軸かで基準を入れ替える
	const DsVec3d* Raa;
	const DsVec3d* Rbb;
	const DsVec3d* paa;
	const DsVec3d* pbb;
	const DsVec3d* Sa;
	const DsVec3d* Sb;
	if (axType < _N_AXIS::B_AXIS){
		Raa = Ra;
		Rbb = Rb;
		paa = &posA;
		pbb = &posB;
		Sa = &sideA;
		Sb = &sideB;
	}
	else{
		Raa = Rb;
		Rbb = Ra;
		paa = &posB;
		pbb = &posA;
		Sa = &sideB;
		Sb = &sideA;
	}
	const DsVec3d normal2 = (axType < _N_AXIS::B_AXIS) ? (normal) : (-normal);
	
	//衝突面が重なる領域を今から求める。

	//Raaの中心から、Rbbの衝突面の中心へのベクトルを求めたい
	const DsVec3d nr = DsVec3d(_Dot(Rbb[0], normal2), _Dot(Rbb[1], normal2), _Dot(Rbb[2], normal2));
	const DsVec3d anr = DsVec3d(fabs(nr.x), fabs(nr.y), fabs(nr.z));

	//Rbbの衝突面方向と垂直方向を求める
	int lanr;//Rbb衝突面の方向
	int a1;//Rbb衝突面に垂直な方向1
	int a2;//Rbb衝突面に垂直な方向2
	if (anr.v[1] > anr.v[0]) {
		if (anr.v[1] > anr.v[2]) {
			a1 = 0;
			lanr = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}
	else {
		if (anr.v[0] > anr.v[2]) {
			lanr = 0;
			a1 = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}

	//Raaの中心からRbbの衝突面までのベクトル
	DsVec3d center;
	if (nr.v[lanr] < 0.0) {
		center = (*pbb) - (*paa) + (Rbb[lanr] * Sb->v[lanr]);
	}
	else {
		center = (*pbb) - (*paa) - (Rbb[lanr] * Sb->v[lanr]);
	}
	
	//const_cast<DsActor*>(pActorA)->SetLineColor(DsVec4d(1, 0, 0, 1));
	//const_cast<DsActor*>(pActorB)->SetLineColor(DsVec4d(0, 0, 1, 1));
	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 1)).DrawSphere(center+(*paa), 0.1);
	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 1, 1)).DrawLine(center + (*paa), center + (*paa)+(normal2*10.0));

	//今度はRaa側の衝突面方向と垂直方向を求める
	int codeN;
	int code1;
	int code2;
	if (axType == _N_AXIS::AX) codeN = 0;
	else if (axType == _N_AXIS::AY) codeN = 1;
	else if (axType == _N_AXIS::AZ) codeN = 2;
	else if (axType == _N_AXIS::BX) codeN = 0;
	else if (axType == _N_AXIS::BY) codeN = 1;
	else if (axType == _N_AXIS::BZ) codeN = 2;
	else DS_ASSERT(false, "実装ミス");
	if (codeN == 0) {
		code1 = 1;
		code2 = 2;
	}
	else if (codeN == 1) {
		code1 = 0;
		code2 = 2;
	}
	else {
		code1 = 0;
		code2 = 1;
	}

	//RaaとRbbの重なっている領域を求める
	//Raaの座標系で考える
	//Rbbの衝突面をRaaの衝突面座標に投影し、２次元で計算する

	//Rbbの衝突面を求める
	double quad[8]; //投影後のRbbの面の頂点座標
	double c1, c2;	//投影後のRbbの中心座標
	double m11, m12, m21, m22;//投影後のRbbの座標系
	c1 = _Dot(center, Raa[code1]);
	c2 = _Dot(center, Raa[code2]);
	m11 = _Dot(Raa[code1], Rbb[a1]);
	m12 = _Dot(Raa[code1], Rbb[a2]);
	m21 = _Dot(Raa[code2], Rbb[a1]);
	m22 = _Dot(Raa[code2], Rbb[a2]);
	{
		//Raからみた、衝突以外の２軸のRbの図形の長さ
		double k1 = m11*Sb->v[a1];
		double k2 = m21*Sb->v[a1];
		double k3 = m12*Sb->v[a2];
		double k4 = m22*Sb->v[a2];

		//Rbbの面の各頂点を求める
		//中心点 + Rbbの各軸の長さ（Rbbは回転しているのでRaaの軸のRbbの縦横成分それぞれ足す）
		quad[0] = c1 - k1 - k3;
		quad[1] = c2 - k2 - k4;
		quad[2] = c1 - k1 + k3;
		quad[3] = c2 - k2 + k4;
		quad[4] = c1 + k1 + k3;
		quad[5] = c2 + k2 + k4;
		quad[6] = c1 + k1 - k3;
		quad[7] = c2 + k2 - k4;
	}

	//Raaの衝突面を求める。Raaが基準座標になっているので、寸法そのまま。
	const double rect[2] = {
		Sa->v[code1],
		Sa->v[code2],
	};

	double ret[16];
	const int n = _intersectRectQuad(rect, quad, ret);//重なる領域
	if (n < 1){
		//DS_ASSERT(false, "box box の当たり判定で面と面の交差数が０");
		return m_info;		// OBBで判定済みなので、交差してないことなんてありえないはず。
	}


	DsVec3d point[8];//３次元に戻した交差点。採用されたものだけ格納される
	double dep[8];//貫通深度
	double det1 = 1.0/(m11*m22 - m12*m21);
	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;

	int cnum = 0;
	for (int j = 0; j < n; j++) {
		//ret を Rbbのローカル座標に変換
		//mxx はRaa座標系からみたRbb座標系。mxxの逆行列はRbbの方を基準(=単位行列)に持ってくる変換
		double k1 = m22*(ret[j * 2] - c1) - m12*(ret[j * 2 + 1] - c2);
		double k2 = -m21*(ret[j * 2] - c1) + m11*(ret[j * 2 + 1] - c2);

		//中心点+Rbbを掛けることによりグローバル座標へ(centerはRaaの中心からRbbの面の中心までのベクトルなのでまだ完全にグローバルではない)
		//Raaの中心から交差点(Rbb衝突面上の)までのベクトル
		point[cnum] = center + (Rbb[a1]*k1) + (Rbb[a2]*k2);
		
		//Saの面までの高さ -交差点の法線方向の高さ。埋まってるならプラス。マイナスは交差点の方が高い=衝突点ではない
		dep[cnum] = Sa->v[codeN] - _Dot(normal2, point[cnum]);
		if (dep[cnum] >= 0.0) {
			cnum++;
		}
	}
	if (cnum < 1) {
		return m_info;//衝突している交差点が１つもなかったので衝突無し
	}

	for (int j = 0; j < cnum; j++) {
		const DsVec3d colPos = point[j] + (*paa);
		m_info.AddInfo(colPos, -normal, dep[j], m_pBox1->RefOwnerId(), m_pBox2->RefOwnerId());
		//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0, 0, 1)).DrawSphere(colPos, 0.1);
	}

	return m_info;
}