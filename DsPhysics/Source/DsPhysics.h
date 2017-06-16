#ifndef _DS_PHYSICS_H_
#define _DS_PHYSICS_H_

/*コピペ用
#ifndef _DS_PHYSICS_H_
#include "DsPhysics.h"
#endif
*/

#ifndef _DS_LIB_H_
#include "DsLib.h"
#endif

//他プロジェクトではプリコンパイル済みヘッダでusing namespace DsLibが定義されてないので
//他のプロジェクトでusing namespace DsLib前提のヘッダをincludeしちゃうとエラー出ちゃうためここで定義
using namespace DsLib;

#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef __DS_PHYSICS_MANAGER__
#include "DsPhysicsManager.h"
#endif
#ifndef _DS_RIGID_BODY__
#include "Actor/DsRigidCube.h"
#endif
#ifndef _DS_RIGID_MESH_H_
#include "Actor/DsRigidMesh.h"
#endif
#ifndef __DS_RIGID_SPHERE__
#include "Actor/DsRigidSphere.h"
#endif
#ifndef __DS_RIGID_CAPSULE__
#include "Actor/DsRigidCapsule.h"
#endif
#ifndef __DS_RAY__
#include "Actor/DsRay.h"
#endif
#ifndef _DS_JOINT_
#include "Joint/DsJoint.h"
#endif
#ifndef _DS_BALL_JOINT_H_
#include "Joint/DsBallJoint.h"
#endif
#ifndef _DS_HINGE_JOINT_
#include "Joint/DsHingeJoint.h"
#endif
#ifndef _DS_RAGDOLL_
#include "Ragdoll/DsRagdoll.h"
#endif

#endif