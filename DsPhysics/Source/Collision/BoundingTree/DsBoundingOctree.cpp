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

どの物体とどの物体が当たっているか、の捜査を絞るための空間分割。
ツリー状になってて、一番大きな、全ての物体を包む空間がルート空間
それを8分割したのが子空間
さらにその子空間それぞれを8分割し・・・以下繰り返しで構成される空間。


とある空間Aに所属する物体は、同じAに所属する物体か、親の空間に所属する物体にのみ当たる可能性がある。
というのを一番下の階層からやっていけば、全ての当たる可能性のパターンを網羅できる。
(実際は高速化のために再帰関数は使わず、whileで巡回する。)


毎フレUpdateで、物体がどの空間に所属するのかを更新してやる必要がある。
なるべく高速に所属グループを更新したいので、座標値がそのまま格子番号になるようにしている(ちょっとできてないけど)。
物体の所属空間は、その物体を完全に内包する空間を選ぶようにする。
そうしないと、物体が所属空間をはみ出てしまい、「同じ空間か、親としか当たる可能性がない」の前提が崩れる。
空間が小さすぎて入らない場合は、収まるサイズになるまで親を遡る。そうして所属空間の階層を決めている(この検索が微妙かも。１発で求めたい)。

↓一発で求められてるので参考にしたい。
http://marupeke296.com/COL_2D_No8_QuadTree.html


CreateTree(const int resolution)
引数は、一番下の層の（つまり一番細かい）空間分割数を指定する。

例えば、64なら、ルート空間を縦横奥に64分割した空間が一番細かい分割数の階層として作られる。
そこから、2*2*2のサイズの空間を1つの空間として作りそれを親する。以上を繰り返して中間の層を作る。

一番細かい分割数を指定する方が精度と速度の調整がしやすいだろう・・・と思ったため。


