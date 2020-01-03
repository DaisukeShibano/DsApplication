#include "DsPch.h"
#include "Collision/DsGridTree.h"
#include "Collision/DsCollisionBuffer.h"
#include "Collision/DsGridEntity.h"
#include "Collision/DsGrid.h"

using namespace DsLib;

namespace
{
	const int MAX_TREE_DEPTH = 4;
	const int CELL_LINE_NUM = 1 << 4;//1����������̑��Z����
	const float CELL_LINE_NUM_F = static_cast<float>(CELL_LINE_NUM);
	const int BUF_INDEX_NUM = 20000;
}

namespace
{
	size_t _CalcTreeSize()
	{
		size_t ret = 0;
		for (int i = 0; i <= MAX_TREE_DEPTH; ++i) {
			ret += static_cast<size_t>(pow(8, i));
		}
		return ret;
	}
}

//static
size_t DsGridTree::CalcMemSize(size_t entityNum)
{
	return (_CalcTreeSize() * sizeof(DsGrid)) + (entityNum * sizeof(DsGridEntity)) + sizeof(DsGridTree);
}

static size_t s_dbgMaxIndex = 0;

//static
DsGridTree* DsGridTree::Create(ds_uint8* pBuf, const DsCollisionBuffer* pCollisionBuf, ds_uint8* pDbgTop, size_t dbgSize)
{
	ds_uint8* pTreeBufTop = pBuf;

	//DsGridTree* pGridTree = new(pBuf) DsGridTree;  //reinterpret_cast<DsGridTree*>(pBuf);
	//const size_t entitySize = gridTreeSize - sizeof(DsGridTree);//�c�肪DsGridEntity�z��
	//const size_t entityNum = entitySize / sizeof(DsGridEntity);//�z��̌��ɕϊ�
	//pGridTree->m_pGrids = new(pBuf + sizeof(DsGridTree)) DsGridEntity[entityNum]; //reinterpret_cast<DsGridEntity*>(pBuf + sizeof(DsGridTree));

	//�������͏d���̂łȂ�ׂ��Ă΂��A0�N���O��ɂ���
	DsGridTree* pGridTree = new(pTreeBufTop) DsGridTree;
	pGridTree->m_pGrids = reinterpret_cast<DsGrid*>(pTreeBufTop + sizeof(DsGridTree));
	const size_t treeSize = _CalcTreeSize();
	pGridTree->m_pGridEntities = reinterpret_cast<DsGridEntity*>(pTreeBufTop + sizeof(DsGridTree) + treeSize *sizeof(DsGrid) );

	ds_uint8* pEnd = pDbgTop + dbgSize;
	ds_uint64 diff = pEnd - (ds_uint8*)(pGridTree->m_pGridEntities);

	s_dbgMaxIndex = diff / sizeof(DsGridEntity);

	pGridTree->_Build(pCollisionBuf);





	return pGridTree;
}


DsGridTree::DsGridTree()
	: m_cellUnitLen{1.0f, 1.0f, 1.0f}
	, m_minP{}
	, m_pBuff(NULL)
	, m_pGrids(NULL)
	, m_pGridEntities(NULL)
{
}


DsGridTree::~DsGridTree()
{
}


namespace
{
	//s��3bit�����̌��Ԃ��󂯂� 1111 �� 1001001001
	//s�͍��W��1�Z���̒����Ŋ��������́��Z������
	unsigned int _Gap3Bit(unsigned int s)
	{
		//�Z���P�ʐ���Root��MAX_TREE_DEPTH��2���������󂾂���(2��4��)
		//s��MAX_TREE_DEPTH[bit]�Ɏ��܂�
		//s��8bit�܂ł�z��(8bit��3bit�̌��Ԃ��󂯂��24bit ���₷���Ƃ͉�)
		s = (s | s << 8) & 0x0000f00f;
		s = (s | s << 4) & 0x000c30c3;
		s = (s | s << 2) & 0x00249249;
		return s;
	}

	//x,y,z�͂P�Z���P�ʒ���
	unsigned int _GetMortonOrder(unsigned int x, unsigned int y, unsigned int z)
	{
		return _Gap3Bit(x) | (_Gap3Bit(y) << 1) | (_Gap3Bit(z) << 2);
	}
}


void DsGridTree::TreeUpdate()
{
}

