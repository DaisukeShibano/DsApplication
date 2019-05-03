#ifndef _DS_ITEM_BOX_COMPONENT_
#define _DS_ITEM_BOX_COMPONENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsItemBoxComponent : public DsComponent
	{
	public:
		struct ITEM
		{
			ITEM(int id) :itemId(id) {}
			int itemId;
		};

	public:
		DsItemBoxComponent();
		virtual ~DsItemBoxComponent();

	public:
		virtual bool Update(COMPONENT_UPDATE_RESULT& outResult, const COMPONENT_UPDATE_ARG& arg) override { return true; };

	public:
		void AddItem(int itemId);
		const DsItemBoxComponent::ITEM* GetItem(int index)const;

		void AddWep(int itemId);
		const DsItemBoxComponent::ITEM* GetWep(int index)const;

	private:
		std::vector<ITEM> m_items;
		std::vector<ITEM> m_wepItems;

	};


	typedef DsItemBoxComponent DsItemBox;

}


#endif