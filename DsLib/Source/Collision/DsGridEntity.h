#pragma once

namespace DsLib
{
}

namespace DsLib 
{
	struct DsGridEntity
	{
		enum {
			INVALID = 0,//����ID
		};
		int modelIndex;
		int faceIndex;
		unsigned int nextEntityId;
	};
}
