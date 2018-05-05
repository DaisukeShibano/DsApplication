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
	class DsResItem
	{
	public:
		virtual void Initialize(const char* path) = 0;
	};
	class DsResItemFactory
	{
	public:
		virtual DsResItem* CreateIns() const= 0;
	};





	class DsResource
	{
	public:
		static DsResource& GetIns()
		{
			static DsResource ins;
			return ins;
		}

		//ファイル拡張子取得
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

		DsResItem* RegisterItem(const char* name, const DsResItemFactory& factory);
		const DsResItem* GetItem(const char* name)const;

	private:
		std::map<std::string, DsAnimRes*> m_animSet;
		std::map<std::string, DsMapRes*> m_mapSet;
		std::map<std::string, DsHitRes*> m_hitSet;
		std::map<std::string, DsResItem* > m_resItemSet;
	};
}

#endif

