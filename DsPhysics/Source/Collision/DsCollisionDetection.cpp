#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_DETECTION__
#include "Collision/DsCollisionDetection.h"
#endif

#include "DsFace.h"
#include "DsLine.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionContext.h"


using namespace DsPhysics;


inline double _DsCD_PointFace( const DsVec3d& p, const DsQuad& f, const DsVec3d* fp )
{
	const double dot = DsVec3d::Dot(p - fp[f.index[0]], f.normal);
	return dot;
}

bool DsPhysics::DsCD_PointFace( const DsCollisionContext* pCube1, const DsCollisionContext* pCube2, DsCollisionResult& info )
{
	bool ret = false;

	const DsActorId& p1Id = pCube1->RefOwnerId();
	const DsVec3d* p1 = pCube1->GetVertex();
	const int p1VetexNum = pCube1->GetVertexNum();
	const DsActorId& p2Id = pCube2->RefOwnerId();
	const DsVec3d* p2 = pCube2->GetVertex();
	const DsQuad* p2Face = pCube2->GetFace();
	const int p2FaceNum = pCube2->GetFaceNum();
	const DsVec3d toDist = pCube2->GetBasePos() - pCube1->GetBasePos();

	if( p1 && p2 && p2Face )
	{
		for( int p1Idx = 0; p1VetexNum > p1Idx; ++p1Idx )
		{
			//衝突方向は相手側の面の法線になる
			const DsQuad* nearFace = NULL;
			double maxDot = -100000000000.0;
			for(int p2Idx = 0; p2FaceNum > p2Idx; ++p2Idx )
			{
				//法線に対する相手の点の高さを求める
				double dot = _DsCD_PointFace(p1[p1Idx], p2Face[p2Idx], p2);
				
				if( dot >= 0.0 )
				{
					if (dot <= 0.0001)
					{
						////表面上に居たものの誤差の範囲だったので含める
						dot = -0.0001;
					}
					else
					{
						nearFace = NULL;
						break; //面の表にいるので衝突なし
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
				//ここまで来たらp1[p1Idx]は衝突してる
				//めり込み量はめり込んでればプラス
				info.AddInfo(p1[p1Idx], nearFace->normal, -maxDot, p1Id, p2Id);
				ret = true;
			}
		}
	}

	return ret;
}

bool DsPhysics::DsCD_PointFace(const DsActorId& p1Id, const DsVec3d& p1, const DsCollisionContext* pCube2, DsCollisionResult& info)
{
	bool ret = false;

	const DsActorId& p2Id = pCube2->RefOwnerId();
	const DsVec3d* p2 = pCube2->GetVertex();
	const DsQuad* p2Face = pCube2->GetFace();
	const int p2FaceNum = pCube2->GetFaceNum();
	const DsVec3d toDist = DsVec3d::Zero();

	if (p2 && p2Face)
	{
		
		//衝突方向は相手側の面の法線になる
		const DsQuad* nearFace = NULL;
		double maxDot = -100000000000.0;
		for (int p2Idx = 0; p2FaceNum > p2Idx; ++p2Idx)
		{
			//法線に対する相手の点の高さを求める
			const double dot = _DsCD_PointFace(p1, p2Face[p2Idx], p2);

			if (dot >= 0.0)
			{
				nearFace = NULL;
				break; //面の表にいるので衝突なし
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
			//ここまで来たらp1[p1Idx]は衝突してる
			//めり込み量はめり込んでればプラス
			info.AddInfo(p1, nearFace->normal, -maxDot, p1Id, p2Id);
			ret = true;
		}
	}

	return ret;
}

//位置p1を基準としてpConvexのnormal方向の一番深い深度を求める
double DsPhysics::DsCD_NormalFaceDepthMax(const DsVec3d& p1, const DsVec3d& normal, const DsCollisionContext* pConvex)
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


inline bool _DsCD_LineFace(const DsActorId& p1Id, const DsVec3d& lineV0, const DsVec3d& lineV1, const DsActorId& p2Id, const DsVec3d* p2, const DsQuad& face, const DsVec3d& toDist, DsCollisionResult& info)
//inline bool _DsCD_LineFace(const DsActorId& p1Id, const DsVec3d* p1, const DsLine& line, const DsActorId& p2Id, const DsVec3d* p2, const DsQuad& face, const DsVec3d& toDist, DsCollisionResult& info)
{
	//衝突方向は相手側の面の法線

	//平面までの高さを求める
	const double d1 = DsVec3d::Dot(lineV0 - p2[face.index[0]], face.normal);
	const double d2 = DsVec3d::Dot(lineV1 - p2[face.index[0]], face.normal);
	//const double d1 = DsVec3d::Dot( p1[line.index[0]] - p2[face.index[0]], face.normal );
	//const double d2 = DsVec3d::Dot( p1[line.index[1]] - p2[face.index[0]], face.normal );

	//衝突してない
	if( (0.0 >= d1) && (0.0 >= d2) )
	{
		return false;
	}
	if( (0.0 <= d1 ) && (0.0 <= d2) )
	{
		return false;
	}

	const double fd1 = fabs(d1);
	const double fd2 = fabs(d2);

	if( 0.000001 > (fd1+fd2) )
	{
		//線と面はほぼ水平
		return false;
	}

	//めり込んでる方の深度を求めておく。マイナスの方がめり込んでる方
	//これをめり込み量にしちゃうと長い辺はすごいめり込むことになる
	double depth = (0.0 > d1) ? d1 : d2;


	//衝突点までの比率を求める
	const double internalDivision =	fd1/(fd1+fd2);
	//衝突点までのベクトルを求める
	const DsVec3d lineVec = lineV1 - lineV0;
	const DsVec3d colVec = lineVec * internalDivision;
	//衝突点の位置を求める
	const DsVec3d colPos = lineV0 + colVec;

	//衝突点が４角形の中に入っているか調べる
	std::vector<DsVec3d> edges(face.vn); edges.clear();
	const int endIdx = face.vn - 1;
	for (int faceIdx = 0; faceIdx < face.vn; ++faceIdx)
	{
		//四角形を構成する辺
		const DsVec3d edge = (endIdx != faceIdx) ? (p2[face.index[faceIdx + 1]] - p2[face.index[faceIdx]]) : (p2[face.index[0]] - p2[face.index[faceIdx]]);
		edges.push_back(edge);//後で使うのでとっておく

		//外積を取り、１つでも面と反対方向なら面に衝突点は含まれない
		const DsVec3d cross = DsVec3d::Cross( edge, colPos - p2[face.index[faceIdx]] );
		if( DsVec3d::Dot(cross, face.normal) <= -0.00001 )
		{
			return false;
		}
	}

	double retDepth = depth;
	DsVec3d retNormal = face.normal;
	//面エッジと貫通線の交点を求め、その直下の線の高さをめり込み量とする。方向は面の方向
	const DsVec3d lineCol = (0.0 > d1) ? (colPos - lineV0) : (colPos - lineV1);//めり込んでる頂点から衝突点までのベクトル
	const DsVec3d lineFlat = lineCol - (face.normal*DsVec3d::Dot(lineCol, face.normal));//面の平面に投影
	DsVec3d intersection;
	for (int faceIdx = 0; faceIdx < face.vn; ++faceIdx)
	{
		if (DsVec3d::GetIntersection(colPos, lineFlat, p2[face.index[faceIdx]], edges[faceIdx], intersection))
		{
			//交点がlineColのどの割合の箇所にあるか求める
			const double intersectionRate = ( (colPos - intersection).Length() / lineFlat.Length() );
			//衝突線のめり込んでる方の高さに割合を掛ければ交差点の直下の点までの高さになる
			retDepth *= intersectionRate;
			break;
		}
	}
	retDepth = -retDepth;

	//面エッジと貫通点の最も近い距離がめり込み量。方向は面の中心からエッジに向かう方向
	//retDepth = 0.0;
	//for (int faceIdx = 0; faceIdx < face.vn; ++faceIdx){
	//	const DsVec3d tmp = p2[face.index[faceIdx]] - colPos;
	//	const DsVec3d edge = (faceIdx != (face.vn - 1)) ? (p2[face.index[faceIdx+1]] - p2[face.index[faceIdx]]) : (p2[face.index[0]] - p2[face.index[faceIdx]]);
	//	DsVec3d normal = DsVec3d::Normalize( DsVec3d::Cross(edge, face.normal) );
	//	const double dot = DsVec3d::Dot(tmp, normal);
	//	if (dot < 0.0){
	//		normal *= -1.0;//衝突点に向かう方向なので逆
	//	}
	//	const double dotAbs = fabs(dot);
	//	if (retDepth < dotAbs){
	//		retDepth = dotAbs;
	//		retNormal = normal;
	//	}
	//	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3f(1, 0, 0)).DrawLine(p2[face.index[faceIdx]], p2[face.index[faceIdx]]+retNormal);
	//}

	//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3f(1, 0, 0)).DrawSphere(colPos, 0.05);
	//衝突点確定
	//めり込み量はめり込んでればプラス
	info.AddInfo(colPos, retNormal, retDepth, p1Id, p2Id);
	return true;
}

//面と線が平行に接触したとき
inline bool _DsCD_LineFaceParallel(const DsActorId& p1Id, const DsVec3d& lineV0, const DsVec3d& lineV1, const DsActorId& p2Id, const DsVec3d* p2, const DsQuad& face, const DsVec3d& toDist, DsCollisionResult& info)
{
	const DsVec3d facePlane = DsVec3d::Normalize( p2[face.index[0]] - p2[face.index[1]] );
	const DsVec3d linePlane = DsVec3d::Normalize(lineV1 - lineV0);

}

bool DsPhysics::DsCD_LineFace( const DsCollisionContext* pCube1, const DsCollisionContext* pCube2, DsCollisionResult& info )
{
	bool ret = false;

	const DsActorId& p1Id = pCube1->RefOwnerId();
	const DsVec3d* p1 = pCube1->GetVertex();
	const DsLine* p1Line = pCube1->GetLine();
	const int p1LineNum = pCube1->GetLineNum();
	const DsActorId& p2Id = pCube2->RefOwnerId();
	const DsVec3d* p2 = pCube2->GetVertex();
	const DsQuad* p2Face = pCube2->GetFace();
	const int p2FaceNum = pCube2->GetFaceNum();
	const DsVec3d toDist = pCube2->GetBasePos() - pCube1->GetBasePos();

	if( p1 && p1Line && p2 && p2Face )
	{
		for(int lineIdx=0; p1LineNum > lineIdx; ++lineIdx)
		{
			for(int faceIdx=0; p2FaceNum > faceIdx; ++faceIdx)
			{
				const DsLine& line = p1Line[lineIdx];
				ret |= _DsCD_LineFace(p1Id, p1[line.index[0]], p1[line.index[1]], p2Id, p2, p2Face[faceIdx], toDist, info);
			}
		}
	}
	return ret;
}


bool DsPhysics::DsCD_LineFaceVelocity(const DsCollisionContext* pCube1, const DsCollisionContext* pCube2, DsCollisionResult& info)
{
	bool ret = false;

	const DsActorId& p1Id = pCube1->RefOwnerId();
	const DsVec3d* p1 = pCube1->GetVertex();
	const DsVec3d* preP1 = pCube1->GetPreVertex();//１フレ前の頂点
	const int p1Vn = pCube1->GetVertexNum();
	const DsActorId& p2Id = pCube2->RefOwnerId();
	const DsVec3d* p2 = pCube2->GetVertex();
	const DsQuad* p2Face = pCube2->GetFace();
	const int p2FaceNum = pCube2->GetFaceNum();
	const DsVec3d toDist = pCube2->GetBasePos() - pCube1->GetBasePos();

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
		DsVec3d start;//始点
		DsVec3d edge;//辺
	};
	std::vector<_Edge> edges(4);//faceを構成する辺のベクトル。反時計回り
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

	//全ての頂点が面の一方側にあれば当たってない
	if ((0.0 < dot1) && (0.0 < dot2) && (0.0 < dot3))
	{
		return false;
	}
	if ((0.0 > dot1) && (0.0 > dot2) && (0.0 > dot3))
	{
		return false;
	}

	int crossCount = 0;//線が面を貫通した個数

	//面の中に点が含まれているかを判定
	auto IsInFace = [](const DsVec3d& p, const DsQuad& face, const DsVec3d* faceV, const std::vector<_Edge>& edges)-> bool
	{
		bool ret = false;
		bool firstDepth = true;
		const int endIdx = face.vn - 1;
		
		for (int fi = 0; fi < face.vn; ++fi)
		{
			//衝突点と辺の外積を取り、１つでも面の法線と同じ方向なら面に衝突点は含まれない
			const DsVec3d cross = DsVec3d::Cross(edges[fi].edge, p - faceV[face.index[fi]]);
			if ( 0.0 < DsVec3d::Dot(cross, face.normal) )//法線と同じなら面の中
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
		DsVec3d intersctionEnd;//faceを貫通している辺の裏側にある方の頂点。
	};
	std::vector<_CollisionPoint> colPoints;

	//頂点1,2,3の内、相手の面を跨いでいる組み合わせをみつける。
	if((dot1*dot2) < 0.0)
	{
		const double innnerRate = (fdot1 / (fdot1+fdot2)); //両面に跨いでいるはずなので符号はマイナスになるはず。-1かけて打ち消す
		const DsVec3d p = ((triV[tri.index[1]] - triV[tri.index[0]])*innnerRate) + triV[tri.index[0]];
		const bool result = IsInFace(p, face, faceV, edges);
		if (result)//三角系の内側にあった
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
		if (result)//三角系の内側にあった
		{
			_CollisionPoint cp;
			cp.hitPos = p;
			cp.depth = (dot1 < 0.0) ? (-dot1) : (-dot3);
			cp.intersctionEnd = (dot1 < 0.0) ? (triV[tri.index[0]]) : (triV[tri.index[2]]);
			colPoints.push_back(cp);
		}
		++crossCount;
	}
	
	//三角形の内、貫通した辺が２本未満の場合は貫通している辺の可能性があるのでやる。
	if (crossCount < 2)
	{
		if ((dot2*dot3) < 0.0)
		{
			const double innnerRate = (fdot2 / (fdot2+fdot3));
			const DsVec3d p = ((triV[tri.index[2]] - triV[tri.index[1]])*innnerRate) + triV[tri.index[1]];
			const bool result = IsInFace(p, face, faceV, edges);
			if (result)//三角系の内側にあった
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
		double depth = cp.depth;//交差してないときのデフォは終端

		//めり込み深度を求める。faceの法線視点で見て、triの貫通辺とfaceの構成辺の交差する点のところにあるtriの貫通辺の高さ
		DsVec3d colVec = cp.intersctionEnd - cp.hitPos;
		colVec = colVec - (face.normal*DsVec3d::Dot(colVec, face.normal));//faceを貫通する辺のface上のベクトル
		DsVec3d intersection;;
		for each(const _Edge& edge in edges)
		{
			if (DsVec3d::GetIntersection(cp.hitPos, colVec, edge.start, edge.edge, intersection))
			{
				//交差したので、そこがめり込み深度を求める点
				//交点がlineColのどの割合の箇所にあるか求める
				const double intersectionRate = ((cp.hitPos - intersection).Length() / colVec.Length());
				depth *= intersectionRate;
				break;
			}
		}
		
		//めり込み量はめり込んでればプラス
		info.AddInfo(cp.hitPos, face.normal, depth, meshActorId, faceActorId);
	}

	//return ret;
	return true;//三角形の中に面が完全に入っているときを検知したいので、三角形と面を入れ替えるためtrueを返す
}

bool DsPhysics::DsCD_TriangleFace(const DsCollisionContext* pMesh, const DsCollisionContext* pCube, DsCollisionResult& info)
{
	bool ret = false;

	const DsActorId& mesh = pMesh->RefOwnerId();
	const DsActorId& cube = pCube->RefOwnerId();
	const DsVec3d* meshV = pMesh->GetVertex();
	const DsVec3d* faceV = pCube->GetVertex();
	const int triN = pMesh->GetFaceNum();
	const int faceN = pCube->GetFaceNum();

	if (meshV && faceV)
	{
		for (int ti = 0; ti < triN; ++ti)
		{
			for (int fi = 0; fi < faceN; ++fi)
			{
				const bool isHit = _DsCD_TriangleFace(mesh, pMesh->GetFace()[ti], meshV, cube, pCube->GetFace()[fi], faceV, info);
				ret |= isHit;


				//当たってるのであれば、もう一方の面も当たってる可能性があるので入れ替える
				if (isHit)
				{
					DsCollisionResult tmp;
					if (pCube->GetFace()[fi].vn == 3)
					{
						//_DsCD_TriangleFace(mesh, pMesｔh->GetFace()[ti], meshV, cube, pCube->GetFace()[fi], faceV, info);引数が反転してるのが見やすいように比較
						// _DsCD_TriangleFace(cube, pCube->GetFace()[fi], faceV, mesh, pMesh->GetFace()[ti], meshV, info);
						   _DsCD_TriangleFace(mesh, pCube->GetFace()[fi], faceV, cube, pMesh->GetFace()[ti], meshV, tmp);
						  //master側のオーナーはmeshに統一。拘束解くときに１actorの衝突計算でオーナーが入れ替わると合力計算のときのmaster側の座標がmasterではなくなるため
					}
					else //cubeは４角形で構成されてるので、三角形に分割
					{
						DsQuad splitF1; 
						splitF1.vn = 3;
						splitF1.index[0] = pCube->GetFace()[fi].index[0];
						splitF1.index[1] = pCube->GetFace()[fi].index[1];
						splitF1.index[2] = pCube->GetFace()[fi].index[2];
						splitF1.normal = pCube->GetFace()[fi].normal;
						//_DsCD_TriangleFace(cube, splitF1, faceV, mesh, pMesh->GetFace()[ti], meshV, info);
						  _DsCD_TriangleFace(mesh, splitF1, faceV, cube, pMesh->GetFace()[ti], meshV, tmp);//↑と同じ理由でオーナー入れ替え
				
						DsQuad splitF2;
						splitF2.vn = 3;
						splitF2.index[0] = pCube->GetFace()[fi].index[2];
						splitF2.index[1] = pCube->GetFace()[fi].index[3];
						splitF2.index[2] = pCube->GetFace()[fi].index[0];
						splitF2.normal = pCube->GetFace()[fi].normal;
						//_DsCD_TriangleFace(cube, splitF2, faceV, mesh, pMesh->GetFace()[ti], meshV, info);
						  _DsCD_TriangleFace(mesh, splitF2, faceV, cube, pMesh->GetFace()[ti], meshV, tmp);
					}

					//拘束を解く時、mesh側をマスターとして解くようにしているので、meshがmeshの法線方向にめり込み解決してしまう。
					//法線を反転
					const int cn = tmp.GetColNum();
					for (int ci = 0; ci < cn; ++ci)
					{
						tmp.m_colNormal[ci] *= -1.0;
					}
					info.AddInfo(tmp);
				
					//この時点でretはtrue確定なので、戻り値に反映させる必要はない
				}
			}
		}
	}
	return ret;
}
