#include "DsPhysicsPch.h"
#ifndef __DS_BOUNDING_OCTREE__
#include "Collision/BoundingTree/DsBoundingOctree.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef _DS_AABB_H_
#include "Math/DsAabb.h"
#endif

using namespace DsLib;
using namespace DsPhysics;

static const int CHILD_NUM = 8;


/*

�ǂ̕��̂Ƃǂ̕��̂��������Ă��邩�A�̑{�����i�邽�߂̋�ԕ����B
�c���[��ɂȂ��ĂāA��ԑ傫�ȁA�S�Ă̕��̂��ދ�Ԃ����[�g���
�����8���������̂��q���
����ɂ��̎q��Ԃ��ꂼ���8�������E�E�E�ȉ��J��Ԃ��ō\��������ԁB


�Ƃ�����A�ɏ������镨�̂́A����A�ɏ������镨�̂��A�e�̋�Ԃɏ������镨�̂ɂ̂ݓ�����\��������B
�Ƃ����̂���ԉ��̊K�w�������Ă����΁A�S�Ă̓�����\���̃p�^�[����ԗ��ł���B
(���ۂ͍������̂��߂ɍċA�֐��͎g�킸�Awhile�ŏ��񂷂�B)


���t��Update�ŁA���̂��ǂ̋�Ԃɏ�������̂����X�V���Ă��K�v������B
�Ȃ�ׂ������ɏ����O���[�v���X�V�������̂ŁA���W�l�����̂܂܊i�q�ԍ��ɂȂ�悤�ɂ��Ă���(������Ƃł��ĂȂ�����)�B
���̂̏�����Ԃ́A���̕��̂����S�ɓ�����Ԃ�I�Ԃ悤�ɂ���B
�������Ȃ��ƁA���̂�������Ԃ��͂ݏo�Ă��܂��A�u������Ԃ��A�e�Ƃ���������\�����Ȃ��v�̑O�񂪕����B
��Ԃ����������ē���Ȃ��ꍇ�́A���܂�T�C�Y�ɂȂ�܂Őe��k��B�������ď�����Ԃ̊K�w�����߂Ă���(���̌��������������B�P���ŋ��߂���)�B

���ꔭ�ŋ��߂��Ă�̂ŎQ�l�ɂ������B
http://marupeke296.com/COL_2D_No8_QuadTree.html


CreateTree(const int resolution)
�����́A��ԉ��̑w�́i�܂��ԍׂ����j��ԕ��������w�肷��B

�Ⴆ�΁A64�Ȃ�A���[�g��Ԃ��c������64����������Ԃ���ԍׂ����������̊K�w�Ƃ��č����B
��������A2*2*2�̃T�C�Y�̋�Ԃ�1�̋�ԂƂ��č�肻���e����B�ȏ���J��Ԃ��Ē��Ԃ̑w�����B

��ԍׂ������������w�肷��������x�Ƒ��x�̒��������₷�����낤�E�E�E�Ǝv�������߁B


���Ȃ݂Ƀ��[�g��Ԃ̃T�C�Y�͑S���̂����܂�T�C�Y�ɖ��t���v�Z���Ȃ����Ă���B
�����͈͂��ÓI�ɋ��܂�ꍇ�p�ɁA�Œ�l�ł��p�ӂ����ق���������������Ȃ�(���Ƃ��Ɩ��t���X�V����O��Ȃ̂ł��Ƃ���ݒ肵�Ȃ������Ƃ͑S�R���Ȃ�)

*/


DsBoundingOctree::DsBoundingOctree()
	: m_pRoot(NULL)
	, m_pTerminal(NULL)
	, m_pAllNode(NULL)
	, m_allNodeNum(0)
	, m_resolution(0)
	, m_allActorMaxPos(DsVec3d::Zero())
	, m_allActorMinPos(DsVec3d::Zero())
	, m_isExistActor(false)
{
}

DsBoundingOctree::~DsBoundingOctree()
{
	DsBdOctreeNode* tmp = m_pRoot;
	while (tmp){
		DsBdOctreeNode* det = tmp;
		tmp = tmp->next;
		delete det;
	}
	delete[] m_pAllNode; m_pAllNode = NULL;
}

