#include "DsPch.h"

#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif

#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_SET_
#include "Animation/DsKeyframeAnimSet.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif
#ifndef _DS_AMIM_RAGDOLL_INFO_
#include "Animation/DsAnimCustomProperty.h"
#endif

using namespace DsLib;

namespace
{
//--------------------------------------------------------------------------------
//��������\���̒�`
//--------------------------------------------------------------------------------
	typedef unsigned long vari_size;

	struct DS_VERTEX
	{
		union
		{
			float v[4];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};

		unsigned long long GetSize() const
		{
			return sizeof(DS_VERTEX);
		}
	};


	struct DS_FACE
	{
		DS_FACE()
			: vn(0)
			, pIndex(0)
			, normal()
		{
		}

		~DS_FACE()
		{
			vn = 0;
			delete[] pIndex;
			pIndex = 0;
		}

		vari_size vn;
		vari_size* pIndex;
		DS_VERTEX normal;

		unsigned long long GetSize() const
		{
			return (sizeof(vari_size) + normal.GetSize() + (vn*sizeof(vari_size)));
		}

	};

	struct DS_TEXTURE
	{
		enum TEXTURE_TYPE
		{
			DIFFUSE,
		};

		struct UV
		{
			UV()
				:vertexIndex(-1)
			{
				uv[0] = 0;
				uv[1] = 0;
			}
			float uv[2];
			int vertexIndex;
		};

		struct UV_FACE
		{
			UV_FACE()
				:uvpNum(0)
				, uvpIndex(0)
				, refGeomFaceIndex(-1)
			{}
			~UV_FACE()
			{
				delete[] uvpIndex;
				uvpIndex = 0;
			}
			int uvpNum;
			int* uvpIndex;
			int refGeomFaceIndex;
		};


		DS_TEXTURE()
			: texPathSize(0)
			, texPath(0)
			, uvNum(0)
			, uv(0)
			, textType(DIFFUSE)
		{
		}

		~DS_TEXTURE()
		{
			delete[] texPath;
			texPath = 0;
			delete[] uv;
			uv = 0;
			delete[] uvFace;
			uvFaceNum = 0;
		}

		int texPathSize;
		char* texPath;
		int uvNum;
		UV* uv;
		int uvFaceNum;
		UV_FACE * uvFace;
		int textType;
	};

	struct DS_MATERIAL
	{
		DS_MATERIAL()
			: textureNum(0)
			, texture(0)
		{
		}
		~DS_MATERIAL()
		{
			delete[] texture;
			texture = 0;
		}
		int textureNum;
		DS_TEXTURE* texture;
	};

	struct DS_ANIM_MODEL
	{
		DS_ANIM_MODEL()
			: vn(0)
			, pVertex(0)
			, fn(0)
			, pFace(0)
			, mn(0)
			, pMtr(0)
			, dataSize(sizeof(vari_size) * 2 + sizeof(vari_size))//vn + fn + size�@��
			, vertexSize(0)
			, faceSize(0)
		{
		}

		~DS_ANIM_MODEL()
		{
			vn = 0;
			delete[] pVertex;
			pVertex = 0;
			fn = 0;
			delete[] pFace;
			pFace = 0;
			delete[] pMtr;
			pMtr = 0;
		}

		vari_size vn;
		DS_VERTEX* pVertex;

		vari_size fn;
		DS_FACE* pFace;

		vari_size mn;
		DS_MATERIAL* pMtr;

		unsigned long long dataSize;		//�ۑ��Ώۂ̑S�ẴT�C�Y
		unsigned long long vertexSize;	//vertex�݂̂̃T�C�Y
		unsigned long long faceSize;		//face�݂̂̃T�C�Y

	};

	struct DS_MAT
	{
		enum{ SIZE = 64 };

		DS_MAT(){}

