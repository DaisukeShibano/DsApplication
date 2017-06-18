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
			, worldPose({ 0 })
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
		DsMat44d worldPose;
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
		void UpdateWorldPose();

	public:
		std::vector<DsAnimBone*>& RefRootBone() { return  m_pRootBone; }
		const std::vector<DsAnimBone*>& RefRootBone() const{ return  m_pRootBone; }
		std::vector<DsAnimBone*>& RefBoneArray() { return  m_boneArray; }
		const std::vector<DsAnimBone*>& RefBoneArray() const { return  m_boneArray; }
		const DsVec3d& GetRootPos() const { return m_rootPos; }
		void SetRootPos(const DsVec3d& p){ m_rootPos = p; }
		const DsMat33d& GetRootRot() const { return m_rootRot; }
		void SetRootRot(const DsMat33d& r){ m_rootRot = r; }

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