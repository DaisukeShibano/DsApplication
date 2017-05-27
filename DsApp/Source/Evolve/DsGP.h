#ifndef _DS_GP_H_
#define _DS_GP_H_

namespace DsApp
{
	class DsGene;
	class DsDecisionTreeOperator;
}

namespace DsApp
{
	class DsGP
	{
	public:
		DsGP();
		virtual ~DsGP();

	public:
		void Generation(double dt);

	public:
		void Development();
		bool Evaluation( double dt);
		void Selection();
		void Crossover();
		void Mutation();

	public:
		int _Roulette(int totalSum);

	public:
		std::vector<DsGene*> m_pop;
		int m_curEval;
		bool m_isDevelopment;

	};
}

#endif