#pragma once

#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

class btCharacterControllerInterface;
class btDynamicCharacterController;
class btKinematicCharacterController;
class btCollisionShape;

class LogicWorld
{
public:
    LogicWorld(void);
    ~LogicWorld(void);

    void FrameMove(float dt);
    void GetGhostPos(float* pos) const;
    void AddCylinderShape(const btVector3& pos, const btVector3& size);
    void AddCollisionObject(btCollisionObject* obj);
    void RemoveCollisionObject(btCollisionObject* obj);

private:
    void CreateFlatFloor();
    void UpdatePlayerLocation(float dt);

    btRigidBody* localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape);
    
    ///this is the most important class
	btDynamicsWorld* m_dynamicsWorld;

    btKinematicCharacterController* m_character;
    class btPairCachingGhostObject* m_ghostObject;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
    class btBroadphaseInterface* m_overlappingPairCache;
    class btCollisionDispatcher* m_dispatcher;
    class btConstraintSolver* m_constraintSolver;
    class btDefaultCollisionConfiguration* m_collisionConfiguration;
    btScalar m_defaultContactProcessingThreshold;
};
