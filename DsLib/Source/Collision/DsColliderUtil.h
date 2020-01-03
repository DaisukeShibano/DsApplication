#pragma once
#include "Collision/DsShapeSegment.h"
#include "Math/DsAabb.h"

namespace DsLib
{
	class DsColliderUtil
	{
	public:

		//線分と三角形
		static float SegTri(const DsShapeSegment& seg, const DsVec3f vert[3])
		{
			//OPCODE\OPC_RayTriOverlap.hより
			const DsVec3f edge1 = vert[1] - vert[0];
			const DsVec3f edge2 = vert[2] - vert[0];

			const DsVec3f pvec = DsVec3f::Cross(seg.dir, edge2);

			float det = DsVec3f::Dot(edge1, pvec);

			if (det <= 0.0f) {
				return -1.0f;
			}

			const DsVec3f tvec = seg.origin - vert[0];

			const float u = DsVec3f::Dot(tvec, pvec);
			if (u < 0.0f) {
				return -1.0f;
			}
			if (u > det) {
				return -1.0f;
			}

			const DsVec3f qvec = DsVec3f::Cross(tvec, edge1);

			const float v = DsVec3f::Dot(seg.dir, qvec);
			if (v < 0.0f) {
				return -1.0f;
			}

			if ((u + v) > det) {
				return -1.0f;
			}

			float distance = DsVec3f::Dot(seg.dir, qvec);
			if (distance < 0.0f) {
				return -1.0f;
			}

			distance *= 1.0f / det;

			if (seg.len < distance) {
				return -1.0f;
			}

			return distance;
		}

		//線分と三角形 裏面除外
		static float SegTriBackfaceCull(const DsShapeSegment& seg, const DsVec3f vert[3])
		{
			//OPCODE\OPC_RayTriOverlap.hより
			const DsVec3f edge1 = vert[1] - vert[0];
			const DsVec3f edge2 = vert[2] - vert[0];

			const DsVec3f pvec = DsVec3f::Cross(seg.dir, edge2);

			float det = DsVec3f::Dot(edge1, pvec);

			if (std::fabs(det) <= 0.0f) {
				return -1.0f;
			}

			const float OneOverDet = 1.0f / det;

			// Calculate distance from vert0 to ray origin
			const DsVec3f tvec = seg.origin - vert[0];

			// Calculate U parameter and test bounds
			const float u = DsVec3f::Dot(tvec, pvec) * OneOverDet;
			//		if(IR(u)&0x80000000 || u>1.0f)					return FALSE;
			if (u < 0.0f) {
				return -1.0f;
			}
			if (u > 1.0f) {
				return -1.0f;
			}

			// prepare to test V parameter
			const DsVec3f qvec = DsVec3f::Cross(tvec, edge1);

			// Calculate V parameter and test bounds
			float v = DsVec3f::Dot(seg.dir, qvec) * OneOverDet;
			if (v < 0.0f) {
				return -1.0f;
			}
			if ((u + v) > 1.0f) {
				return -1.0f;
			}

			// Calculate t, ray intersects triangle
			float distance = DsVec3f::Dot(edge2, qvec) * OneOverDet;
			// Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
			if (distance < 0.0f) {
				return -1.0f;
			}

			if (seg.len < distance) {
				return -1.0f;
			}

			return distance;
		}

		//線分とaabb
		static bool SegAabb(const DsShapeSegment& seg, const DsAabbf& aabb)
		{
			//mDataは半分
			//mData2は中点
			//mFDirはfabs(mData)
			//OPCODE\OPC_RayAABBOverlap.h
			//inline_ BOOL RayCollider::SegmentAABBOverlap(const Point& center, const Point& extents)
			//より
			//float Dx = mData2.x - center.x;		if (fabsf(Dx) > extents.x + mFDir.x)	return FALSE;
			//float Dy = mData2.y - center.y;		if (fabsf(Dy) > extents.y + mFDir.y)	return FALSE;
			//float Dz = mData2.z - center.z;		if (fabsf(Dz) > extents.z + mFDir.z)	return FALSE;
			//
			//float f;
			//f = mData.y * Dz - mData.z * Dy;	if (fabsf(f) > extents.y*mFDir.z + extents.z*mFDir.y)	return FALSE;
			//f = mData.z * Dx - mData.x * Dz;	if (fabsf(f) > extents.x*mFDir.z + extents.z*mFDir.x)	return FALSE;
			//f = mData.x * Dy - mData.y * Dx;	if (fabsf(f) > extents.x*mFDir.y + extents.y*mFDir.x)	return FALSE;
			//
			//return TRUE;

			const DsVec3f center = aabb.GetCenter();
			const DsVec3f extents = aabb.GetExtents();

			float Dx = seg.halfVec.x - center.x;
			if (fabs(Dx) > extents.x + seg.halfVecFabs.x) {
				return false;
			}

			float Dy = seg.halfVec.y - center.y;
			if (fabs(Dy) > extents.y + seg.halfVecFabs.y) {
				return false;
			}

			float Dz = seg.halfVec.z - center.z;
			if (fabs(Dz) > extents.z + seg.halfVecFabs.z) {
				return false;
			}

			float f;

			f = seg.halfVec.y * Dz - seg.halfVec.z * Dy;
			if (fabs(f) > extents.y*seg.halfVecFabs.z + extents.z*seg.halfVecFabs.y) {
				return false;
			}

			f = seg.halfVec.z * Dx - seg.halfVec.x * Dz;
			if (fabs(f) > extents.x*seg.halfVecFabs.z + extents.z*seg.halfVecFabs.x) {
				return false;
			}

			f = seg.halfVec.x * Dy - seg.halfVec.y * Dx;
			if (fabs(f) > extents.x*seg.halfVecFabs.y + extents.y*seg.halfVecFabs.x) {
				return false;
			}

			return true;
		}
	};
}

