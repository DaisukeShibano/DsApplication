#pragma once
#include "Math/DsAabb.h"

namespace DsLib
{
	class DsAabbTree;
}

namespace DsLib 
{
	class DsAabbNode
	{
		friend DsAabbTree;

	public:
		DsAabbNode();
		~DsAabbNode();

	private:
		DsAabbf m_aabb;
		int m_myIdx;
		int m_parentIdx;
		int m_childIdx[8];

		//Å™Ç¢ÇÁÇÒ

		
	};
}