		DS_MAT& operator =(const double src[4][4])
		{
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					m[col][row] = static_cast<float>(src[col][row]);
				}
			}
			return (*this);
		}

		float m[4][4];
	};

	struct DS_BONE
	{
		DS_BONE()
			: indexNum(0)
			, pIndex(0)
			, pWeight(0)
			, initMat()
			, nameSize(0)
			, name(0)
			, parentIdx(0)
			, childNum(0)
			, pChildIdx(0)
		{}

		~DS_BONE()
		{
			indexNum = 0;
			delete[] pIndex;
			pIndex = 0;
			delete[] pWeight;
			pWeight = 0;

			delete[] name;
			name = 0;
			nameSize = 0;

			delete[] pChildIdx;
			pChildIdx = 0;
		}

		int indexNum;
		vari_size* pIndex;
		float* pWeight;

		DS_MAT initMat;

		size_t nameSize;
		char* name;

		int parentIdx;
		int childNum;
		int* pChildIdx;

	};

	struct DS_KEY_FRAME
	{
		float value;
		float localTimeMs;
	};



	struct DS_ANIM_POSE
	{
		DS_ANIM_POSE()
			: keyFrameNumTX(0)
			, keyFrameTX(0)
			, keyFrameNumTY(0)
			, keyFrameTY(0)
			, keyFrameNumTZ(0)
			, keyFrameTZ(0)

			, keyFrameNumRX(0)
			, keyFrameRX(0)
			, keyFrameNumRY(0)
			, keyFrameRY(0)
			, keyFrameNumRZ(0)
			, keyFrameRZ(0)
			, keyFrameNumRQ(0)
			, keyFrameRQ(0)

			, keyFrameNumSX(0)
			, keyFrameSX(0)
			, keyFrameNumSY(0)
			, keyFrameSY(0)
			, keyFrameNumSZ(0)
			, keyFrameSZ(0)
		{
		}

		~DS_ANIM_POSE()
		{
			delete[] keyFrameTX;
			keyFrameTX = 0;
			delete[] keyFrameTY;
			keyFrameTY = 0;
			delete[] keyFrameTZ;
			keyFrameTZ = 0;

			delete[] keyFrameRX;
			keyFrameRX = 0;
			delete[] keyFrameRY;
			keyFrameRY = 0;
			delete[] keyFrameRZ;
			keyFrameRZ = 0;
			delete[] keyFrameRQ;
			keyFrameRQ = 0;

			delete[] keyFrameSX;
			keyFrameSX = 0;
			delete[] keyFrameSY;
			keyFrameSY = 0;
			delete[] keyFrameSZ;
			keyFrameSZ = 0;
		}

		long keyFrameNumTX;
		DS_KEY_FRAME* keyFrameTX;

		long keyFrameNumTY;
		DS_KEY_FRAME* keyFrameTY;

		long keyFrameNumTZ;
		DS_KEY_FRAME* keyFrameTZ;

		long keyFrameNumRX;
		DS_KEY_FRAME* keyFrameRX;

		long keyFrameNumRY;
		DS_KEY_FRAME* keyFrameRY;

		long keyFrameNumRZ;
		DS_KEY_FRAME* keyFrameRZ;

		long keyFrameNumRQ;
		DS_KEY_FRAME* keyFrameRQ;

		long keyFrameNumSX;
		DS_KEY_FRAME* keyFrameSX;

		long keyFrameNumSY;
		DS_KEY_FRAME* keyFrameSY;

		long keyFrameNumSZ;
		DS_KEY_FRAME* keyFrameSZ;
	};

	struct DS_ANIM
	{
		static long PoseNum;

		DS_ANIM()
			: animNameSize(0)
			, animName(0)
			, poseNum(PoseNum)
			, pose(0)
		{
			pose = new DS_ANIM_POSE[poseNum];
		}

		~DS_ANIM()
		{
			delete[] animName;
			animName = 0;
			delete[] pose;
			pose = 0;
		}

		size_t animNameSize;
		char* animName;

		long poseNum;
		DS_ANIM_POSE* pose;
	};
	long DS_ANIM::PoseNum;

	struct DS_ANIM_BONE
	{
		DS_ANIM_BONE()
			: bn(0)
			, pBone(0)
			, an(0)
			, pAnim(0)
		{}

		~DS_ANIM_BONE()
		{
			bn = 0;
			delete[] pBone;
			pBone = 0;
			an = 0;
			delete[] pAnim;
			pAnim = 0;
		}

		long bn;
		DS_BONE* pBone;

		long an;
		DS_ANIM* pAnim;
	};

	struct DS_RAGDOLL_PARAM_ID
	{
		DS_RAGDOLL_PARAM_ID()
			: id(0)
			, boneIdx(0)
		{}
		int id;
		unsigned long boneIdx;
	};

	struct DS_CUSTOM_PROPERTY
	{
		DS_CUSTOM_PROPERTY()
			: pRagdollParamId(NULL)
			, ragdollNum(0)
		{}
		~DS_CUSTOM_PROPERTY()
		{
			delete[] pRagdollParamId;
			pRagdollParamId = NULL;
		}
		DS_RAGDOLL_PARAM_ID* pRagdollParamId;
		long ragdollNum;
	};

	class OutputRes
	{
	public:
		OutputRes()
			: dsAnimModel()
			, dsAnimBone()
			, dsCustomProperty()
			, dataSize(0)
		{};
		virtual ~OutputRes(){};

	public:
		DS_ANIM_MODEL dsAnimModel;
		DS_ANIM_BONE dsAnimBone;
		DS_CUSTOM_PROPERTY dsCustomProperty;
		unsigned long long dataSize;
	};