bool DsGridTree::Test(const float _start[3], const float _end[3]) const
{
	const unsigned int start[3]=
	{
		static_cast<unsigned int>(_start[0] - m_minP[0]) / static_cast<unsigned int>(m_cellUnitLen[0]),
		static_cast<unsigned int>(_start[1] - m_minP[1]) / static_cast<unsigned int>(m_cellUnitLen[1]),
		static_cast<unsigned int>(_start[2] - m_minP[2]) / static_cast<unsigned int>(m_cellUnitLen[2]),
	};

	const unsigned int end[3]=
	{
		static_cast<unsigned int>(end[0] - m_minP[0]) / static_cast<unsigned int>(m_cellUnitLen[0]),
		static_cast<unsigned int>(end[1] - m_minP[1]) / static_cast<unsigned int>(m_cellUnitLen[1]),
		static_cast<unsigned int>(end[2] - m_minP[2]) / static_cast<unsigned int>(m_cellUnitLen[2]),
	};

	const unsigned int morton[2] = {
		_GetMortonOrder(start[0], start[1], start[2]),
		_GetMortonOrder(end[0], end[1], end[2]),
	};

	//���`�W����K�w�ʔz��C���f�b�N�X�I�t�Z�b�g
	unsigned int gridIndexOffset[MAX_TREE_DEPTH + 1];
	for (int level = 0; level < MAX_TREE_DEPTH + 1; ++level) {
		gridIndexOffset[level] = (static_cast<unsigned int>((pow(8, level)) - 1) / 7);
	}

	unsigned int gridNo = 0;
	unsigned int level = 0;
	unsigned int levelMorton = morton[0] ^ morton[1];//xor������ĈႤbit�������悤��
	//1�K�w�ڂ��珇�Ɉ���Ă��܂��K�w�𔻒�
	//���傤��level������K�w-1�Őe�ɂȂ��Ă�̂ŁA���ꂪ�ʂ̊K�w
	for (; level < MAX_TREE_DEPTH; ++level) {
		//���3bit�𔻒�
		const unsigned int bit = levelMorton & 0xE0000000;
		if (bit == 0) {
			//��v���Ă����̂�3���_�S�Ċ܂܂����
		}
		else {
			//3���_�̊K�w�̂ǂꂩ�̊K�w����������S���܂߂��ĂȂ�
			break;
		}
		levelMorton <<= 3;//3bit�i��Ŏ��̊K�w��
	}
	//��v���Ă����K�w�̂Ƃ���܂ł̊i�q�ԍ�
	gridNo= morton[0] >> (MAX_TREE_DEPTH - level) * 3;

	const DsCollisionBuffer::ModelData* pModel = m_pBuff->GetBuff()->pModel;

	{//���K�w�̓����蔻��
		//�K�w�ƊK�w���ԍ�����z��C���f�b�N�X�����߂�
		const unsigned int arrayIdx = gridIndexOffset[level] + gridNo;
		const DsGrid& grid = m_pGrids[arrayIdx];
		unsigned int entityId = grid.entityId - 1;
		while (1) {
			if (entityId != DsGrid::INVALID) {
				const DsGridEntity& entity = m_pGridEntities[entityId - 1];
				const DsCollisionBuffer::ModelData& model = pModel[entity.modelIndex];
				const DsCollisionBuffer::FaceData& face = model.pFace[entity.faceIndex];
				const DsCollisionBuffer::VertexData* pVertex = model.pVertex;

				//�����œ����蔻��

				entityId = entity.nextEntityId;
			}
			else {
				break;
			}
		}
	}

	{//�q�K�w�̂����蔻��
		unsigned int beginGridNo = gridNo * 8;
		int levelAdd = 1;
		for (int currentLevel = level+1; currentLevel <= MAX_TREE_DEPTH; ++currentLevel) {
			//gridNo(=�ŏ��̒T�����)���܂ދ�Ԃ̍ŏ��ƍő�܂ŒT��
			//beginIdx,endIdx ��gridNo�̍ŏ��ƍő�������q�K�w�̊i�q�ԍ�
			//beginIdx,endIdx �͊K�w���オ���Ă����W�I�ɂ͓���gridNo�̍ŏ��ƍő�������ʒu�ɂ��邾��,���̊K�w�ł̔ԍ��ɂ��Ȃ���(begin�`end�Ԃ͂ǂ�ǂ�ׂ����Ȃ�=�����Ȃ�)
			const unsigned int offset = gridIndexOffset[currentLevel];
			const unsigned int beingIdx = offset + beginGridNo;
			const unsigned int childNum = static_cast<unsigned int>(pow(8, levelAdd));//�T���J�n�K�w����[���Ȃ�قǍׂ����Ȃ�
			const unsigned int endIdx   = offset + beginGridNo + childNum;

			for (unsigned int gridIdx = beingIdx; gridIdx < endIdx; ++gridIdx) {
				const DsGrid& grid = m_pGrids[gridIdx];
				unsigned int entityId = grid.entityId - 1;
				while (1) {
					if (entityId != DsGrid::INVALID) {
						const DsGridEntity& entity = m_pGridEntities[entityId - 1];
						const DsCollisionBuffer::ModelData& model = pModel[entity.modelIndex];
						const DsCollisionBuffer::FaceData& face = model.pFace[entity.faceIndex];
						const DsCollisionBuffer::VertexData* pVertex = model.pVertex;

						//�����œ����蔻��

						entityId = entity.nextEntityId;
					}
					else {
						break;
					}
				}
			}

			beginGridNo *= 8;
			++levelAdd;
		}
	}

	{//�e�K�w�̂����蔻��
		unsigned int currentGridNo = gridNo / 8;

		for (int currentLevel = level - 1; 0 <= currentLevel ; --currentLevel) {
			const unsigned int currentArrayIdx = gridIndexOffset[currentLevel] + currentGridNo;
			const DsGrid& grid = m_pGrids[currentArrayIdx];
			unsigned int entityId = grid.entityId - 1;
			while (1) {
				if (entityId != DsGrid::INVALID) {
					const DsGridEntity& entity = m_pGridEntities[entityId - 1];
					const DsCollisionBuffer::ModelData& model = pModel[entity.modelIndex];
					const DsCollisionBuffer::FaceData& face = model.pFace[entity.faceIndex];
					const DsCollisionBuffer::VertexData* pVertex = model.pVertex;

					//�����œ����蔻��

					entityId = entity.nextEntityId;
				}
				else {
					break;
				}
			}

			currentGridNo /= 8;
		}
	}


	return false;
}



