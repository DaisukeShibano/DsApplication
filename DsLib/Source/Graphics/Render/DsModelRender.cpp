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
	m_pShader->SetTime(static_cast<float>(m_time));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for(const DsModel* pModel : m_drawList) 
	{
#ifndef		USE_OLD_MODEL_COOD
		//座標
		const DsMat44f modelMat = DsMathUtil::ToMat44f(DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition()));
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(modelMat.mat);
		m_pShader->SetDrawModelTransform(modelMat.mat);
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
						glTexCoord2f(uv.x, uv.y);
						glVertex3dv(pVertex[vIdx].v);
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
