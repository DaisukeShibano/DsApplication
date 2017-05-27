#ifndef _DS_NEURON_

namespace DsApp
{
	struct DsNeuronInput;

	struct DsNeuron
	{
		DsNeuron()
			:input()
			,outVal(0)
		{}
		std::vector<DsNeuronInput> input;
		float outVal;
	};

	struct DsNeuronInput
	{
		DsNeuronInput()
			:pNeuron(NULL)
			,w(0)
		{}
		DsNeuron* pNeuron;
		float w;
	};

	struct DsNeuralNetwork
	{
		DsNeuralNetwork()
			:pInput(NULL)
			,pInner(NULL)
			,pOutput(NULL)
			,inputNum(0)
			,outputNum(0)
			,innerLayerNum(0)
			,innerNeuronNum(0)
		{}

		DsNeuron* pInput;
		DsNeuron** pInner;
		DsNeuron* pOutput;

		int inputNum;
		int outputNum;
		int innerLayerNum;
		int innerNeuronNum;
	};





}


#endif // !_DS_NEURON_
