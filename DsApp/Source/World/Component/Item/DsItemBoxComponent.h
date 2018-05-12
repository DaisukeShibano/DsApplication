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
			ITEM(const std::string& _dispName, const std::string& _resName) :dispName(_dispName), resName(_resName){}
			std::string dispName;
			std::string resName;
		};

	public:
		DsItemBoxComponent();
		virtual ~DsItemBoxComponent();

	public:
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) override { return true; };

	public:
		void AddWep(const std::string& dispName, const std::string& resName);
		const DsItemBoxComponent::ITEM* GetWepItem(int index)const;

	private:
		std::vector<ITEM> m_wepItems;
	};


	typedef DsItemBoxComponent DsItemBox;

}


#endif