#pragma once


#include "Ogre.h"
#include "EnemySkeleton.h"
#include "EnemySlime.h"


class ValhallaMob
{
public:

  ValhallaMob(SceneManager* sm, SceneNode* root, std::vector<ActiveItem*>* items)
  {
    mSceneManager = sm;
    mRoot = root;
    mItems = items;
  }


  ~ValhallaMob()
  {

  }


  void update(Vector3 p)
  {
    // Count
    int sk = 0;
    int sl = 0;

    for (auto i : *mItems)
    {
      if (i->classType() == "Skeleton")
      {
        ++sk;
      }
      if (i->classType() == "Slime")
      {
        ++sl;
      }
    }

    // Calculate
    int nSk = (mSpawned + 15) / 20;
    int nSl = (mSpawned + 5) / 11 + 3;
    if (nSk > 4) nSk = 4;
    if (nSl > 6) nSl = 6;

    // Spawn
    while (sk < nSk)
    {
        Real t = Ogre::Math::RangeRandom(0.0, 360.0);
        Real r = Ogre::Math::RangeRandom(450.0, 650.0);
        Real x = r * Ogre::Math::Cos(Ogre::Degree(t));// + mChara->getPosition().x;
        Real y = r * Ogre::Math::Sin(Ogre::Degree(t));// + mChara->getPosition().z;

        (*mItems).push_back(
          new EnemySkeleton(
            mSceneManager,
            mRoot,
            Ogre::Vector3(x, 12 * 1.5, y))
        );

        ++sk;
        ++mSpawned;
      }

      while (sl < nSl)
      {
          Real t = Ogre::Math::RangeRandom(0.0, 360.0);
          Real r = Ogre::Math::RangeRandom(450.0, 650.0);
          Real x = r * Ogre::Math::Cos(Ogre::Degree(t));// + mChara->getPosition().x;
          Real y = r * Ogre::Math::Sin(Ogre::Degree(t));// + mChara->getPosition().z;

          (*mItems).push_back(
            new EnemySlime(
              mSceneManager,
              mRoot,
              Ogre::Vector3(x, 12 * 1.0, y))
          );

          ++sl;
          ++mSpawned;
        }

  }

protected:

  Ogre::SceneManager* mSceneManager;
  SceneNode* mRoot;
  std::vector<ActiveItem*>* mItems;

public:
  int mSpawned = 0;
};