/*
resolution �������B�m�[�h����2�̎q�����؂����֌W��resolution��2~n �łȂ���΂��߁B
*/
void DsBoundingOctree::CreateTree(const int resolution)
{
	DS_ASSERT(2 <= resolution, "DsBoundingOctree�쐬���s");
	m_resolution = resolution;

	//���[�m�[�h��S�č��
	DsBdOctreeNode* pTerminals = NULL;
	DsBdOctreeNode* pListNode = NULL;
	{
		//Actor�̍��W���I�t�Z�b�g���ĕ������Ŋ����������l(x, y, z)����ǂ̃O���b�h�������킩��悤�ȃm�[�h�\��
		//�m�[�h�C���f�b�N�X = x*resolution*resolution + y*resolution + z
		const int nodeNum = resolution*resolution*resolution;
		DsBdOctreeNode* pNode = new DsBdOctreeNode[nodeNum];
		m_allNodeNum += nodeNum;

		const int resolution2 = resolution*resolution;
		for (int xi = 0; xi < resolution; ++xi){
			const int xOffset = xi*resolution2;
			for (int yi = 0; yi < resolution; ++yi){
				const int yOffset = yi*resolution;
				for (int zi = 0; zi < resolution; ++zi){
					DsBdOctreeNode* pSetNode = pNode + (xOffset + yOffset + zi);
					pSetNode->child = NULL;
				}
			}
		}
		pTerminals = pNode;
		m_pTerminal = pTerminals;

		//�P�����[�v�A�N�Z�X�p
		pNode[nodeNum - 1].next = NULL;//���[
		for (long i = nodeNum-2; 0 <= i; --i){
			pNode[i].next = &pNode[i + 1];
		}
		pListNode = m_pTerminal;
	}

	//�q�m�[�h����e�m�[�h�𐶐����Ă����B
	DsBdOctreeNode* pParent = m_pTerminal;
	{
		int parentResolution = resolution;//resolution��2��n��
		//�q����e�𐶐����Ă�
		while (1){
			const int childResolution = parentResolution;
			parentResolution /= 2;
			if (0 < parentResolution){
				DsBdOctreeNode* pChild = pParent;
				const int nodeNum = parentResolution*parentResolution*parentResolution;
				pParent = new DsBdOctreeNode[nodeNum];
				m_allNodeNum += nodeNum;

				//���X�g�A�N�Z�X�p
				pParent[nodeNum - 1].next = pListNode;//�e�̖��[�Ǝq�̐擪���q����
				for (long i = nodeNum - 2; 0 <= i; --i){
					pParent[i].next = &pParent[i + 1];
				}
				pListNode = pParent;//���X�g�̐擪�X�V

				const int childResolution2 = childResolution*childResolution;
				////�q�C���f�b�N�X�͊e�� 2n �� 2n+1 ��2���Ōv8�B
				for (int xi = 0; xi < parentResolution; ++xi){
					const int xOffset0 = xi * 2 * childResolution2;
					const int xOffset1 = (xi * 2 + 1)*childResolution2;
					const int xParentOffset = xi*parentResolution*parentResolution;
					for (int yi = 0; yi < parentResolution; ++yi){
						const int yOffset0 = yi * 2 * childResolution;
						const int yOffset1 = (yi * 2 + 1)*childResolution;
						const int yParentOffset = yi*parentResolution;
						for (int zi = 0; zi < parentResolution; ++zi){
							DsBdOctreeNode* pSetNode = pParent + (xParentOffset + yParentOffset + zi);
							const int zOffset0 = zi * 2;
							const int zOffset1 = zi * 2 + 1;
							pSetNode->child = pChild + (xOffset0 + yOffset0 + zOffset0);//�q���͈�ԍ��[�����ł����B

							//�q�������̌Z��֌W�Ɛe��ݒ�
							DsBdOctreeNode* tmp = pSetNode->child;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset0 + yOffset0 + zOffset1); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset0 + yOffset1 + zOffset0); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset0 + yOffset1 + zOffset1); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset1 + yOffset0 + zOffset0); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset1 + yOffset0 + zOffset1); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset1 + yOffset1 + zOffset0); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = pChild + (xOffset1 + yOffset1 + zOffset1); tmp = tmp->brother;
							tmp->parent = pSetNode; tmp->brother = NULL;
						}
					}
				}
			}
			else{
				pParent->parent = NULL;
				pParent->brother = NULL;
				break;
			}
		}
	}
	m_pRoot = pParent;


	//�z��ŃA�N�Z�X����p
	m_pAllNode = new DsBdOctreeNode*[m_allNodeNum];
	int nodeNum = 0;
	DsBdOctreeNode* tmp = m_pRoot;
	while (tmp) {
		m_pAllNode[nodeNum] = tmp;
		tmp->actor = NULL;
		tmp = tmp->next;
		++nodeNum;
	}
	DS_ASSERT(nodeNum == m_allNodeNum, "DsBdOctreeNode�̑���������Ȃ�");
}



