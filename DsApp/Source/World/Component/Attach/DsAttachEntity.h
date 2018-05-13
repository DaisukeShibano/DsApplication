#ifndef _DS_ATTACH_ENTITY_
#define _DS_ATTACH_ENTITY_

/*
#ifndef _DS_ATTACH_ENTITY_
#include "World/Component/Attach/DsAttachEntity.h"
#endif
*/

namespace DsApp
{
	class DsAttachEntity
	{
	public:
		virtual void SetTransform(const DsMat44d& transform) = 0;
	};
}

#endif