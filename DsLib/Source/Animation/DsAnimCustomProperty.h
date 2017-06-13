#ifndef _DS_AMIM_CUSTOM_PROPERTY_
#define _DS_AMIM_CUSTOM_PROPERTY_

namespace DsLib
{
	struct DsAnimCustomProperty
	{
		struct RagdollParamId
		{
			int boneIndex;
			int ragdollParamId;
		};
		std::vector<RagdollParamId> ragdollParamIds;
	};
}

#endif