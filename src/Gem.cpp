#include "Gem.h"


Gem::Gem(Ogre::SceneManager* scene, const Ogre::Vector3& position)
{
  m_scene = scene;
  m_sceneNode = m_scene->getRootSceneNode()->createChildSceneNode(position);
  m_position = position;
  m_velocity = Ogre::Vector3(0, 8, 0);

  m_entity = m_scene->createEntity("Gem.mesh");
  m_sceneNode->attachObject(m_entity);

  Ogre::SceneNode* lsn = m_sceneNode->createChildSceneNode(Ogre::Vector3(0, 1, 0));
  Ogre::Light* light = m_scene->createLight();
  light->setType(Ogre::Light::LT_POINT);
  light->setDiffuseColour(0, 0, 1);
  light->setSpecularColour(0, 0, 1);
  light->setCastShadows(false);
  light->setAttenuation(
    100, // range
    1,  // constant
    0.0,  // linear
    0.021);// quadratic
  lsn->attachObject(light);
  m_light = light;
}


Gem::~Gem()
{
  m_scene->destroySceneNode(m_sceneNode);
  m_scene->destroyEntity(m_entity);
  m_scene->destroyLight(m_light);
}


void Gem::update(float dt)
{
  if (m_stationary) return;

  // movement
  m_position += m_velocity * dt;

  // resistance
  //m_velocity *= 0.999;

  // bounce
  if (m_position.y < 0 && m_velocity.y < 0)
  {
    float damp = 0.69;
    m_position.y = 0;// -damp * m_position.y;

    // Elastic force
    m_velocity.y = -damp * m_velocity.y;

    // Damping
    if (m_velocity.squaredLength() < 4)
    {
      // Stop
      m_velocity = Ogre::Vector3::ZERO;
      m_position.y = 0;
      m_stationary = true;
    }
  }
  else
  {
    // gravity
    m_velocity.y += -980 * dt * dt;
  }

  // Apply
  m_sceneNode->setPosition(m_position);
}
