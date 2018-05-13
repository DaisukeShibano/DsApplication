#ifndef _DS_RESOURCE_
#define _DS_RESOURCE_

namespace DsLib
{
	class DsAnimRes;
	class DsMapRes;
	class DsHitRes;
	class DsResItem;
	class DsResItemFactory;
}

namespace DsLib
{
	class DsResource
	{
	private:
		struct KEY
		{
			KEY(std::string n, const std::type_info& t) :name(n), type(t) {};
			bool operator == (const KEY& r) const
			{
				return (type == r.type) && (name == r.name);
			}
			std::string name;
			std::type_index type;
		};

		class Hash
		{
		public:
			std::size_t operator()(const KEY& key) const {
				return key.type.hash_code() + std::hash<std::string>()(key.name);
			}
		};

		//ファイル拡張子取得
		static std::string GetExt(const std::string& path);

	public:
		DsResource();
		virtual ~DsResource();

	public:
		void Finalize();

	public:	
		template<class CLASS>
		CLASS* RegisterItem(const char* name)
		{
			const KEY key(name, typeid(CLASS));
			const auto find = m_resItems.find(key);
			if (find != m_resItems.end()){
				return dynamic_cast<CLASS*>(find->second);
			}
			else{
				CLASS* pItem = new CLASS();
				DS_ASSERT(pItem, "メモリ確保失敗");
				pItem->Ref();
				pItem->SetResName(name);
				if (pItem->Initialize(name, *this)) {
					m_resItems[key] = pItem;
				}
				else {
					delete pItem; pItem = NULL;
				}
				return pItem;
			}
		}

		template<class CLASS>
		CLASS* UnregisterItem(const std::string& name)
		{
			const KEY key(name, typeid(CLASS));
			const auto it = m_resItems.find(key);
			if (it != m_resItems.end()) {
				DsResItem* pItem = it->second;
				pItem->Unref();
				if (pItem->GetCount() <= 0) {
					m_resItems.erase(key);
					delete pItem;
				}
			}
			return NULL;
		}

		template<class CLASS>
		CLASS* UnregisterItem(const DsResItem* pResName)
		{
			if (pResName) {
				UnregisterItem<CLASS>(pResName->RefName());
			}
			return NULL;
		}

		template<class CLASS>
		const CLASS* GetItem(const char* name)const
		{
			const KEY key(name, typeid(CLASS));
			const auto find = m_resItems.find(name);
			if (find != m_resItems.end()){
				return find->second;
			}
			else{
				return NULL;
			}
		}

	private:
		std::unordered_map<KEY, DsResItem*, Hash> m_resItems;
	};
}

#endif

