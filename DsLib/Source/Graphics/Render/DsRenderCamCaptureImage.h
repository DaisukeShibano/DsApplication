#ifndef _DS_RENDER_CAM_CAPTURE_IMAGE_H_
#define _DS_RENDER_CAM_CAPTURE_IMAGE_H_


namespace DsLib
{
	class DsCamera;
}

namespace DsLib
{
	/*
		•ÊŽ‹“_‚ÌƒŒƒ“ƒ_ƒŠƒ“ƒOŒ‹‰Ê‚ð‰æ‘œ‚Æ‚µ‚Ä•Û‘¶‚·‚é
	*/
	class DsRenderCamCaptureImage
	{
	public:
		static DsRenderCamCaptureImage* Create(const DsCamera& cam, int imageW, int imageH);

	public:
		virtual void BeginCapture() = 0;
		virtual void EndCapture() = 0;

		unsigned char* GetImage() { return m_pImage; }
		const unsigned char* GetImage() const { return m_pImage; }
		int GetImageSize() const { return m_imageSize; }
		int GetWidth() const { return m_imageW; }
		int GetHeight() const { return m_imageH; }

	protected:
		DsRenderCamCaptureImage(const DsCamera& cam, int imageW, int imageH);

	protected:
		virtual ~DsRenderCamCaptureImage();

	protected:
		const DsCamera& m_cam;
		unsigned char* m_pImage;
		int m_imageSize;
		int m_imageW;
		int m_imageH;
	};

}

#endif