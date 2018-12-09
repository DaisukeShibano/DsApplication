#include "DsPch.h"
#ifndef _DS_IMAGE_H_
#include "Graphics/Image/DsImage.h"
#endif

using namespace DsLib;
using namespace std;

DsImage::DsImage()
	: m_imgSize(0)
	, m_pImgTop(NULL)
	, m_imgWidth(0)
	, m_imgHeight(0)
	, m_path()
{
}

DsImage::~DsImage()
{
	delete[] m_pImgTop;
	m_pImgTop = NULL;
}

namespace
{
	//Tga�摜�̓ǂݍ���
	bool _LoadTga(const char* path, unsigned char** image, int& width, int& height)
	{
		DsFile ifs(path, ios::in | ios::binary);

		if (ifs.IsFail())
		{
			return false;
		}

		//�摜�T�C�Y�̎擾
		unsigned short w;
		unsigned short h;
		ifs.Seekg(12, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)&w, 2);
		ifs.Read((ds_uint8*)&h, 2);

		const int size = w*h * 4;
		unsigned char* imgData = new unsigned char[size];

		//�摜�f�[�^�̎擾(32bit���k�Ȃ�)
		ifs.Seekg(18, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)imgData, size);

		//BGRA�Ȃ̂ŁARGBA�ɕϊ�����
		for (int i = 0; size > i; i += 4)
		{
			unsigned int temp = imgData[i];
			imgData[i] = imgData[i + 2];
			imgData[i + 2] = temp;
		}

		(*image) = imgData;
		width = w;
		height = h;

		return true;
	}

	//bmp�摜�̓ǂݍ���
	bool _LoadBmp(const char* path, unsigned char** image, int& width, int& height)
	{
		DsFile ifs(path, ios::in | ios::binary);
		if (ifs.IsFail())
		{
			return false;
		}

		//�摜�T�C�Y�̎擾
		int w = 0;
		int h = 0;
		ifs.Seekg(18, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)&w, 4);
		ifs.Read((ds_uint8*)&h, 4);

		//�r�b�g�J�E���g
		unsigned short bitCount;
		ifs.Seekg(28, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)&bitCount, 2);

		//�J���[�e�[�u���ɂ͑Ή����Ȃ�
		if (bitCount <= 8)
		{
			printf("�J���[�e�[�u���ɂ͑Ή� %s\n", path);
			return false;
		}

		const int fileSize = w*h * 3;
		const int loadSize = w*h * 4;
		unsigned char* imgData = new unsigned char[fileSize];
		unsigned char* pTexture = new unsigned char[loadSize];

		//�摜�f�[�^�̎擾
		ifs.Seekg(54, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)imgData, fileSize);

		for (int i = 0, pix = 0; i<loadSize; i += 4, ++pix)
		{
			pTexture[i + 0] = imgData[i + 0 - pix] > 0 ? 255 : 0;
			pTexture[i + 1] = imgData[i + 1 - pix] > 0 ? 255 : 0;
			pTexture[i + 2] = imgData[i + 2 - pix] > 0 ? 255 : 0;
			//�A���t�@�l�͍����قǍ���
			unsigned int alpha = min<unsigned int>((imgData[i + 0 - pix] + imgData[i + 1 - pix] + imgData[i + 2 - pix]) / 3, 255);
			pTexture[i + 3] = alpha;
		}

		width = w;
		height = h;
		(*image) = pTexture;

		delete imgData;
		return true;
	}

	//bmp�摜�̓ǂݍ���
	void _DummyImage(unsigned char** image, int& width, int& height)
	{
		static unsigned char dummy[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
		*image = dummy;
		width = 1;
		height = 1;
	}

}

void DsImage::Load(const char* path)
{
	m_path = path;
	const size_t strLen = m_path.length();
	const string ext = path + strLen - 4; // .***�̕��������o��
	bool isLoad = false;

	if ((ext == ".tga") || (ext == ".TGA") )
	{
		isLoad = _LoadTga(path, &m_pImgTop, m_imgWidth, m_imgHeight);
	}

	else if ((ext == ".bmp") || (ext == ".BMP"))
	{
		isLoad = _LoadBmp(path, &m_pImgTop, m_imgWidth, m_imgHeight);
	}
	else
	{
		printf("��Ή��̉摜���ǂ܂�܂��� %s\n", path);
	}

	if (!isLoad)
	{
		//�e�N�X�`���͑S�����Ȃ��̂łȂ��͋��e
	}
}