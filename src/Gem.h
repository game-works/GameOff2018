#pragma once


#include <Ogre.h>


class Gem
{
public:

  Gem(Ogre::SceneManager* m_scene, const Ogre::Vector3& position);
  ~Gem();

  void update(float dt);

  const Ogre::Vector3 & getPosition() { return m_position; }

protected:

  int m_type;

  Ogre::SceneManager* m_scene;

  Ogre::Vector3 m_position;
  Ogre::Vector3 m_velocity;
  Ogre::Vector3 m_colour;

  Ogre::SceneNode* m_sceneNode;
  Ogre::Entity* m_entity;
  Ogre::Light* m_light;

  bool m_stationary = false;
};