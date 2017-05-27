#ifndef _DS_AMIM_RES_
#define _DS_AMIM_RES_

namespace DsLib
{
	class DsAnimSkelton;
	struct DsAnimBone;
	class DsKeyFrameAnimCtrl;
	class DsAnimModel;
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
		DsAnimSkelton* CreateSkelton() const;
		DsKeyFrameAnimCtrl* CreateKeyFrameAnim() const;
		DsAnimModel* CreateAnimModel() const;
		int GetAnimNum();

	public:
		void DbgDraw(int animIdx, float dt, DsDrawCommand& com);


	private:
		void _CreateBone(DsAnimBone* pParent, const void* pParentSrcData) const;

	private:
		void* m_resTop;
		std::string m_name;
	};
}

#endif