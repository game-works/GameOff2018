#ifndef __Character_H__
#define __Character_H__


#include "SdkSample.h"      // ??
#include <vector>

// Player Character
#include "SinbadCharacterController6.h"

// Active Items
#include "Gem.h"
#include "Enemy.h"
#include "Projectile.h"
#include "CoinScene.h"
#include "ValhallaMob.h"


using namespace Ogre;
using namespace OgreBites;


//class _OgreSampleClassExport ValhallaScene : public SdkSample
class ValhallaScene : public SdkSample
{
public:



    ValhallaScene()
    {
    }


    //
    //  Frame Update
    //
    bool frameRenderingQueued(const FrameEvent& evt)
    {
      Real dt = evt.timeSinceLastFrame;

              // Find Cursor Position

              float width = (float) mCamera->getViewport()->getActualWidth(); // viewport width
              float height = (float) mCamera->getViewport()->getActualHeight(); // viewport height
              float x = m_mouseX / width;
              float y = m_mouseY / height;
              Ray ray = mCamera->getCameraToViewportRay(x, y);
              //Ray ray = mTrayMgr->getCursorRay(mCamera);
              Plane plane = Plane(Vector3::UNIT_Y, 0);
              auto intersects = ray.intersects(plane);
              if (intersects.first)
              {
                cursorPos = ray.getPoint(intersects.second);
                cursorPos.y = mChara->getPosition().y;
              }


              // Animated Heart
              // mTSUAnimTimer += 0.3;
              // int n = (int)floor(mTSUAnimTimer * 3.0) % 6;
              static int m = 0;
              static int n = 0;
              m = (m + 1) % 20;
              if (m == 0)
              {
                n = (n + 1) % 6;
                // mAnimatedTSU->setTextureUScroll(0.25 +
                //   n * 1.0 / 6.0);
                // mAnimatedTSU->_update();
                //mElementHeart->updateTextureGeometry();
                //overlay->update();
                Real a, b, c, d;
                //((PanelOverlayElement*)mElementHeart)->getUV(a, b, c, d);
                ((PanelOverlayElement*)mElementHeart)->setUV(
                  ((float)n),
                  0,
                  ((float)(n+1)),
                  1);
                mElementHeart->_update();
                //((PanelOverlayElement*)mElementHeart)->setTiling(6,1,0);
                //mElementHeart->getMaterial()->getTechnique(0)->getPass(0)->_update();
              }

      mMob.update();

      updateActiveItems(dt);
      m_coinScene->update(dt);

      // Update Camera
      float dist = 320 * 1.5;
      height = 420 * 1.5;
      mCamera->getParentNode()->setPosition(
        mChara->getPosition() +
        Vector3::UNIT_Z * dist  + Vector3::UNIT_Y * height
      );
      mCamera->getParentSceneNode()->lookAt(mChara->getPosition(), Node::TS_PARENT);

      return SdkSample::frameRenderingQueued(evt);
    }


    //
    //  Input
    //
    bool keyPressed(const KeyboardEvent& evt)
    {
        // relay input events to character controller
        if (!mTrayMgr->isDialogVisible()) mChara->injectKeyDown(evt);
        return SdkSample::keyPressed(evt);
    }


    bool keyReleased(const KeyboardEvent& evt)
    {
        // relay input events to character controller
        if (!mTrayMgr->isDialogVisible()) mChara->injectKeyUp(evt);
        return SdkSample::keyReleased(evt);
    }


    bool mouseMoved(const MouseMotionEvent& evt)
    {
        // Relay input events to character controller.
        if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);

        m_mouseX = evt.x;
        m_mouseY = evt.y;

        return false;//SdkSample::mouseMoved(evt);
    }


    virtual bool mouseWheelRolled(const MouseWheelEvent& evt) {
        // Relay input events to character controller.
        if (!mTrayMgr->isDialogVisible()) mChara->injectMouseWheel(evt);
        return SdkSample::mouseWheelRolled(evt);
    }


    bool mousePressed(const MouseButtonEvent& evt)
    {
        // Relay input events to character controller.
        if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt);

        // Action
        //if (mSceneMgr)
        if (evt.button == BUTTON_RIGHT)
        {
          m_projectiles.push_back(
            new Projectile(
              mCamera->getSceneManager(),
              mChara->getPosition(),
              cursorPos )
          );
        }

        return SdkSample::mousePressed(evt);
    }


