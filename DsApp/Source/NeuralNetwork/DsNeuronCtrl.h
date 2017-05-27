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
		int m_inputNum;//入力ニューロン数
		int m_outputNum;//出力ニューロン数
		int m_innerLayerNum;//中間層数
		int m_innerNeuronNum;//中間層ニューロン数

	};
}

#endif