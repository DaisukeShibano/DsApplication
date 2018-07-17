#include "DsPhysicsPch.h"
#ifndef _DS_COLLISION_BOX_CAPSULE_
#include "Collision/EachGeom/DsCollisionBoxCapsule.h"
#endif
//‘¼‚Ìƒwƒbƒ_
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
#endif
#ifndef __DS_COLLISION_BOX_BOX__
#include "Collision/EachGeom/DsCollisionBoxBox.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif

using namespace DsLib;
using namespace DsPhysics;


//«‚Ü‚¾—‰ð‚µ‚«‚Á‚Ä‚È‚¢

#define dRecip(x) (1.0/(x))


//‹ts—ñ‚Æ“¯‚¶ˆÓ–¡
static __inline void dMultiply1_331(DsVec3d& res, const DsMat33d& a, const DsVec3d& b)
{
	//const double res_0 = dCalcVectorDot3_41(a, b);
	//const double res_1 = dCalcVectorDot3_41(a + 1, b);
	//const double res_2 = dCalcVectorDot3_41(a + 2, b);
	///* Only assign after all the calculations are over to avoid incurring memory aliasing*/
	//res[0] = res_0; res[1] = res_1; res[2] = res_2;
	res = a.ToTransposition() * b;
}

//DsMat33*DsVec3 ‚Æ“¯‚¶
static __inline void dMultiply0_331(DsVec3d& res, const DsMat33d& a, const DsVec3d& b)
{
	//const dReal res_0 = dCalcVectorDot3(a, b);
	//const dReal res_1 = dCalcVectorDot3(a + 4, b);
	//const dReal res_2 = dCalcVectorDot3(a + 8, b);
	///* Only assign after all the calculations are over to avoid incurring memory aliasing*/
	//res[0] = res_0; res[1] = res_1; res[2] = res_2;
	res = a * b;
}

static __inline double dCalcPointsDistance3(const DsVec3d& a, const DsVec3d& b)
{
	const DsVec3d tmp = a-b;
	return tmp.Length();
}

// a simple root finding algorithm is used to find the value of 't' that
// satisfies:
//		d|D(t)|^2/dt = 0
// where:
//		|D(t)| = |p(t)-b(t)|
// where p(t) is a point on the line parameterized by t:
//		p(t) = p1 + t*(p2-p1)
// and b(t) is that same point clipped to the boundary of the box. in box-
// relative coordinates d|D(t)|^2/dt is the sum of three x,y,z components
// each of which looks like this:
//
//	    t_lo     /
//	      ______/    -->t
//	     /     t_hi
//	    /
//
// t_lo and t_hi are the t values where the line passes through the planes
// corresponding to the sides of the box. the algorithm computes d|D(t)|^2/dt
// in a piecewise fashion from t=0 to t=1, stopping at the point where
// d|D(t)|^2/dt crosses from negative to positive.

