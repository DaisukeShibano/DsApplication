#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#ifndef _DS_TEXTURE_H_
#include "Graphics/Render/DsTexture.h"
#endif
#ifndef _DS_IMAGE_H_
#include "Graphics/Image/DsImage.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif

using namespace DsLib;
using namespace std;

DsTexture::DsTexture()
	: m_imgs()
	, m_texMap()
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

void DsTexture::Load(const DsModel& model)
{
	const DsModel::Material* pMaterial = model.GetMaterial();
	const int mn = model.GetMaterialNum();
	for (int mi = 0; mi < mn; ++mi)
	{
		const DsModel::Material::Texture* pTexture = pMaterial[mi].pTexture;
		const int tn = pMaterial[mi].textureNum;
		for (int ti = 0; ti < tn; ++ti)
		{
			Load(pTexture[ti].pathAlbedo);
			Load(pTexture[ti].pathNormal);
		}
	}
}

void DsTexture::UnLoad(const DsModel& model)
{
	const DsModel::Material* pMaterial = model.GetMaterial();
	const int mn = model.GetMaterialNum();
	for (int mi = 0; mi < mn; ++mi)
	{
		const DsModel::Material::Texture* pTexture = pMaterial[mi].pTexture;
		const int tn = pMaterial[mi].textureNum;
		for (int ti = 0; ti < tn; ++ti)
		{
			UnLoad(pTexture[ti].pathAlbedo);
			UnLoad(pTexture[ti].pathNormal);
		}
	}
}

void DsTexture::_RegisterTexture(DsImage* img, std::string mapKey, const unsigned char* pData, int width, int height)
{
	GLuint texName;
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//テクスクチャ繰り返し
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//テクスクチャ繰り返し
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
	glBindTexture(GL_TEXTURE_2D, 0);

	TexMap map;
	map.pImg = img;
	map.id = texName;
	m_texMap[mapKey] = map;
	m_texMap[mapKey].refCounter = 1;
}
void DsTexture::Load(const std::string& path)
{
	if (m_texMap.end() == m_texMap.find(path))
	{
		DsImage* pImg = new DsImage;
		pImg->Load(path.c_str());
		m_imgs.push_back(pImg);
		if (pImg->GetData()) {
			_RegisterTexture(pImg, path, pImg->GetData(), pImg->GetWidth(), pImg->GetHeight());
		}
		else 
		{//ダミー
			//今のところ出番なし
			//std::string mapKey = _DummyKey();
			//if (m_texMap.end() == m_texMap.find(mapKey))
			//{
			//	static unsigned char dummy[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
			//	_RegisterTexture(pImg, mapKey, dummy, 1, 1);
			//}
			//else {
			//	m_texMap[mapKey].refCounter += 1;
			//}
		}
	}
	else
	{
		//同じテクスチャは使い回し
		m_texMap[path].refCounter += 1;
	}
}

void DsTexture::UnLoad(const std::string& path)
{
	//このテクスチャがどこからも参照されていないことを確認してけす
	auto it = m_texMap.find(path);
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