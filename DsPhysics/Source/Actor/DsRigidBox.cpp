#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_BOX__
#include "Actor/DsRigidBox.h"
#endif

using namespace DsPhysics;



DsRigidBox::DsRigidBox(const DsActorId& id, const char* name )
:DsRigidBody(id, name)
//, m_vertex()
//, m_vertexOriginal()
//, m_face()
//, m_line()
{
}

DsRigidBox::~DsRigidBox()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

void DsRigidBox::Create(const DsVec3d* pv, const double mass )
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	{
		DsRigidGeometryInfo& gi = m_geomInfo;
		gi.vn = VERTEX_NUM;
		gi.fn = FACE_NUM;
		gi.ln = LINE_NUM;
		gi.pVertex = m_vertex;
		gi.pVertexOriginal = m_vertexOriginal;
		gi.pFace = m_face;
		gi.pLine = m_line;

		for(int vn=0; VERTEX_NUM > vn; ++vn )
		{
			gi.pVertex[ vn ] = pv[ vn ];
			gi.pVertexOriginal[vn] = pv[vn];
		}

		/*
				  3_________2
				 /         /|
			  0 /________1/ |
				|         | |
				|         | | 
				|	7	  | / 6      y
				|_________|/         |__ x 
				4         5          /
									z
		*/

		
		//反時計回り
		gi.pFace[0].index[0] = 0;	gi.pFace[0].index[1] = 1;	gi.pFace[0].index[2] = 2;	gi.pFace[0].index[3] = 3;	//上面
		gi.pFace[1].index[0] = 4;	gi.pFace[1].index[1] = 7;	gi.pFace[1].index[2] = 6;	gi.pFace[1].index[3] = 5;	//下面
		gi.pFace[2].index[0] = 0;	gi.pFace[2].index[1] = 4;	gi.pFace[2].index[2] = 5;	gi.pFace[2].index[3] = 1;	//前面
		gi.pFace[3].index[0] = 3;	gi.pFace[3].index[1] = 2;	gi.pFace[3].index[2] = 6;	gi.pFace[3].index[3] = 7;	//後面
		gi.pFace[4].index[0] = 1;	gi.pFace[4].index[1] = 5;	gi.pFace[4].index[2] = 6;	gi.pFace[4].index[3] = 2;	//右面
		gi.pFace[5].index[0] = 0;	gi.pFace[5].index[1] = 3;	gi.pFace[5].index[2] = 7;	gi.pFace[5].index[3] = 4;	//左面
	
		//法線
		for(int fn=0; FACE_NUM > fn; ++fn)
		{
			const DsVec3d normal = DsVec3d::Cross( pv[gi.pFace[fn].index[1]] - pv[gi.pFace[fn].index[0]], pv[gi.pFace[fn].index[3]] - pv[gi.pFace[fn].index[0]] );
			gi.pFace[fn].normal = DsVec3d::Normalize(normal);
		}

		gi.pLine[0].index[0] = 0;	gi.pLine[0].index[1] = 1;	//上面
		gi.pLine[1].index[0] = 1;	gi.pLine[1].index[1] = 2;
		gi.pLine[2].index[0] = 2;	gi.pLine[2].index[1] = 3;
		gi.pLine[3].index[0] = 3;	gi.pLine[3].index[1] = 0;
		gi.pLine[4].index[0] = 0;	gi.pLine[4].index[1] = 4;	//縦
		gi.pLine[5].index[0] = 1;	gi.pLine[5].index[1] = 5;
		gi.pLine[6].index[0] = 2;	gi.pLine[6].index[1] = 6;
		gi.pLine[7].index[0] = 3;	gi.pLine[7].index[1] = 7;
		gi.pLine[8].index[0] = 4;	gi.pLine[8].index[1] = 7;	//下面
		gi.pLine[9].index[0] = 7;	gi.pLine[9].index[1] = 6;
		gi.pLine[10].index[0] = 6;	gi.pLine[10].index[1] = 5;
		gi.pLine[11].index[0] = 5;	gi.pLine[11].index[1] = 4;
	}

	{
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DS_MAX_MASS : mass;
			const DsVec3d maxLength = _GetMaxVector(pv, VERTEX_NUM);
			const DsVec3d bias = DsVec3d(1.0, 1.0, 1.0) + m_biasInertia;
			const double Ixx = M / 3.0 * (maxLength.y*maxLength.y + maxLength.z*maxLength.z) * bias.x;
			const double Iyy = M / 3.0 * (maxLength.x*maxLength.x + maxLength.z*maxLength.z) * bias.y;
			const double Izz = M / 3.0 * (maxLength.x*maxLength.x + maxLength.y*maxLength.y) * bias.z;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = Ixx;
			pi.mass.inertia[4] = Iyy;
			pi.mass.inertia[8] = Izz;
			pi.mass.inertiaOriginal = pi.mass.inertia;
			pi.mass.mass = M;
		}

		//初期姿勢
		pi.rotation = DsMat33d::Identity();

		//初期位置
		pi.pos.Set( 0.0, 0.0, 0.0 );

		//重心位置
		DsRigidGeometryInfo& gi = m_geomInfo;
		//pi.centerOfGravity = _GetCenterOfGravity(gi.vertex, DsRigidBoxGeometryInfo::VERTEX_NUM);
		pi.centerOfGravity = GetPosition();

		//AABB
		DsVec3d maxLen = DsVec3d::Zero();
		for (int i = 0; i < VERTEX_NUM; ++i)
		{
			const DsVec3d len = gi.pVertex[i] - pi.centerOfGravity;
			maxLen.x = max(maxLen.x, fabs(len.x));
			maxLen.y = max(maxLen.y, fabs(len.y));
			maxLen.z = max(maxLen.z, fabs(len.z));
		}
		m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		m_sideSize.x = maxLen.x;
		m_sideSize.y = maxLen.y;
		m_sideSize.z = maxLen.z;
	}

	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	_Update(m_initPos, m_initRot);

	m_pCollisionGeometry = new DsCollisionGeometry(m_geomInfo.pVertex, VERTEX_NUM, m_geomInfo.pFace, FACE_NUM,
		m_geomInfo.pLine, LINE_NUM, GetId(), m_physicsInfo.centerOfGravity, NULL, m_sideSize, NULL, &m_aabb, GetRotation());
}

