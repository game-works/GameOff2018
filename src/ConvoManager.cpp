#include "ConvoManager.h"
#include "ActiveNPC.h"
#include "SinbadCharacterController6.h"


ConvoManager::ConvoManager()
{
  Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
  Overlay* overlay = overlayManager.create( "OverlayName_Convo" );
  panel0 = overlayManager.createOverlayElement("TextArea", "ConvoPanel");
  panel0->setMetricsMode(GMM_PIXELS);
  panel0->setPosition( 0, 24 );
  panel0->setDimensions( 128, 64 );
  //panel0->setMaterialName( "Wolf/BarBackground" );
  //panel0->setParameter("font_name", "SdkTrays/Value");
  //panel0->setParameter("char_height", StringConverter::toString(fontSize));
  //panel0->setCaption("Hey!");
  Real fontSize = 24.0;
  panel0->setParameter("font_name", "GameFont/Dialog");
  panel0->setParameter("alignment", "center");
  panel0->setParameter("char_height", StringConverter::toString(fontSize));
  //panel0->setHorizontalAlignment(GHA_CENTER);
  overlay->add2D( static_cast<Ogre::OverlayContainer*>(panel0) );
  overlay->show();
}


ConvoManager::~ConvoManager()
{

}


void ConvoManager::setConvo(Dialog convo, ActiveNPC1* npc, AnimeCharacterController* pc)
{
  if (mNPC == npc) return;

  mNPC = npc;
  mPC = pc;
  m_dialog = convo;
  m_timer = 2.500;
  m_ticker = 0;
}


void ConvoManager::update(Real dt, Camera* cam)
{

  if (m_ticker < m_dialog.lines.size())
  {
    panel0->setCaption(m_dialog.lines[m_ticker].text);

    // NPC Overlay
    Vector3 hcsPosition;
    int dx = 0;
    int dy = -50;
    if (m_dialog.lines[m_ticker].id == 0)
    {
      hcsPosition = cam->getProjectionMatrix() * cam->getViewMatrix() * mNPC->getPosition();
    }
    else
    {
      hcsPosition = cam->getProjectionMatrix() * cam->getViewMatrix() * mPC->getPosition();
      dy = -150;
    }
    panel0->setPosition(
      0.5 * (hcsPosition.x + 1) * cam->getViewport()->getActualWidth() + dx,
      0.5 * (1.0 - hcsPosition.y) * cam->getViewport()->getActualHeight() + dy
    );

    m_timer -= dt;
    if (m_timer < 0)
    {
      m_ticker += 1;
      m_timer = 2.000;

      if (m_ticker >= m_dialog.lines.size())
        ended = true;
    }
  }
  else
  {
    panel0->setCaption("");
  }
}
