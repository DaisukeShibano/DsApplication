#include "DsPch.h"
#include <fstream>
#include <gl/GL.h>
#include "Graphics/Render/Tool/Cloud/DsToolCloud.h"
#include "Graphics/Command/DsDrawCommand.h"
#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Render/Tool/Cloud/DsCloudEmitter.h"

using namespace DsLib;
using namespace std;

namespace
{
	class DsToolCloudImp : public DsToolCloud
	{
	public:
		DsToolCloudImp(DsDrawCommand& com, const DsCamera& rendCam);
		virtual ~DsToolCloudImp();

	public:
		virtual void Update(const float dt) override;

	private:
		void _Initialize();
		void _LoadTga();
		void _LoadBmp();

	private:
		std::string m_texPath;
		size_t m_texWidth;
		size_t m_texHeight;
		GLuint m_texId;
		unsigned char* m_pTexture;
		DsDrawCommand& m_com;
		const DsCamera& m_rendCam;
		DsCloudEmitter m_emitter;

	};

	DsToolCloudImp::DsToolCloudImp(DsDrawCommand& com, const DsCamera& rendCam)
	: m_texPath()
	, m_texWidth(0)
	, m_texHeight(0)
	, m_texId(0)
	, m_pTexture(NULL)
	, m_com(com)
	, m_rendCam(rendCam)
	, m_emitter(rendCam)
	{
		_Initialize();	
	}

	//virtual
	DsToolCloudImp::~DsToolCloudImp()
	{
		glDeleteTextures(1, &m_texId);
		delete m_pTexture; m_pTexture = NULL;
	}

	void DsToolCloudImp::_Initialize()
	{
//#ifdef _DEBUG
		m_texPath = __FILE__;
//#else
		m_texPath = "C:\\Users\\SIVA2\\Documents\\Visual Studio 2013\\Projects\\DsLib\\DsLib\\Source\\Graphics\\Render\\Tool\\Cloud\\DsToolCloud.cpp";
//#endif
		auto pos = m_texPath.find("DsToolCloud.cpp");
		m_texPath = m_texPath.erase(pos);
		m_texPath.append("..\\Cloud\\Texture\\cloud.bmp");

		_LoadBmp();
		glGenTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_2D, m_texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//テクスクチャ繰り返し
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//テクスクチャ繰り返し
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(m_texWidth), static_cast<GLsizei>(m_texHeight),
			0, GL_RGBA, GL_UNSIGNED_BYTE, m_pTexture);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_emitter.Initialize(m_texId);
		m_emitter.Enable();
	}

	void DsToolCloudImp::_LoadTga()
	{
		ifstream  ifs(m_texPath.c_str(), ios::in | ios::binary);
		if (ifs.fail())return;

		//画像サイズの取得
		unsigned short w;
		unsigned short h;
		ifs.seekg(12, std::ios::beg);
		ifs.read((char*)&w, 2);
		ifs.read((char*)&h, 2);

		const int size = w*h * 4;
		unsigned char* imgData = new unsigned char[size];
		if (NULL == imgData)return;

		//画像データの取得(32bit圧縮なし)
		ifs.seekg(18, std::ios::beg);
		ifs.read((char*)imgData, size);

		//BGRAなので、RGBAに変換する
		for (int i = 0; size > i; i += 4)
		{
			unsigned char temp = imgData[i];
			imgData[i] = imgData[i + 2];
			imgData[i + 2] = temp;
			//アルファ値は黒いほど高い
			unsigned int alpha = min<unsigned int>((imgData[i] + imgData[i + 1] + imgData[i + 2]) / 3, 255);
			imgData[i + 3] = static_cast<unsigned char>(alpha);
		}

		ifs.close();

		m_pTexture = imgData;
		m_texWidth = w;
		m_texHeight = h;
	}

	void DsToolCloudImp::_LoadBmp()
	{
		ifstream  ifs(m_texPath.c_str(), ios::in | ios::binary);
		if (ifs.fail()) return;

		//画像サイズの取得
		int width = 0;
		int height=0;
		ifs.seekg(18, std::ios::beg);
		ifs.read((char*)&width, 4);
		ifs.read((char*)&height, 4);

		//ビットカウント
		unsigned short bitCount;
		ifs.seekg(28, std::ios::beg);
		ifs.read((char*)&bitCount, 2);

		//カラーテーブルには対応しない
		if (bitCount <= 8) return;

		const int fileSize = width*height * 3;
		const int loadSize = width*height * 4;
		unsigned char *imgData = new unsigned char[fileSize];
		m_pTexture = new unsigned char[loadSize];

		if ((NULL == imgData) || (NULL == m_pTexture))
		{
			delete imgData;
			delete m_pTexture; m_pTexture = NULL;
			return;
		}

		//画像データの取得
		ifs.seekg(54, std::ios::beg);
		ifs.read((char*)imgData, fileSize);

		for (int i = 0, pix = 0; i<loadSize; i += 4, ++pix)
		{
			m_pTexture[i + 0] = imgData[i + 0 - pix] > 0 ? 255 : 0;
			m_pTexture[i + 1] = imgData[i + 1 - pix] > 0 ? 255 : 0;
			m_pTexture[i + 2] = imgData[i + 2 - pix] > 0 ? 255 : 0;
			//アルファ値は黒いほど高い
			unsigned int alpha = min<unsigned int>((imgData[i + 0 - pix] + imgData[i + 1 - pix] + imgData[i + 2 - pix]) / 3, 255);
			m_pTexture[i + 3] = alpha;
		}

		ifs.close();
		
		m_texWidth = width;
		m_texHeight = height;
		
		delete imgData;
	}

	//virtual
	void DsToolCloudImp::Update(const float dt)
	{
		m_emitter.Update(dt);
	}
}

DsToolCloud::DsToolCloud()
{
}

DsToolCloud::~DsToolCloud()
{
}

//static
DsToolCloud& DsToolCloud::Create(DsDrawCommand& com, const DsCamera& rendCam)
{
	DsToolCloud* ret = new DsToolCloudImp(com, rendCam);
	if (NULL == ret)
	{
		DsError::Panic("メモリ確保失敗\n");
	}
	return *ret;
}
