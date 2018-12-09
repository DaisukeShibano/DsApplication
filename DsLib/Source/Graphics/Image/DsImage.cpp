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
	//Tga画像の読み込み
	bool _LoadTga(const char* path, unsigned char** image, int& width, int& height)
	{
		DsFile ifs(path, ios::in | ios::binary);

		if (ifs.IsFail())
		{
			return false;
		}

		//画像サイズの取得
		unsigned short w;
		unsigned short h;
		ifs.Seekg(12, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)&w, 2);
		ifs.Read((ds_uint8*)&h, 2);

		const int size = w*h * 4;
		unsigned char* imgData = new unsigned char[size];

		//画像データの取得(32bit圧縮なし)
		ifs.Seekg(18, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)imgData, size);

		//BGRAなので、RGBAに変換する
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

	//bmp画像の読み込み
	bool _LoadBmp(const char* path, unsigned char** image, int& width, int& height)
	{
		DsFile ifs(path, ios::in | ios::binary);
		if (ifs.IsFail())
		{
			return false;
		}

		//画像サイズの取得
		int w = 0;
		int h = 0;
		ifs.Seekg(18, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)&w, 4);
		ifs.Read((ds_uint8*)&h, 4);

		//ビットカウント
		unsigned short bitCount;
		ifs.Seekg(28, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)&bitCount, 2);

		//カラーテーブルには対応しない
		if (bitCount <= 8)
		{
			printf("カラーテーブルには対応 %s\n", path);
			return false;
		}

		const int fileSize = w*h * 3;
		const int loadSize = w*h * 4;
		unsigned char* imgData = new unsigned char[fileSize];
		unsigned char* pTexture = new unsigned char[loadSize];

		//画像データの取得
		ifs.Seekg(54, DsFile::SEEK_TYPE::BEG);
		ifs.Read((ds_uint8*)imgData, fileSize);

		for (int i = 0, pix = 0; i<loadSize; i += 4, ++pix)
		{
			pTexture[i + 0] = imgData[i + 0 - pix] > 0 ? 255 : 0;
			pTexture[i + 1] = imgData[i + 1 - pix] > 0 ? 255 : 0;
			pTexture[i + 2] = imgData[i + 2 - pix] > 0 ? 255 : 0;
			//アルファ値は黒いほど高い
			unsigned int alpha = min<unsigned int>((imgData[i + 0 - pix] + imgData[i + 1 - pix] + imgData[i + 2 - pix]) / 3, 255);
			pTexture[i + 3] = alpha;
		}

		width = w;
		height = h;
		(*image) = pTexture;

		delete imgData;
		return true;
	}

	//bmp画像の読み込み
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
	const string ext = path + strLen - 4; // .***の部分を取り出す
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
		printf("非対応の画像が読まれました %s\n", path);
	}

	if (!isLoad)
	{
		//テクスチャは全部作らないのでなしは許容
	}
}