#ifndef _DS_KEYFRAME_ANIM_
#define _DS_KEYFRAME_ANIM_

namespace DsLib
{
	class DsAnimRes;
	class DsAnimBlend;
}
namespace DsLib
{
	class DsKeyframeAnim
	{
		friend DsAnimRes;
		friend DsAnimBlend;//補間の計算するときにメンドイので丸見えにする

	private:
		struct Vec3Key
		{
			DsVec3d val;
			double localTime;
		};
		struct Vec4Key
		{
			DsVec4d val;
			double localTime;
		};

		class Pose
		{
			friend DsAnimRes;
			friend DsAnimBlend;//補間の計算するときにメンドイので丸見えにする

		private:
			enum END_FLAG_BIT
			{
				END_FLAG_POS = 1,
				END_FLAG_ROT = 1 << 1,
				END_FLAG_SCALE = 1 << 2,
				END_FLAG_ALL = END_FLAG_POS | END_FLAG_ROT | END_FLAG_SCALE,
			};

		public:
			Pose()
			: m_pPos(0)
			, m_pRot(0)
			, m_pScale(0)
			, m_keyFrameNumRot(0)
			, m_keyFrameNumPos(0)
			, m_keyFrameNumScale(0)
			, m_currentTimeMs(0.0)
			, m_currentIdxRot(0)
			, m_currentIdxPos(0)
			, m_currentIdxScale(0)
			, m_endFlag(0)
			{
			}

			~Pose()
			{
				delete[] m_pPos; m_pPos = NULL;
				delete[] m_pRot; m_pRot = NULL;
				delete[] m_pScale; m_pScale = NULL;
			}

		public:
			void Update(double dt);
			void Reset();
			void SetLocalTime(double time);
			bool IsEnd() const;
			bool IsEndPosRot() const;

		public:
			const DsVec3d& RefCurrentPos() const
			{
				return m_pPos[m_currentIdxPos].val;
			}
			const DsVec4d& RefCurrentRot() const
			{
				return m_pRot[m_currentIdxRot].val;
			}
			const DsVec3d& RefCurrentScale() const
			{
				return m_pScale[m_currentIdxScale].val;
			}

		private:
			void _Update();

		private:
			Vec3Key* m_pPos;
			Vec4Key* m_pRot;
			Vec3Key* m_pScale;
			//ボーンごとにキースレーム数が違う。おそらく変化の無いやつは省かれてる
			int m_keyFrameNumRot;
			int m_keyFrameNumPos;
			int m_keyFrameNumScale;

			double m_currentTimeMs;
			int m_currentIdxRot;
			int m_currentIdxPos;
			int m_currentIdxScale;
			unsigned char m_endFlag;
		};

	public:
		DsKeyframeAnim();
		virtual ~DsKeyframeAnim();

	public:
		void Update(double dt);
		void Reset();
		void SetLocalTime(double dt);
		bool IsEnd() const;

	public:
		const DsVec3d& RefCurrentPos(int boneIdx) const
		{
			DS_ASSERT(boneIdx < m_boneNum, "ボーン数オーバー");
			return m_pBone[boneIdx].RefCurrentPos();
		}
		const DsVec4d& RefCurrentRot(int boneIdx) const
		{
			DS_ASSERT(boneIdx < m_boneNum, "ボーン数オーバー");
			return m_pBone[boneIdx].RefCurrentRot();
		}
		const DsVec3d& RefCurrentScale(int boneIdx) const
		{
			DS_ASSERT(boneIdx < m_boneNum, "ボーン数オーバー");
			return m_pBone[boneIdx].RefCurrentScale();
		}

		const DsVec3d& RefMasterMovePos() const
		{
			return m_masterMove.RefCurrentPos();
		}
		const DsVec4d& RefMasterMoveRot() const
		{
			return m_masterMove.RefCurrentRot();
		}

		const int GetBoneNum() const { return m_boneNum; }
		
	public:
		const std::string& RefName() const
		{
			return m_name;
		}

	private:
		std::string m_name;
		Pose* m_pBone;
		int m_boneNum;
		Pose m_masterMove;
	};
}

#endif