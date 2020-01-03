#include "DsPch.h"
#include "Test/DsGridTreeTest.h"
#include "Collision/DsGridTree.h"
#include "Collision/DsCollisionBuffer.h"
#include "Model/DsModel.h"

using namespace DsLib;

DsGridTreeTest::DsGridTreeTest()
{
}


DsGridTreeTest::~DsGridTreeTest()
{
}

void DsGridTreeTest::Test(const std::list<const DsModel*>& drawList)
{
	std::vector < const DsModel*> models;
	models.assign(drawList.begin(), drawList.end());

	size_t buffSize = DsCollisionBuffer::CalcMemSize(models.data(), models.size());
	size_t entiryNum = 0;
	for (const DsModel* pModel :  models) {
		entiryNum += pModel->GetFaceNum();
	}
	size_t treeSize = DsGridTree::CalcMemSize(entiryNum);

	size_t totalSize = buffSize + treeSize;
	ds_uint8* pBuf = new ds_uint8[totalSize];
	std::memset(pBuf, 0, totalSize);

	DsCollisionBuffer collisionBuf;
	collisionBuf.WriteBuffer(pBuf, buffSize, models.data(), models.size());

	DsGridTree* pTree = DsGridTree::Create(pBuf+buffSize, &collisionBuf, pBuf, totalSize);


	delete[] pBuf;
}