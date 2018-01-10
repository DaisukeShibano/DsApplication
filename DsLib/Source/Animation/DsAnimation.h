#ifndef _DS_ANIMATION_H_
#define _DS_ANIMATION_H_

#ifndef _DS_ANIM_SM_H_
#include "Animation/AnimSM/DsAnimSM.h"
#endif
#ifndef _DS_ANIM_BLEND_H_
#include "Animation/Blend/DsAnimBlend.h"
#endif

namespace DsLib
{
	class DsAnimController;
	class DsAnimSkeleton;
	class DsKeyframeAnimSet;
	class DsAnimModel;
	class DsSkinMesh;
	class DsDrawCommand;
	class DsAnimRes;
	struct DsAnimCustomProperty;
	class DsAnimSkeletonModifier;
}

namespace DsLib
{
	class DsAnimation
	{
	public:
		DsAnimation(const DsAnimController& animController, const DsAnimRes& anim, DsDrawCommand& com);
		virtual ~DsAnimation();

	public:
		void RegisterDraw();
		void Update(double dt);
		void SetRootMatrix(const DsVec3d& p, const DsMat33d& r);
		const DsVec3d& GetPosition() const;
		const DsMat33d& GetRotation() const;
		const DsAnimCustomProperty* GetCustomProperty() const { return m_pCustomProperty; }
		void SetAnimSkeletonModifier(DsAnimSkeletonModifier* pModifier) { m_animModifier = pModifier; }
		DsAnimSkeleton* GetSkeleton() { return m_pSkeleton; }
		const DsAnimSkeleton* GetSkeleton() const { return m_pSkeleton; }
		DsKeyframeAnimSet* GetKeyframeAnim() const { return m_pKeyframeAnim; }

	private:
		DsAnimModel* _GetAnimModel();

	private:
		DsAnimSM m_animSM;
		DsAnimSkeleton* m_pSkeleton;
		DsKeyframeAnimSet* m_pKeyframeAnim;
		DsAnimModel* m_pAnimModel;
		DsSkinMesh* m_pSkinMesh;
		DsAnimCustomProperty* m_pCustomProperty;
		DsAnimSkeletonModifier* m_animModifier;
		DsAnimBlend m_blend;
		DsVec3d m_pos;
		DsMat33d m_rot;

	private:
		DsDrawCommand& m_com;
	};

	
}

#endif