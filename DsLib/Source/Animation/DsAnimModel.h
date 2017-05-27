#ifndef _DS_ANIM_MODEL_
#define _DS_ANIM_MODEL_

namespace DsLib
{
	class DsAnimRes;
}

namespace DsLib
{
	class DsAnimModel
	{
		friend DsAnimRes;

	public:
		struct Face
		{
			Face() 
				: vn(0)
				, pIndex(0)
				, normal()
			{}
			~Face()
			{
				delete pIndex; pIndex = NULL;
			}
			int vn;
			int* pIndex;
			DsVec3d normal;
		};

		struct Material
		{
			struct Texture
			{
				struct UV
				{
					float x;
					float y;
					int vertexIdx;
				};
				Texture()
					: uvNum(0)
					, pUV(NULL)
					, refGeomFaces()
					, refGeomFacesIndex()
					, path()
				{}
				~Texture()
				{
					delete[] pUV;
					pUV = NULL;
				}
				int uvNum;
				UV* pUV;
				std::vector<Face*> refGeomFaces;
				std::vector<int> refGeomFacesIndex; //インデックス版。クローン作るときに必要
				std::string path;
			};
			Material()
				: textureNum(0)
				, pTexture(NULL)
			{}
			~Material()
			{
				delete[] pTexture;
				pTexture = NULL;
			}
			int textureNum;
			Texture* pTexture;
		};

	public:
		DsAnimModel();
		virtual ~DsAnimModel();

	public:
		//自分と同じ内容の別インスタンスを作成
		DsAnimModel* CreateClone() const;

	public:
		const DsVec4d* GetVertex() const { return m_pVertex; }
		DsVec4d* GetVertex() { return m_pVertex; }
		int GetVertexNum() const { return m_vn; }
		const Face* GetFace() const{ return m_pFace; }
		Face* GetFace() { return m_pFace; }
		int GetFaceNum()const { return m_fn; }
		const Material* GetMaterial() const { return m_pMaterial; }
		int GetMaterialNum() const { return m_mn; }

	public:
		void CreateVertexNormal();
		bool IsCreateVertexNormal() const { return (m_pVertexNormalIdxs && m_pVertexNormals); }
		void UpdateNormal();
		const DsVec3f* GetVertexNormals() const { return m_pVertexNormals; }

	private:
		DsVec4d* m_pVertex;
		int m_vn;
		Face* m_pFace;
		int m_fn;
		Material* m_pMaterial;
		int m_mn;

	private:
		//１つの頂点に隣接している面のインデックス
		typedef std::vector<int> ShareNlmIdxs;
		ShareNlmIdxs* m_pVertexNormalIdxs;
		DsVec3f* m_pVertexNormals;
	};
}

#endif