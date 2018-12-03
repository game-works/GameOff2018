#pragma once


#include "BulletScene.h"
#include <Ogre.h>
#include <vector>


class CoinScene
{
public:
  CoinScene();
  ~CoinScene();

  void init(Ogre::SceneManager* sm, Ogre::SceneNode* root);

  void update(float dt);

  void createCoin(float x, float y, float z, int i);
  void createCoin(Ogre::Vector3 p);


protected:

  BulletScene* m_bulletScene = 0;

  std::vector<btRigidBody*> m_bodys;
  std::vector<Ogre::SceneNode*> m_nodes;

  Ogre::SceneManager* m_sm = 0;
  Ogre::SceneNode* m_root;
};