//--------------------------------------------------------------------------------
//�����܂ō\���̒�`
//--------------------------------------------------------------------------------



	OutputRes* _LoadAnim(const char* path)
	{
		OutputRes& res= *(new OutputRes());

		std::ifstream  fs(path, std::ios::out | std::ios::binary);
		if (!fs)
		{
			DS_ASSERT(false, "�t�@�C���I�[�v���Ɏ��s");
			return &res;
		}

		{//header
			long ver;
			fs.read((char*)(&ver), sizeof(long));

			unsigned long long vertexOffset;
			fs.read((char*)(&vertexOffset), sizeof(vertexOffset));

			unsigned long long faceOffset;
			fs.read((char*)(&faceOffset), sizeof(faceOffset));
		}


		///////////////////////////////�������烂�f��
		{//vertex
			vari_size vn;
			fs.read((char*)(&vn), sizeof(vn));
			res.dsAnimModel.vn = vn;

			res.dsAnimModel.pVertex = new DS_VERTEX[vn];
			for (vari_size i = 0; i < vn; ++i)
			{
				float v[4];
				fs.read((char*)(&v[0]), sizeof(v));
				res.dsAnimModel.pVertex[i].x = v[0];
				res.dsAnimModel.pVertex[i].y = v[1];
				res.dsAnimModel.pVertex[i].z = v[2];
				res.dsAnimModel.pVertex[i].w = v[3];
			}
		}
		{//face
			vari_size fn;
			fs.read((char*)(&fn), sizeof(fn));
			res.dsAnimModel.fn = fn;

			res.dsAnimModel.pFace = new DS_FACE[fn];
			for (vari_size fIdx = 0; fIdx < fn; ++fIdx)
			{
				vari_size vn;
				fs.read((char*)(&vn), sizeof(vn));
				res.dsAnimModel.pFace[fIdx].vn = vn;

				float normal[4];
				fs.read((char*)(&normal[0]), sizeof(normal));
				res.dsAnimModel.pFace[fIdx].normal.x = normal[0];
				res.dsAnimModel.pFace[fIdx].normal.y = normal[1];
				res.dsAnimModel.pFace[fIdx].normal.z = normal[2];
				res.dsAnimModel.pFace[fIdx].normal.w = normal[3];

				res.dsAnimModel.pFace[fIdx].pIndex = new vari_size[vn];
				for (vari_size vIdx = 0; vIdx < vn; ++vIdx)
				{
					vari_size index;
					fs.read((char*)(&index), sizeof(index));
					res.dsAnimModel.pFace[fIdx].pIndex[vIdx] = index;
				}
			}
		}
		{//material
			vari_size mn;
			fs.read((char*)(&mn), sizeof(mn));
			res.dsAnimModel.pMtr = new DS_MATERIAL[mn];
			res.dsAnimModel.mn = mn;

			for (vari_size mi = 0; mi < mn; ++mi)
			{
				vari_size tn;
				fs.read((char*)(&tn), sizeof(tn));
				res.dsAnimModel.pMtr[mi].textureNum = tn;
				res.dsAnimModel.pMtr[mi].texture = new DS_TEXTURE[tn];
				
				for (vari_size ti = 0; ti < tn; ++ti)
				{
					int nameSize;
					fs.read((char*)(&nameSize), sizeof(nameSize));
					res.dsAnimModel.pMtr[mi].texture[ti].texPathSize = nameSize;
					res.dsAnimModel.pMtr[mi].texture[ti].texPath = new char[nameSize+1];
					fs.read(res.dsAnimModel.pMtr[mi].texture[ti].texPath, nameSize);
					res.dsAnimModel.pMtr[mi].texture[ti].texPath[nameSize] = '\0'; //�I�[�����������ĂȂ��̂ŁB

					int textureType;
					fs.read((char*)(&textureType), sizeof(textureType));
					res.dsAnimModel.pMtr[mi].texture[ti].textType = textureType;

					vari_size uvNum;
					fs.read((char*)(&uvNum), sizeof(uvNum));
					res.dsAnimModel.pMtr[mi].texture[ti].uvNum = uvNum;
					res.dsAnimModel.pMtr[mi].texture[ti].uv = new DS_TEXTURE::UV[uvNum];

					for (vari_size uvi = 0; uvi < uvNum; ++uvi)
					{
						float uvCoord[2];
						fs.read((char*)(&uvCoord[0]), sizeof(uvCoord));
						res.dsAnimModel.pMtr[mi].texture[ti].uv[uvi].uv[0] = uvCoord[0];
						res.dsAnimModel.pMtr[mi].texture[ti].uv[uvi].uv[1] = uvCoord[1];

						vari_size vertexIndex;
						fs.read((char*)(&vertexIndex), sizeof(vertexIndex));
						res.dsAnimModel.pMtr[mi].texture[ti].uv[uvi].vertexIndex = vertexIndex;
					}

					vari_size uvFaceNum;
					fs.read((char*)(&uvFaceNum), sizeof(uvFaceNum));
					res.dsAnimModel.pMtr[mi].texture[ti].uvFaceNum = uvFaceNum;
					res.dsAnimModel.pMtr[mi].texture[ti].uvFace = new DS_TEXTURE::UV_FACE[uvFaceNum];
					for (vari_size uvfi = 0; uvfi < uvFaceNum; ++uvfi)
					{
						vari_size uvpNum;
						fs.read((char*)(&uvpNum), sizeof(uvpNum));
						res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].uvpNum = uvpNum;
						vari_size refGeomFaceIndex;
						fs.read((char*)(&refGeomFaceIndex), sizeof(refGeomFaceIndex));
						res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].refGeomFaceIndex = refGeomFaceIndex;

						res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].uvpIndex = new int[uvpNum];
						for (vari_size uvpi = 0; uvpi < uvpNum; ++uvpi)
						{
							vari_size uvIndex;
							fs.read((char*)(&uvIndex), sizeof(uvIndex));
							res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].uvpIndex[uvpi] = uvIndex;
						}
					}
				}
			}
		}



		/////////////////////////��������L�[�t���[���A�j��
		{//bone
			unsigned long boneNum;
			fs.read((char*)(&boneNum), sizeof(boneNum));
			res.dsAnimBone.bn = boneNum;
			res.dsAnimBone.pBone = new DS_BONE[boneNum];

			for (unsigned long bIdx = 0; bIdx < boneNum; ++bIdx)
			{
				unsigned long nameSize;
				fs.read((char*)(&nameSize), sizeof(nameSize));
				res.dsAnimBone.pBone[bIdx].nameSize = nameSize;
				res.dsAnimBone.pBone[bIdx].name = new char[nameSize];

				fs.read(res.dsAnimBone.pBone[bIdx].name, nameSize);

				fs.read((char*)(&res.dsAnimBone.pBone[bIdx].initMat.m[0][0]), DS_MAT::SIZE);

				long indexNum;
				fs.read((char*)(&indexNum), sizeof(indexNum));
				res.dsAnimBone.pBone[bIdx].indexNum = indexNum;

				//�{�[���ɕR�Â����_�C���f�b�N�X
				res.dsAnimBone.pBone[bIdx].pIndex = new vari_size[indexNum];
				for (int wIdx = 0; wIdx < indexNum; ++wIdx)
				{
					vari_size vertexIndex;
					fs.read((char*)(&vertexIndex), sizeof(vertexIndex));
					res.dsAnimBone.pBone[bIdx].pIndex[wIdx] = vertexIndex;
				}
				//�{�[���ɕR�Â����_�ɑ΂���d��
				res.dsAnimBone.pBone[bIdx].pWeight = new float[indexNum];
				for (int wIdx = 0; wIdx < indexNum; ++wIdx)
				{
					float weight;
					fs.read((char*)(&weight), sizeof(weight));
					res.dsAnimBone.pBone[bIdx].pWeight[wIdx] = weight;
				}

				int parentIdx = -1;
				fs.read((char*)(&parentIdx), sizeof(parentIdx));
				res.dsAnimBone.pBone[bIdx].parentIdx = parentIdx;
				int childNum = 0;
				fs.read((char*)(&childNum), sizeof(childNum));
				res.dsAnimBone.pBone[bIdx].childNum = childNum;
				if (0 < childNum)
				{
					res.dsAnimBone.pBone[bIdx].pChildIdx = new int[childNum];
					fs.read((char*)(res.dsAnimBone.pBone[bIdx].pChildIdx), sizeof(int)*childNum);
				}
			}
		}

		{//anim
			unsigned long animNum;
			fs.read((char*)(&animNum), sizeof(animNum));
			res.dsAnimBone.an = animNum;
			res.dsAnimBone.pAnim = new DS_ANIM[animNum];

			//�A�j���[�V����
			for (unsigned long aIdx = 0; aIdx < animNum; ++aIdx)
			{
				unsigned long animNameSize;
				fs.read((char*)(&animNameSize), sizeof(animNameSize));
				res.dsAnimBone.pAnim[aIdx].animNameSize = animNameSize;
				res.dsAnimBone.pAnim[aIdx].animName = new char[animNameSize];

				fs.read(res.dsAnimBone.pAnim[aIdx].animName, animNameSize);

				unsigned long poseNum;
				fs.read((char*)(&poseNum), sizeof(poseNum));
				res.dsAnimBone.pAnim[aIdx].poseNum = poseNum;
				res.dsAnimBone.pAnim[aIdx].pose = new DS_ANIM_POSE[poseNum];

				//�|�[�Y
				for (unsigned long poseIdx = 0; poseIdx < poseNum; ++poseIdx)
				{
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTX = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTY = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTZ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].value = value;
						}
					}


					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRX = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRY = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRZ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRQ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].value = value;
						}
					}


					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSX = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSY = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].value = value;
						}
					}
					{
						long keyFrameNum;
						fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSZ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ = new DS_KEY_FRAME[keyFrameNum];

						for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
						{
							float localTime;
							float value;
							fs.read((char*)(&localTime), sizeof(localTime));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].localTimeMs = localTime;
							fs.read((char*)(&value), sizeof(value));
							res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].value = value;
						}
					}
				}
			}
		}

		////////////////////��������J�X�^���v���p�e�B
		{//ragdollParam
			unsigned long ragdollParamNum=0;
			fs.read((char*)(&ragdollParamNum), sizeof(ragdollParamNum));
			const long ragParaSize = ragdollParamNum * sizeof(DS_RAGDOLL_PARAM_ID);
			res.dsCustomProperty.ragdollNum = ragdollParamNum;
			if (0 < ragdollParamNum) {
				DS_RAGDOLL_PARAM_ID* pRagdollaramId= new DS_RAGDOLL_PARAM_ID[ragdollParamNum];
				fs.read((char*)(pRagdollaramId), sizeof(DS_RAGDOLL_PARAM_ID)*ragdollParamNum);
				res.dsCustomProperty.pRagdollParamId = pRagdollaramId;
			}

		}


		return &res;
	}
}