protected:


    void setupContent()
    {
      setupLights();
      setupGround(); // Plane setup before shadows
      setupShadows();

        m_coinScene = new CoinScene();
        m_coinScene->init(mSceneMgr);

      mMob.init(mSceneMgr, &m_enemies);

        // create a floor mesh resource
        MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Plane(Vector3::UNIT_Y, 0),
            320, 320, // Size
            40, 40,     // Divs
            true, 1,
            1, 1,   // UV
            Vector3::UNIT_Z);

        // create a floor entity, give it a material, and place it at the origin
        Entity* floor = mSceneMgr->createEntity("Floor", "floor");
        //floor->setMaterialName("Examples/Rockwall");
        floor->setMaterialName("Wolf/texture_blend");
        floor->setCastShadows(false);
        mSceneMgr->getRootSceneNode()->attachObject(floor);

        //      LogManager::getSingleton().logMessage("creating sinbad");
        // create our character controller
        mChara = new AnimeCharacterController(mCamera);

        //      LogManager::getSingleton().logMessage("toggling stats");
        //mTrayMgr->toggleAdvancedFrameStats();
        mTrayMgr->hideFrameStats();

        //      LogManager::getSingleton().logMessage("creating panel");
        // StringVector items;
        // items.push_back("Help");
        // ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
        // help->setParamValue("Help", "H / F1");

        //      LogManager::getSingleton().logMessage("all done");


        // Interface
        mTrayMgr->hideLogo();
        mTrayMgr->hideBackdrop();

        mTrayMgr->setTrayWidgetAlignment(TL_TOP, GHA_CENTER);

        Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

        // Create an overlay
        overlay = overlayManager.create( "OverlayName" );

        Ogre::OverlayElement* panel0 = overlayManager.createOverlayElement("Panel", "Decor/0");
        Ogre::OverlayElement* panel1 = overlayManager.createOverlayElement("Panel", "Decor/Heart");
        Ogre::OverlayElement* panel2 = overlayManager.createOverlayElement("Panel", "Decor/Moon");
        Ogre::OverlayElement* panelBar1a = overlayManager.createOverlayElement("Panel", "Decor/Bar1A");
        Ogre::OverlayElement* panelBar2a = overlayManager.createOverlayElement("Panel", "Decor/Bar2A");
        Ogre::OverlayElement* panelBar1b = overlayManager.createOverlayElement("Panel", "Decor/Bar1B");
        Ogre::OverlayElement* panelBar2b = overlayManager.createOverlayElement("Panel", "Decor/Bar2B");

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

        panelBar1b->setMetricsMode(GMM_PIXELS);
        panelBar1b->setPosition( 400 - 264 - 64, offsetTop + 16 + 5 );
        panelBar1b->setDimensions( 264, 22 );
        panelBar1b->setMaterialName( "Wolf/BarHealth" );

        panelBar2b->setMetricsMode(GMM_PIXELS);
        panelBar2b->setPosition( 400 + 64, offsetTop + 16 + 5 );
        panelBar2b->setDimensions( 264 / 2, 22 );
        panelBar2b->setMaterialName( "Wolf/BarMoon" );

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
        overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBar1b) );
        overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBar2b) );
        overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBarL) );
        overlay->add2D( static_cast<Ogre::OverlayContainer*>(panelBarR) );

        // Show the overlay
        overlay->show();


    }


    void setupLights()
    {
      // Set ambient light ...
      mSceneMgr->setAmbientLight(ColourValue(0.4, 0.4, 0.4));

      // Fixed light, dim
      mSunLight = mSceneMgr->createLight("SunLight");
      mSunLight->setType(Light::LT_SPOTLIGHT);

      Vector3 pos(1500, 1750, 1300);
      SceneNode* sunSceneNode =
      mSceneMgr->getRootSceneNode()->createChildSceneNode(pos);
      sunSceneNode->attachObject(mSunLight);
      mSunLight->setSpotlightRange(Degree(30), Degree(50));
      mSunLight->setDirection(-pos.normalisedCopy());
      mSunLight->setDiffuseColour(0.35, 0.35, 0.38);
      mSunLight->setSpecularColour(0.9, 0.9, 1);

            sunSceneNode->yaw(Degree(-8));
            sunSceneNode->pitch(Degree(-88));

      // Point light, movable, reddish
      mLight = mSceneMgr->createLight("Light2");
      mLight->setDiffuseColour(0.5, 0.3, 0.1);
      mLight->setSpecularColour(1, 1, 1);
      mLight->setAttenuation(8000, 1, 0.0005, 0);

      // Create light node
      mLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
          "MovingLightNode");
      mLightNode->attachObject(mLight);
      // create billboard set
      //mLight->setPosition(Vector3(300,250,-300));
      mLight->setPosition(Vector3(300, 750, -200));

      mLightNode->yaw(Degree(8));
      mLightNode->pitch(Degree(-59));
    }


    void setupGround()
    {
      // Another node?
      SceneNode* node;
      node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

      // Floor plane (use POSM plane def)
      mPlane = new MovablePlane("*mPlane");
      mPlane->normal = Vector3::UNIT_Y;
      mPlane->d = 0;
      MeshManager::getSingleton().createPlane("Myplane",
          ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *mPlane,
          1500 * 4, 1500 * 4,
          50, 50,
          true,
          1,
          5, 5,
          Vector3::UNIT_Z);
      pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
      pPlaneEnt->setMaterialName("Wolf/DirtBlend");
      pPlaneEnt->setCastShadows(false);
      mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);
      mSceneMgr->setShadowTextureSettings(1024, 2);
      mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
      //mSceneMgr->setShowDebugShadows(true);
    }


    void setupShadows()
    {
      // do this first so we generate edge lists
      mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);

          // Fixed light, dim
          mSunLight->setCastShadows(false);
          mSunLight->setType(Light::LT_DIRECTIONAL);

          // Change moving light to spotlight
          // Point light, movable, reddish
          mLight->setType(Light::LT_DIRECTIONAL);
          mLight->setCastShadows(true);
          mLight->setDiffuseColour(0.5, 0.3, 0.1);
          mLight->setSpecularColour(1, 1, 1);
          mLight->setAttenuation(8000,1,0.0005,0);
          mLight->setSpotlightRange(Degree(80),Degree(90));

      handleProjectionChanged();
    }


    void cleanupContent()
    {
        // clean up character controller and the floor mesh
        if (mChara)
        {
            delete mChara;
            mChara = 0;
        }
        MeshManager::getSingleton().remove("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }


    void updateActiveItems(Real dt)
    {

      // Let the player character update with animations and camera
      mChara->addTime(dt);

      // Update the active items
      // ... Gems
      for (auto i : m_gems)
      {
        i->update(dt);
      }

      // ... Enemies
      for (auto i : m_enemies)
      {
        i->update(dt, mChara->getPosition());
      }

      // Projectiles
      for (auto i : m_projectiles)
        i->update(dt);

      // Check proximity of active items
      // ... Gems
      for (auto i = m_gems.begin(); i != m_gems.end();)
      {
        Real d2 = (*i)->getPosition().squaredDistance(mChara->getPosition());
        if (d2 < 42)
        {
          // Collect Gem
          delete *i;
          i = m_gems.erase(i);
        }
        else
        {
          ++i;
        }
      }

      // ... Enemies
      for (auto i = m_enemies.begin(); i != m_enemies.end();)
      {
        Real d2 = (*i)->getPosition().squaredDistance(mChara->getPosition());
        if (d2 < 36 * 12 * 12)
        {
          // Kill enemy, leave a gem
          SceneManager* sm = mCamera->getSceneManager();
          m_gems.push_back(new Gem(sm, (*i)->getPosition()));

          // Leave a coin
          int nk = 1 + rand() % 3;
          for (int k = 0; k < nk; ++k)
            m_coinScene->createCoin((*i)->getPosition());

          delete *i;
          i = m_enemies.erase(i);
        }
        else
        {
          ++i;
        }
      }

      // Enemies vs projectiles
      for (auto i = m_enemies.begin(); i != m_enemies.end();)
      {
        for (auto j = m_projectiles.begin(); j != m_projectiles.end();)
        {
          Real d2 = (*i)->getPosition().squaredDistance((*j)->getPosition());
          if (d2 < 25 * 12 * 12)
          {
            // Kill enemy, leave a gem
            SceneManager* sm = mCamera->getSceneManager();
            m_gems.push_back(new Gem(sm, (*i)->getPosition()));

            // Leave a coin
            int nk = 1 + rand() % 3;
            for (int k = 0; k < nk; ++k)
              m_coinScene->createCoin((*i)->getPosition());

            delete *i;
            i = m_enemies.erase(i);

            // Remove projectile
            delete *j;
            j = m_projectiles.erase(j);
          }
          else
          {
            ++j;
          }
        }
        if (i != m_enemies.end()) ++i; // bug?
      }

      // More projectiles... Expiration
      for (auto j = m_projectiles.begin(); j != m_projectiles.end();)
      {
        if ((*j)->isStationary())
        {
          delete *j;
          j = m_projectiles.erase(j);
        }
        else
        {
          ++j;
        }
      }

      // Check Enemy Spawn potential
      // if (m_enemies.size() < 24)
      // {
      //   Real t = Ogre::Math::RangeRandom(0.0, 360.0);
      //   Real r = Ogre::Math::RangeRandom(45.0, 65.0);
      //   Real x = r * Ogre::Math::Cos(Ogre::Degree(t)) + mChara->getPosition().x;
      //   Real y = r * Ogre::Math::Sin(Ogre::Degree(t)) + mChara->getPosition().z;
      //
      //   int k = rand() % 6;
      //   if (k <= 3)
      //     m_enemies.push_back(
      //       new EnemySample(
      //         mCamera->getSceneManager(),
      //         Ogre::Vector3(x, 5.5, y),
      //         "Mage.mesh",
      //         "my_animation",
      //         0.5)
      //     );
      //   if (k == 4)
      //     m_enemies.push_back(
      //       new EnemySample(
      //         mCamera->getSceneManager(),
      //         Ogre::Vector3(x, 5.5, y),
      //         "Cylinder.001.mesh",
      //         "Skeleton_Idle",
      //         2)
      //     );
      //   if (k == 5)
      //     m_enemies.push_back(
      //       new EnemySample(
      //         mCamera->getSceneManager(),
      //         Ogre::Vector3(x, 5.5, y),
      //         "Dragon.mesh",
      //         "Dragon_Flying",
      //        4)
      //     );
      // }

    }


    void handleProjectionChanged()
    {
      mCurrentShadowCameraSetup =
        ShadowCameraSetupPtr(new PlaneOptimalShadowCameraSetup(mPlane));
      mSceneMgr->setShadowCameraSetup(mCurrentShadowCameraSetup);
    }



    //
    //  Elements of the scene
    //

    AnimeCharacterController* mChara;
    std::vector<Enemy*> m_enemies;
    std::vector<Projectile*> m_projectiles;
    std::vector<Gem*> m_gems;
    Vector3 cursorPos = Vector3::ZERO;

    float m_mouseX = 0;
    float m_mouseY = 0;

    TextureUnitState* mAnimatedTSU;
    float mTSUAnimTimer = 0;
    Ogre::Overlay* overlay;
    OverlayElement* mElementHeart;

    CoinScene* m_coinScene;

    Light* mLight;
    Light* mSunLight;
    SceneNode* mLightNode;
    Entity* pPlaneEnt;
    MovablePlane* mPlane;
    ShadowCameraSetupPtr mCurrentShadowCameraSetup;
    ValhallaMob mMob;

};

#endif
