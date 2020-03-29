#include "DsPch.h"
#include "Collision/DsGridTree.h"
#include "Collision/DsCollisionBuffer.h"
#include "Collision/DsGridEntity.h"
#include "Collision/DsGrid.h"

using namespace DsLib;

namespace
{
	const int MAX_TREE_DEPTH = 4;
	const int CELL_LINE_NUM = 1 << 4;//1次元あたりの総セル数
	const float CELL_LINE_NUM_F = static_cast<float>(CELL_LINE_NUM);
	const int BUF_INDEX_NUM = 20000;
}

namespace
{
	size_t _CalcGridNum()
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
	return (_CalcGridNum() * sizeof(DsGrid)) + (entityNum * sizeof(DsGridEntity)) + sizeof(DsGridTree);
}

//static
DsGridTree* DsGridTree::Create(ds_uint8* pBuf, const DsCollisionBuffer* pCollisionBuf, ds_uint8* pDbgTop, size_t dbgSize)
{
	ds_uint8* pTreeBufTop = pBuf;

	//初期化は重いのでなるべく呼ばず、0クリ前提にする
	DsGridTree* pGridTree = new(pTreeBufTop) DsGridTree;
	pGridTree->m_pGrids = reinterpret_cast<DsGrid*>(pTreeBufTop + sizeof(DsGridTree));
	const size_t gridNum = _CalcGridNum();
	pGridTree->m_pGridEntities = reinterpret_cast<DsGridEntity*>(pTreeBufTop + sizeof(DsGridTree) + gridNum *sizeof(DsGrid) );

	ds_uint8* pEnd = pDbgTop + dbgSize;
	ds_uint64 diff = pEnd - (ds_uint8*)(pGridTree->m_pGridEntities);

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
	//sを3bitおきの隙間を空ける 1111 → 1001001001
	//sは座標を1セルの長さで割ったもの＝セル何個分
	unsigned int _Gap3Bit(unsigned int s)
	{
		//セル単位数はRootをMAX_TREE_DEPTH回2分割される訳だから(2の4乗)
		//sはMAX_TREE_DEPTH[bit]に収まる
		//sは8bitまでを想定(8bitを3bitの隙間を空けると24bit 増やすことは可)
		s = (s | s << 8) & 0x0000f00f;
		s = (s | s << 4) & 0x000c30c3;
		s = (s | s << 2) & 0x00249249;
		return s;
	}

	//x,y,zは１セル単位長さ
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

	//線形８分岐階層別配列インデックスオフセット
	unsigned int gridIndexOffset[MAX_TREE_DEPTH + 1];
	for (int level = 0; level < MAX_TREE_DEPTH + 1; ++level) {
		gridIndexOffset[level] = (static_cast<unsigned int>((pow(8, level)) - 1) / 7);
	}

	unsigned int gridNo = 0;
	unsigned int level = 0;
	unsigned int levelMorton = morton[0] ^ morton[1];//xorを取って違うbitだけ立つように
	//1階層目から順に違ってしまう階層を判定
	//ちょうどlevelが判定階層-1で親になってるので、これが面の階層
	for (; level < MAX_TREE_DEPTH; ++level) {
		//上位3bitを判定
		const unsigned int bit = levelMorton & 0xE0000000;
		if (bit == 0) {
			//一致していたので3頂点全て含まれる空間
		}
		else {
			//3頂点の階層のどれかの階層が違った＝全部含められてない
			break;
		}
		levelMorton <<= 3;//3bit進んで次の階層へ
	}
	//一致していた階層のところまでの格子番号
	gridNo= morton[0] >> (MAX_TREE_DEPTH - level) * 3;

	const DsCollisionBuffer::ModelData* pModel = m_pBuff->GetBuff()->pModel;

	{//同階層の当たり判定
		//階層と階層内番号から配列インデックスを求める
		const unsigned int arrayIdx = gridIndexOffset[level] + gridNo;
		const DsGrid& grid = m_pGrids[arrayIdx];
		unsigned int entityId = grid.entityId - 1;
		while (1) {
			if (entityId != DsGrid::INVALID) {
				const DsGridEntity& entity = m_pGridEntities[entityId - 1];
				const DsCollisionBuffer::ModelData& model = pModel[entity.modelIndex];
				const DsCollisionBuffer::FaceData& face = model.pFace[entity.faceIndex];
				const DsCollisionBuffer::VertexData* pVertex = model.pVertex;

				//ここで当たり判定

				entityId = entity.nextEntityId;
			}
			else {
				break;
			}
		}
	}

	{//子階層のあたり判定
		unsigned int beginGridNo = gridNo * 8;
		int levelAdd = 1;
		for (int currentLevel = level+1; currentLevel <= MAX_TREE_DEPTH; ++currentLevel) {
			//gridNo(=最初の探索空間)を含む空間の最小と最大まで探索
			//beginIdx,endIdx はgridNoの最小と最大を示す子階層の格子番号
			//beginIdx,endIdx は階層を上がっても座標的には同じgridNoの最小と最大を示す位置にあるだが,その階層での番号につけなおす(begin～end間はどんどん細かくなる=多くなる)
			const unsigned int offset = gridIndexOffset[currentLevel];
			const unsigned int beingIdx = offset + beginGridNo;
			const unsigned int childNum = static_cast<unsigned int>(pow(8, levelAdd));//探索開始階層から深くなるほど細かくなる
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

						//ここで当たり判定

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

	{//親階層のあたり判定
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

					//ここで当たり判定

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

	//最大AABB
/////////外からもらう予定
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

	//線形８分岐階層別配列インデックスオフセット
	unsigned int gridIndexOffset[MAX_TREE_DEPTH+1];
	for (int level = 0; level < MAX_TREE_DEPTH+1; ++level) {
		gridIndexOffset[level] = (static_cast<unsigned int>((pow(8, level)) - 1) / 7);
	}


	//長さ
	const float rootLen[3]={
		maxP[0] - minP[0],
		maxP[1] - minP[1],
		maxP[2] - minP[2],
	};
	m_minP[0] = minP[0];
	m_minP[1] = minP[1];
	m_minP[2] = minP[2];

	//１格子あたりの長さ
	const float cellUnit[3] = {
		rootLen[0] / CELL_LINE_NUM_F,
		rootLen[1] / CELL_LINE_NUM_F,
		rootLen[2] / CELL_LINE_NUM_F,
	};
	m_cellUnitLen[0] = cellUnit[0];
	m_cellUnitLen[1] = cellUnit[1];
	m_cellUnitLen[2] = cellUnit[2];


	unsigned int registerEntityId = 1;//割り当てID

	for (int modelIdx = 0; modelIdx < modelNum; ++modelIdx) {
		const DsCollisionBuffer::ModelData& model = pData->pModel[modelIdx];
		const int vertexNum = model.vertexNum;
		const int faceNum = model.faceNum;
		const DsCollisionBuffer::VertexData* pVertex = model.pVertex;
		const DsCollisionBuffer::FaceData* pFace = model.pFace;


		for (int faceIdx = 0; faceIdx < faceNum; ++faceIdx, ++pFace) {

			//3頂点座標
			const float* pV[3]{
				pVertex[pFace->index[0]].v,
				pVertex[pFace->index[1]].v,
				pVertex[pFace->index[2]].v,
			};

			//座標→格子番号
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

			//モートン順序は下記のように3bitごとに各階層の所属番号が格納されている
			//|  3bit |  3bit |  3bit |  3bit |
			//|1階層目|2階層目|3階層目|4階層目|
			//※0階層目=全体を包むAABBは含まれてない
			//3頂点を上位から一致判定し、違ってしまう直前が3頂点全てを含む階層となる
			unsigned int gridNo = 0;
			unsigned int level = 0;
			unsigned int levelMorton = morton[0] ^ morton[1] ^ morton[2];//xorを取って違うbitだけ立つように
			//1階層目から順に違ってしまう階層を判定
			//ちょうどlevelが判定階層-1で親になってるので、これが面の階層
			for (; level < MAX_TREE_DEPTH; ++level) {
				//上位3bitを判定
				const unsigned int bit = levelMorton & 0xE0000000;
				if (bit == 0) {
					//一致していたので3頂点全て含まれる空間
				}
				else {
					//3頂点の階層のどれかの階層が違った＝全部含められてない
					break;
				}
				levelMorton <<= 3;//3bit進んで次の階層へ
			}
			//一致していた階層のところまでの格子番号
			gridNo = morton[0] >> (MAX_TREE_DEPTH - level) * 3;

			//level内のgridNo番目が面の所属空間とする

			//線形８分木.DsGrid配列は空間を区切る格子を一次元に並べたもの.階層と階層内番号から配列indexを求めることが出来る
			//| 0 | 1,2,3,4,5,6,7,8 | 9,10,11,12,13,14,15,16,17,18,19,20,...
			const unsigned int arrayIdx = gridIndexOffset[level] + gridNo;//←((pow(8, level))-1) / 7)は事前計算可
			DsGrid& grid = m_pGrids[arrayIdx];
			
			//DsGridに所属エンティティを登録する
			//registerEntityIdはインクリメントしていく
			unsigned int useEntityId = grid.entityId;
			if(useEntityId == DsGrid::INVALID){
				//先頭に登録
				grid.entityId = registerEntityId;
				grid.endEntityId = registerEntityId;
			}
			else {
				//リストに追加.entityの配列indexはid-1で求まる
				const unsigned int entityIdx = grid.endEntityId - 1;//空間の最後尾を取得
				DsGridEntity& entity = m_pGridEntities[entityIdx];
				if (entity.nextEntityId == DsGridEntity::INVALID) {
					entity.nextEntityId = registerEntityId;
					grid.endEntityId = registerEntityId;//空間の最後尾を更新
				}
				else {
					DS_ASSERT(false, "最後尾に追加しているはずなのでINVALID以外はありえない");
				}
			}

			//新規エンティティ初期化
			const unsigned int registerIdx = registerEntityId - 1;
			m_pGridEntities[registerIdx].modelIndex = modelIdx;
			m_pGridEntities[registerIdx].faceIndex = faceIdx;
			m_pGridEntities[registerIdx].nextEntityId = DsGridEntity::INVALID;
			++registerEntityId;

		}
	}
}

void DsGridTree::DbgDraw(DsDrawCommand& draw)
{
	//各格子の中心点を生成し、実際に当たり判定をさせる


	{//格子頂点データ用意
		const int num = static_cast<int>(pow(2, MAX_TREE_DEPTH));//一片の格子数
		for (int x = 0; x < num; ++x) {
			for (int y = 0; y < num; ++y) {
				for (int z = 0; z < num; ++z) {

					const float xCell = m_cellUnitLen[0] * x;
					const float yCell = m_cellUnitLen[1] * y;
					const float zCell = m_cellUnitLen[2] * z;

					const float pos[3] =
					{
						m_minP[0] + xCell,
						m_minP[1] + yCell,
						m_minP[2] + zCell,
					};

					if (Test(pos, pos))
					{
						//描画
					}

				}
			}

		}
	}



	

}