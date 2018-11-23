#pragma once


#include "BulletScene.h"
#include "btOgreSoftBody.hpp"


class AnimeCharacterController;


class Tail
{
public:
  Tail();
  ~Tail();

  void init(Ogre::SceneManager* sm, AnimeCharacterController* character);

  void update(float dt, AnimeCharacterController* character);

protected:

  BulletScene* m_bulletScene = 0;
  btSoftBody* m_softbodyTail = 0;
  btRigidBody* m_anchorBody = 0;

  BtOgre::BtOgreSoftBody* m_btOgreBody = 0;

  Ogre::SceneManager* m_sm = 0;
  Ogre::SceneNode* m_tailNode = 0;
  Ogre::Entity* m_tailEntity;

};
