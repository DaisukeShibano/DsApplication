#pragma once

namespace DsLib
{
}

namespace DsLib 
{
	struct DsGridEntity
	{
		enum {
			INVALID = 0,//–³ŒøID
		};
		int modelIndex;
		int faceIndex;
		unsigned int nextEntityId;
	};
}
