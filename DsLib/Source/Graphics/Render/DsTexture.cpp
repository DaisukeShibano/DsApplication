#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#ifndef _DS_TEXTURE_H_
#include "Graphics/Render/DsTexture.h"
#endif
#ifndef _DS_IMAGE_H_
#include "Graphics/Image/DsImage.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif

using namespace DsLib;
using namespace std;

DsTexture::DsTexture()
	: m_imgs()
{

}
DsTexture::~DsTexture()
{
	for each(DsImage* pImg in m_imgs)
	{
		delete pImg;
	}
	m_imgs.clear();
}

void DsTexture::Load(const DsAnimModel& model)
{
	//glActiveTexture(GL_TEXTURE0);//いらないかも
	const DsAnimModel::Material* pMaterial = model.GetMaterial();
	const int mn = model.GetMaterialNum();
	for (int mi = 0; mi < mn; ++mi)
	{
		const DsAnimModel::Material::Texture* pTexture = pMaterial[mi].pTexture;
		const int tn = pMaterial[mi].textureNum;
		for (int ti = 0; ti < tn; ++ti)
		{
			if (m_texMap.end() == m_texMap.find(pTexture[ti].path) )
			{
				DsImage* img = new DsImage;
				img->Load(pTexture[ti].path.c_str());
				m_imgs.push_back(img);
				
				GLuint texName;
				glGenTextures(1, &texName);
				glBindTexture(GL_TEXTURE_2D, texName);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//テクスクチャ繰り返し
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//テクスクチャ繰り返し
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA8, img->GetWidth(), img->GetHeight(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, img->GetData());
				glBindTexture(GL_TEXTURE_2D, 0);

				TexMap map;
				map.pImg = img;
				map.id = texName;
				m_texMap[pTexture[ti].path] = map;
			}
			else
			{
				//同じテクスチャは使い回し
				m_texMap[pTexture[ti].path].refCounter += 1;
			}
		}
	}
}

void DsTexture::UnLoad(const DsAnimModel& model)
{
	const DsAnimModel::Material* pMaterial = model.GetMaterial();
	const int mn = model.GetMaterialNum();
	for (int mi = 0; mi < mn; ++mi)
	{
		const DsAnimModel::Material::Texture* pTexture = pMaterial[mi].pTexture;
		const int tn = pMaterial[mi].textureNum;
		for (int ti = 0; ti < tn; ++ti)
		{
			//このテクスチャがどこからも参照されていないことを確認してけす
			auto it = m_texMap.find(pTexture[ti].path);
			if (m_texMap.end() != it)
			{
				it->second.refCounter -= 1;
				if (it->second.refCounter <= 0)
				{
					glDeleteTextures(1, &it->second.id);
					m_texMap.erase(it);
				}
			}
		}
	}
}