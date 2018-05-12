#ifndef _DS_GENE_H_
#define _DS_GENE_H_

namespace DsApp
{
	class DsDecisionRootNode;
	class DsFieldChr;
	struct DsDecisionTreeData;
}

namespace DsApp
{
	class DsGene
	{
	public:
		DsGene();
		~DsGene();

	public:
		void SetNode(DsDecisionRootNode* pNode){ m_pRoot = pNode; }
		DsDecisionRootNode* GetNode() { return m_pRoot; }
		DsDecisionRootNode* GetNode() const { return m_pRoot; }
		int GetScore() const { return m_score; }
		DsDecisionTreeData* GetData(){ return m_pData; }

	public:
		void Initialize(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		bool Evaluation(double dt, double evalTime, int trialNum);

	private:
		DsDecisionRootNode* m_pRoot;
		DsDecisionTreeData* m_pData;
		int m_score;
		double m_curTime;
		int m_curTrial;
		DsFieldChr* m_pChr;
	};
}

#endif