/*
�c���[�ɃA�N�^�[��o�^����
allActorMinPos �S�ẴA�N�^�[�̂����Ƃ�������AABB�̒[���W
allActorMaxPos �S�ẴA�N�^�[�̂����Ƃ��傫��AABB�̒[���W
*/
void DsBoundingOctree::Update(DsActor** pActors, const int actNum, const DsVec3d& allActorMinPos, const DsVec3d& allActorMaxPos)
{
	m_allActorMaxPos = allActorMaxPos;
	m_allActorMinPos = allActorMinPos;
	m_isExistActor = 0 < actNum;

	//����actor�N���A
	DsBdOctreeNode** tmp = m_pAllNode;
	for (int i = 0; i < m_allNodeNum; ++i, ++tmp) {
		(*tmp)->actor = NULL;
	}

	const DsVec3d gridSize = DsVec3d::Abs(allActorMaxPos - allActorMinPos) / static_cast<double>(m_resolution);

	for (int actIdx = 0; actIdx < actNum; ++actIdx) {
		DsActor* pActor = pActors[actIdx];

		//pActor�����S�ɓ����镪����Ԃ̃m�[�h�����߂�B

		const DsAabb aabb = pActor->RefAabb();
		const DsVec3d maxPos = aabb.GetMax();
		const DsVec3d minPos = aabb.GetMin();
		//�e�����̍��W�����Ԗڂ̊i�q�ɑ�������̂��B
		//��ԑ傫�����W�̊i�q�ԍ�
		const DsVec3d maxGridPos = DsVec3d::Div(maxPos - allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);//�҂�����Max���ƂP���͂ݏo�邱�ƂɂȂ�̂�-FLT_EPSILON�B
		const int maxGridNum[3] = {
			static_cast<int>(maxGridPos.x),
			static_cast<int>(maxGridPos.y),
			static_cast<int>(maxGridPos.z),
		};
		//��ԏ��������W�̊i�q�ԍ�
		const DsVec3d minGridPos = DsVec3d::Div(minPos - allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);;
		const int minGridNum[3] = {
			static_cast<int>(minGridPos.x),
			static_cast<int>(minGridPos.y),
			static_cast<int>(minGridPos.z),
		};
		//���̎��_�Ń}�C�i�X�ɂ͂Ȃ�Ȃ��͂��B

		const int resolution2 = m_resolution*m_resolution;
		const int maxIdx = maxGridNum[0] * resolution2 + maxGridNum[1] * m_resolution + maxGridNum[2];//��ԑ傫�ȍ��W�̏�������i�q�̃m�[�h�ւ̃C���f�b�N�X
		const int minIdx = minGridNum[0] * resolution2 + minGridNum[1] * m_resolution + minGridNum[2];//��ԏ����ȍ��W�̏�������i�q�̃m�[�h�ւ̃C���f�b�N�X

		//�ő���W�ƍŏ����W�̃O���b�h�̐e�������̂ڂ��Ă����āA�����ɂȂ�Η������܂܂���ԁ�����actor�����S�Ɋ܂ދ��
		DsBdOctreeNode* pMaxParent = m_pTerminal[maxIdx].parent;
		DsBdOctreeNode* pMinParent = m_pTerminal[minIdx].parent;
		while (pMaxParent != pMinParent) {//�ň��̃��[�v�񐔂͍ő�K�w���B�����炭�ő�K�w��7�`8���炢�ɂȂ�Ɨ\�z
			pMaxParent = pMaxParent->parent;
			pMinParent = pMinParent->parent;
		}//���Ȃ��Ƃ����[�g��ԂŕK����v����͂��BNULL�Ȃ�����~�X
		DS_ASSERT(pMaxParent && pMinParent, "DsBoundingOctree�ŏ�����Ԃ�������Ȃ�actor������ %S", pActor->GetName());

		 //������Ԃ����������̂ł����ɓo�^
		pActor->SetOctreeNodeNext(pMaxParent->actor);//��ԍŏ���NULL
		pMaxParent->actor = pActor;//�ォ��ǉ����������擪�ɂȂ�
	}
}