void DsAnimRes::Initialize(const char* path)
{
	OutputRes* pRes = _LoadAnim(path);
	m_resTop = pRes;

	m_name = path;

	size_t pos1;
	pos1 = m_name.rfind('\\');
	if (pos1 != std::string::npos){
		m_name = m_name.substr(pos1 + 1, m_name.size() - pos1 - 1);
	}

	pos1 = m_name.rfind('/');
	if (pos1 != std::string::npos){
		m_name = m_name.substr(pos1 + 1, m_name.size() - pos1 - 1);
	}
}

//�q�{�[��
void DsAnimRes::_CreateBone(DsAnimBone* pParent, const void* pParentSrcData, std::vector<DsAnimBone*>& boneArray) const
{
	const DS_BONE* pParentSrc = static_cast<const DS_BONE*>(pParentSrcData);
	const OutputRes* pRes = static_cast<OutputRes*>(m_resTop);

	for (int cIdx = 0; cIdx < pParentSrc->childNum; ++cIdx)
	{
		const int tmpIdx = pParentSrc->pChildIdx[cIdx];
		const DS_BONE* tmp = &pRes->dsAnimBone.pBone[tmpIdx];
		
		DsAnimBone* pBone = new DsAnimBone;
		pBone->pParent = pParent;
		pBone->name = tmp->name;
		
		//�O���[�o�����烍�[�J�������߂�
		pBone->initWorldPose = tmp->initMat.m;
		const DsMat44d parentWorldMat = pParent->initWorldPose;
		const DsVec3d localPos = pBone->initWorldPose.GetPos() - parentWorldMat.GetPos();
		const DsMat33d localMat = parentWorldMat.ToMat33().ToTransposition()*pBone->initWorldPose.ToMat33();
		pBone->localPose = DsMat44d::Identity();
		pBone->modelPose = DsMat44d::Identity();
		pBone->initParentToChildPose = DsMat44d::Get(localMat, localPos);
		pBone->arrayIdx = tmpIdx;	//�z��ł̃C���f�b�N�X�ۑ�
		pBone->vIndexNum = tmp->indexNum;
		pBone->pIndex = new int[tmp->indexNum];
		pBone->pWeight = new double[tmp->indexNum];
		for (int vIdx = 0; vIdx < tmp->indexNum; ++vIdx)
		{
			pBone->pIndex[vIdx] = tmp->pIndex[vIdx];
			pBone->pWeight[vIdx] = tmp->pWeight[vIdx];
		}
		boneArray[pBone->arrayIdx] = pBone;
		pParent->child.push_back(pBone);
	}
	for (int cIdx = 0; cIdx < pParentSrc->childNum; ++cIdx)
	{
		//DS_BONE�͎q�{�[���ւ̃C���f�b�N�X�������Ă�̂ł�������ɐ���
		const int tmpIdx = pParentSrc->pChildIdx[cIdx];
		_CreateBone(pParent->child[cIdx], &pRes->dsAnimBone.pBone[tmpIdx], boneArray);
	}
}

