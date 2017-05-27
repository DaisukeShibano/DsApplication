#ifndef _DS_RESOURCE_
#define _DS_RESOURCE_

namespace DsLib
{
	class DsAnimRes;
	class DsMapRes;
	class DsHitRes;
}

namespace DsLib
{
	class DsResource
	{
	public:
		static DsResource& GetIns()
		{
			static DsResource ins;
			return ins;
		}

		//�t�@�C���g���q�擾
		static std::string GetExt(const std::string& path);

	public:
		DsResource();
		virtual ~DsResource();

	public:
		void Finalize();

	public:
		DsAnimRes* RegisterAnimRes(const char* name);
		const DsAnimRes* GetAnimRes(const char* name) const;

		DsMapRes* RegisterMapRes(const char* name);
		const DsMapRes* GetMapRes(const char* name) const;

		DsHitRes* RegisterHitRes(const char* name);
		const DsHitRes* GetHitRes(const char* name) const;

	private:
		std::map<std::string, DsAnimRes*> m_animSet;
		std::map<std::string, DsMapRes*> m_mapSet;
		std::map<std::string, DsHitRes*> m_hitSet;
	};
}

#endif

