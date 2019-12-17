#include "DsPch.h"
#include "Graphics/GL/DsGLFunc.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#ifndef _DS_MODEL_RENDER_H_
#include "Graphics/Render/DsModelRender.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#include "Graphics/Shader/DsShader.h"
#include "Graphics/Image/DsImage.h"
#include "Collision/DsColliderUtil.h"
#include "Collision/DsCollisionBuffer.h"

using namespace DsLib;



DsModelRender::DsModelRender()
: m_drawList()
, m_texture()
, m_pShader(NULL)
, m_time(0.0)
{

}

DsModelRender::~DsModelRender()
{

}

void DsModelRender::Initialize(DsShader& shader)
{
	m_pShader = &shader;
}

void DsModelRender::Register(const DsModel* pModel)
{
	if (pModel)
	{
		m_drawList.push_back(pModel);

		//場所を考える
		m_texture.Load(*pModel);
	}
}

void DsModelRender::UnRegister(const DsModel* pModel)
{
	if (pModel)
	{
		m_texture.UnLoad(*pModel);
		m_drawList.remove(pModel);
	}
}

void DsModelRender::UpdateTime(double dt)
{
	m_time += dt;
}

void DsModelRender::Render() const
{
	{//UV座標デバッグ
		static double dbgRayH = 0.0;
		static double dbgRayDx = 0.003;
		dbgRayH += dbgRayDx;
		if (1.0 < dbgRayH) {
			dbgRayDx = -0.003;
		}
		if (dbgRayH < 0.0) {
			dbgRayDx = 0.003;
		}

		const DsShapeSegment seg = DsShapeSegment::Setup2Point(DsVec3d(0.01, dbgRayH, -1.0), DsVec3d(0.01, dbgRayH, 10.0));
		double distance = DBL_MAX;
		bool isHit = false;
		const DsModel::Material::Texture* pHitTex = NULL;
		DsVec3d hitVertex[4];
		DsModel::Material::Texture::UV hitUV[4];

		DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(1, 1, 1)).DrawCapsule(seg.origin, seg.origin + seg.dir*seg.len, 0.002);

		for (const DsModel* pModel : m_drawList) {

			const int fn = pModel->GetFaceNum();
			const DsVec4d* pVertex = pModel->GetVertex();
			const DsModel::Face* pFace = pModel->GetFace();

			const int mn = pModel->GetMaterialNum();
			const DsModel::Material* pMtr = pModel->GetMaterial();

			const DsVec3d modelPos = pModel->GetPosition();
			const DsMat33d modelRot = pModel->GetRotation();

			for (int mi = 0; mi < mn; ++mi, ++pMtr) {
				const int tn = pMtr->textureNum;
				const DsModel::Material::Texture* pTex = pMtr->pTexture;
				if (pTex->pAlbedoImage) {
					for (int ti = 0; ti < tn; ++ti, ++pTex) {
						int uvIdx = 0;
						const DsModel::Material::Texture::UV *pUV = pTex->pUV;
						for (const DsModel::Face* pFace : pTex->refGeomFaces) {
							const int vn = pFace->vn;

							DsVec3d buf[64];
							DsModel::Material::Texture::UV bufUV[64];

							for (int vi = 0; vi < vn; ++vi) {
								const DsModel::Material::Texture::UV& uv = pUV[uvIdx];
								const int vIdx = uv.vertexIdx;

								buf[vi] = modelRot*pVertex[vIdx] + modelPos;
								bufUV[vi] = uv;

								double tmpDistance = -1.0;
								if (vi == 2) {
									tmpDistance = DsColliderUtil::SegTriBackfaceCull(seg,buf);
								}
								else if (vi == 3) {
									DsVec3d tmp[3] =
									{
										buf[2],
										buf[3],
										buf[0],
									};

									tmpDistance = DsColliderUtil::SegTriBackfaceCull(seg, tmp);
								}

								if (0.0 < tmpDistance) {
									isHit = true;
									if (tmpDistance < distance) {
										pHitTex = pTex;
										hitVertex[0] = buf[0];
										hitVertex[1] = buf[1];
										hitVertex[2] = buf[2];
										hitVertex[3] = buf[3];
										distance = tmpDistance;
										hitUV[0] = bufUV[0];
										hitUV[1] = bufUV[1];
										hitUV[2] = bufUV[2];
										hitUV[3] = bufUV[3];
									}
								}



								//glTexCoord2f(uv.x, uv.y);
								//glVertex3dv(pVertex[vIdx].v);
								++uvIdx;
							}

						}
					}
				}

			}
		}

		if (isHit) {
			//UV展開参考
			//http://esprog.hatenablog.com/entry/2016/05/08/165445
			const unsigned char* pImp = pHitTex->pAlbedoImage->GetData();
			const int height = pHitTex->pAlbedoImage->GetHeight();
			const int width = pHitTex->pAlbedoImage->GetWidth();
			const DsVec3d hitPos = seg.origin + seg.dir*distance;

			DsMat44d projMat = DsMat44d::Identity();
			glGetDoublev(GL_PROJECTION_MATRIX, projMat.mat);
			projMat = projMat.ToTransposition();

			DsMat44d viewMat = DsMat44d::Identity();
			glGetDoublev(GL_MODELVIEW_MATRIX, viewMat.mat);
			viewMat = viewMat.ToTransposition();

			DsMat44d projViewMat = projMat * viewMat;

			DsVec2d uv1 = DsVec2d(hitUV[0].x, hitUV[0].y);
			DsVec2d uv2 = DsVec2d(hitUV[1].x, hitUV[1].y);
			DsVec2d uv3 = DsVec2d(hitUV[2].x, hitUV[2].y);

			//各点をProjectionSpaceへの変換
			const DsVec4d p1_p = projViewMat * DsVec4d(hitVertex[0], 1.0);
			const DsVec4d p2_p = projViewMat * DsVec4d(hitVertex[1], 1.0);
			const DsVec4d p3_p = projViewMat * DsVec4d(hitVertex[2], 1.0);
			const DsVec4d ph_p = projViewMat * DsVec4d(hitPos, 1.0);

			//通常座標への変換(ProjectionSpace)
			const DsVec2d p1_n = DsVec2d(p1_p.x, p1_p.y) / p1_p.w;
			const DsVec2d p2_n = DsVec2d(p2_p.x, p2_p.y) / p2_p.w;
			const DsVec2d p3_n = DsVec2d(p3_p.x, p3_p.y) / p3_p.w;
			const DsVec2d p_n = DsVec2d(ph_p.x, ph_p.y) / ph_p.w;

			//頂点のなす三角形を点pにより3分割し、必要になる面積を計算
			double s = 0.5f * ((p2_n.x - p1_n.x) * (p3_n.y - p1_n.y) - (p2_n.y - p1_n.y) * (p3_n.x - p1_n.x));
			double s1 = 0.5f * ((p3_n.x - p_n.x) * (p1_n.y - p_n.y) - (p3_n.y - p_n.y) * (p1_n.x - p_n.x));
			double s2 = 0.5f * ((p1_n.x - p_n.x) * (p2_n.y - p_n.y) - (p1_n.y - p_n.y) * (p2_n.x - p_n.x));

			//s = DsVec2d::Cross(p2_n - p1_n, p3_n - p1_n)*0.5;
			//s1 = DsVec2d::Cross(p2_n - p1_n, p_n - p1_n)*0.5;
			//s2 = DsVec2d::Cross(p3_n - p2_n, p_n - p1_n)*0.5;

			//面積比からuvを補間
			double u = s1 / s;
			double v = s2 / s;
			double w = 1.0 / ((1.0 - u - v) * 1.0 / p1_p.w + u * 1.0 / p2_p.w + v * 1.0 / p3_p.w);
			DsVec2d uv = (uv1*(1.0 - u - v) / p1_p.w + uv2 * u / p2_p.w + uv3 * v / p3_p.w)*w;

			if ((uv.x <= 1.0) && (uv.y <= 1.0)) {
				int texU = static_cast<int>(uv.x * static_cast<double>(width));
				int texV = static_cast<int>(uv.y * static_cast<double>(height));

				if((uv.x<0.0) || (uv.y<0.0)){
					DS_LOG("");
				}
				else {

					ds_uint8 r = pImp[texV*width*4 + texU*4 + 0];
					ds_uint8 g = pImp[texV*width*4 + texU*4 + 1];
					ds_uint8 b = pImp[texV*width*4 + texU*4 + 2];
					DsVec3d col(
						static_cast<double>(r) / 255.0,
						static_cast<double>(g) / 255.0,
						static_cast<double>(b) / 255.0
					);

					DsDbgSys::GetIns().RefDrawCom().SetColor(col).DrawSphere(seg.origin + seg.dir*distance, 0.05);
				}
			}
			else {
				DS_LOG("");
			}

		}
	}

	{
		DsCollisionBuffer buff;
		std::vector < const DsModel* > tmpModel(m_drawList.begin(), m_drawList.end());
		size_t size = DsCollisionBuffer::CalcMemSize(tmpModel.data(), static_cast<int>(tmpModel.size()));
		ds_uint8* pBuf = new ds_uint8[size];
		buff.WriteBuffer(pBuf, size, tmpModel.data(), static_cast<int>(tmpModel.size()));
		delete[] pBuf;
	}


	m_pShader->SetTime(static_cast<float>(m_time));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	DsMat44f projMat = DsMat44f::Identity();
	DsMat44f modelViewMat = DsMat44f::Identity();
	glGetFloatv(GL_PROJECTION_MATRIX, projMat.mat);
	
	DsMat44f projInv = DsMat44f::Identity();
	DsInverseMatrix<4, float>(projMat.mat, projInv.mat);

	m_pShader->SetProjectionTransform(projMat.mat);
	m_pShader->SetProjectionInverseTransform(projInv.mat);

	for(const DsModel* pModel : m_drawList) 
	{
#ifndef		USE_OLD_MODEL_COOD
		//座標
		const DsMat44f modelMat = DsMathUtil::ToMat44f(DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition()));
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(modelMat.mat);
		
		glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat.mat);

		//glの行列形式で掛け算
		DsMat44f modelProjMat = DsMat44f::Identity();
		glPushMatrix();
		glLoadMatrixf(projMat.mat);
		glMultMatrixf(modelViewMat.mat);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelProjMat.mat);
		glPopMatrix();

		DsMat44f modelProjInv = DsMat44f::Identity();
		DsInverseMatrix<4, float>(modelProjMat.mat, modelProjInv.mat);

		//DsMat44f check = DsMat44f::Identity();
		//check = DsMat44f::GetTranspose(projMat) * DsMat44f::GetTranspose(modelMat);
		//check = DsMat44f::GetTranspose(check);
		//if (check.IsNearEqual(modelProjMat)){
		//	DS_LOG("");
		//}else {
		//	DS_LOG("");
		//}

		m_pShader->SetModelTransform(modelMat.mat);
		m_pShader->SetModelViewProjectionTransform(modelProjMat.mat);
		m_pShader->SetModelViewProjectionInverseTransform(modelProjInv.mat);

