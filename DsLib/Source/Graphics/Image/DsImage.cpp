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
		ifstream  ifs(path, ios::in | ios::binary);

		if (ifs.fail())
		{
			printf("ファイルオープン失敗 %s\n", path);
			return false;
		}

		//画像サイズの取得
		unsigned short w;
		unsigned short h;
		ifs.seekg(12, std::ios::beg);
		ifs.read((char*)&w, 2);
		ifs.read((char*)&h, 2);

		const int size = w*h * 4;
		unsigned char* imgData = new unsigned char[size];

		//画像データの取得(32bit圧縮なし)
		ifs.seekg(18, std::ios::beg);
		ifs.read((char*)imgData, size);

		//BGRAなので、RGBAに変換する
		for (int i = 0; size > i; i += 4)
		{
			unsigned int temp = imgData[i];
			imgData[i] = imgData[i + 2];
			imgData[i + 2] = temp;
		}

		ifs.close();

		(*image) = imgData;
		width = w;
		height = h;

		return true;
	}

	//bmp画像の読み込み
	bool _LoadBmp(const char* path, unsigned char** image, int& width, int& height)
	{
		ifstream ifs(path, ios::in | ios::binary);
		if (ifs.fail())
		{
			printf("ファイルオープン失敗 %s\n", path);
			return false;
		}

		//画像サイズの取得
		int w = 0;
		int h = 0;
		ifs.seekg(18, std::ios::beg);
		ifs.read((char*)&w, 4);
		ifs.read((char*)&h, 4);

		//ビットカウント
		unsigned short bitCount;
		ifs.seekg(28, std::ios::beg);
		ifs.read((char*)&bitCount, 2);

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
		ifs.seekg(54, std::ios::beg);
		ifs.read((char*)imgData, fileSize);

		for (int i = 0, pix = 0; i<loadSize; i += 4, ++pix)
		{
			pTexture[i + 0] = imgData[i + 0 - pix] > 0 ? 255 : 0;
			pTexture[i + 1] = imgData[i + 1 - pix] > 0 ? 255 : 0;
			pTexture[i + 2] = imgData[i + 2 - pix] > 0 ? 255 : 0;
			//アルファ値は黒いほど高い
			unsigned int alpha = min<unsigned int>((imgData[i + 0 - pix] + imgData[i + 1 - pix] + imgData[i + 2 - pix]) / 3, 255);
			pTexture[i + 3] = alpha;
		}

		ifs.close();

		width = w;
		height = h;
		(*image) = pTexture;

		delete imgData;
		return true;
	}

}

void DsImage::Load(const char* path)
{
	m_path = path;
	const size_t strLen = m_path.length();
	const string ext = path + strLen - 4; // .***の部分を取り出す
	bool isLoad = false;

	if ((ext == ".tga") || (ext == ".tga") )
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
		DS_ASSERT(false, "DsImagの読み込みに失敗しました");
	}
}