#ifndef _DS_TEXTURE_H_
#define _DS_TEXTURE_H_

namespace DsLib
{
	class DsImage;
	class DsAnimModel;
}

namespace DsLib
{
	class DsTexture
	{
	private:
		//テクスチャ識別子。
		struct TexMap
		{
			enum { INVALID_ID=0 };
			TexMap()
				: pImg(0)
				, id(INVALID_ID)
				, refCounter(0)
			{}
			DsImage* pImg;
			unsigned int id;
			int refCounter;
		};

	public:
		DsTexture();
		virtual ~DsTexture();

	public:
		void Load(const DsAnimModel& model);
		void UnLoad(const DsAnimModel& model);

		void Load(const std::string& path);
		void UnLoad(const std::string& path);

		inline unsigned int GetTexId(const std::string texPath) const
		{
			auto it = m_texMap.find(texPath);
			return (m_texMap.end() != it) ? (it->second.id) : (TexMap::INVALID_ID);
		}

	private:
		std::vector<DsImage*> m_imgs;
		std::map<std::string, TexMap> m_texMap;
	};
}

#endif