DsAnimSkeleton* DsAnimRes::CreateSkeleton() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	const int bn = pRes->dsAnimBone.bn;
	if (0 >= bn) {
		return NULL;
	}

	std::vector<DsAnimBone*> roots;
	std::vector<DsAnimBone*> boneArray(bn, NULL);

	for (int bIdx = 0; bIdx < bn; ++bIdx)
	{
		if (pRes->dsAnimBone.pBone[bIdx].parentIdx < 0)
		{
			const DS_BONE* rootSrc = &pRes->dsAnimBone.pBone[bIdx];
			DsAnimBone* root = new DsAnimBone;
			root->name = rootSrc->name;
			root->localPose = DsMat44d::Identity();
			root->modelPose = DsMat44d::Identity();
			root->initParentToChildPose = DsMat44d::Identity();
			root->initWorldPose = rootSrc->initMat.m;
			root->arrayIdx = bIdx;
			root->vIndexNum = rootSrc->indexNum;
			root->pIndex = new int[rootSrc->indexNum];
			root->pWeight = new double[rootSrc->indexNum];
			for (int vIdx = 0; vIdx < rootSrc->indexNum; ++vIdx)
			{
				root->pIndex[vIdx] = rootSrc->pIndex[vIdx];
				root->pWeight[vIdx] = rootSrc->pWeight[vIdx];
			}
			boneArray[root->arrayIdx] = root;
			_CreateBone(root, rootSrc, boneArray);
			roots.push_back(root);
		}
	}

	DsAnimSkeleton* pSkeleton = new DsAnimSkeleton(roots, boneArray);
	return pSkeleton;
}

