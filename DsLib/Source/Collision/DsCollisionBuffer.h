#pragma once

namespace DsLib
{
	class DsModel;
}

namespace DsLib
{
	class DsCollisionBuffer
	{
	public:
		struct FaceData
		{
			int index[3];
			float normal[3];
		};
		struct VertexData
		{
			float v[3];
		};
		struct ModelData
		{
			int vertexNum;
			int faceNum;
			VertexData* pVertex;
			FaceData* pFace;
		};
		struct BufData
		{
			int modelNum;
			int pad;
			ModelData* pModel;
		};

	public:
		DsCollisionBuffer();
		~DsCollisionBuffer();

	public:
		void WriteBuffer(ds_uint8* pBuf, ds_uint64 buffSize, const DsModel*const* pModel, int modelNum);
		const BufData* GetBuff() const { return reinterpret_cast<BufData*>(m_pBuffer); }

	public:
		static size_t CalcMemSize(const DsModel*const* pModel, int modelNum);

	private:
		ds_uint8* m_pBuffer;

	};




}