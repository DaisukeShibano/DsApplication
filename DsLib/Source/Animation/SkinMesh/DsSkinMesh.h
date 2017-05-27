#ifndef _DS_SKINMESH_
#define _DS_SKINMESH_

namespace DsLib
{
	class DsKeyFrameAnimCtrl;
	class DsAnimSkelton;
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

		void ApplySkelton(const DsAnimSkelton& skelton);
		DsAnimModel* GetSkinMeshModel() { return m_pModel; }

	public:
		void DbgDraw() const;

	public:
		const DsAnimModel& m_srcModel;//変形前
		DsAnimModel* m_pModel;//変形後
	};
}

#endif