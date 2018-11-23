#include "BulletScene.h"



BulletScene::BulletScene()
{

}


BulletScene::~BulletScene()
{

}

void BulletScene::init()
{
  createEmptyDynamicsWorld();
}


void BulletScene::update(float deltaTime)
{
  if (m_dynamicsWorld)
  {
    m_dynamicsWorld->stepSimulation(deltaTime);
  }
}