DsKeyframeAnimSet* DsAnimRes::CreateKeyframeAnim() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);

	if (0 >= pRes->dsAnimBone.an) return NULL;//�A�j�����Ȃ�
	if (0 >= pRes->dsAnimBone.bn) return NULL;//�{�[�����Ȃ�

	DsKeyframeAnim* pKeyAnim = new DsKeyframeAnim[pRes->dsAnimBone.an];
	for (int aIdx = 0; aIdx < pRes->dsAnimBone.an; ++aIdx)
	{
		const DS_ANIM& anim = pRes->dsAnimBone.pAnim[aIdx];
		if (anim.poseNum != pRes->dsAnimBone.bn)
		{
			DsError::Panic("�L�[�t���[���A�j���{�[�����ƃX�P���g���{�[��������v���Ȃ�");
		}
		pKeyAnim[aIdx].m_name = anim.animName;
		pKeyAnim[aIdx].m_pBone = new DsKeyframeAnim::Pose[anim.poseNum];
		pKeyAnim[aIdx].m_boneNum = anim.poseNum;
		
		for (int bIdx = 0; bIdx < anim.poseNum; ++bIdx)
		{
			if (anim.pose[bIdx].keyFrameNumRX != anim.pose[bIdx].keyFrameNumRY ||
				anim.pose[bIdx].keyFrameNumRY != anim.pose[bIdx].keyFrameNumRZ ||
				anim.pose[bIdx].keyFrameNumRZ != anim.pose[bIdx].keyFrameNumRQ )
			{
				DsError::Panic("��]�L�[�t���[�����������Ԃň�v���Ȃ�");
			}
			if (anim.pose[bIdx].keyFrameNumTX != anim.pose[bIdx].keyFrameNumTY ||
				anim.pose[bIdx].keyFrameNumTY != anim.pose[bIdx].keyFrameNumTZ)
			{
				DsError::Panic("�ʒu�L�[�t���[�����������Ԃň�v���Ȃ�");
			}			
			if (anim.pose[bIdx].keyFrameNumSX != anim.pose[bIdx].keyFrameNumSY ||
				anim.pose[bIdx].keyFrameNumSY != anim.pose[bIdx].keyFrameNumSZ )
			{
				DsError::Panic("�g��L�[�t���[�����������Ԃň�v���Ȃ�");
			}
			


			//��]
			pKeyAnim[aIdx].m_pBone[bIdx].m_keyFrameNumRot = anim.pose[bIdx].keyFrameNumRX;
			pKeyAnim[aIdx].m_pBone[bIdx].m_pRot = new DsKeyframeAnim::Vec4Key[anim.pose[bIdx].keyFrameNumRX];
			for (int k = 0; k < anim.pose[bIdx].keyFrameNumRX; ++k)
			{
				if (anim.pose[bIdx].keyFrameRX[k].localTimeMs != anim.pose[bIdx].keyFrameRY[k].localTimeMs ||
					anim.pose[bIdx].keyFrameRY[k].localTimeMs != anim.pose[bIdx].keyFrameRZ[k].localTimeMs ||
					anim.pose[bIdx].keyFrameRZ[k].localTimeMs != anim.pose[bIdx].keyFrameRQ[k].localTimeMs )
				{
					DsError::Panic("��]���[�J���^�C���������Ԃň�v���Ȃ�");
				}
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].localTime = anim.pose[bIdx].keyFrameRX[k].localTimeMs;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.x = anim.pose[bIdx].keyFrameRX[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.y = anim.pose[bIdx].keyFrameRY[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.z = anim.pose[bIdx].keyFrameRZ[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.w = anim.pose[bIdx].keyFrameRQ[k].value;
				
			}


			//�ʒu
			pKeyAnim[aIdx].m_pBone[bIdx].m_keyFrameNumPos = anim.pose[bIdx].keyFrameNumTX;
			pKeyAnim[aIdx].m_pBone[bIdx].m_pPos = new DsKeyframeAnim::Vec3Key[anim.pose[bIdx].keyFrameNumTX];
			for (int k = 0; k < anim.pose[bIdx].keyFrameNumTX; ++k)
			{
				if (anim.pose[bIdx].keyFrameTX[k].localTimeMs != anim.pose[bIdx].keyFrameTY[k].localTimeMs ||
					anim.pose[bIdx].keyFrameTY[k].localTimeMs != anim.pose[bIdx].keyFrameTZ[k].localTimeMs )
				{
					DsError::Panic("�ʒu���[�J���^�C���������Ԃň�v���Ȃ�");
				}
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].localTime = anim.pose[bIdx].keyFrameTX[k].localTimeMs;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].val.x = anim.pose[bIdx].keyFrameTX[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].val.y = anim.pose[bIdx].keyFrameTY[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].val.z = anim.pose[bIdx].keyFrameTZ[k].value;

			}


			//�g��
			pKeyAnim[aIdx].m_pBone[bIdx].m_keyFrameNumScale = anim.pose[bIdx].keyFrameNumSX;
			pKeyAnim[aIdx].m_pBone[bIdx].m_pScale = new DsKeyframeAnim::Vec3Key[anim.pose[bIdx].keyFrameNumSX];
			for (int k = 0; k < anim.pose[bIdx].keyFrameNumSX; ++k)
			{
				if (anim.pose[bIdx].keyFrameSX[k].localTimeMs != anim.pose[bIdx].keyFrameSY[k].localTimeMs ||
					anim.pose[bIdx].keyFrameSY[k].localTimeMs != anim.pose[bIdx].keyFrameSZ[k].localTimeMs)
				{
					DsError::Panic("�g�働�[�J���^�C���������Ԃň�v���Ȃ�");
				}
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].localTime = anim.pose[bIdx].keyFrameSX[k].localTimeMs;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].val.x = anim.pose[bIdx].keyFrameSX[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].val.y = anim.pose[bIdx].keyFrameSY[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].val.z = anim.pose[bIdx].keyFrameSZ[k].value;

			}
			

		}

	}

	DsKeyframeAnimSet* ret = new DsKeyframeAnimSet(pKeyAnim, pRes->dsAnimBone.an);
	return ret;
}