void dClosestLineBoxPoints (const DsVec3d& p1, const DsVec3d& p2,
                            const DsVec3d& c, const DsMat33d& R,
                            const DsVec3d& side,
                            DsVec3d& lret, DsVec3d& bret)
{
    int i;

    // compute the start and delta of the line p1-p2 relative to the box.
    // we will do all subsequent computations in this box-relative coordinate
    // system. we have to do a translation and rotation for each point.
    DsVec3d tmp, s, v;
	tmp = p1 - c;
    dMultiply1_331 (s, R, tmp);//‰~’Œ‚ÌŽn“_(BoxÀ•WŒn)
    tmp = p2 - p1;
    dMultiply1_331 (v, R, tmp);//‰~’Œ‚ÌƒxƒNƒgƒ‹(BoxÀ•WŒn)

    // mirror the line so that v has all components >= 0
    DsVec3d sign;
    for (i=0; i<3; i++) {
        if (v[i] < 0) {
            s[i] = -s[i];
            v[i] = -v[i];
            sign[i] = -1;
        }
        else sign[i] = 1;
    }

    // compute v^2
    DsVec3d v2;
    v2[0] = v[0]*v[0];
    v2[1] = v[1]*v[1];
    v2[2] = v[2]*v[2];

    // compute the half-sides of the box
    double h[3];
    h[0] = /*0.5 * */side[0];
    h[1] = /*0.5 * */side[1];
    h[2] = /*0.5 * */side[2];

    // region is -1,0,+1 depending on which side of the box planes each
    // coordinate is on. tanchor is the next t value at which there is a
    // transition, or the last one if there are no more.
    int region[3];
    double tanchor[3];

    // Denormals are a problem, because we divide by v[i], and then 
    // multiply that by 0. Alas, infinity times 0 is infinity (!)
    // We also use v2[i], which is v[i] squared. Here's how the epsilons 
    // are chosen:
    // float epsilon = 1.175494e-038 (smallest non-denormal number)
    // double epsilon = 2.225074e-308 (smallest non-denormal number)
    // For single precision, choose an epsilon such that v[i] squared is 
    // not a denormal; this is for performance.
    // For double precision, choose an epsilon such that v[i] is not a 
    // denormal; this is for correctness. (Jon Watte on mailinglist)

    const double tanchor_eps = 1e-307;

    // find the region and tanchor values for p1
    for (i=0; i<3; i++) {
        if (v[i] > tanchor_eps) {
            if (s[i] < -h[i]) {
                region[i] = -1;
                tanchor[i] = (-h[i]-s[i])/v[i];
            }
            else {
                region[i] = (s[i] > h[i]);
                tanchor[i] = (h[i]-s[i])/v[i];
            }
        }
        else {
            region[i] = 0;
            tanchor[i] = 2;		// this will never be a valid tanchor
        }
    }

    // compute d|d|^2/dt for t=0. if it's >= 0 then p1 is the closest point
    double t=0;
    double dd2dt = 0;
    for (i=0; i<3; i++) dd2dt -= (region[i] ? v2[i] : 0) * tanchor[i];
    if (dd2dt >= 0) goto got_answer;

    do {
        // find the point on the line that is at the next clip plane boundary
        double next_t = 1;
        for (i=0; i<3; i++) {
            if (tanchor[i] > t && tanchor[i] < 1 && tanchor[i] < next_t)
                next_t = tanchor[i];
        }

        // compute d|d|^2/dt for the next t
        double next_dd2dt = 0;
        for (i=0; i<3; i++) {
            next_dd2dt += (region[i] ? v2[i] : 0) * (next_t - tanchor[i]);
        }

        // if the sign of d|d|^2/dt has changed, solution = the crossover point
        if (next_dd2dt >= 0) {
            double m = (next_dd2dt-dd2dt)/(next_t - t);
            t -= dd2dt/m;
            goto got_answer;
        }

        // advance to the next anchor point / region
        for (i=0; i<3; i++) {
            if (tanchor[i] == next_t) {
                tanchor[i] = (h[i]-s[i])/v[i];
                region[i]++;
            }
        }
        t = next_t;
        dd2dt = next_dd2dt;
    }
    while (t < 1);
    t = 1;

got_answer:

    // compute closest point on the line
    for (i=0; i<3; i++) lret[i] = p1[i] + t*tmp[i];	// note: tmp=p2-p1

    // compute closest point on the box
    for (i=0; i<3; i++) {
        tmp[i] = sign[i] * (s[i] + t*v[i]);
        if (tmp[i] < -h[i]) tmp[i] = -h[i];
        else if (tmp[i] > h[i]) tmp[i] = h[i];
    }
    dMultiply0_331 (s, R, tmp);
    for (i=0; i<3; i++) bret[i] = s[i] + c[i];
}

