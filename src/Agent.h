#pragma once


#include "ActiveItem.h"


class Agent : public ActiveItem
{
public:

  virtual ~Agent() {}

  virtual void hit() {}

protected:

};