DsAnimModel* DsAnimRes::CreateAnimModel() const
{
	//���_�f�[�^���쐬����
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	DsAnimModel* pAnimModel = new DsAnimModel();

	pAnimModel->m_pVertex = new DsVec4d[pRes->dsAnimModel.vn];
	pAnimModel->m_vn = pRes->dsAnimModel.vn;
	for (unsigned int vIdx = 0; vIdx < pRes->dsAnimModel.vn; ++vIdx)
	{
		const DS_VERTEX& tmp = pRes->dsAnimModel.pVertex[vIdx];
		pAnimModel->m_pVertex[vIdx] = DsVec4d::ToVec4(tmp.x, tmp.y, tmp.z, tmp.w);
	}

	pAnimModel->m_pFace = new DsAnimModel::Face[pRes->dsAnimModel.fn];
	pAnimModel->m_fn = pRes->dsAnimModel.fn;
	for (unsigned int fIdx = 0; fIdx < pRes->dsAnimModel.fn; ++fIdx)
	{
		const DS_FACE& tmp = pRes->dsAnimModel.pFace[fIdx];
		pAnimModel->m_pFace[fIdx].pIndex = new int[tmp.vn];
		pAnimModel->m_pFace[fIdx].vn = tmp.vn;
		for (unsigned int vIdx = 0; vIdx < tmp.vn; ++vIdx)
		{
			pAnimModel->m_pFace[fIdx].pIndex[vIdx] = tmp.pIndex[vIdx];
		}
		pAnimModel->m_pFace[fIdx].normal = DsVec3d::ToVec3(tmp.normal.x, tmp.normal.y, tmp.normal.z);//4vec���邯�ǎg��Ȃ��̂�3vec
	}
	
	//�}�e���A��
	pAnimModel->m_mn = pRes->dsAnimModel.mn;
	pAnimModel->m_pMaterial = new DsAnimModel::Material[pAnimModel->m_mn];
	for (int mi = 0; mi < pAnimModel->m_mn; ++mi)
	{
		const int tn = pRes->dsAnimModel.pMtr[mi].textureNum;
		pAnimModel->m_pMaterial[mi].textureNum = tn;
		pAnimModel->m_pMaterial[mi].pTexture = new DsAnimModel::Material::Texture[tn];
		for (int ti = 0; ti < tn; ++ti)
		{
			DsAnimModel::Material::Texture& texture = pAnimModel->m_pMaterial[mi].pTexture[ti];
			const int uvn = pRes->dsAnimModel.pMtr[mi].texture[ti].uvNum;
			texture.path = pRes->dsAnimModel.pMtr[mi].texture[ti].texPath;
			texture.uvNum = uvn;
			texture.pUV = new DsAnimModel::Material::Texture::UV[uvn];
			for (int uvi = 0; uvi < uvn; ++uvi)
			{
				texture.pUV[uvi].x = pRes->dsAnimModel.pMtr[mi].texture[ti].uv[uvi].uv[0];
				texture.pUV[uvi].y = pRes->dsAnimModel.pMtr[mi].texture[ti].uv[uvi].uv[1];
				texture.pUV[uvi].vertexIdx = pRes->dsAnimModel.pMtr[mi].texture[ti].uv[uvi].vertexIndex;
			}

			const int uvFaceNum = pRes->dsAnimModel.pMtr[mi].texture[ti].uvFaceNum;
			for (int uvfi = 0; uvfi < uvFaceNum; ++uvfi)
			{
				const int refGeomFaceIndex = pRes->dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].refGeomFaceIndex;
				DS_ASSERT(refGeomFaceIndex < pAnimModel->m_fn, "UV�ւ̖ʂւ̎Q�ƃC���f�b�N�X���I�[�o�[");
				DsAnimModel::Face* pRefFace = &pAnimModel->m_pFace[refGeomFaceIndex];
				texture.refGeomFaces.push_back(pRefFace);
				texture.refGeomFacesIndex.push_back(refGeomFaceIndex);
			}
		}
	}

	return pAnimModel;
}

int DsAnimRes::GetAnimNum() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	const int ret = pRes->dsAnimBone.an;
	return ret;
}

DsAnimCustomProperty* DsAnimRes::CustomProperty() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	DsAnimCustomProperty* ret = NULL;
	const long num = pRes->dsCustomProperty.ragdollNum;
	ret = new DsAnimCustomProperty();
	DS_ASSERT(ret, "�������m�ێ��s");
	if (0 < num) {
		ret->ragdollParamIds.reserve(num);
		for (int i = 0; i < num; ++i) {
			const DS_RAGDOLL_PARAM_ID& id = pRes->dsCustomProperty.pRagdollParamId[i];
			DsAnimRagdollParamId param;
			param.boneIndex = id.boneIdx;
			param.ragdollParamId = id.id;
			ret->ragdollParamIds.push_back(param);
		}
	}
	return ret;
}

void DsAnimRes::DbgDraw(int animIdx, float dt, DsDrawCommand& com)
{

}

DsAnimRes::~DsAnimRes()
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	delete pRes;
	m_resTop = NULL;
}