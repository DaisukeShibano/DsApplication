#ifndef _DS_AMIM_SKELETON_
#define _DS_AMIM_SKELETON_

namespace DsLib
{
	struct DsAnimBone
	{
		DsAnimBone()
			: pParent(0)
			, child()
			, name()
			, localPose({ 0 })
			, modelPose({ 0 })
			, initWorldPose({ 0 })
			, initParentToChildPose({ 0 })
			, arrayIdx(-1)
			, vIndexNum(0)
			, pIndex(0)
			, pWeight(0)
		{}
		~DsAnimBone()
		{
			delete[] pIndex; pIndex = NULL;
			delete[] pWeight; pWeight = NULL;

			for each(DsAnimBone* pBone in child)
			{
				delete pBone;
			}
		}

		DsAnimBone* pParent;
		std::vector<DsAnimBone*> child;
		std::string name;
		DsMat44d localPose;
		DsMat44d modelPose;
		DsMat44d initParentToChildPose;//親→自分への変換行列
		DsMat44d initWorldPose;
		int arrayIdx;

		int vIndexNum;
		int *pIndex;
		double* pWeight;
	};


	class DsAnimSkeleton
	{
	public:
		DsAnimSkeleton(const std::vector<DsAnimBone*>& pRootBone, const std::vector<DsAnimBone*>& boneArray);
		virtual ~DsAnimSkeleton();

	public:
		void UpdatePose();

	public:
		std::vector<DsAnimBone*>& RefRootBone() { return  m_pRootBone; }
		const std::vector<DsAnimBone*>& RefRootBone() const{ return  m_pRootBone; }
		std::vector<DsAnimBone*>& RefBoneArray() { return  m_boneArray; }
		const std::vector<DsAnimBone*>& RefBoneArray() const { return  m_boneArray; }
		
	public://削除予定
		DsVec3d GetRootPos()const { return m_rootPos; }
		void SetRootPos(DsVec3d pos) { m_rootPos = pos; }
		DsMat33d GetRootRot()const { return m_rootRot; }
		void SetRootRot(DsMat33d rot) { m_rootRot = rot; }

		template<typename FUNC>
		void GetAllBone(FUNC func)
		{
			for each(DsAnimBone* bone in m_pRootBone)
			{
				_GetAllBone(bone, func);
			}
		}
	private:
		template<typename FUNC>
		void _GetAllBone(DsAnimBone* b, FUNC func)
		{
			func(b);
			for each(DsAnimBone* child in b->child)
			{
				_GetAllBone(child, func);
			}
		}

	private:
		std::vector<DsAnimBone*> m_pRootBone;
		std::vector<DsAnimBone*> m_boneArray;//配列アクセス用。最初からこっち使う方がよかったかも。
		DsVec3d m_rootPos;
		DsMat33d m_rootRot;

	public:
		void DbgDraw() const;
	};
}

#endif