#ifndef _DS_ANIM_SM_H_
#define _DS_ANIM_SM_H_

namespace DsLib
{
	class DsAnimBaseNode;
	class DsAnimController;
	class DsKeyframeAnim;
	class DsAnimClip;
}

namespace DsLib
{
	class DsAnimSM
	{
	public:
		enum STARE
		{
			NONE,
			IDLE,
			WALK,
			RUN,
			STATE_NUM,
		};

	public:
		DsAnimSM(const DsAnimController& animController);
		virtual ~DsAnimSM();

	public:
		void Initialize(DsKeyframeAnim* pAnim, int animNum);
		void Update(double dt);
		const DsAnimClip* GetActiveClip() const;
		const DsAnimClip* GetPreActiveClip() const;
		const DsAnimClip* GetDefaultClip() const;

	private:
		void _SetActiveState(STARE state);

	private:
		void _Idle(double dt);
		void _Walk(double dt);
		void _Run(double dt);

	private:
		const DsAnimController& m_animController;
		STARE m_activeState;
		STARE m_prevActiveState;

	private:
		DsAnimClip* m_pClips[STATE_NUM];

	};
}

#endif