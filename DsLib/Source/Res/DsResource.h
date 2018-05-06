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
		DsResItem() :m_resPath(), m_count(0) {}
		virtual void Initialize(const char* path) = 0;

	public:
		void Ref() { ++m_count; }
		void Unref() { --m_count; }
		int GetCount()const { return m_count; }
		const std::string& RefPath()const { return m_resPath; }
		void SetPath(const char* path) { m_resPath = path; }

	private:
		std::string m_resPath;
		int m_count;
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
		//todo:Unregister用意する
		DsAnimRes* RegisterAnimRes(const char* name);
		const DsAnimRes* GetAnimRes(const char* name) const;

		DsMapRes* RegisterMapRes(const char* name);
		const DsMapRes* GetMapRes(const char* name) const;

		DsHitRes* RegisterHitRes(const char* name);
		const DsHitRes* GetHitRes(const char* name) const;

		DsResItem* RegisterItem(const char* name, const DsResItemFactory& factory);
		//開放されるのでpItemはもう使っちゃいけない
		void UnregisterItem(DsResItem* pItem);
		const DsResItem* GetItem(const char* name)const;

	private:
		std::map<std::string, DsAnimRes*> m_animSet;
		std::map<std::string, DsMapRes*> m_mapSet;
		std::map<std::string, DsHitRes*> m_hitSet;
		std::map<std::string, DsResItem* > m_resItemSet;
	};
}

#endif

