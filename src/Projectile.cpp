#include "Projectile.h"


Projectile::Projectile(Ogre::SceneManager* scene, const Ogre::Vector3& position, const Ogre::Vector3& target)
{
  m_scene = scene;
  m_position = position;
  m_position.y += 90.0;
  m_sceneNode = m_scene->getRootSceneNode()->createChildSceneNode(m_position);
  m_sceneNode->setScale(Ogre::Vector3(15));
  m_velocity = -15.0 * 40.0 * (position - target).normalisedCopy();


  // static Ogre::Entity* s_clone = m_scene->createEntity("Icosphere.mesh");
  // static std::string s_name = "s";
  // s_name += "s";
  //
  // //m_entity = m_scene->createEntity("Gem.mesh");
  // m_entity = s_clone->clone(s_name);
  m_entity = m_scene->createEntity("Icosphere.mesh");
  m_entity->setCastShadows(false);
  //m_entity->setReceivesShadows(false);
  m_sceneNode->attachObject(m_entity);

  m_entity->setMaterialName("Orb2");

  Ogre::SceneNode* lsn = m_sceneNode->createChildSceneNode(Ogre::Vector3(0, 1, 0));
  Ogre::Light* light = m_scene->createLight();
  light->setType(Ogre::Light::LT_POINT);
  light->setDiffuseColour(0.76862745098, 0.90588235294, 0.87450980392);
  light->setSpecularColour(0.76862745098, 0.90588235294, 0.87450980392);
  light->setCastShadows(false);
  light->setAttenuation(
    15 * 100, // range
    0.01,  // constant
    0.0,  // linear
    0.021);// quadratic
  lsn->attachObject(light);
  m_light = light;
}


Projectile::~Projectile()
{
  m_scene->destroySceneNode(m_sceneNode);
  m_scene->destroyEntity(m_entity);
  m_scene->destroyLight(m_light);
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