#endif

		//頂点法線
		const bool isUseVertexNormal = pModel->IsCreateVertexNormal();
		const DsVec3d* pVertexNormals = pModel->GetVertexNormals();

		//頂点、面
		const int fn = pModel->GetFaceNum();
		const DsVec4d* pVertex = pModel->GetVertex();
		const DsModel::Face* pFace = pModel->GetFace();

		//マテリアル
		const int mn = pModel->GetMaterialNum();
		const DsModel::Material* pMtr = pModel->GetMaterial();
		
		for (int mi = 0; mi < mn; ++mi, ++pMtr)
		{
			glMaterialfv(GL_FRONT, GL_SPECULAR, pMtr->specular.v);
			glMaterialfv(GL_FRONT, GL_AMBIENT, pMtr->ambient.v);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, pMtr->diffuse.v);
			glMaterialfv(GL_FRONT, GL_EMISSION, pMtr->emissive.v);
			//材質の色は適当
			const GLfloat col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glColor3fv(col);

			m_pShader->SetUseWaveNormalMap(pMtr->isWaveNormal);

			const int tn = pMtr->textureNum;
			const DsModel::Material::Texture* pTex = pMtr->pTexture;
			for (int ti = 0; ti < tn; ++ti, ++pTex)
			{
				DsGLActiveTexture(DS_GL_TEXTURE0);
				const GLuint albedoTexId = m_texture.GetTexId(pTex->pathAlbedo);
				glBindTexture(GL_TEXTURE_2D, albedoTexId);

				//ノーマルマップのある材質だけ有効
				const GLuint normalTexId = m_texture.GetTexId(pTex->pathNormal);
				if (m_texture.IsValidId(normalTexId)) {
					m_pShader->SetUseNormalMap(true);
					DsGLActiveTexture(DS_GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, normalTexId);
				}
				else {
					m_pShader->SetUseNormalMap(false);
				}

				const GLuint specularTexId = m_texture.GetTexId(pTex->pathSpecular);
				if (m_texture.IsValidId(specularTexId)) {
					m_pShader->SetUseSpecularMap(true);
					DsGLActiveTexture(DS_GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, specularTexId);
				}
				else {
					m_pShader->SetUseSpecularMap(false);
				}
				DsGLActiveTexture(DS_GL_TEXTURE0);

				int uvIdx = 0;
				const DsModel::Material::Texture::UV *pUV = pTex->pUV;
				for(const DsModel::Face* pFace : pTex->refGeomFaces)
				{
					if (!isUseVertexNormal)
					{
						glNormal3dv(pFace->normal.v);
					}

					glBegin(GL_POLYGON);
					const int vn = pFace->vn;
					for (int vi = 0; vi < vn; ++vi)
					{
						DS_ASSERT(uvIdx < pTex->uvNum, "uv座標のインデックオーバー");
						const DsModel::Material::Texture::UV& uv = pUV[uvIdx];
						const int vIdx = uv.vertexIdx;
						if (isUseVertexNormal)
						{
							glNormal3dv(pVertexNormals[vIdx].v);
						}

						//DsVec4f a(pVertex[vIdx], 1.0f);
						//DsVec4f aa = DsMat44f::GetTranspose(modelProjMat) * a;
						//a = DsMat44f::GetTranspose(modelProjMat) * a;
						//a /= a.w;
						//DsVec4d c = pVertex[vIdx];
						//
						////元に戻せる
						//DsVec4f b = DsMat44f::GetTranspose(projInv)*DsVec4f(a.x, a.y, a.z, 1.0f);
						//b /= b.w;
						//c = b;

						//const double thre = 0.9;
						//if( (thre < a.x) ) {
						//	a *= aa.w;
						//	DsVec4f b = DsMat44f::GetTranspose(modelProjInv)*DsVec4f(thre*aa.w, a.y, a.z, a.w);
						//	c = b;
						//}
						//else if ((-thre > a.x)) {
						//	a *= aa.w;
						//	DsVec4f b = DsMat44f::GetTranspose(modelProjInv)*DsVec4f(-thre*aa.w, a.y, a.z, a.w);
						//	c = b;
						//}
						//else if ((thre < a.y)) {
						//	a *= aa.w;
						//	DsVec4f b = DsMat44f::GetTranspose(modelProjInv)*DsVec4f(a.x, thre*aa.w, a.z, a.w);
						//	c.y = b.y;
						//}
						//else if ((-thre > a.y)) {
						//	a *= aa.w;
						//	DsVec4f b = DsMat44f::GetTranspose(modelProjInv)*DsVec4f(a.x, -thre*aa.w, a.z, a.w);
						//	c.y = b.y;
						//}

						glTexCoord2f(uv.x, uv.y);
						glVertex3dv(pVertex[vIdx].v);
						//glVertex3dv(c.v);
						++uvIdx;

						
					}
					glEnd();
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
#ifndef		USE_OLD_MODEL_COOD
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
#endif
	}

	glDisable(GL_BLEND);
}

