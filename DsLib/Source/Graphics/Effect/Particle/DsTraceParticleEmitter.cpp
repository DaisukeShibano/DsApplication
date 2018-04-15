#include "DsPch.h"

#ifndef _DS_TRACE_PARTICLE_EMITTER_
#include "Graphics/Effect/Particle/DsTraceParticleEmitter.h"
#endif
//���̃w�b�_


using namespace DsLib;

//�p�����[�^�ł��������������Ǎ��͂Ƃ肠�������ߑł�
static const double INTERVAL_TIME = 0.01;//�����Ԋu[�b]
static const double PARTICLE_LIFE_TIME = 1.5f;//�p�[�e�B�N������



static const int BLOCK_SIZE = 256;//���̌��P�ʂŃ������m�ۂ����
static const double DELETE_BLOCK_TIME = 5.0f;//�u���b�N�����̎��Ԉȏ�󂾂�����폜����


DsTraceParticleEmitter::DsTraceParticleEmitter()
	: m_particle()
	, m_emitPos()
	, m_emitPrePos()
	, m_texPath()
	, m_nextEmitTime(0)
	, m_isRequestEmit(false)
{
	m_texPath = "TestParticle.tga";
}

DsTraceParticleEmitter::~DsTraceParticleEmitter()
{
	for (ParticleBlock& block : m_particle) {
		delete block.pDataTop;
	}
	m_particle.clear();
}

void DsTraceParticleEmitter::Update(double dt)
{
	/*
	|                     1 block  block���̃������͘A��                      |...
	|pHead->pNext->pNext->...pUseTail->NULL   pFreeTail->pNext->pNext->...NULL|
	*/


	//�X�V/�폜
	for (ParticleBlock& block : m_particle) {
		DsSquareParticle* pParticle = block.pUseHead;
		DsSquareParticle* pPre = NULL;
		while (pParticle) {
			pParticle->lifeTime -= dt;

			//�����s�����̂ŏ���(�t���[���X�g�Ɉړ�������)
			if (pParticle->lifeTime <= 0.0) {
				DsSquareParticle* pNext = pParticle->pNext;

				//�g�p�����X�g����͂���
				if (pPre) {//���Ԃ�������
					pPre->pNext = pNext;
				}
				else {//�擪��������
					block.pUseHead = pNext;
				}

				//�������̂��g�p�������Ȃ���
				if (pParticle == block.pUseTail) {
					block.pUseTail = pPre;//�擪==�g�p�������Ȃ�pPre��NULL�Ŏg�p��������NULL
					if (block.pUseTail) {
						block.pUseTail->pNext = NULL;
					}
				}

				//�t���[���X�g�ɒǉ�
				pParticle->pNext = block.pFreeTail;
				block.pFreeTail = pParticle;

				//�i��łȂ��̂ň�O(pPre)�͍X�V���Ȃ�
				pParticle = pNext;
			}
			else {
				pPre = pParticle;
				pParticle = pParticle->pNext;
			}
		}
	}


	//���N�G�X�g���~�܂����̂ŋO�Ղ��r�؂��
	if (!m_isRequestEmit) {
		m_emitPrePos[0] = m_emitPos[0];
		m_emitPrePos[1] = m_emitPos[1];
	}

	//��������
	//��~�������̔������Ԃ̓J�E���g����B�ĊJ�����Ƃ��ɂ��΂炭�҂��Ȃ��Ɛ������Ȃ��̂��s���R������
	if (0.0 <= m_nextEmitTime) {
		m_nextEmitTime -= dt;
	}
	if (m_isRequestEmit) {
		if (m_nextEmitTime <= 0.0) {

			//��Ŋm��
			//�󂢂Ă���u���b�N��T��
			ParticleBlock* pFreeBlock = NULL;
			for (ParticleBlock& block : m_particle) {
				if (block.pFreeTail) {
					pFreeBlock = &block;
					break;
				}
			}
			if (NULL == pFreeBlock) {
				DsSquareParticle* pParticle = new DsSquareParticle[BLOCK_SIZE];
				DS_ASSERT(pParticle, "�������m�ێ��s");
				for (int idx = 0; idx < BLOCK_SIZE-1; ++idx) {
					pParticle[idx].pNext = pParticle + idx + 1;
				}
				ParticleBlock block;
				block.pDataTop = pParticle;
				block.pUseHead = pParticle;
				block.pUseTail = NULL;
				block.pFreeTail = pParticle;
				m_particle.push_back(block);
				pFreeBlock = &m_particle.back();
			}
			
			DsSquareParticle* pParticle = pFreeBlock->pFreeTail;
			pParticle->pos[0] = m_emitPos[0];
			pParticle->pos[1] = m_emitPos[1];
			pParticle->pos[2] = m_emitPrePos[0];
			pParticle->pos[3] = m_emitPrePos[1];
			pParticle->speed = DsVec3d::Zero();//�O�Ղ͂��̏�ɂƂǂ܂邾���Ȃ̂ő��x�͂Ȃ�
			pParticle->lifeTime = PARTICLE_LIFE_TIME;

			//�t���[���X�g������
			pFreeBlock->pFreeTail = pFreeBlock->pFreeTail->pNext;

			//�g�p���ɒǉ�
			if (pFreeBlock->pUseTail) {
				pFreeBlock->pUseTail->pNext = pParticle;
			}
			pFreeBlock->pUseTail = pParticle;
			pParticle->pNext = NULL;

			m_nextEmitTime = INTERVAL_TIME;

			m_emitPrePos[0] = m_emitPos[0];
			m_emitPrePos[1] = m_emitPos[1];
		}
	}


	//�u���b�N�̃������J��
	{
		auto it = m_particle.begin();
		auto end = m_particle.end();
		while (it != end) {
			bool isErase = false;
			
			if (it->pUseTail == NULL) {
				it->deleteBlockTime -= dt;
				if (it->deleteBlockTime <= 0.0) {
					delete it->pDataTop;
					it = m_particle.erase(it);
					isErase = true;
				}
			}
			else {
				it->deleteBlockTime = DELETE_BLOCK_TIME;
			}

			if (!isErase) {
				++it;
			}
		}

	}

	m_isRequestEmit = false;
}

void DsTraceParticleEmitter::SetPosition(const DsVec3d& pos1, const DsVec3d& pos2)
{
	m_emitPos[0] = pos1;
	m_emitPos[1] = pos2;
}



