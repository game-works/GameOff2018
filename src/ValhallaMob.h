#pragma once


#include "Ogre.h"
#include "EnemySkeleton.h"
#include "EnemySlime.h"


class ValhallaMob
{
public:

  ValhallaMob()
  {
  }


  void init(Ogre::SceneManager* sm, std::vector<Enemy*>* en)
  {
    mSceneManager = sm;
    m_enemies = en;
  }


  ~ValhallaMob()
  {

  }


  void update()
  {
    while (m_enemies->size() < 2)
    {
        Real t = Ogre::Math::RangeRandom(0.0, 360.0);
        Real r = Ogre::Math::RangeRandom(450.0, 650.0);
        Real x = r * Ogre::Math::Cos(Ogre::Degree(t));// + mChara->getPosition().x;
        Real y = r * Ogre::Math::Sin(Ogre::Degree(t));// + mChara->getPosition().z;

        (*m_enemies).push_back(
          new EnemySkeleton(
            mSceneManager,
            Ogre::Vector3(x, 12 * 5.5, y))
        );
      }

      while (m_enemies->size() < 4)
      {
          Real t = Ogre::Math::RangeRandom(0.0, 360.0);
          Real r = Ogre::Math::RangeRandom(450.0, 650.0);
          Real x = r * Ogre::Math::Cos(Ogre::Degree(t));// + mChara->getPosition().x;
          Real y = r * Ogre::Math::Sin(Ogre::Degree(t));// + mChara->getPosition().z;

          (*m_enemies).push_back(
            new EnemySlime(
              mSceneManager,
              Ogre::Vector3(x, 12 * 1.5, y))
          );
        }


  }

protected:

  std::vector<Enemy*>* m_enemies;
  Ogre::SceneManager* mSceneManager;
};
