#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_BOX_MESH_H_
#include "Collision/EachGeom/DsCollisionBoxMesh.h"
#endif
#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
#endif
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
#endif
#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeBase.h"
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
	struct _ResultSeparateAxis
	{
		_ResultSeparateAxis()
			:isColide(false)
			, axisType(-1)
			, normal()
			, depth(0)
		{}
		bool isColide;
		int axisType;
		DsVec3d normal;
		double depth;
	};


	struct _BoxGeom
	{
		_BoxGeom(const DsCollisionGeometry* pBox)
			: pContext(pBox)
			, pos(pBox->GetBasePos())
		{
			size[0] = pBox->GetSide().x;
			size[1] = pBox->GetSide().y;
			size[2] = pBox->GetSide().z;
			rot[0] = pBox->RefOwnerId().GetActor()->GetRotation().GetAxisX();
			rot[1] = pBox->RefOwnerId().GetActor()->GetRotation().GetAxisY();
			rot[2] = pBox->RefOwnerId().GetActor()->GetRotation().GetAxisZ();
		}
		const DsCollisionGeometry* pContext;
		DsVec3d pos;
		double size[3];
		DsVec3d rot[3];
	};

	struct _TriGeom
	{
		_TriGeom(const DsCollisionGeometry* pMesh, int faceIdx)
			: pContext(pMesh)
			, pos(pMesh->GetBasePos())
			, normal(pMesh->GetFace()[faceIdx].normal)
		{
			DS_ASSERT(3 == pMesh->GetFace()[faceIdx].vn, "キューブ*メッシュのあたり判定でメッシュが三角形ではない");
			v[0] = pMesh->GetVertex()[pMesh->GetFace()[faceIdx].index[0]];
			v[1] = pMesh->GetVertex()[pMesh->GetFace()[faceIdx].index[1]];
			v[2] = pMesh->GetVertex()[pMesh->GetFace()[faceIdx].index[2]];
			vE0 = v[1] - v[0];
			vE1 = v[2] - v[0];
			vE2 = vE1 - vE0;
			vN = DsVec3d::Cross(vE0, vE1);//これ予め求めときたい
		}
		const DsCollisionGeometry* pContext;
		DsVec3d pos;
		DsVec3d normal;
		DsVec3d v[3];
		DsVec3d vE0;
		DsVec3d vE1;
		DsVec3d vE2;
		DsVec3d vN;
	};


	void _IsCldTestNormal(double fp0, double fR, const DsVec3d& vNormal, const int iAxis, _ResultSeparateAxis& ret)
	{
		// calculate overlapping interval of box and triangle
		double fDepth = fR + fp0;//fp0は２物体間の距離。マイナスになるように。

		// if we do not overlap
		if (fDepth < 0.0) {
			// do nothing
			ret.isColide = false;
			return;
		}

		const double fLength = vNormal.Length();
		const double  fOneOverLength = 1.0 / fLength;
		fDepth = fDepth*fOneOverLength;
		if (fDepth < ret.depth) {
			ret.normal = -vNormal*fOneOverLength;
			ret.axisType = iAxis;
			ret.depth = fDepth;
			ret.isColide = true;
		}
		return;
	}


	void _IsCldTestFace(double fp0, double fp1, double fp2, double fR, const DsVec3d& vNormal, int iAxis, _ResultSeparateAxis& ret)
	{
		const double fMin = min(fp0, min(fp1, fp2));
		const double fMax = max(fp0, max(fp1, fp2));

		// calculate minimum and maximum depth
		const double fDepthMin = fR - fMin;
		const double fDepthMax = fMax + fR;

		// if we dont't have overlapping interval
		if ((fDepthMin < 0.0) || (fDepthMax < 0.0)) {
			// do nothing
			ret.isColide = false;
			return;
		}

		double fDepth = 0;
		bool isMinus = false;
		// if greater depth is on negative side
		if (fDepthMin > fDepthMax) {
			// use smaller depth (one from positive side)
			fDepth = fDepthMax;
			// flip normal direction
			isMinus = true;
		}
		else {
			// use smaller depth (one from negative side)
			fDepth = fDepthMin;
		}

		// if lower depth than best found so far
		if (fDepth < ret.depth) {
			// remember current axis as best axis
			ret.normal = (isMinus) ? (-vNormal) : (vNormal);
			ret.axisType = iAxis;
			ret.depth = fDepth;
			ret.isColide = true;
		}

		return;
	}

	// Test cross products of box axis and triangle edges as separating axis
	void _IsCldTestEdge(double fp0, double fp1, double fR, const DsVec3d& vNormal, int iAxis, _ResultSeparateAxis& ret)
	{
		double fMin, fMax;

		// ===== Begin Patch by Francisco Leon, 2006/10/28 =====

		// Fixed Null Normal. This prevents boxes passing
		// through trimeshes at certain contact angles

		const double lenSq = vNormal.v[0] * vNormal.v[0] + vNormal.v[1] * vNormal.v[1] + vNormal.v[2] * vNormal.v[2];
		if (lenSq <= DBL_EPSILON){ /// THIS NORMAL WOULD BE DANGEROUS
			//ここしか引っかからなかった場合、m_infoへの追加処理が走ってしまうけど、axisTypeが-1なので弾かれる
			ret.isColide = true;
			return;
		}
		// ===== Ending Patch by Francisco Leon =====

		// calculate min and max interval values
		if (fp0 < fp1) {
			fMin = fp0;
			fMax = fp1;
		}
		else {
			fMin = fp1;
			fMax = fp0;
		}

		// check if we overlapp
		double fDepthMin = fR - fMin;
		double fDepthMax = fMax + fR;

		// if we don't overlapp
		if (fDepthMin < 0 || fDepthMax < 0) {
			// do nothing
			ret.isColide = false;
			return;
		}

		double fDepth;
		bool isMinus = false;
		// if greater depth is on negative side
		if (fDepthMin > fDepthMax) {
			// use smaller depth (one from positive side)
			fDepth = fDepthMax;
			// flip normal direction
			isMinus = true;
			// if greater depth is on positive side
		}
		else {
			// use smaller depth (one from negative side)
			fDepth = fDepthMin;
		}

		// calculate normal's length
		double fLength = sqrt(lenSq);

		// if long enough
		if (fLength > 0.0f) {
			// normalize depth
			double fOneOverLength = 1.0 / fLength;
			fDepth = fDepth*fOneOverLength;

			// if lower depth than best found so far (favor face over edges)
			if ((fDepth*1.5) < ret.depth) {//同じくらいのめり込み量だったら３軸or法線の方が採用されるように、こちらは1.5倍して採用されにくく
				// remember current axis as best axis
				ret.normal = (isMinus) ? (vNormal*(-fOneOverLength)) : (vNormal*(fOneOverLength));
				ret.axisType = iAxis;
				ret.depth = fDepth;
				ret.isColide = true;
			}
		}

		return;
	}


	_ResultSeparateAxis _CalcSeparateAxis(const _BoxGeom& box, const _TriGeom& tri)
	{
		_ResultSeparateAxis ret;
		//分離軸を計算する。
		//組み合わせは、三角形法線１軸、box基本３軸、box３軸と三角形の辺の外積方向９パターンの計１３個

		const DsVec3d vD = tri.v[0] - box.pos;
		const DsVec3d& vE0 = tri.vE0;
		const DsVec3d& vE1 = tri.vE1;
		const DsVec3d& vE2 = tri.vE2;
		const DsVec3d& vN = tri.vN;
		if ((vN.v[0] * vN.v[0] + vN.v[1] * vN.v[1] + vN.v[2] * vN.v[2]) <= DBL_EPSILON){
			return _ResultSeparateAxis();//衝突無し
		}

		ret.depth = DBL_MAX;

		{//Axis 1－三角形の法線
			const double fp0 = DsVec3d::Dot(vN, vD);
			const double fR = box.size[0] * fabs(DsVec3d::Dot(vN, box.rot[0])) + box.size[1] * fabs(DsVec3d::Dot(vN, box.rot[1])) + box.size[2] * fabs(DsVec3d::Dot(vN, box.rot[2]));
			_IsCldTestNormal(fp0, fR, vN, 1, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 2 - Box X-Axis
			const double fp0 = DsVec3d::Dot(box.rot[0], vD);
			const double fp1 = fp0 + DsVec3d::Dot(box.rot[0], vE0);
			const double fp2 = fp0 + DsVec3d::Dot(box.rot[0], vE1);
			const double fR = box.size[0];
			_IsCldTestFace(fp0, fp1, fp2, fR, box.rot[0], 2, ret);
			if (!ret.isColide) {
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 3 - Box Y-Axis
			const double fp0 = DsVec3d::Dot(box.rot[1], vD);
			const double fp1 = fp0 + DsVec3d::Dot(box.rot[1], vE0);
			const double fp2 = fp0 + DsVec3d::Dot(box.rot[1], vE1);
			const double fR = box.size[1];
			_IsCldTestFace(fp0, fp1, fp2, fR, box.rot[1], 3, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 4 - Box Z-Axis
			const double fp0 = DsVec3d::Dot(box.rot[2], vD);
			const double fp1 = fp0 + DsVec3d::Dot(box.rot[2], vE0);
			const double fp2 = fp0 + DsVec3d::Dot(box.rot[2], vE1);
			const double fR = box.size[2];
			_IsCldTestFace(fp0, fp1, fp2, fR, box.rot[2], 4, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 5 - Box X-Axis cross Edge0
			const DsVec3d vL = DsVec3d::Cross(box.rot[0], vE0);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0;
			const double fp2 = fp0 + DsVec3d::Dot(box.rot[0], vN);
			const double fR = box.size[1] * fabs(DsVec3d::Dot(box.rot[2], vE0)) + box.size[2] * fabs(DsVec3d::Dot(box.rot[1], vE0));
			_IsCldTestEdge(fp1, fp2, fR, vL, 5, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 6 - Box X-Axis cross Edge1
			const DsVec3d vL = DsVec3d::Cross(box.rot[0], vE1);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0 - DsVec3d::Dot(box.rot[0], vN);
			const double fp2 = fp0;
			const double fR = box.size[1] * fabs(DsVec3d::Dot(box.rot[2], vE1)) + box.size[2] * fabs(DsVec3d::Dot(box.rot[1], vE1));
			_IsCldTestEdge(fp0, fp1, fR, vL, 6, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 7 - Box X-Axis cross Edge2
			const DsVec3d vL = DsVec3d::Cross(box.rot[0], vE2);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0 - DsVec3d::Dot(box.rot[0], vN);
			const double fp2 = fp0 - DsVec3d::Dot(box.rot[0], vN);
			const double fR = box.size[1] * fabs(DsVec3d::Dot(box.rot[2], vE2)) + box.size[2] * fabs(DsVec3d::Dot(box.rot[1], vE2));
			_IsCldTestEdge(fp0, fp1, fR, vL, 7, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 8 - Box Y-Axis cross Edge0
			const DsVec3d vL = DsVec3d::Cross(box.rot[1], vE0);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0;
			const double fp2 = fp0 + DsVec3d::Dot(box.rot[1], vN);
			const double fR = box.size[0] * fabs(DsVec3d::Dot(box.rot[2], vE0)) + box.size[2] * fabs(DsVec3d::Dot(box.rot[0], vE0));
			_IsCldTestEdge(fp0, fp2, fR, vL, 8, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 9 - Box Y-Axis cross Edge1
			const DsVec3d vL = DsVec3d::Cross(box.rot[1], vE1);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0 - DsVec3d::Dot(box.rot[1], vN);
			const double fp2 = fp0;
			const double fR = box.size[0] * fabs(DsVec3d::Dot(box.rot[2], vE1)) + box.size[2] * fabs(DsVec3d::Dot(box.rot[0], vE1));
			_IsCldTestEdge(fp0, fp1, fR, vL, 9, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 10 - Box Y-Axis cross Edge2
			const DsVec3d vL = DsVec3d::Cross(box.rot[1], vE2);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0 - DsVec3d::Dot(box.rot[1], vN);
			const double fp2 = fp0 - DsVec3d::Dot(box.rot[1], vN);
			const double fR = box.size[0] * fabs(DsVec3d::Dot(box.rot[2], vE2)) + box.size[2] * fabs(DsVec3d::Dot(box.rot[0], vE2));
			_IsCldTestEdge(fp0, fp1, fR, vL, 10, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 11 - Box Z-Axis cross Edge0
			const DsVec3d vL = DsVec3d::Cross(box.rot[2], vE0);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0;
			const double fp2 = fp0 + DsVec3d::Dot(box.rot[2], vN);
			const double fR = box.size[0] * fabs(DsVec3d::Dot(box.rot[1], vE0)) + box.size[1] * fabs(DsVec3d::Dot(box.rot[0], vE0));
			_IsCldTestEdge(fp0, fp2, fR, vL, 11, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 12 - Box Z-Axis cross Edge1
			const DsVec3d vL = DsVec3d::Cross(box.rot[2], vE1);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0 - DsVec3d::Dot(box.rot[2], vN);
			const double fp2 = fp0;
			const double fR = box.size[0] * fabs(DsVec3d::Dot(box.rot[1], vE1)) + box.size[1] * fabs(DsVec3d::Dot(box.rot[0], vE1));
			_IsCldTestEdge(fp0, fp1, fR, vL, 12, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}

		{// Axis 13 - Box Z-Axis cross Edge2
			const DsVec3d vL = DsVec3d::Cross(box.rot[2], vE2);
			const double fp0 = DsVec3d::Dot(vL, vD);
			const double fp1 = fp0 - DsVec3d::Dot(box.rot[2], vN);
			const double fp2 = fp0 - DsVec3d::Dot(box.rot[2], vN);
			const double fR = box.size[0] * fabs(DsVec3d::Dot(box.rot[1], vE2)) + box.size[1] * fabs(DsVec3d::Dot(box.rot[0], vE2));
			_IsCldTestEdge(fp0, fp1, fR, vL, 13, ret);
			if (!ret.isColide){
				return _ResultSeparateAxis();//衝突無し
			}
		}
		return ret;
	}

	inline double _PointDistance(const DsVec4d& plane, const DsVec3d& point)
	{
		return (plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w);
	}

	// clip polygon with plane and generate new polygon points
	void _IsCldClipPolyToPlane(const DsVec3d* avArrayIn, int ctIn, DsVec3d* avArrayOut, int &ctOut, const DsVec4d &plPlane)
	{
		//planeの法線側にある頂点と交差点をavArrayOutへ追加

		// start with no output points
		ctOut = 0;
		int i0 = ctIn - 1;
		// for each edge in input polygon
		for (int i1 = 0; i1<ctIn; i0 = i1, i1++) {
			// calculate distance of edge points to plane
			const double fDistance0 = _PointDistance(plPlane, avArrayIn[i0]);
			const double fDistance1 = _PointDistance(plPlane, avArrayIn[i1]);

			// if first point is in front of plane
			// 法線側にある頂点
			if (fDistance0 >= 0) {
				// emit point
				avArrayOut[ctOut] = avArrayIn[i0];
				ctOut++;
			}

			// if points are on different sides
			// 交差点
			if (((fDistance0 > 0.0) && (fDistance1 < 0.0)) || ((fDistance0 < 0.0) && (fDistance1 > 0.0))) {

				// find intersection point of edge and plane
				const DsVec3d vIntersectionPoint = avArrayIn[i0] - ( (avArrayIn[i0] - avArrayIn[i1]) * (fDistance0 / (fDistance0 - fDistance1)) );
				// emit intersection point
				avArrayOut[ctOut] = vIntersectionPoint;
				ctOut++;
			}
		}
	}

	void _IsCldClipping(const _ResultSeparateAxis& spa, const _BoxGeom& box, const _TriGeom& tri, DsCollisionResult& out)
	{
		const DsVec3d& v0 = tri.v[0];
		const DsVec3d& v1 = tri.v[1];
		const DsVec3d& v2 = tri.v[2];
		const DsVec3d& vE0 = tri.vE0;
		const DsVec3d& vE1 = tri.vE1;
		const DsVec3d& vE2 = tri.vE2;
		const DsVec3d& vN = tri.vN;

		// エッジが最接近点なので、これを衝突点にする
		if ( 4 < spa.axisType ) {
			DsVec3d vub;//三角形側の辺
			DsVec3d vPb;//三角形側の辺の始点
			DsVec3d vPa = box.pos;//box側の辺の始点

			// 衝突方向に一番近い辺、の頂点。
			for (int i = 0; i < 3; i++) {
				const double fSign = (DsVec3d::Dot(spa.normal, box.rot[i]) > 0.0) ? (1.0) : (-1.0);
				vPa += box.rot[i] * (fSign * box.size[i]);
			}

			//axisTypeからどの辺とどの辺が交差しているパターンなのか判定
			const int iEdge = (spa.axisType - 5) % 3;
			if (iEdge == 0) {
				vPb = v0;
				vub = vE0;
			}
			else if (iEdge == 1) {
				vPb = v2;//始点にあわせるならv0の方がいい気がするけど、方向関係ないからどっちでもいいのか
				vub = vE1;
			}
			else {
				vPb = v1;
				vub = vE2;
			}

			// setup direction parameter for face edge
			vub = DsVec3d::Normalize(vub);

			double fParam1;
			double fParam2;

			// setup direction parameter for box edge
			const int col = (spa.axisType - 5) / 3;
			const DsVec3d& vua = box.rot[col];//box側の始点
			// find two closest points on both edges
			CalcNearestPoint(vPa, vua, vPb, vub, fParam1, fParam2);
			vPa += vua * fParam1;
			vPb += vub * fParam2;

			// calculate collision point
			const DsVec3d vPntTmp = (vPa + vPb)*0.5;
			//法線はboxが跳ね返る方向にするので反転させる
			out.AddInfo(vPntTmp, -spa.normal, spa.depth, box.pContext->RefOwnerId(), tri.pContext->RefOwnerId());
		}
		else if (spa.axisType == 1) {
			//三角形の法線方向が分離軸
			
			/*
			三角形の法線と反対方向にあるboxの頂点を割り出す。
			その後、三角柱の領域内にある頂点（と三角形との交差点）に絞る。（柱方向は法線）
			手順は、
			１．三角形の法線の反対方向の頂点求める
			２．三角形の辺tとboxの交差点を求める
			３．三角形の辺それぞれで面を作り、反対側の点を取り除く
			イメージとしてboxを三角形で切断し、切り取られた外側の方の頂点郡をめり込み頂点にする感じ
			これもほぼODEを参考
			*/

			const DsVec3d vNormal2 = -spa.normal;
			const DsVec3d vNr(DsVec3d::Dot(box.rot[0], vNormal2), DsVec3d::Dot(box.rot[1], vNormal2), DsVec3d::Dot(box.rot[2], vNormal2));//box座標に変換（意味合い的には転置かけてるのと同じ）
			const DsVec3d vAbsNormal( fabs(vNr.x), fabs(vNr.y), fabs(vNr.z) );

			// get closest face from box
			//iB0は衝突方向に一番近いboxの基準軸のうちどれか。iB1とiB2はそれ以外
			int iB0, iB1, iB2;
			if (vAbsNormal.v[1] > vAbsNormal.v[0]) {
				if (vAbsNormal.v[1] > vAbsNormal.v[2]) {
					iB1 = 0;  iB0 = 1;  iB2 = 2;
				}
				else {
					iB1 = 0;  iB2 = 1;  iB0 = 2;
				}
			}
			else {
				if (vAbsNormal.v[0] > vAbsNormal.v[2]) {
					iB0 = 0;  iB1 = 1;  iB2 = 2;
				}
				else {
					iB1 = 0;  iB2 = 1;  iB0 = 2;
				}
			}

			// v0 から boxの接触面の中心点vCenterを求める
			DsVec3d vCenter;
			DsVec3d vRotCol = box.rot[iB0];
			if (vNr.v[iB0] > 0.0) {
				vCenter = box.pos - v0 - (vRotCol*box.size[iB0]);
			}
			else {
				vCenter = box.pos - v0 + (vRotCol*box.size[iB0]);
			}

			// boxの接触面の４隅の頂点
			DsVec3d avPoints[4];
			vRotCol = box.rot[iB1];
			DsVec3d vRotCol2 = box.rot[iB2];
			avPoints[0] = vCenter + (vRotCol*box.size[iB1]) - (vRotCol2*box.size[iB2]);
			avPoints[1] = vCenter - (vRotCol*box.size[iB1]) - (vRotCol2*box.size[iB2]);
			avPoints[2] = vCenter - (vRotCol*box.size[iB1]) + (vRotCol2*box.size[iB2]);
			avPoints[3] = vCenter + (vRotCol*box.size[iB1]) + (vRotCol2*box.size[iB2]);

			// clip Box face with 4 planes of triangle (1 face plane, 3 egde planes)
			DsVec3d avTempArray1[9];
			DsVec3d avTempArray2[9];

			int iTempCnt1 = 0;
			int iTempCnt2 = 0;

			{// Normal plane
				const DsVec3d vTemp = DsVec3d::Normalize(-vN);
				const DsVec4d plPlane = DsVec4d(vTemp, 0);
				//boxの平面の辺と三角形が貫通した点と、三角形の法線方向より高さが０以上のboxの平面の頂点がavTempArray1に入る
				_IsCldClipPolyToPlane(avPoints, 4, avTempArray1, iTempCnt1, plPlane);
			}

			{// Plane p0
				const DsVec3d vTemp = DsVec3d::Normalize(DsVec3d::Cross(vN, v1 - v0));
				const DsVec4d plPlane = DsVec4d(vTemp, 0);
				_IsCldClipPolyToPlane(avTempArray1, iTempCnt1, avTempArray2, iTempCnt2, plPlane);
			}
			
			{// Plane p1
				const DsVec3d vTemp = DsVec3d::Normalize(DsVec3d::Cross(vN, v2 - v1));
				const DsVec4d plPlane = DsVec4d(vTemp, DsVec3d::Dot(v0 - v2, vTemp));
				_IsCldClipPolyToPlane(avTempArray2, iTempCnt2, avTempArray1, iTempCnt1, plPlane);
			}

			{// Plane p2
				const DsVec3d vTemp = DsVec3d::Normalize(DsVec3d::Cross(vN, v0 - v2));
				const DsVec4d plPlane = DsVec4d(vTemp, 0);
				_IsCldClipPolyToPlane(avTempArray1, iTempCnt1, avTempArray2, iTempCnt2, plPlane);
			}

			// for each generated contact point
			for (int i = 0; i < iTempCnt2; i++) {
				// calculate depth
				double fTempDepth = DsVec3d::Dot(vNormal2, avTempArray2[i]);
				// clamp depth to zero
				if (fTempDepth > 0) {
					fTempDepth = 0;
				}
				const DsVec3d vPntTmp = avTempArray2[i] + v0;
				//法線はboxが跳ね返る方向にするので反転させる
				out.AddInfo(vPntTmp, -spa.normal, spa.depth, box.pContext->RefOwnerId(), tri.pContext->RefOwnerId());
			}
		}
		else if ( (2 <= spa.axisType) && (spa.axisType <= 4) ) {
			//spa.axisType == 1ときと同じだけど三角形が切り取られる側になる
			// get normal of box face
			const DsVec3d vNormal2 = spa.normal;
			
			// get indices of box axes in correct order
			int iA0, iA1, iA2;
			iA0 = spa.axisType - 2;
			if (iA0 == 0) {
				iA1 = 1; iA2 = 2;
			}
			else if (iA0 == 1) {
				iA1 = 0; iA2 = 2;
			}
			else {
				iA1 = 0; iA2 = 1;
			}

			// calculate triangle vertices in box frame
			const DsVec3d avPoints[3]={
				v0 - box.pos,
				v1 - box.pos,
				v2 - box.pos,
			};
			
			// CLIP Polygons
			// define temp data for clipping
			DsVec3d avTempArray1[9];
			DsVec3d avTempArray2[9];

			int iTempCnt1, iTempCnt2;

			// clip triangle with 5 box planes (1 face plane, 4 edge planes)

			{// Normal plane
				const DsVec4d plPlane = DsVec4d(-vNormal2, box.size[iA0]);
				_IsCldClipPolyToPlane(avPoints, 3, avTempArray1, iTempCnt1, plPlane);
			}

			{// Plane p0
				const DsVec4d plPlane(box.rot[iA1], box.size[iA1]);
				_IsCldClipPolyToPlane(avTempArray1, iTempCnt1, avTempArray2, iTempCnt2, plPlane);
			}

			{// Plane p1
				const DsVec4d plPlane(-box.rot[iA1], box.size[iA1]);
				_IsCldClipPolyToPlane(avTempArray2, iTempCnt2, avTempArray1, iTempCnt1, plPlane);
			}
			
			{// Plane p2
				const DsVec4d plPlane(box.rot[iA2], box.size[iA2]);
				_IsCldClipPolyToPlane(avTempArray1, iTempCnt1, avTempArray2, iTempCnt2, plPlane);
			}

			{// Plane p3
				const DsVec4d plPlane(-box.rot[iA2], box.size[iA2]);
				_IsCldClipPolyToPlane(avTempArray2, iTempCnt2, avTempArray1, iTempCnt1, plPlane);
			}

			// for each generated contact point
			for (int i = 0; i < iTempCnt1; i++) {
				// calculate depth
				double fTempDepth = DsVec3d::Dot(vNormal2, avTempArray1[i]) - box.size[iA0];
				// clamp depth to zero
				if (fTempDepth > 0) {
					fTempDepth = 0;
				}

				// generate contact data
				const DsVec3d vPntTmp = avTempArray1[i]+box.pos;
				//法線はboxが跳ね返る方向にするので反転させる
				out.AddInfo(vPntTmp, -spa.normal, spa.depth, box.pContext->RefOwnerId(), tri.pContext->RefOwnerId());
			}
		}
		else{
			//DS_ASSERT(false, "BoxMesh で実装ミス spa.axisType = %d", spa.axisType);
			//衝突無し
		}
	}
}

DsCollisionResult& DsCollisionBoxMesh::Collide()
{
	//return m_info;
	m_info.Clear();
	if (_ColideAABB()){

		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pMesh->RefOwnerId().GetActor(), *m_pBox->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		return _ColideFinal();
	}
	return m_info;
}

DsCollisionResult& DsCollisionBoxMesh::_ColideFinal()
{
	_BoxGeom box(m_pBox);
	const int faceNum = m_pMesh->GetFaceNum();
	for (int fi = 0; fi < faceNum; ++fi){
		_TriGeom tri(m_pMesh, fi);
		const _ResultSeparateAxis spa = _CalcSeparateAxis(box, tri);
		if (spa.isColide){
			_IsCldClipping(spa, box, tri, m_info);
		}
	}
	
	//for (int i = 0; i < m_info.GetColNum(); ++i) {
	//	DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(1, 0, 0)).DrawSphere(m_info.m_colPos[i], 0.05);
	//}

	return m_info;
}

bool DsCollisionBoxMesh::_ColideAABB()
{
	const DsBoundingTreeBase* pMTree = m_pMesh->GetBoungingTree();
	const DsBoundingTreeBase* pBTree = m_pBox->GetBoungingTree();

	if (pMTree && pBTree){
		return pBTree->IsContain(*pMTree);
	}
	else{
		return true;
	}
}