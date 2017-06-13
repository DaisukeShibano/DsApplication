#ifndef _DS_AMIM_RES_
#define _DS_AMIM_RES_

namespace DsLib
{
	class DsAnimSkeleton;
	struct DsAnimBone;
	class DsKeyFrameAnimCtrl;
	class DsAnimModel;
	class DsAnimCustomProperty;
}

namespace DsLib
{
	class DsAnimRes
	{
	public:
		DsAnimRes() :m_resTop(0), m_name(){}
		virtual ~DsAnimRes();

		void Initialize(const char* path);
		const std::string& RefName() const { return m_name; }

		//既存のアニメリソースから生成される。DsAnimResに新しくデータが追加される訳ではない。
		DsAnimSkeleton* CreateSkeleton() const;
		DsKeyFrameAnimCtrl* CreateKeyFrameAnim() const;
		DsAnimModel* CreateAnimModel() const;
		int GetAnimNum() const;
		DsAnimCustomProperty* CreateRagdollInfo()const;

	public:
		void DbgDraw(int animIdx, float dt, DsDrawCommand& com);


	private:
		void _CreateBone(DsAnimBone* pParent, const void* pParentSrcData, std::vector<DsAnimBone*> boneArray) const;

	private:
		void* m_resTop;
		std::string m_name;
	};
}

#endif