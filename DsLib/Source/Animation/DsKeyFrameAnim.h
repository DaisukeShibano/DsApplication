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
			Pose();
			~Pose();

		public:
			void Initialize();
			void Update(double dt, DsVec4d* pOutDeltaRot, DsVec3d* pOutDeltaPos);
			void Reset();
			void SetLocalTime(double time, DsVec4d* pOutDeltaRot, DsVec3d* pOutDeltaPos);
			bool IsEnd() const;

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

			double GetMaxLocalTime() const { return m_maxLocalTimeMs; }

		private:
			void _Update(DsVec4d* pOutDeltaRot, DsVec3d* pOutDeltaPos);

		private:
			double m_currentTimeMs;
			double m_maxLocalTimeMs;

			Vec3Key* m_pPos;
			Vec4Key* m_pRot;
			Vec3Key* m_pScale;
			//ボーンごとにキースレーム数が違う。おそらく変化の無いやつは省かれてる
			int m_keyFrameNumRot;
			int m_keyFrameNumPos;
			int m_keyFrameNumScale;

			int m_currentIdxRot;
			int m_currentIdxPos;
			int m_currentIdxScale;
			unsigned char m_endFlag;
		};

	public:
		DsKeyframeAnim();
		virtual ~DsKeyframeAnim();

	public:
		void Initialize();
		void Update(double dt);
		void Reset();
		void SetLocalTime(double time);
		bool IsEnd() const;
		void SetLoop(bool isLoop);

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
			return m_masterMoveDeltaPos;
		}
		const DsVec4d& RefMasterMoveRot() const
		{
			return m_masterMoveDeltaRot;
		}

		const int GetBoneNum() const { return m_boneNum; }
		
	public:
		const std::string& RefName() const
		{
			return m_name;
		}

	private:
		Pose m_masterMove;
		DsVec4d m_masterMoveDeltaRot;
		DsVec3d m_masterMoveDeltaPos;
		DsVec4d m_masterMoveDeltaRotPre;
		DsVec3d m_masterMoveDeltaPosPre;
		std::string m_name;
		double m_currentTimeMs;
		double m_maxLocalTimeMs;
		Pose* m_pBone;
		int m_boneNum;
		bool m_isLoop;
	};
}

#endif