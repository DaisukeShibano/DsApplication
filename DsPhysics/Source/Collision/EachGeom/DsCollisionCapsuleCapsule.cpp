#include "DsPhysicsPch.h"
#ifndef _DS_COLLISION_CAPSULE_CAPSULE_
#include "Collision/EachGeom/DsCollisionCapsuleCapsule.h"
#endif
//‘¼‚Ìƒwƒbƒ_
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif

using namespace DsLib;
using namespace DsPhysics;

#define dRecip(x) (1.0/(x))

static __inline double dCalcPointsDistance3(const DsVec3d& a, const DsVec3d& b)
{
	const DsVec3d tmp = a - b;
	return tmp.Length();
}

//“¯‚¶ˆ—‚ªDsCollisionBoxCapsule‚É‚à‚ ‚é‚Ì‚Å“Z‚ß‚éB
int DsCollisionCapsuleCapsule::_dCollideSpheres(const DsVec3d& p1, double r1, const DsVec3d& p2, double r2, DsCollisionResult& c) const
{
	// printf ("d=%.2f  (%.2f %.2f %.2f) (%.2f %.2f %.2f) r1=%.2f r2=%.2f\n",
	//	  d,p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],r1,r2);

	double d = dCalcPointsDistance3(p1, p2);
	if (d > (r1 + r2)) return 0;
	if (d <= 0) {
		c.AddInfo(p1, DsVec3d(1, 0, 0), r1 + r2, m_pCapsule1->RefOwnerId(), m_pCapsule2->RefOwnerId());
		//c->pos[0] = p1[0];
		//c->pos[1] = p1[1];
		//c->pos[2] = p1[2];
		//c->normal[0] = 1;
		//c->normal[1] = 0;
		//c->normal[2] = 0;
		//c->depth = r1 + r2;
	}
	else {
		double d1 = dRecip(d);
		const DsVec3d normal = (p1 - p2)*d1;
		//c->normal[0] = (p1[0]-p2[0])*d1;
		//c->normal[1] = (p1[1]-p2[1])*d1;
		//c->normal[2] = (p1[2]-p2[2])*d1;
		double k = 0.5 * (r2 - r1 - d);
		const DsVec3d colPos = p1 + (normal*k);
		//c->pos[0] = p1[0] + c->normal[0]*k;
		//c->pos[1] = p1[1] + c->normal[1]*k;
		//c->pos[2] = p1[2] + c->normal[2]*k;
		//c->depth = r1 + r2 - d;
		c.AddInfo(colPos, normal, r1 + r2 - d, m_pCapsule1->RefOwnerId(), m_pCapsule2->RefOwnerId());
	}
	return 1;
}



// given two line segments A and B with endpoints a1-a2 and b1-b2, return the
// points on A and B that are closest to each other (in cp1 and cp2).
// in the case of parallel lines where there are multiple solutions, a
// solution involving the endpoint of at least one line will be returned.
// this will work correctly for zero length lines, e.g. if a1==a2 and/or
// b1==b2.
//
// the algorithm works by applying the voronoi clipping rule to the features
// of the line segments. the three features of each line segment are the two
// endpoints and the line between them. the voronoi clipping rule states that,
// for feature X on line A and feature Y on line B, the closest points PA and
// PB between X and Y are globally the closest points if PA is in V(Y) and
// PB is in V(X), where V(X) is the voronoi region of X.

