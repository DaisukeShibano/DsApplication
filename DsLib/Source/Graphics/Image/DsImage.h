#ifndef _DS_IMAGE_H_
#define _DS_IMAGE_H_

namespace DsLib
{
	class DsImage
	{
	public:
		DsImage();
		virtual ~DsImage();

	public:
		void Load(const char* path);
		const std::string& GetPath() const { return m_path; }
		const int GetWidth() const { return m_imgWidth; }
		const int GetHeight() const { return m_imgHeight; }
		const unsigned char* GetData() const { return m_pImgTop; }

	private:
		int m_imgSize;
		unsigned char* m_pImgTop;
		int m_imgWidth;
		int m_imgHeight;
		std::string m_path;
	};
}

#endif