#include "DsAppPch.h"
#ifndef _DS_NEURON_CTRL_
#include "NeuralNetwork/DsNeuronCtrl.h"
#endif

#ifndef _DS_NEURON_
#include "NeuralNetwork/DsNeuron.h"
#endif

using namespace DsApp;


DsNeuralNetwork* DsNeuronCtrl::CreateNeuralNetwork()
{
	DsNeuron* pInput = new DsNeuron[m_inputNum];
	DS_ASSERT(pInput, "�������m�ێ��s");
	DsNeuron* pOutput = new DsNeuron[m_outputNum];
	DS_ASSERT(pOutput, "�������m�ێ��s");
	DsNeuron** pInner = new DsNeuron*[m_innerLayerNum];
	DS_ASSERT(pInner, "�������m�ێ��s");
	for (int i = 0; i < m_innerLayerNum; ++i) {
		pInner[i] = new DsNeuron[m_innerNeuronNum];
		DS_ASSERT(pInner[i], "�������m�ێ��s");
	}

	//���͑w�ƒ��ԑw�̌���
	for (int i = 0; i < m_innerNeuronNum; ++i) {
		//�S����
		for (int j = 0; j < m_inputNum; ++j) {
			DsNeuronInput child;
			child.w = 0.5;//�d���͓K��
			child.pNeuron = &pInput[j];
			pInner[0][i].input.push_back(child);
		}
	}

	//���ԑw�ƒ��ԑw
	for (int i = 1; i < m_innerLayerNum; ++i) {
		for (int j = 0; j < m_innerNeuronNum; ++j) {
			for (int k = 0; k < m_innerNeuronNum; ++k) {
				DsNeuronInput child;
				child.w = 0.5;
				child.pNeuron = &pInner[i - 1][k];
				pInner[i][j].input.push_back(child);
			}
		}
	}

	//�o�͑w�ƒ��ԑw
	for (int i = 0; i < m_outputNum; ++i) {
		for (int j = 0; j < m_innerNeuronNum; ++j) {
			DsNeuronInput child;
			child.w = 0.5;
			child.pNeuron = &pInner[m_innerLayerNum-1][j];
			pOutput[i].input.push_back(child);
		}
	}


	DsNeuralNetwork* ret = new DsNeuralNetwork;
	DS_ASSERT(ret, "�������m�ێ��s");
	ret->pInput = pInput;
	ret->pInner = pInner;
	ret->pOutput = pOutput;
	ret->inputNum = m_inputNum;
	ret->innerLayerNum = m_innerLayerNum;
	ret->innerNeuronNum = m_innerNeuronNum;
	ret->outputNum = m_outputNum;

	return ret;
}

void DsNeuronCtrl::CalcOutput(DsNeuralNetwork* pNet)
{
	const int outputNum = pNet->outputNum;
	const int inputNum = pNet->inputNum;
	const int innnerNum = pNet->innerNeuronNum;
	const int innerLayerNum = pNet->innerLayerNum;

	DsNeuron* pOutput = pNet->pOutput;
	DsNeuron** pInner = pNet->pInner;
	DsNeuron* pInput = pNet->pInput;

	//���͑w-���ԑw
	for (int layerNum = 0; layerNum < innerLayerNum; ++layerNum) {
		for (int innerIdx = 0; innerIdx < innnerNum; ++innerIdx) {
			float sum = 0;
			for each(const DsNeuronInput& input in pInner[layerNum][innerIdx].input) {
				sum += input.w * input.pNeuron->outVal;
			}

			//�V�O���C�h�֐�
			pInner[layerNum][innerIdx].outVal = 0;

		}
	}

	//���ԑw-�o�͑w
	for (int outIdx = 0; outIdx < outputNum; ++outIdx) {
		float sum = 0;
		for each(const DsNeuronInput& input in pOutput[outIdx].input) {
			sum += input.w * input.pNeuron->outVal;
		}
		pOutput[outIdx].outVal = 0;
	}
}
