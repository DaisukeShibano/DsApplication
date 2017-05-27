#ifndef _DS_ANIM_SET_H_
#define _DS_ANIM_SET_H_

namespace DsLib
{
	class DsAnimRes;
};

namespace DsLib
{
	class DsAnimSet
	{
	public:
		struct InitInfo
		{
			const char* dataPath;
		};

	public:
		DsAnimSet();
		virtual ~DsAnimSet();

	public:
		void Initialize(const InitInfo& initInfo );
		const DsAnimRes* GetAnimRes(const char* name) const;

	private:
		std::vector<DsAnimRes*> m_res;

	};
}


#endif