#include "DsPch.h"
#include "Collision/DsGridTree.h"
#include "Collision/DsCollisionBuffer.h"
#include "Collision/DsGridEntity.h"
#include "Collision/DsGrid.h"

using namespace DsLib;

namespace
{
	const int MAX_TREE_DEPTH = 4;
	const int CELL_LINE_NUM = 1 << 4;//1ŸŒ³‚ ‚½‚è‚Ì‘ƒZƒ‹”
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
	//const size_t entitySize = gridTreeSize - sizeof(DsGridTree);//c‚è‚ªDsGridEntity”z—ñ
	//const size_t entityNum = entitySize / sizeof(DsGridEntity);//”z—ñ‚ÌŒÂ”‚É•ÏŠ·
	//pGridTree->m_pGrids = new(pBuf + sizeof(DsGridTree)) DsGridEntity[entityNum]; //reinterpret_cast<DsGridEntity*>(pBuf + sizeof(DsGridTree));

	//‰Šú‰»‚Íd‚¢‚Ì‚Å‚È‚é‚×‚­ŒÄ‚Î‚¸A0ƒNƒŠ‘O’ñ‚É‚·‚é
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
	//s‚ğ3bit‚¨‚«‚ÌŒ„ŠÔ‚ğ‹ó‚¯‚é 1111 ¨ 1001001001
	//s‚ÍÀ•W‚ğ1ƒZƒ‹‚Ì’·‚³‚ÅŠ„‚Á‚½‚à‚ÌƒZƒ‹‰½ŒÂ•ª
	unsigned int _Gap3Bit(unsigned int s)
	{
		//ƒZƒ‹’PˆÊ”‚ÍRoot‚ğMAX_TREE_DEPTH‰ñ2•ªŠ„‚³‚ê‚é–ó‚¾‚©‚ç(2‚Ì4æ)
		//s‚ÍMAX_TREE_DEPTH[bit]‚Éû‚Ü‚é
		//s‚Í8bit‚Ü‚Å‚ğ‘z’è(8bit‚ğ3bit‚ÌŒ„ŠÔ‚ğ‹ó‚¯‚é‚Æ24bit ‘‚â‚·‚±‚Æ‚Í‰Â)
		s = (s | s << 8) & 0x0000f00f;
		s = (s | s << 4) & 0x000c30c3;
		s = (s | s << 2) & 0x00249249;
		return s;
	}

	//x,y,z‚Í‚PƒZƒ‹’PˆÊ’·‚³
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

	//üŒ`‚W•ªŠòŠK‘w•Ê”z—ñƒCƒ“ƒfƒbƒNƒXƒIƒtƒZƒbƒg
	unsigned int gridIndexOffset[MAX_TREE_DEPTH + 1];
	for (int level = 0; level < MAX_TREE_DEPTH + 1; ++level) {
		gridIndexOffset[level] = (static_cast<unsigned int>((pow(8, level)) - 1) / 7);
	}

	unsigned int gridNo = 0;
	unsigned int level = 0;
	unsigned int levelMorton = morton[0] ^ morton[1];//xor‚ğæ‚Á‚Äˆá‚¤bit‚¾‚¯—§‚Â‚æ‚¤‚É
	//1ŠK‘w–Ú‚©‚ç‡‚Éˆá‚Á‚Ä‚µ‚Ü‚¤ŠK‘w‚ğ”»’è
	//‚¿‚å‚¤‚Çlevel‚ª”»’èŠK‘w-1‚Åe‚É‚È‚Á‚Ä‚é‚Ì‚ÅA‚±‚ê‚ª–Ê‚ÌŠK‘w
	for (; level < MAX_TREE_DEPTH; ++level) {
		//ãˆÊ3bit‚ğ”»’è
		const unsigned int bit = levelMorton & 0xE0000000;
		if (bit == 0) {
			//ˆê’v‚µ‚Ä‚¢‚½‚Ì‚Å3’¸“_‘S‚ÄŠÜ‚Ü‚ê‚é‹óŠÔ
		}
		else {
			//3’¸“_‚ÌŠK‘w‚Ì‚Ç‚ê‚©‚ÌŠK‘w‚ªˆá‚Á‚½‘S•”ŠÜ‚ß‚ç‚ê‚Ä‚È‚¢
			break;
		}
		levelMorton <<= 3;//3biti‚ñ‚ÅŸ‚ÌŠK‘w‚Ö
	}
	//ˆê’v‚µ‚Ä‚¢‚½ŠK‘w‚Ì‚Æ‚±‚ë‚Ü‚Å‚ÌŠiq”Ô†
	gridNo= morton[0] >> (MAX_TREE_DEPTH - level) * 3;

	const DsCollisionBuffer::ModelData* pModel = m_pBuff->GetBuff()->pModel;

