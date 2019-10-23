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
		DsBdOctreeNode* next;//ループアクセス用
	};

	/*
	空間を8分割していって当たり判定総数を減らす。
	*/
	class DsBoundingOctree 
	{		
	public:
		DsBoundingOctree();
		virtual ~DsBoundingOctree();

	public:
		/*
		resolution 分割数。ノード数はresolution~3になる。2つの子をもつ木を作る関係上resolutionは2~n でなければだめ。２か４くらい
		*/
		void CreateTree(const int resolution);
		
		/*
		ツリーにアクターを登録する
		allActorMinPos 全てのアクターのもっとも小さいAABBの端座標
		allActorMaxPos 全てのアクターのもっとも大きいAABBの端座標
		*/
		void Update(DsActor** pActors, const int actNum, const DsVec3d& allActorMinPos, const DsVec3d& allActorMaxPos);

	public:
		const DsBdOctreeNode* GetRootNode() const { return m_pRoot; }
		const int GetResolutionNum() const{ return m_resolution; }

	public:
		/*
		actorを含む空間に所属する全てのactorを取得
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