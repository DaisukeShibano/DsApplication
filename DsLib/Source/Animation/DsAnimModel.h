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

		struct Dmypoly
		{
			int index[3];
			int id;
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
				std::vector<int> refGeomFacesIndex; //�C���f�b�N�X�ŁB�N���[�����Ƃ��ɕK�v
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
		//�����Ɠ������e�̕ʃC���X�^���X���쐬
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
		const DsVec3d* GetVertexNormals() const { return m_pVertexNormals; }

	public:
		void SetPosition(const DsVec3d& pos) { m_pos = pos; }
		DsVec3d GetPosition() const { return m_pos; }
		void SetRotation(const DsMat33d& rot) { m_rot = rot; }
		DsMat33d GetRotation()const { return m_rot; }

	public:
		DsMat44d GetDmypoly(int id)const;

	private:
		DsVec4d* m_pVertex;
		int m_vn;
		Face* m_pFace;
		int m_fn;
		Dmypoly* m_pDmypoly;
		int m_dn;
		Material* m_pMaterial;
		int m_mn;

	private:
		DsVec3d m_pos;
		DsMat33d m_rot;

	private:
		//�P�̒��_�ɗאڂ��Ă���ʂ̃C���f�b�N�X
		typedef std::vector<int> ShareNlmIdxs;
		ShareNlmIdxs* m_pVertexNormalIdxs;
		DsVec3d* m_pVertexNormals;
	};
}

#endif