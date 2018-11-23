#include "Tail.h"
#include "SinbadCharacterController6.h"


    btSoftBody* generateSoftBody(Entity* entity, BtOgre::BtOgreSoftBody* ogreSoftBody, BulletScene* bulletScene)
    {
      size_t vertex_count;
      size_t index_count;

      MeshPtr mesh = entity->getMesh();
      Ogre::SubMesh* submesh = mesh->getSubMesh(0);
      Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

      index_count = submesh->indexData->indexCount;

      // if (!submesh->useSharedVertices)
      // {
      //   vertex_count = submesh->vertexData->vertexCount;
      // }
      vertex_count = vertex_data->vertexCount;

      Ogre::Vector3* vertices = new Ogre::Vector3[vertex_count];
      unsigned int* indices = new unsigned int[index_count];

      //
      Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;
      Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE;
      Ogre::Vector3 position = Ogre::Vector3::ZERO;

      // per submesh
      bool added_shared = false;
      size_t current_offset = 0;
    	size_t shared_offset = 0;
    	size_t next_offset = 0;
    	size_t index_offset = 0;

      		//----------------------------------------------------------------
      		// GET VERTEXDATA
      		//----------------------------------------------------------------

      		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
      		{
      			if(submesh->useSharedVertices)
      			{
      				added_shared = true;
      				shared_offset = current_offset;
      			}

      			const Ogre::VertexElement* posElem =
      				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

      			Ogre::HardwareVertexBufferSharedPtr vbuf =
      				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

      			unsigned char* vertex =
      				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

      			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
      			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
      			//  be comiled/typedefed as double:
      			//      Ogre::Real* pReal;
      			float* pReal;

      			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
      			{
      				posElem->baseVertexPointerToElement(vertex, &pReal);

      				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

      				vertices[current_offset + j] = (orient * (pt * scale)) + position;
      			}

      			vbuf->unlock();
      			next_offset += vertex_data->vertexCount;
      		}


      		Ogre::IndexData* index_data = submesh->indexData;
      		size_t numTris = index_data->indexCount / 3;
      		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

      		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

      		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
      		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


      		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
      		size_t index_start = index_data->indexStart;
      		size_t last_index = numTris*3 + index_start;

      		if (use32bitindexes)
      			for (size_t k = index_start; k < last_index; ++k)
      			{
      				indices[index_offset++] = pLong[k] + static_cast<unsigned long>( offset );
      			}

      		else
      			for (size_t k = index_start; k < last_index; ++k)
      			{
      				indices[ index_offset++ ] = static_cast<unsigned long>( pShort[k] ) +
      					static_cast<unsigned long>( offset );
      			}

      			ibuf->unlock();
      			current_offset = next_offset;
      	// end submesh

      btSoftBody* psb = ogreSoftBody->create(
                         entity,
  										   vertex_count,
  										   vertices,
  										   index_count,
  										   indices);

     // set linear stiffness
     psb->m_materials[0]->m_kLST = 0.50;
     psb->m_materials[0]->m_kAST = 0.50;
     psb->m_materials[0]->m_kVST = 0.90;
     //psb->setTotalMass(10.0001);

     psb->generateBendingConstraints(2);

     // FIX
     //#define IDX(_x_, _y_) ((_y_)*rx + (_x_))
     //psb->setMass(IDX(0, 0), 0);
     // for (int i = 0; i < 8; ++i)
     //   psb->setMass(i, 0);

    //  softBody.generateBendingConstraints(2, pm);
    // pm.setKLST(0.2f);
    // pm.setFlags(0);
    // softBody.generateBendingConstraints(2, pm);
    // softBody.setConfig_piterations(7);
    // softBody.setConfig_kDF(0.2f);
    // softBody.randomizeConstraints();
    // softBody.setTotalMass(1);

     btSoftRigidDynamicsWorld* world = (btSoftRigidDynamicsWorld*)bulletScene->m_dynamicsWorld;
     world->addSoftBody(psb);

     return psb;
    }



Tail::Tail()
{

}


Tail::~Tail()
{

}


void Tail::init(Ogre::SceneManager* sm, AnimeCharacterController* character)
{
  m_bulletScene = new BulletScene();
  m_bulletScene->gravity = false; // TODO
  m_bulletScene->init();

  m_sm = sm;

  btTransform t;
  t.setIdentity();

  m_tailNode = m_sm->getRootSceneNode()->createChildSceneNode();

    // Tail Entity
    m_tailEntity = m_sm->createEntity("Tail.mesh");
    m_tailEntity->setMaterialName("Wolf/DirtBlend");

    m_tailNode->attachObject(m_tailEntity);

    m_btOgreBody = new BtOgre::BtOgreSoftBody(&m_bulletScene->softBodyWorldInfo);
    m_softbodyTail = generateSoftBody(m_tailEntity, m_btOgreBody, m_bulletScene);

    // Anchor point
    btTransform startTransform;
    startTransform.setIdentity();
    Ogre::Vector3 p = character->getTailPosition();
    Ogre::Quaternion o = character->getTailOrientation();
    startTransform.setOrigin(btVector3(p.x, p.y, p.z));
    startTransform.setRotation(btQuaternion(0, 1, 0, 0) * btQuaternion(-o.w, o.x, o.y, o.z));
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo characterBodyInfo(
      0.0,
      myMotionState,
      new btEmptyShape()
    );
    m_anchorBody = new btRigidBody(characterBodyInfo);
    m_bulletScene->m_dynamicsWorld->addRigidBody(m_anchorBody);

    // Attach and anchor softbody (tail)
    for (int i = 0; i < 8; ++i)
      m_softbodyTail->appendAnchor(i, m_anchorBody, true);

    for (int i = 8; i < 16; ++i)
      m_softbodyTail->appendAnchor(i, m_anchorBody, true, 0.5);

    m_softbodyTail->appendAnchor(56, m_anchorBody, true, 0.7);

}


void Tail::update(float dt, AnimeCharacterController* character)
{
  // SoftBody position from character
  btTransform xform;
  xform.setIdentity();
  Ogre::Vector3 p = character->getTailPosition();
  Ogre::Quaternion o = character->getTailOrientation();
  xform.setOrigin(btVector3(p.x, p.y, p.z));
  xform.setRotation(btQuaternion(0, 1, 0, 0) * btQuaternion(-o.w, o.x, o.y, o.z)); // Quaternion Hack

  m_anchorBody->setCenterOfMassTransform(xform);

  // Update
  m_bulletScene->update(dt);

  m_btOgreBody->updateOgreMesh();
  m_tailEntity->_initialise(true);
}
