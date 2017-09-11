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
	//glActiveTexture(GL_TEXTURE0);//����Ȃ�����
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
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//�e�N�X�N�`���J��Ԃ�
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//�e�N�X�N�`���J��Ԃ�
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
				//�����e�N�X�`���͎g����
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
			//���̃e�N�X�`�����ǂ�������Q�Ƃ���Ă��Ȃ����Ƃ��m�F���Ă���
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