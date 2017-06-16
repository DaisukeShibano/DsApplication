#ifndef _DS_AMIM_CUSTOM_PROPERTY_
#define _DS_AMIM_CUSTOM_PROPERTY_

namespace DsLib
{
	struct DsAnimRagdollParamId
	{
		int boneIndex;
		int ragdollParamId;
	};

	struct DsAnimCustomProperty
	{
		
		std::vector<DsAnimRagdollParamId> ragdollParamIds;
	};
}

#endif