ちなみにルート空間のサイズは全物体が収まるサイズに毎フレ計算しなおしている。
活動範囲が静的に求まる場合用に、固定値版も用意したほうがいいかもしれない(もともと毎フレ更新する前提なのであとから設定しなおすことは全然問題ない)

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
resolution 分割数。ノード数は2つの子をもつ木を作る関係上resolutionは2~n でなければだめ。
*/
void DsBoundingOctree::CreateTree(const int resolution)
{
	DS_ASSERT(2 <= resolution, "DsBoundingOctree作成失敗");
	m_resolution = resolution;

	//末端ノードを全て作る
	DsBdOctreeNode* pTerminals = NULL;
	DsBdOctreeNode* pListNode = NULL;
	{
		//Actorの座標をオフセットして分割数で割った整数値(x, y, z)からどのグリッド所属かわかるようなノード構成
		//ノードインデックス = x*resolution*resolution + y*resolution + z
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

		//単純ループアクセス用
		pNode[nodeNum - 1].next = NULL;//末端
		for (long i = nodeNum-2; 0 <= i; --i){
			pNode[i].next = &pNode[i + 1];
		}
		pListNode = m_pTerminal;
	}

	//子ノードから親ノードを生成していく。
	DsBdOctreeNode* pParent = m_pTerminal;
	{
		int parentResolution = resolution;//resolutionは2のn乗
		//子から親を生成してく
		while (1){
			const int childResolution = parentResolution;
			parentResolution /= 2;
			if (0 < parentResolution){
				DsBdOctreeNode* pChild = pParent;
				const int nodeNum = parentResolution*parentResolution*parentResolution;
				pParent = new DsBdOctreeNode[nodeNum];
				m_allNodeNum += nodeNum;

				//リストアクセス用
				pParent[nodeNum - 1].next = pListNode;//親の末端と子の先頭を繋げる
				for (long i = nodeNum - 2; 0 <= i; --i){
					pParent[i].next = &pParent[i + 1];
				}
				pListNode = pParent;//リストの先頭更新

				const int childResolution2 = childResolution*childResolution;
				////子インデックスは各軸 2n と 2n+1 の2つずつで計8。
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
							pSetNode->child = pChild + (xOffset0 + yOffset0 + zOffset0);//子供は一番左端だけでいい。

							//子供たちの兄弟関係と親を設定
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


	//配列でアクセスする用
	m_pAllNode = new DsBdOctreeNode*[m_allNodeNum];
	int nodeNum = 0;
	DsBdOctreeNode* tmp = m_pRoot;
	while (tmp) {
		m_pAllNode[nodeNum] = tmp;
		tmp->actor = NULL;
		tmp = tmp->next;
		++nodeNum;
	}
	DS_ASSERT(nodeNum == m_allNodeNum, "DsBdOctreeNodeの総数が合わない");
}



/*
ツリーにアクターを登録する
allActorMinPos 全てのアクターのもっとも小さいAABBの端座標
allActorMaxPos 全てのアクターのもっとも大きいAABBの端座標
*/
void DsBoundingOctree::Update(DsActor** pActors, const int actNum, const DsVec3d& allActorMinPos, const DsVec3d& allActorMaxPos)
{
	m_allActorMaxPos = allActorMaxPos;
	m_allActorMinPos = allActorMinPos;
	m_isExistActor = 0 < actNum;

	//所属actorクリア
	DsBdOctreeNode** tmp = m_pAllNode;
	for (int i = 0; i < m_allNodeNum; ++i, ++tmp) {
		(*tmp)->actor = NULL;
	}

	const DsVec3d gridSize = DsVec3d::Abs(allActorMaxPos - allActorMinPos) / static_cast<double>(m_resolution);

	for (int actIdx = 0; actIdx < actNum; ++actIdx) {
		DsActor* pActor = pActors[actIdx];

		//pActorが完全に内包される分割空間のノードを求める。

		const DsAabb aabb = pActor->RefAabb();
		const DsVec3d maxPos = aabb.GetMax();
		const DsVec3d minPos = aabb.GetMin();
		//各成分の座標が何番目の格子に相当するのか。
		//一番大きい座標の格子番号
		const DsVec3d maxGridPos = DsVec3d::Div(maxPos - allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);//ぴったりMaxだと１つ分はみ出ることになるので-FLT_EPSILON。
		const int maxGridNum[3] = {
			static_cast<int>(maxGridPos.x),
			static_cast<int>(maxGridPos.y),
			static_cast<int>(maxGridPos.z),
		};
		//一番小さい座標の格子番号
		const DsVec3d minGridPos = DsVec3d::Div(minPos - allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);;
		const int minGridNum[3] = {
			static_cast<int>(minGridPos.x),
			static_cast<int>(minGridPos.y),
			static_cast<int>(minGridPos.z),
		};
		//この時点でマイナスにはならないはず。

		const int resolution2 = m_resolution*m_resolution;
		const int maxIdx = maxGridNum[0] * resolution2 + maxGridNum[1] * m_resolution + maxGridNum[2];//一番大きな座標の所属する格子のノードへのインデックス
		const int minIdx = minGridNum[0] * resolution2 + minGridNum[1] * m_resolution + minGridNum[2];//一番小さな座標の所属する格子のノードへのインデックス

		//最大座標と最小座標のグリッドの親をさかのぼっていって、同じになれば両方が含まれる空間＝そのactorを完全に含む空間
		DsBdOctreeNode* pMaxParent = m_pTerminal[maxIdx].parent;
		DsBdOctreeNode* pMinParent = m_pTerminal[minIdx].parent;
		while (pMaxParent != pMinParent) {//最悪のループ回数は最大階層分。おそらく最大階層は7～8ぐらいになると予想
			pMaxParent = pMaxParent->parent;
			pMinParent = pMinParent->parent;
		}//少なくともルート空間で必ず一致するはず。NULLなら実装ミス
		DS_ASSERT(pMaxParent && pMinParent, "DsBoundingOctreeで所属空間が見つからないactorがいる %S", pActor->GetName());

		 //所属空間が分かったのでここに登録
		pActor->SetOctreeNodeNext(pMaxParent->actor);//一番最初はNULL
		pMaxParent->actor = pActor;//後から追加した方が先頭になる
	}
}

/*
actorを含む空間に所属する全てのactorを取得
*/
void DsBoundingOctree::GetContainAreaActors(const DsActor& actor, std::vector<const DsActor*>& outActors) const
{
	if (!m_isExistActor) {
		return;
	}

	//actorが完全に含まれる空間を求め、その空間に所属する全てのactroをoutActorsとして返す

	const DsVec3d gridSize = DsVec3d::Abs(m_allActorMaxPos - m_allActorMinPos) / static_cast<double>(m_resolution);
	const DsAabb aabb = actor.RefAabb();
	const DsVec3d maxPos = DsVec3d::Clamp3(aabb.GetMax(), m_allActorMinPos, m_allActorMaxPos);//新規actorかもしれないので最大座標でクランプ
	const DsVec3d minPos = DsVec3d::Clamp3(aabb.GetMin(), m_allActorMinPos, m_allActorMaxPos);
	//各成分の座標が何番目の格子に相当するのか。
	//一番大きい座標の格子番号
	const DsVec3d maxGridPos = DsVec3d::Div(maxPos - m_allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);//ぴったりMaxだと１つ分はみ出ることになるので-FLT_EPSILON。
	const int maxGridNum[3] = {
		static_cast<int>(maxGridPos.x),
		static_cast<int>(maxGridPos.y),
		static_cast<int>(maxGridPos.z),
	};
	//一番小さい座標の格子番号
	const DsVec3d minGridPos = DsVec3d::Div(minPos - m_allActorMinPos, gridSize) - DsVec3d(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);;
	const int minGridNum[3] = {
		static_cast<int>(minGridPos.x),
		static_cast<int>(minGridPos.y),
		static_cast<int>(minGridPos.z),
	};
	//この時点でマイナスにはならないはず。

	const int resolution2 = m_resolution*m_resolution;
	const int maxIdx = maxGridNum[0] * resolution2 + maxGridNum[1] * m_resolution + maxGridNum[2];//一番大きな座標の所属する格子のノードへのインデックス
	const int minIdx = minGridNum[0] * resolution2 + minGridNum[1] * m_resolution + minGridNum[2];//一番小さな座標の所属する格子のノードへのインデックス

	//最大座標と最小座標のグリッドの親をさかのぼっていって、同じになれば両方が含まれる空間＝そのactorを完全に含む空間
	const DsBdOctreeNode* pMaxParent = m_pTerminal[maxIdx].parent;
	const DsBdOctreeNode* pMinParent = m_pTerminal[minIdx].parent;
	while (pMaxParent != pMinParent) {
		pMaxParent = pMaxParent->parent;
		pMinParent = pMinParent->parent;
	}//少なくともルート空間で必ず一致するはず。NULLなら実装ミス
	DS_ASSERT(pMaxParent && pMinParent, "DsBoundingOctreeで所属空間が見つからないactorがいる %S", actor.GetName());


	bool isBack = false;
	const DsBdOctreeNode* pSearch = pMaxParent->child;//兄弟も含む探索なので子から

	//pSearch以下の子ノードを探索
	while (pSearch){
		if (!isBack) {//階層を新しく降りたときだけ。isBackがtureということは既に巡回済み
			const DsActor* pOutActor = pSearch->actor;
			while (pOutActor) {
				outActors.push_back(pOutActor);
				pOutActor = pOutActor->GetOctreeNodeNext();
			}
		}

		//深さ優先でツリーを辿る
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
			//戻ってきたので終わり
			break;
		}
	}

	pSearch = pMaxParent;
	//pSearch以上はpSearchを完全に含むので必ず衝突候補
	while (pSearch) {
		const DsActor* pOutActor = pSearch->actor;
		while (pOutActor) {
			outActors.push_back(pOutActor);
			pOutActor = pOutActor->GetOctreeNodeNext();
		}
		pSearch = pSearch->parent;
	}
}