void dClosestLineSegmentPoints (const DsVec3d& a1, const DsVec3d& a2,
                                const DsVec3d& b1, const DsVec3d& b2,
                                DsVec3d& cp1, DsVec3d& cp2)
{
    DsVec3d a1a2,b1b2,a1b1,a1b2,a2b1,a2b2,n;
    double la,lb,k,da1,da2,da3,da4,db1,db2,db3,db4,det;

    // check vertex-vertex features

	a1a2 = a2 - a1;
    b1b2 = b2 - b1;
    a1b1 = b1 - a1;
    da1 = DsVec3d::Dot(a1a2, a1b1);
    db1 = DsVec3d::Dot(b1b2, a1b1);
    if (da1 <= 0 && db1 >= 0) {
        cp1 = a1;
        cp2 = b1;
        return;
    }

    a1b2 = b2 - a1;
    da2 = DsVec3d::Dot(a1a2,a1b2);
    db2 = DsVec3d::Dot(b1b2,a1b2);
    if (da2 <= 0 && db2 <= 0) {
        cp1 = a1;
        cp2 = b2;
        return;
    }

    a2b1 = b1 - a2;
    da3 = DsVec3d::Dot(a1a2,a2b1);
    db3 = DsVec3d::Dot(b1b2,a2b1);
    if (da3 >= 0 && db3 >= 0) {
        cp1 = a2;
        cp2 = b1;
        return;
    }

    a2b2 = b2 - a2;
    da4 = DsVec3d::Dot(a1a2,a2b2);
    db4 = DsVec3d::Dot(b1b2,a2b2);
    if (da4 >= 0 && db4 <= 0) {
        cp1 = a2;
        cp2 = b2;
        return;
    }

    // check edge-vertex features.
    // if one or both of the lines has zero length, we will never get to here,
    // so we do not have to worry about the following divisions by zero.

    la = DsVec3d::Dot(a1a2,a1a2);
    if (da1 >= 0 && da3 <= 0) {
        k = da1 / la;
        n = a1b1 - (a1a2*k);
        if (DsVec3d::Dot(b1b2,n) >= 0) {
            cp1 = a1 + (a1a2*k);
            cp2 = b1;
            return;
        }
    }

    if (da2 >= 0 && da4 <= 0) {
        k = da2 / la;
        n = a1b2 -(a1a2*k);
        if (DsVec3d::Dot(b1b2,n) <= 0) {
            cp1 = a1 + (a1a2*k);
            cp2 = b2;
            return;
        }
    }

    lb = DsVec3d::Dot(b1b2,b1b2);
    if (db1 <= 0 && db2 >= 0) {
        k = -db1 / lb;
        n = -a1b1 - (b1b2*k);
        if (DsVec3d::Dot(a1a2,n) >= 0) {
            cp1 = a1;
            cp2 = b1 + (b1b2*k);
            return;
        }
    }

    if (db3 <= 0 && db4 >= 0) {
        k = -db3 / lb;
        n = -a2b1 - (b1b2*k);
        if (DsVec3d::Dot(a1a2,n) <= 0) {
            cp1 = a2;
            cp2 = b1 + (b1b2*k);
            return;
        }
    }

    // it must be edge-edge

    k = DsVec3d::Dot(a1a2,b1b2);
    det = la*lb - k*k;
    if (det <= 0) {
        // this should never happen, but just in case...
        cp1 = a1;
        cp2 = b1;
        return;
    }
    det = dRecip (det);
    double alpha = (lb*da1 -  k*db1) * det;
    double beta  = ( k*da1 - la*db1) * det;
    cp1 = a1 + (a1a2*alpha);
    cp2 = b1 + (b1b2*beta);
}


