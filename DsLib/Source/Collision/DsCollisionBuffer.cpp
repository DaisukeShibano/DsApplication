#include "DsPch.h"
#include "Collision/DsCollisionBuffer.h"
#include "Model/DsModel.h"

using namespace DsLib;


DsCollisionBuffer::DsCollisionBuffer()
	:m_pBuffer(NULL)
{
}


DsCollisionBuffer::~DsCollisionBuffer()
{
	m_pBuffer = NULL;
}

void DsCollisionBuffer::WriteBuffer(ds_uint8* pBuf, ds_uint64 buffSize, const DsModel*const* pModel, int modelNum)
{
	if (pBuf == NULL) {
		return;
	}
	if (pModel == NULL) {
		return;
	}

	/*
	  モデルが3つの場合の例
	  |  BufData      |
	  |  ModelData 0  |
	  |  ModelData 1  |
	  |  ModelData 2  |
	  |  VertexData 0 |
	  |  FaceData   0 |
	  |  VertexData 1 |
	  |  FaceData   1 |
	  |  VertexData 2 |
	  |  FaceData   2 |
	*/

	m_pBuffer = pBuf;
	ds_uint8* pBufEnd = NULL;

	BufData* pBufData = reinterpret_cast<BufData*>(pBuf);
	pBufData->modelNum = modelNum;

	const size_t modelOffset = sizeof(BufData);//BufData構造体の次にModelが並ぶ
	pBufData->pModel = reinterpret_cast<ModelData*>(pBuf + modelOffset);

	ModelData* pWriteModel = pBufData->pModel;

	//ModelDataの配列の後ろにvetex配列,face配列
	ds_uint8* pVertexFaceBuf = pBuf + modelOffset + sizeof(ModelData) * modelNum;

	for (int modelIdx = 0; modelIdx < modelNum; ++modelIdx) {
		const DsModel& model = *pModel[modelIdx];
		
		const int vertexNum = model.GetVertexNum();
		pWriteModel->vertexNum = vertexNum;

		const int faceNum = model.GetFaceNum();
		pWriteModel->faceNum = faceNum;

		const size_t vertexSize = sizeof(VertexData)*vertexNum;
		const size_t faceSize = sizeof(FaceData)*faceNum;

		pWriteModel->pVertex = reinterpret_cast<VertexData*>(pVertexFaceBuf);
		pVertexFaceBuf += vertexSize;

		pWriteModel->pFace = reinterpret_cast<FaceData*>(pVertexFaceBuf);
		pVertexFaceBuf += faceSize;

		++pWriteModel;
	}

	for (int modelIdx = 0; modelIdx < modelNum; ++modelIdx) {
		const DsModel& model = *pModel[modelIdx];
		ModelData* pWirteModel = pBufData->pModel + modelIdx;

		const DsVec4d* pVertex = model.GetVertex();
		const int vertexNum = pWirteModel->vertexNum;
		VertexData* pWriteVertex = pWirteModel->pVertex;
		for (int idx = 0; idx < vertexNum; ++idx) {
			const DsVec4d& v = pVertex[idx];
			pWriteVertex->v[0] = static_cast<float>(v.x);
			pWriteVertex->v[1] = static_cast<float>(v.y);
			pWriteVertex->v[2] = static_cast<float>(v.z);
			++pWriteVertex;
		}

		const DsModel::Face* pFace = model.GetFace();
		const int faceNum = pWirteModel->faceNum;
		FaceData* pWriteFace = pWirteModel->pFace;
		for (int idx = 0; idx < faceNum; ++idx) {
			const DsModel::Face& face = pFace[idx];
			if (3 == face.vn) {
				pWriteFace->index[0] = face.pIndex[0];
				pWriteFace->index[1] = face.pIndex[1];
				pWriteFace->index[2] = face.pIndex[2];
				pWriteFace->normal[0] = static_cast<float>(face.normal.v[0]);
				pWriteFace->normal[1] = static_cast<float>(face.normal.v[1]);
				pWriteFace->normal[2] = static_cast<float>(face.normal.v[2]);
			}
			else {
				pWriteFace->index[0] = -1;
				pWriteFace->index[1] = -1;
				pWriteFace->index[2] = -1;
				pWriteFace->normal[0] = 0.0f;
				pWriteFace->normal[1] = 0.0f;
				pWriteFace->normal[2] = 0.0f;
			}
			++pWriteFace;

			pBufEnd = reinterpret_cast<ds_uint8*>(pWriteFace);
		}
	}

	/*チェック*/
#ifdef _DEBUG
	{
		{
			ds_uint64 top = (ds_uint64)(m_pBuffer);
			ds_uint64 current = (ds_uint64)(pBufEnd);
			ds_uint64 writeSize = current - top;
			if (buffSize < writeSize) {
				DS_ASSERT(false, "チェックダメ");
			}
		}

		const BufData* pBuf = reinterpret_cast<BufData*>(m_pBuffer);
		if( pBuf->modelNum != modelNum){
			DS_ASSERT(false, "チェックダメ");
		}

		for (int m = 0; m < modelNum; ++m) {
			const ModelData& modelBuf = pBuf->pModel[m];
			if (modelBuf.vertexNum != pModel[m]->GetVertexNum()) {
				DS_ASSERT(false, "チェックダメ");
			}
			if (modelBuf.faceNum != pModel[m]->GetFaceNum()) {
				DS_ASSERT(false, "チェックダメ");
			}
			const VertexData* pVertexBuf = pBuf->pModel[m].pVertex;
			const DsVec4d* pVertex = pModel[m]->GetVertex();
			for (int v = 0; v < modelBuf.vertexNum; ++v) {
				if (0.00001f < fabs( static_cast<float>(pVertex[v].v[0]) - pVertexBuf[v].v[0])) {
					DS_ASSERT(false, "チェックダメ");
				}
				if (0.00001f < fabs(static_cast<float>(pVertex[v].v[1]) - pVertexBuf[v].v[1])) {
					DS_ASSERT(false, "チェックダメ");
				}
				if (0.00001f < fabs(static_cast<float>(pVertex[v].v[2]) - pVertexBuf[v].v[2])) {
					DS_ASSERT(false, "チェックダメ");
				}
			}

			const FaceData* pFaceBuf = pBuf->pModel[m].pFace;
			const DsModel::Face* pFace = pModel[m]->GetFace();
			for (int f = 0; f < modelBuf.faceNum; ++f) {
				if (3 == pFace[f].vn) {
					if (pFaceBuf[f].index[0] != pFace[f].pIndex[0]) {
						DS_ASSERT(false, "チェックダメ");
					}
					if (pFaceBuf[f].index[1] != pFace[f].pIndex[1]) {
						DS_ASSERT(false, "チェックダメ");
					}
					if (pFaceBuf[f].index[2] != pFace[f].pIndex[2]) {
						DS_ASSERT(false, "チェックダメ");
					}
					if (0.00001f < fabs(pFaceBuf[f].normal[0] - static_cast<float>(pFace[f].normal.v[0]))) {
						DS_ASSERT(false, "チェックダメ");
					}
					if (0.00001f < fabs(pFaceBuf[f].normal[1] - static_cast<float>(pFace[f].normal.v[1]))) {
						DS_ASSERT(false, "チェックダメ");
					}
					if (0.00001f < fabs(pFaceBuf[f].normal[2] - static_cast<float>(pFace[f].normal.v[2]))) {
						DS_ASSERT(false, "チェックダメ");
					}
				}
			}
		}
	}
#endif
}

//static
size_t DsCollisionBuffer::CalcMemSize(const DsModel*const* pModel, int modelNum)
{
	size_t ret = sizeof(BufData);
	for (int idx = 0; idx < modelNum; ++idx) {
		ret += sizeof(ModelData);
		ret += pModel[idx]->GetVertexNum() * sizeof(VertexData);
		ret += pModel[idx]->GetFaceNum() * sizeof(FaceData);
	}
	return ret;
}