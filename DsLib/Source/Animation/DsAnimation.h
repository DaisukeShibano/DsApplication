#ifndef _DS_ANIMATION_H_
#define _DS_ANIMATION_H_

#ifndef _DS_ANIM_BLEND_H_
#include "Animation/Blend/DsAnimBlend.h"
#endif

namespace DsLib
{
	class DsAnimSkeleton;
	class DsKeyframeAnimSet;
	class DsAnimModel;
	class DsSkinMesh;
	class DsDrawCommand;
	class DsAnimRes;
	struct DsAnimCustomProperty;
	class DsAnimSkeletonModifier;
	class DsAnimClip;
}

namespace DsLib
{
	class DsAnimation
	{
	public:
		DsAnimation(const DsAnimRes& anim, DsDrawCommand& com);
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
		void RequestPlayAnim(DsAnimClip* pAnim) { m_pRequestAnim = pAnim; }
		std::vector<DsAnimClip*>& RefAnimClips() { return m_animClips; }
		const DsAnimClip* GetPlayAnim()const { return m_pPlayAnim; }

	private:
		DsAnimModel* _GetAnimModel();

	private:
		DsAnimSkeleton* m_pSkeleton;
		DsKeyframeAnimSet* m_pKeyframeAnim;
		DsAnimModel* m_pAnimModel;
		DsSkinMesh* m_pSkinMesh;
		DsAnimCustomProperty* m_pCustomProperty;
		DsAnimSkeletonModifier* m_animModifier;
		DsAnimBlend m_blend;
		DsVec3d m_pos;
		DsMat33d m_rot;
		std::vector<DsAnimClip*> m_animClips;
		DsAnimClip* m_pRequestAnim;
		DsAnimClip* m_pPlayAnim;
		DsAnimClip* m_pPlayAnimPre;

	private:
		DsDrawCommand& m_com;
	};

	
}

#endif