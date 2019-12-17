#include "DsPch.h"
#include "Collision/DsAabbTree.h"
#include "Collision/DsAabbNode.h"
#include "Collision/DsCollisionBuffer.h"


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

	size_t _CalcIndexBufSize(int indexNum)
	{
		return sizeof(int)*indexNum;
	}
}

//static
size_t DsAabbTree::CalcMemSize()
{
	return _CalcTreeSize() * sizeof(DsAabbNode) + sizeof(DsAabbTree);
}

DsAabbTree::DsAabbTree()
	: m_pNodeBuf(NULL)
	, m_pVertex(NULL)
	, m_pIndex(NULL)
	, m_pIndexBuf(NULL)
	, m_vertexNum(0)
	, m_indexNum(0)
	, m_treeSize(0)
	, m_useIndex(0)
	, m_maxDepth(MAX_TREE_DEPTH)
	, m_cellUnitLen{1.0f, 1.0f, 1.0f}
{
}


DsAabbTree::~DsAabbTree()
{
	//delete m_pNodeBuf;
	m_pNodeBuf = NULL;
}


void DsAabbTree::Init(const float* pVetex, int vertexNum, const int* pIndex, int indexNum, ds_uint8* pBuf)
{
	m_pVertex = pVetex;
	m_pIndex = pIndex;
	m_vertexNum = vertexNum;
	m_indexNum = indexNum;
	m_treeSize = indexNum * 2;
	//m_pNodeBuf = new(pBuf) DsAabbNode[m_treeSize];

	
	TreeUpdate();
}

namespace
{
	/*
		�����̂�8����index
		����4��
		3 2
		0 1
		�㕔��
		7 6
		4 5
	*/

	void _DivAabb0(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1]         , minV[2]         ));
	}
	void _DivAabb1(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1]         , minV[2]         ));
	}
	void _DivAabb2(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1]         , minV[2] + len[2]));
	}
	void _DivAabb3(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0]         , minV[1]         , minV[2] + len[2]));
	}
	void _DivAabb4(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1] - len[1], minV[2]         ));
	}
	void _DivAabb5(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2]         ));
	}
	void _DivAabb6(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2] + len[2]));
	}
	void _DivAabb7(DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3])
	{
		aabb.Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2]         ), DsVec3d(minV[0]         , minV[1] - len[1], minV[2] + len[2]));
	}
	
	void (* const _DivAabb[8]) (DsAabbf& aabb, const float maxV[3], const float minV[3], const float len[3]) =
	{
		&_DivAabb0,
		&_DivAabb1,
		&_DivAabb2,
		&_DivAabb3,
		&_DivAabb4,
		&_DivAabb5,
		&_DivAabb6,
		&_DivAabb7,
	};
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


void DsAabbTree::TreeUpdate()
{
	float maxV[3];
	float minV[3];

	const float* pVertex = m_pVertex;
	for (int vi = 0; vi < m_vertexNum; ++vi) {
		float x = pVertex[0];
		float y = pVertex[1];
		float z = pVertex[2];
		maxV[0] = max(maxV[0], x);
		maxV[1] = max(maxV[1], y);
		maxV[2] = max(maxV[2], z);
		minV[0] = min(minV[0], x);
		minV[1] = min(minV[1], y);
		minV[2] = min(minV[2], z);
	}

	m_useIndex = 0;
	DsAabbNode* pRoot = _Create();
	pRoot->m_aabb.Setup(DsVec3d(maxV[0], maxV[1], maxV[2]), DsVec3d(minV[0], minV[1], minV[2]));
	pRoot->m_parentIdx = -1;

	float len[3];
	len[0] = pRoot->m_aabb.LenX();
	len[1] = pRoot->m_aabb.LenY();
	len[2] = pRoot->m_aabb.LenZ();
	
	_CreateChild(*pRoot, m_pIndex, m_indexNum, len, 0);
}

