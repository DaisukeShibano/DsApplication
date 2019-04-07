#include "DsAppPch.h"
#ifndef _DS_GP_H_
#include "Evolve/DsGP.h"
#endif

#ifndef _DS_GENE_H_
#include "Evolve/Gene/DsGene.h"
#endif
#ifndef _DS_DECISION_TREE_OPERATOR_H_
#include "Evolve/Gene/DsDecisionTreeOperator.h"
#endif
#ifndef _DS_DECISION_TREE_NODE_H_
#include "Evolve/Gene/DsDecisionTreeNode.h"
#endif

using namespace DsApp;

static const int POPULATION_NUM = 2000;
static const int GENERATION_NUM = 2000;
static const int TRIAL_NUM = 10;
static const double CROSSOVER_RATE = 0.3;
static const double SELECTION_RATE = 0.7;
static const double MUTATION_RATE = 0.01;
static const double EVALUATION_TIME = 120.0;

static const int SELECTION_NUM = static_cast<int>(static_cast<double>(POPULATION_NUM)*SELECTION_RATE);
static const int CROSSOVER_NUM = static_cast<int>(static_cast<double>(POPULATION_NUM)*CROSSOVER_RATE);
static const int MUTATION_NUM = static_cast<int>(static_cast<double>(POPULATION_NUM)*MUTATION_RATE);


DsGP::DsGP()
	: m_pop(POPULATION_NUM)
	, m_curEval(0)
	, m_isDevelopment(false)
{
	
}

DsGP::~DsGP()
{
	for(DsGene* pGene : m_pop){
		delete pGene;
	}
	m_pop.clear();
}

void DsGP::Generation(double dt)
{
	if (!m_isDevelopment){
		Development();
		m_isDevelopment = true;
	}

	for (int i = 0; i < GENERATION_NUM; ++i){

		if (Evaluation(dt)){
			Selection();
			Crossover();
			Mutation();
		}
	}
}

void DsGP::Development()
{
	DsGene* pGene = new DsGene[POPULATION_NUM];
	for (int i = 0; i < POPULATION_NUM; ++i){
		DsDecisionTreeOperator op;
		op.RefData().push_back(pGene->GetData());
		DsDecisionRootNode* pNode = op.CreateTree();
		pGene[i].SetNode(pNode);
		m_pop.push_back(pGene);
	}
}

bool DsGP::Evaluation(double dt)
{
	const bool isEnd = m_pop[m_curEval]->Evaluation(dt, EVALUATION_TIME, TRIAL_NUM);
	if (isEnd){
		++m_curEval;
	}

	if (POPULATION_NUM <= m_curEval){
		return true;
	}
	else{
		return false;
	}
}

void DsGP::Selection()
{
	class Pre{
	public:
		bool operator()(const DsGene* a, const DsGene* b){
			return a->GetScore() < b->GetScore();
		}
	};
	std::sort(m_pop.begin(), m_pop.end(), Pre());

	DsDecisionTreeOperator op;
	//淘汰
	for (int i = SELECTION_NUM; i < POPULATION_NUM; ++i){
		op.Delete(m_pop[i]->GetNode());
	}

	//増殖
	int totalSum = 0;
	for (int i = 0; i < SELECTION_NUM; ++i){
		totalSum += m_pop[i]->GetScore();
	}
	for (int i = SELECTION_NUM; i < POPULATION_NUM; ++i){
		int select = _Roulette(totalSum);
		while (0 > select){
			select = _Roulette(totalSum);
		}
		DsDecisionTreeNode* pCopy = op.Clone(m_pop[select]->GetNode());
		if (pCopy->GetType() == DECISION_TREE_NODE_TYPE::ROOT){
			m_pop[i]->SetNode(static_cast<DsDecisionRootNode*>(pCopy));
		}
		else{
			DS_ERROR("rootがない。コピーに失敗 ノードタイプ[%d]", static_cast<int>(pCopy->GetType()));
		}
	}
}

void DsGP::Crossover()
{
	DsDecisionTreeOperator op;
	bool isCrossover[POPULATION_NUM - (POPULATION_NUM%2)];//交差済みか？
	for (int i = 0; i < POPULATION_NUM; ++i){
		isCrossover[i] = false;
	}
	for (int i = 0; i < CROSSOVER_NUM; ++i){
		int pair1 = rand() % POPULATION_NUM;
		while (!isCrossover[pair1]){
			pair1 = rand() % POPULATION_NUM;
		}
		int pair2 = rand() % POPULATION_NUM;
		while (!isCrossover[pair2]){
			pair2 = rand() % POPULATION_NUM;
		}

		op.Crossover(m_pop[pair1]->GetNode(), m_pop[pair2]->GetNode());
		isCrossover[pair1] = true;
		isCrossover[pair2] = true;
	}
}

void DsGP::Mutation()
{
	DsDecisionTreeOperator op;
	bool isMutation[POPULATION_NUM];//突然変異済みか？
	for (int i = 0; i < MUTATION_NUM; ++i){
		const int mutationIdx = rand() % POPULATION_NUM;
		op.Mutation(m_pop[mutationIdx]->GetNode());
		isMutation[mutationIdx] = true;
	}
}

int DsGP::_Roulette(int totalSum)
{
	const int select = (rand()*rand()) % (totalSum+1);
	int sum = 0;
	for (int i = 0; i < POPULATION_NUM; ++i){
		sum += m_pop[i]->GetScore();
		if (select <= sum){
			return i;
		}
	}
	return -1;//選択者なし
}
