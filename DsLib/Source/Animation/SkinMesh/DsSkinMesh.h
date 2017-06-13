#ifndef _DS_SKINMESH_
#define _DS_SKINMESH_

namespace DsLib
{
	class DsKeyFrameAnimCtrl;
	class DsAnimSkeleton;
	class DsAnimModel;
}

namespace DsLib
{
	class DsSkinMesh
	{
	public:
		DsSkinMesh(const DsAnimModel& model);
		virtual ~DsSkinMesh();

	public:
		void Initialize();

		void ApplySkeleton(const DsAnimSkeleton& skeleton);
		DsAnimModel* GetSkinMeshModel() { return m_pModel; }

	public:
		void DbgDraw() const;

	public:
		const DsAnimModel& m_srcModel;//•ÏŒ`‘O
		DsAnimModel* m_pModel;//•ÏŒ`Œã
	};
}

#endif