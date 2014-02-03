#include "stdafx.h"
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "logicworld.h"
#include "clientlogic_private.h"

LogicWorld::LogicWorld(void)
    : m_defaultContactProcessingThreshold(BT_LARGE_FLOAT)
{
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    btVector3 worldMin(-1000,-1000,-1000);
    btVector3 worldMax(1000,1000,1000);
    btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
    m_overlappingPairCache = sweepBP;

    m_constraintSolver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);
    m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

    btTransform startTransform;
    startTransform.setIdentity ();
    startTransform.setOrigin (btVector3(0.0, 0.0, 10.0));
    //startTransform.setOrigin (btVector3(10.210098f,-1.6433364f,16.453260f));


    m_ghostObject = new btPairCachingGhostObject();
    m_ghostObject->setWorldTransform(startTransform);
    sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    btScalar characterHeight=4.0;
    btScalar characterWidth =1.0;
    btConvexShape* capsule = new btCapsuleShapeZ(characterWidth,characterHeight);
    m_ghostObject->setCollisionShape (capsule);
    m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

    btScalar stepHeight = btScalar(0.35);
    m_character = new btKinematicCharacterController (m_ghostObject,capsule,stepHeight, 2);
    m_character->setMaxJumpHeight(1);
    m_character->setJumpSpeed(20);
    m_character->setFallSpeed(55);
    m_character->setGravity(80);
    m_dynamicsWorld->addCollisionObject(m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

    m_dynamicsWorld->addAction(m_character);

    CreateFlatFloor();


    /*static btCollisionObject collObj;
    btBoxShape* boxShape = new btBoxShape(btVector3(1,1,1));
    collObj.setCollisionShape(boxShape);
    m_dynamicsWorld->addCollisionObject(&collObj);*/
}

LogicWorld::~LogicWorld(void)
{
}

void LogicWorld::AddCollisionObject(btCollisionObject* obj)
{
    m_dynamicsWorld->addCollisionObject(obj);
}

void LogicWorld::RemoveCollisionObject(btCollisionObject* obj)
{
    m_dynamicsWorld->removeCollisionObject(obj);
}

void LogicWorld::CreateFlatFloor()
{
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,0,1), 0);
    m_collisionShapes.push_back(groundShape);
    float mass = 0.0f;
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(0,0,0));
    localCreateRigidBody(mass, startTransform, groundShape);
}

void LogicWorld::AddCylinderShape(const btVector3& pos, const btVector3& size)
{
    btCollisionShape* cylShape = new btCylinderShapeZ(size);
    m_collisionShapes.push_back(cylShape);
    float mass = 0.0f;
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(pos);
    localCreateRigidBody(mass, startTransform, cylShape);
}

void LogicWorld::FrameMove(float dt)
{
    if (m_dynamicsWorld)
    {
        UpdatePlayerLocation(dt);

        static const int MAX_SUBSTEPS = 2;
        int numSimSteps = m_dynamicsWorld->stepSimulation(dt, MAX_SUBSTEPS);
    }
}

void LogicWorld::UpdatePlayerLocation(float dt)
{
    ///set walkDirection for our character
    btTransform xform;
    xform = m_ghostObject->getWorldTransform ();


    btVector3 forwardDir = xform.getBasis()[1];
    //	printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
    btVector3 upDir = xform.getBasis()[1];
    btVector3 strafeDir = xform.getBasis()[0];
    forwardDir.normalize ();
    upDir.normalize ();
    strafeDir.normalize ();

    btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
    btScalar walkVelocity = 10.0f;
    btScalar walkSpeed = walkVelocity;// * dt;


    //rotate view
    if (ClCtx()->left)
    {
        walkDirection -= strafeDir;
    }

    if (ClCtx()->right)
    {
        walkDirection += strafeDir;
    }

    if (ClCtx()->up)
    {
        walkDirection += forwardDir;
    }

    if (ClCtx()->down)
    {
        walkDirection -= forwardDir;
    }

    if (m_character->canJump() && ClCtx()->jump)
    {
        m_character->jump();
    }

    // 아래 함수는 뭔가 버그가 있는 것 같다. 기대한 것보다 항상 빠른 속도로 움직이게 됨.
    //m_character->setWalkDirection(walkDirection*walkSpeed*dt);

    m_character->setVelocityForTimeInterval(walkDirection*walkSpeed, dt);
}

void LogicWorld::GetGhostPos(float* pos) const
{
    btScalar	m[16];
    btMatrix3x3	rot;rot.setIdentity();
    const int	numObjects=m_dynamicsWorld->getNumCollisionObjects();
    btVector3 wireColor(1,0,0);
    for(int i=0;i<numObjects;i++)
    {
        btCollisionObject*	colObj=m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody*		body=btRigidBody::upcast(colObj);
        if(body&&body->getMotionState())
        {
            btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
            myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
            rot=myMotionState->m_graphicsWorldTrans.getBasis();
        }
        else
        {
            colObj->getWorldTransform().getOpenGLMatrix(m);
            rot=colObj->getWorldTransform().getBasis();
        }
        btVector3 wireColor(1.f,1.0f,0.5f); //wants deactivation
        if(i&1) wireColor=btVector3(0.f,0.0f,1.f);
        ///color differently for active, sleeping, wantsdeactivation states
        if (colObj->getActivationState() == 1) //active
        {
            if (i & 1)
            {
                wireColor += btVector3 (1.f,0.f,0.f);
            }
            else
            {			
                wireColor += btVector3 (.5f,0.f,0.f);
            }
        }
        if(colObj->getActivationState()==2) //ISLAND_SLEEPING
        {
            if(i&1)
            {
                wireColor += btVector3 (0.f,1.f, 0.f);
            }
            else
            {
                wireColor += btVector3 (0.f,0.5f,0.f);
            }
        }

        btVector3 aabbMin,aabbMax;
        m_dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);

        aabbMin-=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
        aabbMax+=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
        //		printf("aabbMin=(%f,%f,%f)\n",aabbMin.getX(),aabbMin.getY(),aabbMin.getZ());
        //		printf("aabbMax=(%f,%f,%f)\n",aabbMax.getX(),aabbMax.getY(),aabbMax.getZ());
        //		m_dynamicsWorld->getDebugDrawer()->drawAabb(aabbMin,aabbMax,btVector3(1,1,1));

        if (i == 0)
        {
            pos[0] = m[12];
            pos[1] = m[13];
            pos[2] = m[14];
        }
    }
}

btRigidBody* LogicWorld::localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        shape->calculateLocalInertia(mass,localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

    btRigidBody* body = new btRigidBody(cInfo);
    body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
    btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);	
    body->setWorldTransform(startTransform);
#endif//

    m_dynamicsWorld->addRigidBody(body);

    return body;
}
