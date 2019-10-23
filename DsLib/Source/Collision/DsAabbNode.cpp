#include "DsPch.h"
#include "Collision/DsAabbNode.h"
#include "Collision/DsAabbTree.h"

using namespace DsLib;

DsAabbNode::DsAabbNode()
	: m_aabb()
	, m_myIdx(-1)
	, m_parentIdx(-1)
	, m_childIdx{-1, -1, -1, -1, -1, -1, -1, -1}
{
}


DsAabbNode::~DsAabbNode()
{
}
