#pragma once


//#include "ActiveItem.h"
#include "Chicken.h"
#include <vector>


class ChickenMob
{
public:

    ChickenMob(SceneManager* sm, std::vector<ActiveItem*>* items )
    {
      mSceneMgr = sm;
      chickens = items;
    }


    void update()
    {
      if (mChickens < 2)
      {
        ++mChickens;
        spawn();
      }
    }


    void spawn()
    {
      Chicken* chicken = new Chicken(mSceneMgr, Ogre::Vector3(140, 0, 104));
      chickens->push_back(chicken);
    }


    std::vector<ActiveItem*>* chickens;
    SceneManager* mSceneMgr;
    int mChickens = 0;

};