/*
	デバッグ用描画
*/
//virtual
void DsRigidBox::Draw(DsDrawCommand& com)
{
	const DsVec3d* pVertex = m_geomInfo.pVertex;
	const int faceNum = m_geomInfo.fn;
	for (int fIdx = 0; faceNum > fIdx; ++fIdx)
	{
		const DsQuad& face = m_geomInfo.pFace[fIdx];
		//色は外側で決める
		if (!RefOption().isDrawWireFrame)
		{
			if (0.00000001 < m_dbgColor.w){//アルファ値が０なら描画しない
				com.SetColor(m_dbgColor);
				com.DrawQuad(pVertex[face.index[0]], pVertex[face.index[1]], pVertex[face.index[2]], pVertex[face.index[3]]);
			}
			com.SetColor(m_dbgLineColor);
			com.DrawLine(pVertex[face.index[0]], pVertex[face.index[1]]);
			com.DrawLine(pVertex[face.index[1]], pVertex[face.index[2]]);
			com.DrawLine(pVertex[face.index[3]], pVertex[face.index[0]]);
		}
		else
		{
			com.DrawLine(pVertex[face.index[0]], pVertex[face.index[1]]);
			com.DrawLine(pVertex[face.index[1]], pVertex[face.index[2]]);
			com.DrawLine(pVertex[face.index[3]], pVertex[face.index[0]]);
		}
	}
}


void DsRigidBox::GetVertex( DsVec3d pv[VERTEX_NUM], double xl, double yl, double zl)
{
	pv[0].Set( -xl/2.0, yl/2.0, zl/2.0 );
	pv[1].Set(  xl/2.0, yl/2.0, zl/2.0 );
	pv[2].Set(  xl/2.0, yl/2.0, -zl/2.0 );
	pv[3].Set( -xl/2.0, yl/2.0, -zl/2.0 );
	pv[4].Set( -xl/2.0, -yl/2.0, zl/2.0 );
	pv[5].Set(  xl/2.0, -yl/2.0, zl/2.0 );
	pv[6].Set(  xl/2.0, -yl/2.0, -zl/2.0 );
	pv[7].Set( -xl/2.0, -yl/2.0, -zl/2.0 );
}


//////////////////////factory//////////////////////////
DsActor* DsRigidBox::DsRigidBoxFactory::CreateIns( const DsActorId& id ) const
{
	DsRigidBox* pRet = new DsRigidBox(id, m_name.c_str());
	if(pRet)
	{
		pRet->SetInertiaBias(m_biasI);
		pRet->m_option = m_initOption;
		pRet->m_initPos = m_initPos;
		pRet->m_initRot = m_initRot;
		pRet->Create(m_pv, m_mass);
		return pRet;
	}
	else
	{
		return NULL;
	}
}