void DsGridTree::_Build(const DsCollisionBuffer* pBuff)
{
	if (!pBuff) {
		return;
	}
	const DsCollisionBuffer::BufData* pData = pBuff->GetBuff();
	if (!pData) {
		return;
	}

	m_pBuff = pBuff;

	//�ő�AABB
/////////�O������炤�\��
	float maxP[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
	float minP[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
	const int modelNum = pData->modelNum;
	for (int modelIdx = 0; modelIdx < modelNum; ++modelIdx) {
		const DsCollisionBuffer::ModelData& model = pData->pModel[modelIdx];
		const int vertexNum = model.vertexNum;
		const DsCollisionBuffer::VertexData* pVertex = model.pVertex;
		
		for (int vertexIdx = 0; vertexIdx < vertexNum; ++vertexIdx, ++pVertex) {
			maxP[0] = max(maxP[0], pVertex->v[0]);
			maxP[1] = max(maxP[1], pVertex->v[1]);
			maxP[2] = max(maxP[2], pVertex->v[2]);
			
			minP[0] = min(minP[0], pVertex->v[0]);
			minP[1] = min(minP[1], pVertex->v[1]);
			minP[2] = min(minP[2], pVertex->v[2]);
		}
	}
////////////////////////////////////////

	//���`�W����K�w�ʔz��C���f�b�N�X�I�t�Z�b�g
	unsigned int gridIndexOffset[MAX_TREE_DEPTH+1];
	for (int level = 0; level < MAX_TREE_DEPTH+1; ++level) {
		gridIndexOffset[level] = (static_cast<unsigned int>((pow(8, level)) - 1) / 7);
	}


	//����
	const float rootLen[3]={
		maxP[0] - minP[0],
		maxP[1] - minP[1],
		maxP[2] - minP[2],
	};
	m_minP[0] = minP[0];
	m_minP[1] = minP[1];
	m_minP[2] = minP[2];

	//�P�i�q������̒���
	const float cellUnit[3] = {
		rootLen[0] / CELL_LINE_NUM_F,
		rootLen[1] / CELL_LINE_NUM_F,
		rootLen[2] / CELL_LINE_NUM_F,
	};
	m_cellUnitLen[0] = cellUnit[0];
	m_cellUnitLen[1] = cellUnit[1];
	m_cellUnitLen[2] = cellUnit[2];


	unsigned int registerEntityId = 1;//���蓖��ID

	for (int modelIdx = 0; modelIdx < modelNum; ++modelIdx) {
		const DsCollisionBuffer::ModelData& model = pData->pModel[modelIdx];
		const int vertexNum = model.vertexNum;
		const int faceNum = model.faceNum;
		const DsCollisionBuffer::VertexData* pVertex = model.pVertex;
		const DsCollisionBuffer::FaceData* pFace = model.pFace;


		for (int faceIdx = 0; faceIdx < faceNum; ++faceIdx, ++pFace) {

			//3���_���W
			const float* pV[3]{
				pVertex[pFace->index[0]].v,
				pVertex[pFace->index[1]].v,
				pVertex[pFace->index[2]].v,
			};

			//���W���i�q�ԍ�
			const unsigned int cellNo[3][3] = {
				{ static_cast<unsigned int>((pV[0][0]-minP[0]) / cellUnit[0]), static_cast<unsigned int>((pV[0][1]-minP[1]) / cellUnit[1]), static_cast<unsigned int>((pV[0][2]-minP[2]) / cellUnit[2]) },
				{ static_cast<unsigned int>((pV[1][0]-minP[0]) / cellUnit[0]), static_cast<unsigned int>((pV[1][1]-minP[1]) / cellUnit[1]), static_cast<unsigned int>((pV[1][2]-minP[2]) / cellUnit[2]) },
				{ static_cast<unsigned int>((pV[2][0]-minP[0]) / cellUnit[0]), static_cast<unsigned int>((pV[2][1]-minP[1]) / cellUnit[1]), static_cast<unsigned int>((pV[2][2]-minP[2]) / cellUnit[2]) },
			};

			const unsigned int morton[3] = { 
				_GetMortonOrder(cellNo[0][0], cellNo[0][1], cellNo[0][2]),
				_GetMortonOrder(cellNo[1][0], cellNo[1][1], cellNo[1][2]),
				_GetMortonOrder(cellNo[2][0], cellNo[2][1], cellNo[2][2]),
			};

			//���[�g�������͉��L�̂悤��3bit���ƂɊe�K�w�̏����ԍ����i�[����Ă���
			//|  3bit |  3bit |  3bit |  3bit |
			//|1�K�w��|2�K�w��|3�K�w��|4�K�w��|
			//��0�K�w��=�S�̂���AABB�͊܂܂�ĂȂ�
			//3���_����ʂ����v���肵�A����Ă��܂����O��3���_�S�Ă��܂ފK�w�ƂȂ�
			unsigned int gridNo = 0;
			unsigned int level = 0;
			unsigned int levelMorton = morton[0] ^ morton[1] ^ morton[2];//xor������ĈႤbit�������悤��
			//1�K�w�ڂ��珇�Ɉ���Ă��܂��K�w�𔻒�
			//���傤��level������K�w-1�Őe�ɂȂ��Ă�̂ŁA���ꂪ�ʂ̊K�w
			for (; level < MAX_TREE_DEPTH; ++level) {
				//���3bit�𔻒�
				const unsigned int bit = levelMorton & 0xE0000000;
				if (bit == 0) {
					//��v���Ă����̂�3���_�S�Ċ܂܂����
				}
				else {
					//3���_�̊K�w�̂ǂꂩ�̊K�w����������S���܂߂��ĂȂ�
					break;
				}
				levelMorton <<= 3;//3bit�i��Ŏ��̊K�w��
			}
			//��v���Ă����K�w�̂Ƃ���܂ł̊i�q�ԍ�
			gridNo = morton[0] >> (MAX_TREE_DEPTH - level) * 3;

			//level����gridNo�Ԗڂ��ʂ̏�����ԂƂ���

			//���`�W����.DsGrid�z��͋�Ԃ���؂�i�q���ꎟ���ɕ��ׂ�����.�K�w�ƊK�w���ԍ�����z��index�����߂邱�Ƃ��o����
			//| 0 | 1,2,3,4,5,6,7,8 | 9,10,11,12,13,14,15,16,17,18,19,20,...
			const unsigned int arrayIdx = gridIndexOffset[level] + gridNo;//��((pow(8, level))-1) / 7)�͎��O�v�Z��
			DsGrid& grid = m_pGrids[arrayIdx];
			
			//DsGrid�ɏ����G���e�B�e�B��o�^����
			//registerEntityId�̓C���N�������g���Ă���
			unsigned int useEntityId = grid.entityId;
			if(useEntityId == DsGrid::INVALID){
				//�擪�ɓo�^
				grid.entityId = registerEntityId;
				grid.endEntityId = registerEntityId;
			}
			else {
				//���X�g�ɒǉ�.entity�̔z��index��id-1�ŋ��܂�
				const unsigned int entityIdx = grid.endEntityId - 1;//��Ԃ̍Ō�����擾
				DsGridEntity& entity = m_pGridEntities[entityIdx];
				if (entity.nextEntityId == DsGridEntity::INVALID) {
					entity.nextEntityId = registerEntityId;
					grid.endEntityId = registerEntityId;//��Ԃ̍Ō�����X�V
				}
				else {
					DS_ASSERT(false, "�Ō���ɒǉ����Ă���͂��Ȃ̂�INVALID�ȊO�͂��肦�Ȃ�");
				}
			}

			//�V�K�G���e�B�e�B������
			const unsigned int registerIdx = registerEntityId - 1;
			m_pGridEntities[registerIdx].modelIndex = modelIdx;
			m_pGridEntities[registerIdx].faceIndex = faceIdx;
			m_pGridEntities[registerIdx].nextEntityId = DsGridEntity::INVALID;
			++registerEntityId;

		}


	}


}

