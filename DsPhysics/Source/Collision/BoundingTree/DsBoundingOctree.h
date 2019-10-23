#ifndef __DS_BOUNDING_OCTREE__
#define __DS_BOUNDING_OCTREE__

namespace DsPhysics
{
	class DsActor;
}

namespace DsPhysics
{
	struct DsBdOctreeNode
	{
		enum{ CHILD_NUM = 8 };
		DsActor* actor;
		DsBdOctreeNode* parent;
		DsBdOctreeNode* child;
		DsBdOctreeNode* right;
		DsBdOctreeNode* next;//���[�v�A�N�Z�X�p
	};

	/*
	��Ԃ�8�������Ă����ē����蔻�葍�������炷�B
	*/
	class DsBoundingOctree 
	{		
	public:
		DsBoundingOctree();
		virtual ~DsBoundingOctree();

	public:
		/*
		resolution �������B�m�[�h����resolution~3�ɂȂ�B2�̎q�����؂����֌W��resolution��2~n �łȂ���΂��߁B�Q���S���炢
		*/
		void CreateTree(const int resolution);
		
		/*
		�c���[�ɃA�N�^�[��o�^����
		allActorMinPos �S�ẴA�N�^�[�̂����Ƃ�������AABB�̒[���W
		allActorMaxPos �S�ẴA�N�^�[�̂����Ƃ��傫��AABB�̒[���W
		*/
		void Update(DsActor** pActors, const int actNum, const DsVec3d& allActorMinPos, const DsVec3d& allActorMaxPos);

	public:
		const DsBdOctreeNode* GetRootNode() const { return m_pRoot; }
		const int GetResolutionNum() const{ return m_resolution; }

	public:
		/*
		actor���܂ދ�Ԃɏ�������S�Ă�actor���擾
		*/
		void GetContainAreaActors(const DsActor& actor, std::vector<const DsActor*>& outActors ) const;

	private:
		DsBdOctreeNode* m_pRoot;
		DsBdOctreeNode* m_pTerminal;
		DsBdOctreeNode** m_pAllNode;
		int m_allNodeNum;
		int m_resolution;

	private:
		DsVec3d m_allActorMaxPos;
		DsVec3d m_allActorMinPos;

	private:
		bool m_isExistActor;
	};
}

#endif