void DsAabbTree::_CreateChild(DsAabbNode& parent, const int* pIndex, int indexNum, const float* len, int depth)
{
	//const float* maxV = parent.m_aabb.m_maxV;
	//const float* minV = parent.m_aabb.m_minV;
	//DsAabbf aabb[8];
	//aabb[0].Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1]         , minV[2]         ));
	//aabb[1].Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1]         , minV[2]         ));
	//aabb[2].Setup(DsVec3d(maxV[0]         , maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1]         , minV[2] + len[2]));
	//aabb[3].Setup(DsVec3d(maxV[0] - len[0], maxV[1] - len[1], maxV[2]         ), DsVec3d(minV[0]         , minV[1]         , minV[2] + len[2]));
	//aabb[4].Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0]         , minV[1] - len[1], minV[2]         ));
	//aabb[5].Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2] - len[2]), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2]         ));
	//aabb[6].Setup(DsVec3d(maxV[0]         , maxV[1]         , maxV[2]         ), DsVec3d(minV[0] + len[0], minV[1] - len[1], minV[2] + len[2]));
	//aabb[7].Setup(DsVec3d(maxV[0] - len[0], maxV[1]         , maxV[2]         ), DsVec3d(minV[0]         , minV[1] - len[1], minV[2] + len[2]));

	if (m_maxDepth < depth) {
		return;
	}

	//���[�g�������𒲂ׂ�

	int indexBuf[20000];
	for (int child = 0; child < 8; ++child) {

		DsAabbf childAabb;
		_DivAabb[child](childAabb, parent.m_aabb.m_maxV, parent.m_aabb.m_minV, len);
		
		int bufNum = 0;
		for (int idx = 0; idx < indexNum; idx += 3) {
			const int val0 = pIndex[idx];
			const int val1 = pIndex[idx+1];
			const int val2 = pIndex[idx+2];

			const DsVec3f& a = DsVec3f(m_pVertex[val0], m_pVertex[val0+1], m_pVertex[val0+2]);
			const DsVec3f& b = DsVec3f(m_pVertex[val1], m_pVertex[val1+1], m_pVertex[val1+2]);
			const DsVec3f& c = DsVec3f(m_pVertex[val2], m_pVertex[val2+1], m_pVertex[val2+2]);
			DsAabbf tmp;
			tmp.Setup(DsVec3f::Max(DsVec3f::Max(a, b), c), DsVec3f::Min(DsVec3f::Min(a, b), c));

			if (DsAabbf::IsContain(tmp, childAabb)) {
				indexBuf[bufNum]     = val0;
				indexBuf[bufNum + 1] = val1;
				indexBuf[bufNum + 2] = val2;
				bufNum += 3;
			}
		}

		if (0 < bufNum) {
			DsAabbNode* pChild = _Create();
			if (pChild) {
				pChild->m_aabb = childAabb;
				pChild->m_parentIdx = parent.m_myIdx;
				parent.m_childIdx[child] = pChild->m_myIdx;
				const float tmpLen[3] = 
				{
					childAabb.LenX(),
					childAabb.LenY(),
					childAabb.LenZ(),
				};
				_CreateChild(*pChild, indexBuf, bufNum, tmpLen, depth+1);
			}
		}
		else {
			parent.m_childIdx[child] = -1;
		}
	}
}

DsAabbNode* DsAabbTree::_Create()
{
	DsAabbNode* ret = NULL;
	if (m_useIndex < m_treeSize) {
		ret = &m_pNodeBuf[m_useIndex];
		ret->m_myIdx = m_useIndex;
		++m_useIndex;
	}
	return ret;
}

bool DsAabbTree::Test(const DsShapeSegment& seg) const
{

	return false;
}



void DsAabbTree::Build(const DsCollisionBuffer* pBuff)
{
	if (!pBuff) {
		return;
	}
	const DsCollisionBuffer::BufData* pData = pBuff->GetBuff();
	if (!pData) {
		return;
	}

	//�ő�AABB
	//�O����������������������
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

	//����
	const float rootLen[3]={
		maxP[0] - minP[0],
		maxP[1] - minP[1],
		maxP[2] - minP[2],
	};

	//�P�i�q������̒���
	const float cellUnit[3] = {
		rootLen[0] / CELL_LINE_NUM_F,
		rootLen[1] / CELL_LINE_NUM_F,
		rootLen[2] / CELL_LINE_NUM_F,
	};
	m_cellUnitLen[0] = cellUnit[0];
	m_cellUnitLen[1] = cellUnit[1];
	m_cellUnitLen[2] = cellUnit[2];

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
			unsigned int faceNo = 0;
			unsigned int level = 0;
			unsigned int levelMorton = morton[0] ^ morton[1] ^ morton[2];//xor������ĈႤbit�������悤��
			//1�K�w�ڂ��珇�Ɉ���Ă��܂��K�w�𔻒�
			//���傤��level������K�w-1�Őe�ɂȂ��Ă�̂ŁA���ꂪ�ʂ̊K�w
			for (; level < MAX_TREE_DEPTH; ++level) {
				//���3bit�𔻒�
				const unsigned int bit = levelMorton & 0xE0000000;
				if (bit == 0) {
					//��v���Ă����̂�3���_�S�Ċ܂܂����
					faceNo = bit;
				}
				else {
					//3���_�̊K�w�̂ǂꂩ�̊K�w����������S���܂߂��ĂȂ�
					break;
				}
				levelMorton <<= 3;//3bit�i��Ŏ��̊K�w��
			}
			//level����faceNo�Ԗڂ��ʂ̏�����ԂƂ���

			//���`�W����
			//| 0 | 1,2,3,4,5,6,7,8 | 9,10,11,12,13,14,15,16,17,18,19,20,...
			const int arrayIdx = ( static_cast<int>((pow(8, level))-1) / 7) + faceNo;
			//m_pNodeBuf[arrayIdx].add(arrayIdx)
			//AabbNode��vector��������H
			//FaceData��next face index ��ǉ����ă��X�g�ŕۑ����Ă݂�H
		}


	}


}

