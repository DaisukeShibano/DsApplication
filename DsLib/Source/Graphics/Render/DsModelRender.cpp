#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#ifndef _DS_MODEL_RENDER_H_
#include "Graphics/Render/DsModelRender.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif

using namespace DsLib;

DsModelRender::DsModelRender()
: m_drawList()
, m_texture()
{

}

DsModelRender::~DsModelRender()
{

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

void DsModelRender::RenderPolygon() const
{
	for each(const DsModel* pModel in m_drawList)
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
	for each(const DsModel* pModel in m_drawList)
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

void DsModelRender::Render() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for(const DsModel* pModel : m_drawList) 
	{
#ifndef		USE_OLD_MODEL_COOD
		//座標
		const DsMat44d modelMat = DsMat44d::GetTranspose(pModel->GetRotation(), pModel->GetPosition());
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//glMultMatrixd(modelMat.mat);

		//glActiveTexture(GL_TEXTURE7);
		//glMatrixMode(GL_TEXTURE);
		//glPushMatrix();
		//glMultMatrixd(modelMat.mat);
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
			const int tn = pMtr->textureNum;
			const DsModel::Material::Texture* pTex = pMtr->pTexture;
			for (int ti = 0; ti < tn; ++ti, ++pTex)
			{
				const GLuint texId = m_texture.GetTexId(pTex->path);
				glBindTexture(GL_TEXTURE_2D, texId);
				
				int uvIdx = 0;
				const DsModel::Material::Texture::UV *pUV = pTex->pUV;
				for(const DsModel::Face* pFace : pTex->refGeomFaces)
				{
					//材質の色は適当
					const GLfloat col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
					const GLfloat spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 鏡面反射色
					const GLfloat ambi[] = { 0.1f, 0.1f, 0.1f, 1.0f };	// 環境光
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
						DsVec3d tmp = pModel->GetRotation()*pVertex[vIdx] + pModel->GetPosition();//glMultMatrixdを使えるようにせねば
						glVertex3dv(tmp.v);

						++uvIdx;
					}
					glEnd();
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
#ifndef		USE_OLD_MODEL_COOD
		//glMatrixMode(GL_TEXTURE);
		//glPopMatrix();
		//glActiveTexture(GL_TEXTURE0);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
#endif
	}

	glDisable(GL_BLEND);
}