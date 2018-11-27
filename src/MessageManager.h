#pragma once


#include "Ogre.h"


using namespace Ogre;


class MessageManager
{
public:
  MessageManager();
  ~MessageManager();

  void update(float dt);

  void setMessage(String message);

  void clearMessage();

protected:

  Ogre::OverlayElement* m_textArea = 0;


};


MessageManager::MessageManager()
{
  OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
  Overlay* overlay = overlayManager.create( "MessageOverlay" );
  m_textArea = overlayManager.createOverlayElement("TextArea", "MessageManager");
  m_textArea->setMetricsMode(GMM_PIXELS);
  m_textArea->setDimensions( 128, 64 );
  m_textArea->setPosition( 0, 105 );
  //panel0->setMaterialName( "Wolf/BarBackground" );
  //panel0->setParameter("font_name", "SdkTrays/Value");
  //panel0->setParameter("char_height", StringConverter::toString(fontSize));
  //panel0->setCaption("Hey!");
  Real fontSize = 40.0;
  m_textArea->setParameter("font_name", "GameFont/Message");
  m_textArea->setParameter("alignment", "center");
  m_textArea->setParameter("char_height", StringConverter::toString(fontSize));
  m_textArea->setHorizontalAlignment(GHA_CENTER);
  overlay->add2D( static_cast<Ogre::OverlayContainer*>(m_textArea) );
  overlay->show();
}


MessageManager::~MessageManager()
{

}


void MessageManager::setMessage(String s)
{
  m_textArea->setCaption(s);
}


void MessageManager::clearMessage()
{
  m_textArea->setCaption("");
}


void MessageManager::update(Real dt)
{

}