	{//“¯ŠK‘w‚Ì“–‚½‚è”»’è
		//ŠK‘w‚ÆŠK‘w“à”Ô†‚©‚ç”z—ñƒCƒ“ƒfƒbƒNƒX‚ğ‹‚ß‚é
		const unsigned int arrayIdx = gridIndexOffset[level] + gridNo;
		const DsGrid& grid = m_pGrids[arrayIdx];
		unsigned int entityId = grid.entityId - 1;
		while (1) {
			if (entityId != DsGrid::INVALID) {
				const DsGridEntity& entity = m_pGridEntities[entityId - 1];
				const DsCollisionBuffer::ModelData& model = pModel[entity.modelIndex];
				const DsCollisionBuffer::FaceData& face = model.pFace[entity.faceIndex];
				const DsCollisionBuffer::VertexData* pVertex = model.pVertex;

				//‚±‚±‚Å“–‚½‚è”»’è

				entityId = entity.nextEntityId;
			}
			else {
				break;
			}
		}
	}

	{//qŠK‘w‚Ì‚ ‚½‚è”»’è
		unsigned int beginGridNo = gridNo * 8;
		int levelAdd = 1;
		for (int currentLevel = level+1; currentLevel <= MAX_TREE_DEPTH; ++currentLevel) {
			//gridNo(=Å‰‚Ì’Tõ‹óŠÔ)‚ğŠÜ‚Ş‹óŠÔ‚ÌÅ¬‚ÆÅ‘å‚Ü‚Å’Tõ
			//beginIdx,endIdx ‚ÍgridNo‚ÌÅ¬‚ÆÅ‘å‚ğ¦‚·qŠK‘w‚ÌŠiq”Ô†
			//beginIdx,endIdx ‚ÍŠK‘w‚ğã‚ª‚Á‚Ä‚àÀ•W“I‚É‚Í“¯‚¶gridNo‚ÌÅ¬‚ÆÅ‘å‚ğ¦‚·ˆÊ’u‚É‚ ‚é‚¾‚ª,‚»‚ÌŠK‘w‚Å‚Ì”Ô†‚É‚Â‚¯‚È‚¨‚·(begin`endŠÔ‚Í‚Ç‚ñ‚Ç‚ñ×‚©‚­‚È‚é=‘½‚­‚È‚é)
			const unsigned int offset = gridIndexOffset[currentLevel];
			const unsigned int beingIdx = offset + beginGridNo;
			const unsigned int childNum = static_cast<unsigned int>(pow(8, levelAdd));//’TõŠJnŠK‘w‚©‚ç[‚­‚È‚é‚Ù‚Ç×‚©‚­‚È‚é
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

						//‚±‚±‚Å“–‚½‚è”»’è

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

	{//eŠK‘w‚Ì‚ ‚½‚è”»’è
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

					//‚±‚±‚Å“–‚½‚è”»’è

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

	//Å‘åAABB
/////////ŠO‚©‚ç‚à‚ç‚¤—\’è
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

	//üŒ`‚W•ªŠòŠK‘w•Ê”z—ñƒCƒ“ƒfƒbƒNƒXƒIƒtƒZƒbƒg
	unsigned int gridIndexOffset[MAX_TREE_DEPTH+1];
	for (int level = 0; level < MAX_TREE_DEPTH+1; ++level) {
		gridIndexOffset[level] = (static_cast<unsigned int>((pow(8, level)) - 1) / 7);
	}


	//’·‚³
	const float rootLen[3]={
		maxP[0] - minP[0],
		maxP[1] - minP[1],
		maxP[2] - minP[2],
	};
	m_minP[0] = minP[0];
	m_minP[1] = minP[1];
	m_minP[2] = minP[2];

	//‚PŠiq‚ ‚½‚è‚Ì’·‚³
	const float cellUnit[3] = {
		rootLen[0] / CELL_LINE_NUM_F,
		rootLen[1] / CELL_LINE_NUM_F,
		rootLen[2] / CELL_LINE_NUM_F,
	};
	m_cellUnitLen[0] = cellUnit[0];
	m_cellUnitLen[1] = cellUnit[1];
	m_cellUnitLen[2] = cellUnit[2];


	unsigned int registerEntityId = 1;//Š„‚è“–‚ÄID

	for (int modelIdx = 0; modelIdx < modelNum; ++modelIdx) {
		const DsCollisionBuffer::ModelData& model = pData->pModel[modelIdx];
		const int vertexNum = model.vertexNum;
		const int faceNum = model.faceNum;
		const DsCollisionBuffer::VertexData* pVertex = model.pVertex;
		const DsCollisionBuffer::FaceData* pFace = model.pFace;


		for (int faceIdx = 0; faceIdx < faceNum; ++faceIdx, ++pFace) {

			//3’¸“_À•W
			const float* pV[3]{
				pVertex[pFace->index[0]].v,
				pVertex[pFace->index[1]].v,
				pVertex[pFace->index[2]].v,
			};

			//À•W¨Šiq”Ô†
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

			//ƒ‚[ƒgƒ“‡˜‚Í‰º‹L‚Ì‚æ‚¤‚É3bit‚²‚Æ‚ÉŠeŠK‘w‚ÌŠ‘®”Ô†‚ªŠi”[‚³‚ê‚Ä‚¢‚é
			//|  3bit |  3bit |  3bit |  3bit |
			//|1ŠK‘w–Ú|2ŠK‘w–Ú|3ŠK‘w–Ú|4ŠK‘w–Ú|
			//¦0ŠK‘w–Ú=‘S‘Ì‚ğ•ï‚ŞAABB‚ÍŠÜ‚Ü‚ê‚Ä‚È‚¢
			//3’¸“_‚ğãˆÊ‚©‚çˆê’v”»’è‚µAˆá‚Á‚Ä‚µ‚Ü‚¤’¼‘O‚ª3’¸“_‘S‚Ä‚ğŠÜ‚ŞŠK‘w‚Æ‚È‚é
			unsigned int gridNo = 0;
			unsigned int level = 0;
			unsigned int levelMorton = morton[0] ^ morton[1] ^ morton[2];//xor‚ğæ‚Á‚Äˆá‚¤bit‚¾‚¯—§‚Â‚æ‚¤‚É
			//1ŠK‘w–Ú‚©‚ç‡‚Éˆá‚Á‚Ä‚µ‚Ü‚¤ŠK‘w‚ğ”»’è
			//‚¿‚å‚¤‚Çlevel‚ª”»’èŠK‘w-1‚Åe‚É‚È‚Á‚Ä‚é‚Ì‚ÅA‚±‚ê‚ª–Ê‚ÌŠK‘w
			for (; level < MAX_TREE_DEPTH; ++level) {
				//ãˆÊ3bit‚ğ”»’è
				const unsigned int bit = levelMorton & 0xE0000000;
				if (bit == 0) {
					//ˆê’v‚µ‚Ä‚¢‚½‚Ì‚Å3’¸“_‘S‚ÄŠÜ‚Ü‚ê‚é‹óŠÔ
				}
				else {
					//3’¸“_‚ÌŠK‘w‚Ì‚Ç‚ê‚©‚ÌŠK‘w‚ªˆá‚Á‚½‘S•”ŠÜ‚ß‚ç‚ê‚Ä‚È‚¢
					break;
				}
				levelMorton <<= 3;//3biti‚ñ‚ÅŸ‚ÌŠK‘w‚Ö
			}
			//ˆê’v‚µ‚Ä‚¢‚½ŠK‘w‚Ì‚Æ‚±‚ë‚Ü‚Å‚ÌŠiq”Ô†
			gridNo = morton[0] >> (MAX_TREE_DEPTH - level) * 3;

			//level“à‚ÌgridNo”Ô–Ú‚ª–Ê‚ÌŠ‘®‹óŠÔ‚Æ‚·‚é

			//üŒ`‚W•ª–Ø.DsGrid”z—ñ‚Í‹óŠÔ‚ğ‹æØ‚éŠiq‚ğˆêŸŒ³‚É•À‚×‚½‚à‚Ì.ŠK‘w‚ÆŠK‘w“à”Ô†‚©‚ç”z—ñindex‚ğ‹‚ß‚é‚±‚Æ‚ªo—ˆ‚é
			//| 0 | 1,2,3,4,5,6,7,8 | 9,10,11,12,13,14,15,16,17,18,19,20,...
			const unsigned int arrayIdx = gridIndexOffset[level] + gridNo;//©((pow(8, level))-1) / 7)‚Í–‘OŒvZ‰Â
			DsGrid& grid = m_pGrids[arrayIdx];
			
			//DsGrid‚ÉŠ‘®ƒGƒ“ƒeƒBƒeƒB‚ğ“o˜^‚·‚é
			//registerEntityId‚ÍƒCƒ“ƒNƒŠƒƒ“ƒg‚µ‚Ä‚¢‚­
			unsigned int useEntityId = grid.entityId;
			if(useEntityId == DsGrid::INVALID){
				//æ“ª‚É“o˜^
				grid.entityId = registerEntityId;
				grid.endEntityId = registerEntityId;
			}
			else {
				//ƒŠƒXƒg‚É’Ç‰Á.entity‚Ì”z—ñindex‚Íid-1‚Å‹‚Ü‚é
				const unsigned int entityIdx = grid.endEntityId - 1;//‹óŠÔ‚ÌÅŒã”ö‚ğæ“¾
				DsGridEntity& entity = m_pGridEntities[entityIdx];
				if (entity.nextEntityId == DsGridEntity::INVALID) {
					entity.nextEntityId = registerEntityId;
					grid.endEntityId = registerEntityId;//‹óŠÔ‚ÌÅŒã”ö‚ğXV
				}
				else {
					DS_ASSERT(false, "ÅŒã”ö‚É’Ç‰Á‚µ‚Ä‚¢‚é‚Í‚¸‚È‚Ì‚ÅINVALIDˆÈŠO‚Í‚ ‚è‚¦‚È‚¢");
				}
			}

			//V‹KƒGƒ“ƒeƒBƒeƒB‰Šú‰»
			const unsigned int registerIdx = registerEntityId - 1;
			m_pGridEntities[registerIdx].modelIndex = modelIdx;
			m_pGridEntities[registerIdx].faceIndex = faceIdx;
			m_pGridEntities[registerIdx].nextEntityId = DsGridEntity::INVALID;
			++registerEntityId;

		}


	}


}

