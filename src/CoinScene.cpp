#include "CoinScene.h"


CoinScene::CoinScene()
{

}


CoinScene::~CoinScene()
{

}


void CoinScene::init(Ogre::SceneManager* sm)
{
  m_bulletScene = new BulletScene();
  m_bulletScene->init(); // duh

  btTransform t;
  t.setIdentity();

  btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0.3);
  m_bulletScene->createRigidBody(
    0.f,      //float mass,
    t,        //const btTransform& startTransform,
    groundShape     //btCollisionShape* shape,
    //const btVector4& color = btVector4(1, 0, 0, 1)
  );

  m_sm = sm;

  // createCoin(10, 11, 0, 0);
  // createCoin(11, 10, 0, 1);
  // createCoin(10, 10, 1, 2);
  // createCoin(9, 10, 0, 3);
  // createCoin(0, 10, -1, 4);

}


void CoinScene::createCoin(float x, float y, float z, int i)
{
  btRigidBody* body;
  Ogre::SceneNode* node;

  btTransform t;
  t.setIdentity();
  t.setOrigin(btVector3(x, y, z));

  btCollisionShape* shape = new btCylinderShapeZ(12.0 * btVector3(0.7, 0.7, 0.1));
  std::cout << "createRigidBody" << std::endl;
  body = m_bulletScene->createRigidBody(
    1.f,      //float mass,
    t,        //const btTransform& startTransform,
    shape     //btCollisionShape* shape,
    //const btVector4& color = btVector4(1, 0, 0, 1)
  );

  node = m_sm->getRootSceneNode()->createChildSceneNode();

  Ogre::String coin_files[5] =
  {"Coin_1.mesh", "Coin_2.mesh", "Coin_3.mesh", "Coin_4.mesh", "Coin_5.mesh"};


  Ogre::Entity* coin = m_sm->createEntity(coin_files[i]);
  node->setScale(Ogre::Vector3(12));
  node->attachObject(coin);

  m_nodes.push_back(node);
  m_bodys.push_back(body);

}


void CoinScene::createCoin(Ogre::Vector3 p)
{
  btRigidBody* body;
  Ogre::SceneNode* node;

  btTransform t;
  t.setIdentity();
  t.setOrigin(btVector3(
    p.x + Ogre::Math::RangeRandom(-1, 1),
    p.y + Ogre::Math::RangeRandom(-1, 1),
    p.z + Ogre::Math::RangeRandom(-1, 1)));

  btCollisionShape* shape = new btCylinderShapeZ(10.0 * btVector3(0.7, 0.7, 0.1));
  std::cout << "createRigidBody" << std::endl;
  body = m_bulletScene->createRigidBody(
    1.f,      //float mass,
    t,        //const btTransform& startTransform,
    shape     //btCollisionShape* shape,
    //const btVector4& color = btVector4(1, 0, 0, 1)
  );

  body->applyCentralImpulse(btVector3(
    Ogre::Math::RangeRandom(-1, 1),
    Ogre::Math::RangeRandom(-1, 1),
    Ogre::Math::RangeRandom(-1, 1)
  ));
  body->applyTorqueImpulse(btVector3(
    Ogre::Math::RangeRandom(-1, 1),
    Ogre::Math::RangeRandom(-1, 1),
    Ogre::Math::RangeRandom(-1, 1)
  ));

  node = m_sm->getRootSceneNode()->createChildSceneNode();
  node->setScale(Ogre::Vector3(10));

  Ogre::String coin_files[5] =
  {"Coin_1.mesh", "Coin_2.mesh", "Coin_3.mesh", "Coin_4.mesh", "Coin_5.mesh"};
  int i = rand() % 5;

  Ogre::Entity* coin = m_sm->createEntity(coin_files[i]);

  node->attachObject(coin);

  m_nodes.push_back(node);
  m_bodys.push_back(body);
}


void CoinScene::update(float dt)
{
  m_bulletScene->update(dt);

  // Apply transforms
  for (int i = 0; i < m_bodys.size(); ++i)
  {
    btTransform x = m_bodys[i]->getCenterOfMassTransform();

    btVector3 v = x.getOrigin();
    btQuaternion q = x.getRotation();

    m_nodes[i]->setPosition(v.x(), v.y(), v.z());
    m_nodes[i]->setOrientation(q.w(), q.x(), q.y(), q.z());
  }
}