int DsCollisionCapsuleCapsule::_dCollideCapsuleCapsule()
{
    int i;
    const double tolerance = (1e-5);

    // copy out some variables, for convenience
	const double lz1 = m_pCapsule1->GetSide().y;
    const double lz2 = m_pCapsule2->GetSide().y;
    const DsVec3d pos1 = m_pCapsule1->GetBasePos();
    const DsVec3d pos2 = m_pCapsule2->GetBasePos();
    //double axis1[3],axis2[3];
    //axis1[0] = o1->final_posr->R[2];
    //axis1[1] = o1->final_posr->R[6];
    //axis1[2] = o1->final_posr->R[10];
    //axis2[0] = o2->final_posr->R[2];
    //axis2[1] = o2->final_posr->R[6];
    //axis2[2] = o2->final_posr->R[10];
	const DsVec3d axis1 = m_pCapsule1->GetRot().GetAxisY();
	DsVec3d axis2 = m_pCapsule2->GetRot().GetAxisY();

    // if the cylinder axes are close to parallel, we'll try to detect up to
    // two contact points along the body of the cylinder. if we can't find any
    // points then we'll fall back to the closest-points algorithm. note that
    // we are not treating this special case for reasons of degeneracy, but
    // because we want two contact points in some situations. the closet-points
    // algorithm is robust in all casts, but it can return only one contact.

    DsVec3d sphere1, sphere2;
    double a1a2 = DsVec3d::Dot (axis1, axis2);
    double det = (1.0)-a1a2*a1a2;
    if (det < tolerance) {
        // the cylinder axes (almost) parallel, so we will generate up to two
        // contacts. alpha1 and alpha2 (line position parameters) are related by:
        //       alpha2 =   alpha1 + (pos1-pos2)'*axis1   (if axis1==axis2)
        //    or alpha2 = -(alpha1 + (pos1-pos2)'*axis1)  (if axis1==-axis2)
        // first compute where the two cylinders overlap in alpha1 space:
        if (a1a2 < 0) {
            axis2[0] = -axis2[0];
            axis2[1] = -axis2[1];
            axis2[2] = -axis2[2];
        }
		const DsVec3d q = pos1 - pos2;
        double k = DsVec3d::Dot (axis1, q);
        double a1lo = -lz1;
        double a1hi = lz1;
        double a2lo = -lz2 - k;
        double a2hi = lz2 - k;
        double lo = (a1lo > a2lo) ? a1lo : a2lo;
        double hi = (a1hi < a2hi) ? a1hi : a2hi;
        if (lo <= hi) {
            if (lo < hi) {
                // generate up to two contacts. if one of those contacts is
                // not made, fall back on the one-contact strategy.
                sphere1 = pos1 + (axis1 * lo);
                sphere2 = pos2 + (axis2*(lo + k));
                int n1 = _dCollideSpheres (sphere1, m_pCapsule1->GetSide().x, sphere2, m_pCapsule2->GetSide().x, m_info);
                if (n1) {
                    for (i=0; i<3; i++) sphere1[i] = pos1[i] + hi*axis1[i];
                    for (i=0; i<3; i++) sphere2[i] = pos2[i] + (hi+k)*axis2[i];
                    int n2 = _dCollideSpheres (sphere1, m_pCapsule1->GetSide().x, sphere2, m_pCapsule2->GetSide().x, m_info);
                    if (n2) {
                        return 2;
                    }
                }
            }

            // just one contact to generate, so put it in the middle of
            // the range
            double alpha1 = (lo + hi) * (0.5);
            double alpha2 = alpha1 + k;
			sphere1 = pos1 + (axis1*alpha1);
			sphere2 = pos2 + (axis2*alpha2);
            return _dCollideSpheres (sphere1, m_pCapsule1->GetSide().x, sphere2, m_pCapsule2->GetSide().x, m_info);
        }
    }

    // use the closest point algorithm
    const DsVec3d a1 = m_pCapsule1->GetBasePos() + (axis1 * lz1);
	const DsVec3d a2 = m_pCapsule1->GetBasePos() - (axis1 * lz1);
	const DsVec3d b1 = m_pCapsule2->GetBasePos() + (axis2 * lz2);
	const DsVec3d b2 = m_pCapsule2->GetBasePos() - (axis2 * lz2);

    dClosestLineSegmentPoints (a1, a2, b1, b2, sphere1, sphere2);
    return _dCollideSpheres (sphere1, m_pCapsule1->GetSide().x, sphere2, m_pCapsule2->GetSide().x, m_info);
}

//virtual 
DsCollisionResult& DsCollisionCapsuleCapsule::Collide()// override
{
	m_info.Clear();
	const bool isContain = DsAabb::IsContain(*m_pCapsule1->GetAabb(), *m_pCapsule2->GetAabb());
	if (isContain) {
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pCapsule1->RefOwnerId().GetActor(), *m_pCapsule2->RefOwnerId().GetActor())) {
				return m_info;
			}
		}
		_dCollideCapsuleCapsule();
	}
	return m_info;
}