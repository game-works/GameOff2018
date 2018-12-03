#pragma once


//#include "ActiveItem.h"
#include "Chicken.h"
#include <vector>


class ChickenMob
{
public:

    ChickenMob(SceneManager* sm, SceneNode* root, std::vector<ActiveItem*>* items)
    {
      mSceneMgr = sm;
      mRoot = root;
      chickens = items;
    }


    void update(Ogre::Vector3 characterPosition)
    {
      mChickens = 0;
      for (auto i : *chickens)
      {
          if (i->classType() == "Hen")
          {
            ++mChickens;
          }
      }

      if (mChickens < 2)
      {
        ++mChickens;
        spawn(characterPosition);
      }
    }


    void spawn(Ogre::Vector3 characterPosition)
    {
      Real z = Ogre::Math::RangeRandom(-350, 500);
      Real x = Ogre::Math::RangeRandom(-650, 650);
      if (characterPosition.x < -600 ||
          characterPosition.x >  700 )
      {
        Chicken* chicken = new Chicken(mSceneMgr, mRoot, Ogre::Vector3(140 + x, 0, z));
        chickens->push_back(chicken);
      }
      else if (characterPosition.x > 0)
      {
        Chicken* chicken = new Chicken(mSceneMgr, mRoot, Ogre::Vector3(-500 + x, 0, z));
        chickens->push_back(chicken);
      }
      else
      {
        Chicken* chicken = new Chicken(mSceneMgr, mRoot, Ogre::Vector3(650 + x, 0, z));
        chickens->push_back(chicken);
      }
    }


    std::vector<ActiveItem*>* chickens;
    SceneManager* mSceneMgr;
    SceneNode* mRoot;
    int mChickens = 0;

};
