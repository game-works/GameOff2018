#pragma once


#include "MessageManager.h"


class Continuity
{
public:


  static int stage;
  static MessageManager* s_messageManager;
  static bool changeLevel;
  static Ogre::Vector3 characterPosition;


  static void died()
  {
    s_messageManager->setMessage("You Died");
    stage = 11;
  }


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

    if (stage == 3 &&
      (key == 'w' || key == 'a' || key == 's' || key == 'd')
    )
    {
      stage = 4;
      s_messageManager->setMessage("Left Click to attack");
    }

    if (stage == 5 &&
      (key == 'w' || key == 'a' || key == 's' || key == 'd')
    )
    {
      stage = 6;
      s_messageManager->setMessage("Collect energy then shoot with Right Click");
    }

    if (stage == 7 &&
      (key == 'w' || key == 'a' || key == 's' || key == 'd')
    )
    {
      stage = 8;
      s_messageManager->setMessage("Fill up your Special Bar to transform");
    }

    if (stage == 11 &&
      (key == 'w' || key == 'a' || key == 's' || key == 'd')
    )
    {
      stage = 12;
      s_messageManager->clearMessage();
    }
  }


  static void onAttack()
  {
    if (stage == 4)
    {
      stage = 5;
      s_messageManager->clearMessage();
    }

      if (stage == 8)
      {
        stage = 9;
        s_messageManager->clearMessage();
      }
  }


    static void onShoot()
    {
      if (stage == 6)
      {
        stage = 7;
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
bool Continuity::changeLevel = false;
MessageManager* Continuity::s_messageManager = 0;
Ogre::Vector3 Continuity::characterPosition = Ogre::Vector3::ZERO;
