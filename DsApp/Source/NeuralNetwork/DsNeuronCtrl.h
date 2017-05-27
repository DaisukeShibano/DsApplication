#ifndef _DS_NEURON_CTRL_

namespace DsApp
{
	struct DsNeuralNetwork;
}

namespace DsApp
{
	class DsNeuronCtrl
	{
	public:
		DsNeuronCtrl()
			: m_inputNum(0)
			, m_outputNum(0)
			, m_innerLayerNum(0)
			, m_innerNeuronNum(0)
		{}
		virtual ~DsNeuronCtrl(){}
		 
	public:
		DsNeuralNetwork* CreateNeuralNetwork();

		void CalcOutput(DsNeuralNetwork* pNet );

	private:
		int m_inputNum;//���̓j���[������
		int m_outputNum;//�o�̓j���[������
		int m_innerLayerNum;//���ԑw��
		int m_innerNeuronNum;//���ԑw�j���[������

	};
}

#endif