/*
actor���܂ދ�Ԃɏ�������S�Ă�actor���擾
*/
void DsBoundingOctree::GetContainAreaActors(const DsActor& actor, std::vector<const DsActor*>& outActors) const
{
	if (!m_isExistActor) {
		return;
	}

	//actor�����S�Ɋ܂܂���Ԃ����߁A���̋�Ԃɏ�������S�Ă�actro��outActors�Ƃ��ĕԂ�

	const DsVec3d gridSize = DsVec3d::Abs(m_allActorMaxPos - m_allActorMinPos) / static_cast<double>(m_resolution);
	const DsAabb aabb = actor.RefAabb();
	const DsVec3d maxPos = DsVec3d::Clamp3(aabb.GetMax(), m_allActorMinPos, m_allActorMaxPos);//�V�Kactor��������Ȃ��̂ōő���W�ŃN�����v
	const DsVec3d minPos = DsVec3d::Clamp3(aabb.GetMin(), m_allActorMinPos, m_allActorMaxPos);
	//�e�����̍��W�����Ԗڂ̊i�q�ɑ�������̂��B
	//��ԑ傫�����W�̊i�q�ԍ�
	const DsVec3d maxGridPos = DsVec3d::Div(maxPos - m_allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);//�҂�����Max���ƂP���͂ݏo�邱�ƂɂȂ�̂�-FLT_EPSILON�B
	const int maxGridNum[3] = {
		static_cast<int>(maxGridPos.x),
		static_cast<int>(maxGridPos.y),
		static_cast<int>(maxGridPos.z),
	};
	//��ԏ��������W�̊i�q�ԍ�
	const DsVec3d minGridPos = DsVec3d::Div(minPos - m_allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);;
	const int minGridNum[3] = {
		static_cast<int>(minGridPos.x),
		static_cast<int>(minGridPos.y),
		static_cast<int>(minGridPos.z),
	};
	//���̎��_�Ń}�C�i�X�ɂ͂Ȃ�Ȃ��͂��B

	const int resolution2 = m_resolution*m_resolution;
	const int maxIdx = maxGridNum[0] * resolution2 + maxGridNum[1] * m_resolution + maxGridNum[2];//��ԑ傫�ȍ��W�̏�������i�q�̃m�[�h�ւ̃C���f�b�N�X
	const int minIdx = minGridNum[0] * resolution2 + minGridNum[1] * m_resolution + minGridNum[2];//��ԏ����ȍ��W�̏�������i�q�̃m�[�h�ւ̃C���f�b�N�X

	//�ő���W�ƍŏ����W�̃O���b�h�̐e�������̂ڂ��Ă����āA�����ɂȂ�Η������܂܂���ԁ�����actor�����S�Ɋ܂ދ��
	const DsBdOctreeNode* pMaxParent = m_pTerminal[maxIdx].parent;
	const DsBdOctreeNode* pMinParent = m_pTerminal[minIdx].parent;
	while (pMaxParent != pMinParent) {
		pMaxParent = pMaxParent->parent;
		pMinParent = pMinParent->parent;
	}//���Ȃ��Ƃ����[�g��ԂŕK����v����͂��BNULL�Ȃ�����~�X
	DS_ASSERT(pMaxParent && pMinParent, "DsBoundingOctree�ŏ�����Ԃ�������Ȃ�actor������ %S", actor.GetName());


	bool isBack = false;
	const DsBdOctreeNode* pSearch = pMaxParent->child;//�Z����܂ޒT���Ȃ̂Ŏq����

	//pSearch�ȉ��̎q�m�[�h��T��
	while (pSearch){
		if (!isBack) {//�K�w��V�����~�肽�Ƃ������BisBack��ture�Ƃ������Ƃ͊��ɏ���ς�
			const DsActor* pOutActor = pSearch->actor;
			while (pOutActor) {
				outActors.push_back(pOutActor);
				pOutActor = pOutActor->GetOctreeNodeNext();
			}
		}

		//�[���D��Ńc���[��H��
		if (pSearch->child && (!isBack)) {
			pSearch = pSearch->child;
		}
		else if (pSearch->brother) {
			pSearch = pSearch->brother;
			isBack = false;
		}
		else {
			pSearch = pSearch->parent;
			isBack = true;
		}

		if (pSearch == pMaxParent) {
			//�߂��Ă����̂ŏI���
			break;
		}
	}

	pSearch = pMaxParent;
	//pSearch�ȏ��pSearch�����S�Ɋ܂ނ̂ŕK���Փˌ��
	while (pSearch) {
		const DsActor* pOutActor = pSearch->actor;
		while (pOutActor) {
			outActors.push_back(pOutActor);
			pOutActor = pOutActor->GetOctreeNodeNext();
		}
		pSearch = pSearch->parent;
	}
}