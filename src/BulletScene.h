#pragma once


#include "root.h"
#include <stdio.h>
#include <iostream>

// SOFTBODY ONLY
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"


class BulletScene
{
public:

  BulletScene();
  ~BulletScene();

  void init();

  void update(float dts);

  bool gravity = true;

  // SoftBody ONLY
  btSoftBodyWorldInfo softBodyWorldInfo;


protected:


  virtual void createEmptyDynamicsWorld()
	{
		///collision configuration contains default setup for memory, collision setup
    std::cout << "btDefaultCollisionConfiguration" << std::endl;
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		//m_collisionConfiguration->setConvexConvexMultipointIterations();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    std::cout << "btCollisionDispatcher" << std::endl;
		m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    std::cout << "btDbvtBroadphase" << std::endl;
		m_broadphase = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    std::cout << "btSequentialImpulseConstraintSolver" << std::endl;
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		m_solver = sol;

    std::cout << "btDiscreteDynamicsWorld" << std::endl;
		//m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

    // SOFTBODY ONLY
    m_dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);


    std::cout << "setGravity" << std::endl;
		if (gravity) m_dynamicsWorld->setGravity(btVector3(0, -9.8*120, 0));

    // SB ONLY
    softBodyWorldInfo.m_broadphase = m_broadphase;
    softBodyWorldInfo.m_dispatcher = m_dispatcher;
    softBodyWorldInfo.m_gravity = btVector3(0, -1, 0);//m_dynamicsWorld->getGravity();
    softBodyWorldInfo.m_sparsesdf.Initialize();
  }


  virtual void exitPhysics()
	{
		//cleanup in the reverse order of creation/initialization

		//remove the rigidbodies from the dynamics world and delete them

		if (m_dynamicsWorld)
		{
			int i;
			for (i = m_dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
			{
				m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(i));
			}
			for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}
				m_dynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}
		}
		//delete collision shapes
		for (int j = 0; j < m_collisionShapes.size(); j++)
		{
			btCollisionShape* shape = m_collisionShapes[j];
			delete shape;
		}
		m_collisionShapes.clear();

		delete m_dynamicsWorld;
		m_dynamicsWorld = 0;

		delete m_solver;
		m_solver = 0;

		delete m_broadphase;
		m_broadphase = 0;

		delete m_dispatcher;
		m_dispatcher = 0;

		delete m_collisionConfiguration;
		m_collisionConfiguration = 0;
}


  void deleteRigidBody(btRigidBody* body)
	{
		//int graphicsUid = body->getUserIndex();

		m_dynamicsWorld->removeRigidBody(body);
		btMotionState* ms = body->getMotionState();
		delete body;
		delete ms;
	}

public:
	btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
	{
		btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

		btRigidBody* body = new btRigidBody(cInfo);
		//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);


		body->setUserIndex(-1);
		m_dynamicsWorld->addRigidBody(body);
		return body;
  }

protected:

  //keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	btBroadphaseInterface* m_broadphase;
	btCollisionDispatcher* m_dispatcher;
	btConstraintSolver* m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
public:
  btDiscreteDynamicsWorld* m_dynamicsWorld;

};
