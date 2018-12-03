#pragma once


#include "ActiveItem.h"


using namespace Ogre;


class Blink : public ActiveItem
{
public:

  Blink(int type, Vector3 pos, Camera* cam)
  {
    mType = type;
    mPos = pos;
    mCamera = cam;
    mTime = 0;

    // Make panel
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    mOverlay = overlayManager.getByName( "OverlayName_Blink" );

    if (mOverlay == 0)
    {
      mOverlay = overlayManager.create( "OverlayName_Blink" );
      mOverlay->show();
    }

    static int s_int = 0;
    String name = "Blink" + Ogre::StringConverter::toString(s_int);
    ++s_int;
    mPanel = overlayManager.createOverlayElement("Panel", name);
    mPanel->setMetricsMode(GMM_PIXELS);
    mPanel->setDimensions( 4, 11 );
    mPanel->setMaterialName( "Wolf/BlinkBlue" );

    mOverlay->add2D( static_cast<Ogre::OverlayContainer*>(mPanel) );


    addTime(0);
  }


  ~Blink()
  {
    mOverlay->remove2D( static_cast<Ogre::OverlayContainer*>(mPanel) );
    delete mPanel;
  }


  void addTime(Real dt)
  {
    mTime += dt;
    if (mTime > 0.5) mRemove = true;

    // Panel position
    Vector3 hcsPosition = mCamera->getProjectionMatrix() * mCamera->getViewMatrix() * mPos;

    mPanel->setPosition(
      0.5 * (hcsPosition.x + 1) * mCamera->getViewport()->getActualWidth() - 7,
      0.5 * (1.0 - hcsPosition.y) * mCamera->getViewport()->getActualHeight() - 125 * mTime
     );
  }

protected:

  int mType;
  Vector3 mPos;
  Camera* mCamera;
  Real mTime;
  Overlay* mOverlay;
  OverlayElement* mPanel;
};
