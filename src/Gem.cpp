#include "Gem.h"


Gem::Gem(Ogre::SceneManager* scene, Ogre::SceneNode* root, const Ogre::Vector3& position, int type)
{
  m_scene = scene;
  m_sceneNode = root->createChildSceneNode(position);
  m_sceneNode->setScale(Ogre::Vector3(15));
  m_position = position;
  m_velocity = Ogre::Vector3(0, 8, 0);

  m_entity = m_scene->createEntity("Gem.mesh");
  m_sceneNode->attachObject(m_entity);

  Ogre::SceneNode* lsn = m_sceneNode->createChildSceneNode(Ogre::Vector3(0, 1, 0));
  Ogre::Light* light = m_scene->createLight();
  light->setType(Ogre::Light::LT_POINT);
  light->setCastShadows(false);
  light->setAttenuation(
    15 * 100, // range
    0.1,  // constant
    0.0,  // linear
    0.021);// quadratic
  lsn->attachObject(light);
  m_light = light;

  if (type == 1)
  {
    mClassType = "Gem1";

    m_entity->setMaterialName("Orb1");
    light->setDiffuseColour(1.0, 0.3568627451, 0.30588235294);
    light->setSpecularColour(1.0, 0.3568627451, 0.30588235294);
  }
  else
  {
    mClassType = "Gem2";

    m_entity->setMaterialName("Orb2");
    light->setDiffuseColour(0.8862745098, 0.92941176471, 0.92941176471);
    light->setSpecularColour(0.8862745098, 0.92941176471, 0.92941176471);
  }
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
    m_velocity.y += -15 * 980 * dt * dt;
  }

  // Apply
  m_sceneNode->setPosition(m_position);
}
