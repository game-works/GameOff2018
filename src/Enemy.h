#ifndef __Enemy_H__
#define __Enemy_H__


#include <Ogre.h>
#include <stdio.h>
#include <iostream>
#include "Agent.h"


using namespace Ogre;
using namespace OgreBites;


class Enemy : public Agent
{

public:

    virtual ~Enemy() {}

    virtual void setup()
    {
    }


    virtual void update(Real deltaTime, Vector3 characterPos)
    {
    }

    virtual const Ogre::Vector3 & getPosition() { return Ogre::Vector3::ZERO; }


protected:

};

#endif