void DsModelRender::RenderPolygon() const
{
	for(const DsModel* pModel : m_drawList)
	{
#ifndef		USE_OLD_MODEL_COOD
		const DsMat44d modelMat = DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition());
		glPushMatrix();
		glMultMatrixd(modelMat.mat);
#endif
		const int fn = pModel->GetFaceNum();
		const DsVec4d* pVertex = pModel->GetVertex();
		const DsModel::Face* pFace = pModel->GetFace();

		for (int fi = 0; fi < fn; ++fi, ++pFace)
		{
			const DsVec3d& normal = pFace->normal;
			glNormal3f(static_cast<float>(normal.x), static_cast<float>(normal.y), static_cast<float>(normal.z));
			glBegin(GL_POLYGON);
			const int* pIndex = pFace->pIndex;
			const int vn = pFace->vn;
			for (int vi = 0; vi < vn; ++vi, ++pIndex)
			{
				glVertex3dv(pVertex[(*pIndex)].v);
			}
			glEnd();
		}

#ifndef		USE_OLD_MODEL_COOD
		glPopMatrix();
#endif
	}
}

void DsModelRender::RenderNonMaterial() const
{
	for(const DsModel* pModel : m_drawList)
	{
#ifndef		USE_OLD_MODEL_COOD
		//座標
		const DsMat44d modelMat = DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition());
		glPushMatrix();
		glMultMatrixd(modelMat.mat);
#endif

		bool isUseTexture = false;
		{
			const int mn = pModel->GetMaterialNum();
			const DsModel::Material* pMtr = pModel->GetMaterial();
			for (int mi = 0; mi < mn; ++mi, ++pMtr)
			{
				const int tn = pMtr->textureNum;
				DsModel::Material::Texture* pTex = pMtr->pTexture;
				isUseTexture = (0 < tn);
				if (isUseTexture)break;
			}
		}

		//テクスチャのないやつだけ
		if (!isUseTexture)
		{
			const int fn = pModel->GetFaceNum();
			const DsVec4d* pVertex = pModel->GetVertex();
			const DsModel::Face* pFace = pModel->GetFace();

			//頂点法線
			const bool isUseVertexNormal = pModel->IsCreateVertexNormal();
			const DsVec3d* pVertexNormals = pModel->GetVertexNormals();

			for (int fi = 0; fi < fn; ++fi, ++pFace)
			{
				//材質の色は適当
				const static GLfloat col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				const static GLfloat spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 鏡面反射色
				const static GLfloat ambi[] = { 0.1f, 0.1f, 0.1f, 1.0f };	// 環境光
				glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
				glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
				glMaterialf(GL_FRONT, GL_SHININESS, 30);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
				glColor3fv(col);

				if (!isUseVertexNormal)
				{
					glNormal3dv(pFace->normal.v);
				}
				glBegin(GL_POLYGON);
				const int vn = pFace->vn;
				const int* pIndex = pFace->pIndex;
				for (int vi = 0; vi < vn; ++vi, ++pIndex)
				{
					const int vIndex = (*pIndex);
					if (isUseVertexNormal)
					{
						glNormal3dv(pVertexNormals[vIndex].v);
					}
					const DsVec4d& v = pVertex[vIndex];
					glVertex3dv(pVertex[vIndex].v);
				}
				glEnd();
			}
		}

#ifndef		USE_OLD_MODEL_COOD
		glPopMatrix();
#endif
	}
}
