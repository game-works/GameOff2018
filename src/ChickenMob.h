#pragma once


//#include "ActiveItem.h"
#include "Chicken.h"
#include <vector>


class ChickenMob
{
public:

    ChickenMob(SceneManager* sm)
    {
      mSceneMgr = sm;
    }


    ActiveItem* spawn()
    {
      return new Chicken(mSceneMgr, Ogre::Vector3(140, 0, 104));
    }


    std::vector<Chicken*> chickens;
    SceneManager* mSceneMgr;

};
