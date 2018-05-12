#ifndef _DS_AMIM_RES_
#define _DS_AMIM_RES_

#ifndef _DS_RES_ITEM_
#include "Res/DsResItem.h"
#endif

namespace DsLib
{
	class DsAnimSkeleton;
	struct DsAnimBone;
	class DsKeyframeAnimSet;
	class DsModel;
	struct DsAnimCustomProperty;
}

namespace DsLib
{
	class DsAnimRes : public DsResItem
	{
	public:
		DsAnimRes() :DsResItem(), m_resTop(0), m_name(){}
		virtual ~DsAnimRes();

	public:
		void Initialize(const char* path, DsResource& resource);

	public:
		DsAnimSkeleton* CreateSkeleton() const;
		DsKeyframeAnimSet* CreateKeyframeAnim() const;
		DsModel* CreateAnimModel() const;
		int GetAnimNum() const;
		DsAnimCustomProperty* CustomProperty()const;

	public:
		void DbgDraw(int animIdx, float dt, DsDrawCommand& com);


	private:
		void _CreateBone(DsAnimBone* pParent, const void* pParentSrcData, std::vector<DsAnimBone*>& boneArray) const;

	private:
		void* m_resTop;
		std::string m_name;
	};
}

#endif