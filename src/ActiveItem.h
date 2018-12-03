#pragma once


#include "Ogre.h"


using namespace Ogre;


class ActiveItem
{
public:

  virtual ~ActiveItem() {}
  virtual void addTime(Real deltaTime) {}
  virtual const Ogre::Vector3& getPosition() { return Ogre::Vector3::ZERO; }
  virtual void setPosition(const Ogre::Vector3& p) {}
  virtual String classType() { return "None"; }
  virtual void hit() {}
  virtual void hitRange() {}
  bool mRemove = false;
  bool mDamaging = false;

};
