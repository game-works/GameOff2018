#pragma once


#include "MessageManager.h"


class Continuity
{
public:


  static int stage;
  static MessageManager* s_messageManager;


  static void onKeyPress(char key)
  {
    if (stage == 0 &&
          (key == 'w' || key == 'a' || key == 's' || key == 'd')
        )
    {
      stage = 1;
      s_messageManager->clearMessage();
    }

    if (stage == 2 &&
          (key == ' ')
        )
    {
      stage = 3;
      s_messageManager->clearMessage();
    }
  }


  static void onConversationEnd()
  {
    if (stage == 1)
    {
      s_messageManager->setMessage("Press Space to Draw Weapons");

      stage = 2;
    }
  }




};


int Continuity::stage = 0;
MessageManager* Continuity::s_messageManager = 0;
