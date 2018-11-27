#pragma once


#include "Ogre.h"


using namespace Ogre;


class ActiveItem
{
public:

  virtual void addTime(Real deltaTime) {}
  virtual const Ogre::Vector3& getPosition() { return Ogre::Vector3::ZERO; }

};
