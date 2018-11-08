#include "Projectile.h"


Projectile::Projectile(Ogre::SceneManager* scene, const Ogre::Vector3& position, const Ogre::Vector3& target)
{
  m_scene = scene;
  m_sceneNode = m_scene->getRootSceneNode()->createChildSceneNode(position);
  m_position = position;
  m_velocity = -40.0 * (position - target);//.normalised();


  static Ogre::Entity* s_clone = m_scene->createEntity("Gem.mesh");
  static std::string s_name = "s";
  s_name += "s";

  //m_entity = m_scene->createEntity("Gem.mesh");
  m_entity = s_clone->clone(s_name);
  m_sceneNode->attachObject(m_entity);

  Ogre::SceneNode* lsn = m_sceneNode->createChildSceneNode(Ogre::Vector3(0, 1, 0));
  Ogre::Light* light = m_scene->createLight();
  light->setType(Ogre::Light::LT_POINT);
  light->setDiffuseColour(0.25, 0, 0);
  light->setSpecularColour(0.5, 0, 0);
  light->setCastShadows(false);
  lsn->attachObject(light);
}


Projectile::~Projectile()
{
  m_scene->destroySceneNode(m_sceneNode);
  m_scene->destroyEntity(m_entity);
  //m_scene->destroyLight(m_entity);
}


void Projectile::update(float dt)
{
  if (m_stationary) return;

  // movement
  m_position += m_velocity * dt;

  // Apply
  m_sceneNode->setPosition(m_position);

  // End of range?
  m_lifetime -= dt;
  if (m_lifetime < 0)
  {
    m_stationary = true;
  }
}
