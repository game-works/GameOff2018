#pragma once


#include "Ogre.h"
#include "SdkSample.h"      // ??


using namespace Ogre;


class ActiveNPC1;
class AnimeCharacterController;


class Dialog
{
public:
  class Line
  {
  public:
    Line(int a_id, String a_text) { id = a_id; text = a_text; }
    int id;
    String text;
  };
  std::vector<Line> lines;
};


class ConvoManager
{
public:

  bool ended = false;

  ConvoManager();
  ~ConvoManager();

  void setConvo(Dialog convo, ActiveNPC1* npc, AnimeCharacterController* pc);

  void update(Real dt, Camera* cam);

protected:

  Real m_timer;
  int m_ticker = 0;
  Dialog m_dialog;
  ActiveNPC1* mNPC = 0;
  AnimeCharacterController* mPC = 0;

    // Dialog panel
    Ogre::OverlayElement* panel0 = 0;
};