int DsCollisionBoxCapsule::_dCollideSpheres(const DsVec3d& p1, double r1,
                    const DsVec3d& p2, double r2, DsCollisionResult& c) const
{
    // printf ("d=%.2f  (%.2f %.2f %.2f) (%.2f %.2f %.2f) r1=%.2f r2=%.2f\n",
    //	  d,p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],r1,r2);

    double d = dCalcPointsDistance3(p1, p2);
    if (d > (r1 + r2)) return 0;
    if (d <= 0) {
		c.AddInfo(p1, DsVec3d(1, 0, 0), r1 + r2, m_pCapsule->RefOwnerId(), m_pBox->RefOwnerId());
        //c->pos[0] = p1[0];
        //c->pos[1] = p1[1];
        //c->pos[2] = p1[2];
        //c->normal[0] = 1;
        //c->normal[1] = 0;
        //c->normal[2] = 0;
        //c->depth = r1 + r2;
    }
    else {
        double d1 = dRecip (d);
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
		c.AddInfo(colPos, normal, r1 + r2 - d, m_pCapsule->RefOwnerId(), m_pBox->RefOwnerId());
    }
    return 1;
}


int DsCollisionBoxCapsule::_dCollideCapsuleBox()
{
    // get p1,p2 = cylinder axis endpoints, get radius
    DsVec3d p1,p2;
    const double clen = m_pCapsule->GetSide().y * 0.5;
	p1 = m_pCapsule->GetBasePos() + (m_pCapsule->GetRot().GetAxisY()*clen);
	p2 = m_pCapsule->GetBasePos() - (m_pCapsule->GetRot().GetAxisY()*clen);
    const double radius = m_pCapsule->GetSide().x;

    // copy out box center, rotation matrix, and side array
    const DsVec3d c = m_pBox->GetBasePos();
    const DsMat33d R = m_pBox->GetRot();
    const DsVec3d side = m_pBox->GetSide();

    // get the closest point between the cylinder axis and the box
    DsVec3d pl,pb;
    dClosestLineBoxPoints (p1, p2, c, R, side, pl, pb);

    // if the capsule is penetrated further than radius 
    //  then pl and pb are equal (up to mindist) -> unknown normal
    // use normal vector of closest box surface
    double mindist = 1e-15;
    if (dCalcPointsDistance3(pl, pb) < mindist) {
        // consider capsule as box
        //DsVec3d normal;
        //double depth;
        //int code;
        // WARNING! rad2 is declared as #define in Microsoft headers (as well as psh2, chx2, grp2, frm2, rct2, ico2, stc2, lst2, cmb2, edt2, scr2). Avoid abbreviations!
        /* double rad2 = radius*REAL(2.0); */ double radiusMul2 = radius * 2.0;
        const DsVec3d capboxside = {radiusMul2, radiusMul2, m_pCapsule->GetSide().y*2.0 + radiusMul2};
		DsCollisionGeometry tmpCapsule(NULL, 0, NULL, 0, NULL, 0, m_pCapsule->RefOwnerId(), NULL, capboxside, m_pCapsule->GetBoungingTree(), m_pCapsule->GetAabb(), m_pCapsule->GetRot());

		DsCollisionBoxBox boxbox(m_world);
		boxbox.Initialize(m_pBox, &tmpCapsule);
		const DsCollisionResult& result = boxbox.CollideDirect();
        //int num = dBoxBox (c, R, side, 
        //    o1->final_posr->pos, o1->final_posr->R, capboxside,
        //    &normal, &depth, &code, flags, contact, skip);
		const int num = result.GetColNum();
        //for (int i=0; i< num; i++) {
        //    dContactGeom *currContact = ((dContactGeom*)(((char*)contact) + (i*skip)));
        //    currContact->normal[0] = normal[0];
        //    currContact->normal[1] = normal[1];
        //    currContact->normal[2] = normal[2];
        //}
		m_info.AddInfo(result);
        return num;
    } else {
        // generate contact point
        return _dCollideSpheres(pl, radius, pb, 0, m_info);
    }
}


//virtual 
DsCollisionResult& DsCollisionBoxCapsule::Collide()// override
{
	m_info.Clear();
	const bool isContain = DsAabb::IsContain(*m_pBox->GetAabb(), *m_pCapsule->GetAabb());
	if (isContain) {
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pBox->RefOwnerId().GetActor(), *m_pCapsule->RefOwnerId().GetActor())) {
				return m_info;
			}
		}
		_dCollideCapsuleBox();
	}
	return m_info;
}