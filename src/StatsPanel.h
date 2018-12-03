#pragma once


#include "Ogre.h"
#include "StatsManager.h"


using namespace Ogre;
using namespace OgreBites;


class StatsPanel
{
public:
  StatsPanel()
  {

  }


  ~StatsPanel()
  {

  }


  void init()
  {
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

    // Create an overlay
    overlay = overlayManager.create( "StatsOverlay" );

    Ogre::OverlayElement* panel0 = overlayManager.createOverlayElement("Panel", "Decor/0");
    Ogre::OverlayElement* panel1 = overlayManager.createOverlayElement("Panel", "Decor/Heart");
    Ogre::OverlayElement* panel2 = overlayManager.createOverlayElement("Panel", "Decor/Moon");
    Ogre::OverlayElement* panelBar1a = overlayManager.createOverlayElement("Panel", "Decor/Bar1A");
    Ogre::OverlayElement* panelBar2a = overlayManager.createOverlayElement("Panel", "Decor/Bar2A");
    mElementBarHP = overlayManager.createOverlayElement("Panel", "Decor/Bar1B");
    mElementBarMoon = overlayManager.createOverlayElement("Panel", "Decor/Bar2B");

    Ogre::OverlayElement* panelBarL = overlayManager.createOverlayElement("Panel", "Decor/BarL");
    Ogre::OverlayElement* panelBarR = overlayManager.createOverlayElement("Panel", "Decor/BarR");

    mElementHeart = panel1;

    Ogre::Real offsetTop = 8.0;

    panel0->setMetricsMode(GMM_PIXELS);
    panel0->setPosition( 400 - 64, offsetTop );
    panel0->setDimensions( 128, 64 );
    panel0->setMaterialName( "Wolf/BarBackground" );

    panel1->setMetricsMode(GMM_PIXELS);
    panel1->setPosition( 400 - 64, offsetTop );
    panel1->setDimensions( 64, 64 );
    panel1->setMaterialName( "Wolf/Heart" );
    //panel1->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureUScroll(0.25 + 1.0 / 6.0);
    mAnimatedTSU = panel1->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    mAnimatedTSU->setTextureUScroll(0.25);

    panel2->setMetricsMode(GMM_PIXELS);
    panel2->setPosition( 400, offsetTop );
    panel2->setDimensions( 64, 64 );
    panel2->setMaterialName( "Wolf/Moon" );

    panelBar1a->setMetricsMode(GMM_PIXELS);
    panelBar1a->setPosition( 400 - 264 - 64, offsetTop + 16 );
    panelBar1a->setDimensions( 264, 32 );
    panelBar1a->setMaterialName( "Wolf/BarBackground" );

    panelBar2a->setMetricsMode(GMM_PIXELS);
    panelBar2a->setPosition( 400 + 64, offsetTop + 16 );
    panelBar2a->setDimensions( 264, 32 );
    panelBar2a->setMaterialName( "Wolf/BarBackground" );

    mElementBarHP->setMetricsMode(GMM_PIXELS);
    mElementBarHP->setPosition( 400 - 264 - 64, offsetTop + 16 + 5 );
    mElementBarHP->setDimensions( 264, 22 );
    mElementBarHP->setMaterialName( "Wolf/BarHealth" );

    mElementBarMoon->setMetricsMode(GMM_PIXELS);
    mElementBarMoon->setPosition( 400 + 64, offsetTop + 16 + 5 );
    mElementBarMoon->setDimensions( 264 / 2, 22 );
    mElementBarMoon->setMaterialName( "Wolf/BarMoon" );

    panelBarL->setMetricsMode(GMM_PIXELS);
    panelBarL->setPosition( 400 - 264 - 64 - 5, offsetTop + 16 );
    panelBarL->setDimensions( 5, 32 );
    panelBarL->setMaterialName( "Wolf/BarBackgroundL" );

    panelBarR->setMetricsMode(GMM_PIXELS);
    panelBarR->setPosition( 400 + 64 + 264, offsetTop + 16 );
    panelBarR->setDimensions( 5, 32 );
    panelBarR->setMaterialName( "Wolf/BarBackgroundR" );

    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panel0) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panel1) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panel2) );

    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBar1a) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBar2a) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(mElementBarHP) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(mElementBarMoon) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBarL) );
    overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBarR) );

    // Show the overlay
    overlay->show();
  }


  void addTime(Real dt)
  {
    // Animated Heart
    // mTSUAnimTimer += 0.3;
    // int n = (int)floor(mTSUAnimTimer * 3.0) % 6;
    static int m = 0;
    static int n = 0;
    m = (m + 1) % 20;
    if (m == 0)
    {
      n = (n + 1) % 6;
      ((PanelOverlayElement*)mElementHeart)->setUV(
        ((float)n),
        0,
        ((float)(n+1)),
        1);
      mElementHeart->_update();
    }

    // something
    mElementBarHP->setPosition(
      400 - 264 * (StatsManager::hp / StatsManager::maxhp) - 64,
      8 + 16 + 5 );

    mElementBarHP->setDimensions(
      264 * (StatsManager::hp / StatsManager::maxhp),
      22 );

    mElementBarMoon->setDimensions(
      264 * (StatsManager::moon / StatsManager::maxmoon),
      22 );
  }


  TextureUnitState* mAnimatedTSU;
  float mTSUAnimTimer = 0;
  Ogre::Overlay* overlay;
  OverlayElement* mElementHeart;
  OverlayElement* mElementBarHP;
  OverlayElement* mElementBarMoon;

};
