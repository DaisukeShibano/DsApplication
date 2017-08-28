#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#ifndef _DS_ANIM_MODEL_RENDER_H_
#include "Graphics/Render/DsAnimModelRender.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif

using namespace DsLib;

DsAnimModelRender::DsAnimModelRender()
: m_drawList()
, m_texture()
{

}

DsAnimModelRender::~DsAnimModelRender()
{

}

void DsAnimModelRender::Register(const DsAnimModel* pModel)
{
	if (pModel)
	{
		m_drawList.push_back(pModel);

		//非同期で読みなら場所を考える
		m_texture.Load(*pModel);
	}
}

void DsAnimModelRender::UnRegister(const DsAnimModel* pModel)
{
	if (pModel)
	{
		m_texture.UnLoad(*pModel);
		m_drawList.remove(pModel);
	}
}

void DsAnimModelRender::RenderPolygon() const
{
	for each(const DsAnimModel* pModel in m_drawList)
	{
		//座標
		const DsMat44d modelMat = DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition());

		const int fn = pModel->GetFaceNum();
		const DsVec4d* pVertex = pModel->GetVertex();
		const DsAnimModel::Face* pFace = pModel->GetFace();

		for (int fi = 0; fi < fn; ++fi, ++pFace)
		{
			const DsVec3d& normal = pFace->normal;
			glNormal3f(static_cast<float>(normal.x), static_cast<float>(normal.y), static_cast<float>(normal.z));

			glPushMatrix();
#ifndef		USE_OLD_MODEL_COOD
			glMultMatrixd(modelMat.mat);
#endif
			glBegin(GL_POLYGON);
			const int* pIndex = pFace->pIndex;
			const int vn = pFace->vn;
			for (int vi = 0; vi < vn; ++vi, ++pIndex)
			{
				glVertex3dv(pVertex[(*pIndex)].v);
			}
			glEnd();
			glPopMatrix();
		}
	}
}

void DsAnimModelRender::RenderNonMaterial() const
{
	for each(const DsAnimModel* pModel in m_drawList)
	{
		//座標
		const DsMat44d modelMat = DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition());

		bool isUseTexture = false;
		{
			const int mn = pModel->GetMaterialNum();
			const DsAnimModel::Material* pMtr = pModel->GetMaterial();
			for (int mi = 0; mi < mn; ++mi, ++pMtr)
			{
				const int tn = pMtr->textureNum;
				DsAnimModel::Material::Texture* pTex = pMtr->pTexture;
				isUseTexture = (0 < tn);
				if (isUseTexture)break;
			}
		}

		//テクスチャのないやつだけ
		if (!isUseTexture)
		{
			const int fn = pModel->GetFaceNum();
			const DsVec4d* pVertex = pModel->GetVertex();
			const DsAnimModel::Face* pFace = pModel->GetFace();

			//頂点法線
			const bool isUseVertexNormal = pModel->IsCreateVertexNormal();
			const DsVec3f* pVertexNormals = pModel->GetVertexNormals();

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
				glPushMatrix();
#ifndef		USE_OLD_MODEL_COOD
				glMultMatrixd(modelMat.mat);
#endif
				glBegin(GL_POLYGON);
				const int vn = pFace->vn;
				const int* pIndex = pFace->pIndex;
				for (int vi = 0; vi < vn; ++vi, ++pIndex)
				{
					const int vIndex = (*pIndex);
					if (isUseVertexNormal)
					{
						glNormal3fv(pVertexNormals[vIndex].v);
					}
					const DsVec4d& v = pVertex[vIndex];
					glVertex3dv(pVertex[vIndex].v);
				}
				glEnd();
				glPopMatrix();
			}
		}
	}
}

void DsAnimModelRender::Render() const
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for each(const DsAnimModel* pModel in m_drawList) 
	{
		//座標
		const DsMat44d modelMat = DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition());

		//頂点法線
		const bool isUseVertexNormal = pModel->IsCreateVertexNormal();
		const DsVec3f* pVertexNormals = pModel->GetVertexNormals();

		//頂点、面
		const int fn = pModel->GetFaceNum();
		const DsVec4d* pVertex = pModel->GetVertex();
		const DsAnimModel::Face* pFace = pModel->GetFace();
		
		//マテリアル
		const int mn = pModel->GetMaterialNum();
		const DsAnimModel::Material* pMtr = pModel->GetMaterial();
		
		for (int mi = 0; mi < mn; ++mi, ++pMtr)
		{
			const int tn = pMtr->textureNum;
			const DsAnimModel::Material::Texture* pTex = pMtr->pTexture;
			for (int ti = 0; ti < tn; ++ti, ++pTex)
			{
				const GLuint texId = m_texture.GetTexId(pTex->path);
				glBindTexture(GL_TEXTURE_2D, texId);
				
				int uvIdx = 0;
				const DsAnimModel::Material::Texture::UV *pUV = pTex->pUV;
				for each(const DsAnimModel::Face* pFace in pTex->refGeomFaces)
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

					glPushMatrix();
#ifndef		USE_OLD_MODEL_COOD
					glMultMatrixd(modelMat.mat);
#endif
					glBegin(GL_POLYGON);
					const int vn = pFace->vn;
					for (int vi = 0; vi < vn; ++vi)
					{
						DS_ASSERT(uvIdx < pTex->uvNum, "uv座標のインデックオーバー");
						const DsAnimModel::Material::Texture::UV& uv = pUV[uvIdx];
						const int vIdx = uv.vertexIdx;
						if (isUseVertexNormal)
						{
							glNormal3fv(pVertexNormals[vIdx].v);
						}
						glTexCoord2f(uv.x, uv.y);
						glVertex3dv(pVertex[vIdx].v);

						++uvIdx;
					}
					glEnd();
					glPopMatrix();
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}