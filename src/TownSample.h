#pragma once


#include "SdkSample.h"      // ??
#include "BulletScene.h"
#include "btOgreSoftBody.hpp"
#include "SinbadCharacterController6.h"
// TODO Active Items
#include <stdlib.h>     /* srand, rand */
#include "ActiveNPC.h"
#include "MessageManager.h"
#include "Continuity.h"
#include "ConvoManager.h"
#include "ChickenMob.h"
#include "ValhallaMob.h"
#include "Gem.h"
#include "Blink.h"
#include "StatsPanel.h"
#include "StatsManager.h"
#include "Projectile.h"
#include "CoinScene.h"


using namespace Ogre;
using namespace OgreBites;


class TownScene : public SdkSample
{
protected:

  Light* mLight;
  Light* mSunLight;
  SceneNode* mLightNode;

  // SceneNode* mCameraNode;
  // SceneNode* mCameraTargetNode;


  Entity* pPlaneEnt;
  MovablePlane* mPlane;


  // ShadowTechnique mCurrentShadowTechnique = SHADOWTYPE_TEXTURE_MODULATIVE;
  // ShadowProjection mCurrentProjection = PLANE_OPTIMAL;

  AnimeCharacterController* mCharacter;

  ShadowCameraSetupPtr mCurrentShadowCameraSetup;

  std::vector<ActiveItem*> mActiveItems;
  std::vector<ActiveItem*> mActiveItemsStashTown;
  std::vector<ActiveItem*> mActiveItemsStashForest;

  // Message panel
  MessageManager mMessageManager;
  ConvoManager mConvoManager;

  ChickenMob* mChickenMob;
  ValhallaMob* mValhallaMob;

  std::vector<Ogre::Vector3> mGemsToAdd;
  std::vector<int> mGemsToAddType;
  std::vector<Ogre::Vector3> mBlinksToAdd;
  std::vector<int> mBlinksToAddType;
  std::vector<Ogre::Vector3> mCoinsToAdd;


  StatsPanel mStatsPanel;

  SceneNode* mTownNode;
  SceneNode* mForestNode;
  int mLevel = 0;

  std::vector<Projectile*> m_projectiles;
  Vector3 cursorPos = Vector3::ZERO;
  float m_mouseX = 0;
  float m_mouseY = 0;

  CoinScene* m_coinScene;


public:

  //
  //  Frame
  //
  bool frameEnded(const FrameEvent& evt)
  {
    float dt = evt.timeSinceLastFrame;

    m_coinScene->update(dt);

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
      cursorPos.y = mCharacter->getPosition().y;
    }

    // Gems?
    while (mGemsToAdd.size())
    {
      Ogre::Vector3 p = mGemsToAdd.back();
      mGemsToAdd.pop_back();

      int i = mGemsToAddType.back();
      mGemsToAddType.pop_back();

      if (mLevel == 0)
        mActiveItems.push_back(
          new Gem(mSceneMgr, mTownNode, p, i));
      else
        mActiveItems.push_back(
          new Gem(mSceneMgr, mForestNode, p, i));
    }

    // Coins
    while (mCoinsToAdd.size())
    {
      Ogre::Vector3 p = mCoinsToAdd.back();
      mCoinsToAdd.pop_back();

      int nk = 1 + rand() % 3;
      for (int k = 0; k < nk; ++k)
        m_coinScene->createCoin(p);
    }

    // Blinks;
    while (mBlinksToAdd.size())
    {
      Ogre::Vector3 p = mBlinksToAdd.back();
      mBlinksToAdd.pop_back();

      int i = mBlinksToAddType.back();
      mBlinksToAddType.pop_back();

      mActiveItems.push_back(
        new Blink(i, p, mCamera));
    }

    // Character handling
    mCharacter->addTime(evt.timeSinceLastFrame);

    // Projectiles
    for (auto i : m_projectiles)
      i->update(dt);

    // Enemies vs projectiles
    for (auto i = mActiveItems.begin(); i != mActiveItems.end();)
    {
      if ((*i)->classType() == "Skeleton" ||
          (*i)->classType() == "Slime" ||
          (*i)->classType() == "Hen")
      {
        for (auto j = m_projectiles.begin(); j != m_projectiles.end();)
        {
          Real d2 = (*i)->getPosition().squaredDistance((*j)->getPosition());
          if (d2 < 4 * 25 * 12 * 12)
          {
            (*i)->hitRange();

            // Remove projectile
            delete *j;
            j = m_projectiles.erase(j);
          }
          else
          {
            ++j;
          }
        }
        if (i != mActiveItems.end()) ++i; // bug?
      }
      else
      {
        ++i;
      }
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

    // Active object timing
    for (auto i : mActiveItems)
    {
      i->addTime(evt.timeSinceLastFrame);

      // proximity of gems
      if (i->classType() == "Gem2" || i->classType() == "Gem1")
      {
        Real d2 = i->getPosition().squaredDistance(mCharacter->getPosition());
        if (d2 < 12 * 15 * 15)
        {
          // There it goes
          i->mRemove = true;
        }
      }
    }

    // Chicken getting hit
    if (mCharacter->mDamaging)
    {
      for (auto i : mActiveItems)
      {
        // Distance check
        Real r = 80;
        if (i->getPosition().squaredDistance(mCharacter->getPosition()) < r * r)
        {
          i->hit();
        }
      }
    }

    // Getting hit back
    for (auto i : mActiveItems)
    {
      if (i->mDamaging)
      {
        i->mDamaging = false;
        if (i->classType() == "Slime" &&
            i->getPosition().squaredDistance(mCharacter->getPosition()) < 35 * 35)
        {
          mCharacter->hit(6);
        }

        if (i->classType() == "Skeleton" &&
            i->getPosition().squaredDistance(mCharacter->getPosition()) < 55 * 55)
        {
          mCharacter->hit(13);
        }
      }
    }

    // Check for respawn
    if (StatsManager::hp <= 0)
    {
      //respawn();

      StatsManager::hp = 100;
      StatsManager::moon = 0;
      mCharacter->setPosition(Vector3(0, 0, 38.0 * 35.0 - 50));

      // clear active items
      for (auto i : mActiveItems)
      {
        i->mRemove = true;
      }

      Continuity::died();

      mValhallaMob->mSpawned = 0;
    }


    // Activate NPC convo
    Real r = 200.0;
    for (auto i : mActiveItems)
    {
      if (i->classType() == "NPC")
      {
        if (i->getPosition().squaredDistance(mCharacter->getPosition()) < r * r)
            //((ActiveNPC1*)i)->m_hasConvo == true)
        {
          //((ActiveNPC1*)i)->m_hasConvo = false;
          mConvoManager.setConvo(((ActiveNPC1*)i)->m_convo, (ActiveNPC1*)i, mCharacter);
        }
      }
    }

    mConvoManager.update(evt.timeSinceLastFrame, mCamera);
    if (mConvoManager.ended)
    {
      Continuity::onConversationEnd();
      mConvoManager.ended = false;
    }

    // Lock Character to edge position
    Vector3 p;
    if (mLevel == 0)
    {
      p = mCharacter->getPosition();
      r = 40.0 * 35.0;
      Real d = 36.0 * 35.0;
      // Circle border
      if (p.x * p.x + p.z * p.z > r * r && p.z >= -d)
      {
        p *= (r * r) / (p.x * p.x + p.z * p.z);
      }
      // Square border
      if (p.x > d) p.x = d;
      if (p.x < -d) p.x = -d;
      if (p.z > d) p.z = d;
      if (p.z < -d)
      {
        if (p.x < -595) p.x = -595;
        if (p.x > 595) p.x = 595;

        //p.z = -d;
      }

      // Move to Next Scene
      if (p.z < -38.0 * 35.0)
      {
        //Continuity::changeLevel = true;
        setLevelForest();
        p.z = 38.0 * 35.0 - 50;
      }
    }
    else
    {
      p = mCharacter->getPosition();
      r = 40.0 * 35.0;
      Real d = 36.0 * 35.0;

      // Square border
      if (p.x < -d) p.x = -d;
      if (p.x > d) p.x = d;
      if (p.z < -d) p.z = -d;
      if (p.z > d)
      {
        if (p.x < -595) p.x = -595;
        if (p.x > 595) p.x = 595;
      }

      // Move to Next Scene
      if (p.z > 38.0 * 35.0)
      {
        //Continuity::changeLevel = true;
        setLevelTown();
        p.z = -38.0 * 35.0 + 50;
      }
    }


    mCharacter->setPosition(p);

    // Lock actives to edge position
    if (mLevel == 0)
    {
      for (auto i : mActiveItems)
      {
        Vector3 p = i->getPosition();
        r = 40.0 * 35.0;
        Real d = 36.0 * 35.0;
        // Circle border
        if (p.x * p.x + p.z * p.z > r * r)
        {
          p *= (r * r) / (p.x * p.x + p.z * p.z);
        }
        // Square border
        if (p.x > d) p.x = d;
        if (p.x < -d) p.x = -d;
        if (p.z > d) p.z = d;
        if (p.z < -d) p.z = -d;

        i->setPosition(p);
      }
    }
    else
    {
      for (auto i : mActiveItems)
      {
        Vector3 p = i->getPosition();
        r = 40.0 * 35.0;
        Real d = 36.0 * 35.0;

        // Square border
        if (p.x > d) p.x = d;
        if (p.x < -d) p.x = -d;
        if (p.z > d) p.z = d;
        if (p.z < -d) p.z = -d;

        i->setPosition(p);
      }
    }

    // Active item removal - delete and erase
    for (auto i = mActiveItems.begin(); i != mActiveItems.end();)
    {
      if ((*i)->mRemove)
      {
        finalMoment(*i);
        delete *i;
        i = mActiveItems.erase(i);
      }
      else
      {
        ++i;
      }
    }

    // Update Camera
    //mCameraMan->getTarget()->setPosition(mCharacter->getPosition());
    float dist = 320 * 1.5;
    height = 420 * 1.5;
    mCamera->getParentNode()->setPosition(
      mCharacter->getPosition() +
      Vector3::UNIT_Z * dist  + Vector3::UNIT_Y * height
    );

    mCamera->getParentSceneNode()->lookAt(mCharacter->getPosition(), Node::TS_PARENT);

    // Mob
    if (mLevel == 0) mChickenMob->update(mCharacter->getPosition());
    else mValhallaMob->update(mCharacter->getPosition());

    Continuity::characterPosition = mCharacter->getPosition();

    // Stats
    mStatsPanel.addTime(evt.timeSinceLastFrame);
    if (mLevel == 0) StatsManager::addHP(4.2 * evt.timeSinceLastFrame);

    // SDK...
    return SdkSample::frameEnded(evt);
  }


  void finalMoment(ActiveItem* item)
  {
    if (item->classType() == "Gem2")
    {
      // Collect moon power
      StatsManager::addMoon(10.01);

      // Add blink decal
      mBlinksToAdd.push_back(item->getPosition());
      mBlinksToAddType.push_back(2);
    }

    if (item->classType() == "Gem1")
    {
      // Collect moon power
      StatsManager::addHP(5);

      // Add blink decal
      mBlinksToAdd.push_back(item->getPosition());
      mBlinksToAddType.push_back(1);
    }

    if (item->classType() == "Hen")
    {
      // Spawn Moon Gem
      mGemsToAdd.push_back(item->getPosition());
      mGemsToAddType.push_back(0);
    }

    if (item->classType() == "Skleton")
    {
      int i = rand() % 12;

      if (i < 5)
      {
        mCoinsToAdd.push_back(item->getPosition());
      }
      else if (i < 7)
      {
        // Spawn Moon Gem
        mGemsToAdd.push_back(item->getPosition());
        mGemsToAddType.push_back(0);
      }
      else if (i == 8)
      {
        // Spawn Moon Gem
        mGemsToAdd.push_back(item->getPosition());
        mGemsToAddType.push_back(1);
      }
    }

    if (item->classType() == "Slime")
    {
      int i = rand() % 12;

      if (i < 5)
      {
        mCoinsToAdd.push_back(item->getPosition());
      }
      else if (i < 7)
      {
        // Spawn Moon Gem
        mGemsToAdd.push_back(item->getPosition());
        mGemsToAddType.push_back(0);
      }
      else if (i == 8)
      {
        // Spawn Moon Gem
        mGemsToAdd.push_back(item->getPosition());
        mGemsToAddType.push_back(1);
      }
    }
  }


  //
  //  Input
  //
  bool keyPressed(const KeyboardEvent& evt)
  {
    // relay input events to character controller
    mCharacter->injectKeyDown(evt);
    Continuity::onKeyPress(evt.keysym.sym);
    return SdkSample::keyPressed(evt);
  }


  bool keyReleased(const KeyboardEvent& evt)
  {
    // relay input events to character controller
    mCharacter->injectKeyUp(evt);
    return SdkSample::keyReleased(evt);
  }


  bool mouseMoved(const MouseMotionEvent& evt)
  {
      // Relay input events to character controller.
      if (!mTrayMgr->isDialogVisible()) mCharacter->injectMouseMove(evt);

      m_mouseX = evt.x;
      m_mouseY = evt.y;

      return false;//SdkSample::mouseMoved(evt);
  }


  virtual bool mouseWheelRolled(const MouseWheelEvent& evt) {
      // Relay input events to character controller.
      if (!mTrayMgr->isDialogVisible()) mCharacter->injectMouseWheel(evt);
      return SdkSample::mouseWheelRolled(evt);
  }


  bool mousePressed(const MouseButtonEvent& evt)
  {
      // Relay input events to character controller.
      if (!mTrayMgr->isDialogVisible()) mCharacter->injectMouseDown(evt);

      if (evt.button == BUTTON_LEFT) Continuity::onAttack();
      if (evt.button == BUTTON_RIGHT) Continuity::onShoot();

      // Action
      //if (mSceneMgr)
      if (evt.button == BUTTON_RIGHT)
      {
        if (StatsManager::moon > 10)
        {
          StatsManager::moon -= 10;

          m_projectiles.push_back(
            new Projectile(
              mCamera->getSceneManager(),
              mCharacter->getPosition(),
              cursorPos )
          );
        }
      }

      return SdkSample::mousePressed(evt);
  }


  //
  // Setup
  //
  // Just override the mandatory create scene method
  void setupContent(void)
  {
    mTrayMgr->hideLogo();
    mTrayMgr->hideFrameStats();

    mMessageManager.setMessage("Press W, A, S, D to move");
    Continuity::s_messageManager = &mMessageManager;

    mForestNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mTownNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    m_coinScene = new CoinScene();
    m_coinScene->init(mSceneMgr, mForestNode);

    setupLights();
    setupGround(); // Plane setup before shadows
    setupShadows();
    setupCharacter();
    setupScenery();
    setupActiveItems();
    setupDialogs();

    mTownNode->setVisible(true);
    mForestNode->setVisible(false);

    // Chicken Mob
    mChickenMob = new ChickenMob(mSceneMgr, mTownNode, &mActiveItems);
    mValhallaMob = new ValhallaMob(mSceneMgr, mForestNode, &mActiveItems);

    //
    mStatsPanel.init();
  }


  void setLevelTown()
  {
    mLevel = 0;
    mTownNode->setVisible(true);
    mForestNode->setVisible(false);

    // Pop em
    while (mActiveItems.size())
    {
      mActiveItemsStashForest.push_back(
        mActiveItems.back()
      );

      mActiveItems.pop_back();
    }

    // Push em
    while (mActiveItemsStashTown.size())
    {
      mActiveItems.push_back(
        mActiveItemsStashTown.back()
      );

      mActiveItemsStashTown.pop_back();
    }
  }


  void setLevelForest()
  {
    mLevel = 1;
    mTownNode->setVisible(false);
    mForestNode->setVisible(true);

    // Pop em
    while (mActiveItems.size())
    {
      mActiveItemsStashTown.push_back(
        mActiveItems.back()
      );

      mActiveItems.pop_back();
    }

    // Push em
    while (mActiveItemsStashForest.size())
    {
      mActiveItems.push_back(
        mActiveItemsStashForest.back()
      );

      mActiveItemsStashForest.pop_back();
    }
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
        node = mTownNode->createChildSceneNode();

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
        node->attachObject(pPlaneEnt);
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


      void setupActiveItems()
      {
        ActiveNPC1* npc;

        npc = new ActiveNPC1(
          mSceneMgr,
          mTownNode,
          Vector3(36.0 * -14.495739, 86.000000, 36.0 * -0.073246),
          "FemaleMage.mesh"
        );
        mActiveItems.push_back(npc);

        npc = new ActiveNPC1(
          mSceneMgr,
          mTownNode,
          Vector3(36.0 * -27.832264, 76.000000, 36.0 * -25.838675),
          "Berzerker.mesh"
        );
        mActiveItems.push_back(npc);

        npc = new ActiveNPC1(
          mSceneMgr,
          mTownNode,
          Vector3(36.0 * 0.135112, 86.000000, 36.0 * -34.888699),
          "Knight.mesh"
        );
        mActiveItems.push_back(npc);

        npc = new ActiveNPC1(
          mSceneMgr,
          mTownNode,
          Vector3(36.0 * 14.612830, 86.000000, 36.0 * -16.448574),
          "Mage.mesh"
        );
        mActiveItems.push_back(npc);

        npc = new ActiveNPC1(
          mSceneMgr,
          mTownNode,
          Vector3(36.0 * 9.378317, 86.000000, 36.0 * 17.837187),
          "Priest.mesh"
        );
        mActiveItems.push_back(npc);
      }


      void setupCharacter()
      {
        //...
        mCharacter = new AnimeCharacterController(mCamera);

        // The very first start pos
        mCharacter->setPosition(Vector3(-1250, 0, 0));
      }


    void setupScenery2()
    {
      SceneNode* node;
      Entity* pEnt;
      String mesh = "column.mesh";

      String meshSelection[10] =
      {"barrel2.011.mesh", "p1.mesh", "p2.mesh", "p3.mesh", "FishtailPalm.mesh",
       "Planea.mesh", "Planeb.mesh", "Planea2.mesh", "Planeb2.mesh", "Barrel.mesh" };

      for (int x = -15; x <= 15; ++x)
      {
          for (int z = -15; z <= 15; ++z)
          {
              if (x != 0 || z != 0)
              {
                int i = rand() % 10;
                if (i >3 ) continue;
                  mesh = meshSelection[ i ];
                  node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
                  pEnt = mSceneMgr->createEntity(mesh);
//                  pColumns.push_back(pEnt);
                  node->attachObject( pEnt );
                  node->translate(x * 200,0, z * 200);

                  Real z1 = Math::RangeRandom(-1.0, 1.0);
                  Real z2 = Math::RangeRandom(-1.0, 1.0);
                  Real zd = 180.0 / (z1*z1 + z2*z2);
                  z1 *= zd;
                  z2 *= zd;
                  node->translate(z1, 0, z2);

                  node->setScale(Vector3(20));
              }
          }
      }
    }


    void handleProjectionChanged()
    {
      mCurrentShadowCameraSetup =
        ShadowCameraSetupPtr(new PlaneOptimalShadowCameraSetup(mPlane));
      mSceneMgr->setShadowCameraSetup(mCurrentShadowCameraSetup);
    }


    virtual void setupView()
    {
        SdkSample::setupView();

        // incase infinite far distance is not supported
        mCamera->setFarClipDistance(100000);

        mCameraMan->setStyle(CS_MANUAL);
    }


    virtual void cleanupContent() // TODO
    {
        MeshManager::getSingleton().remove("Myplane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }


    void setupDialogs()
    {
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(0, "Hey"));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(0, "Are you the Huntress?"));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(1, "I am"));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(0, "Are you... some kind of hybrid?"));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(1, "A part of me is human."));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(1, "But a part of me is of the ancient forest."));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(0, "Your power can save us"));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(1, "What is this place?"));
      ((ActiveNPC1*)mActiveItems[0])->m_convo.lines.push_back(Dialog::Line(0, "It was our home"));

      ((ActiveNPC1*)mActiveItems[1])->m_convo.lines.push_back(Dialog::Line(0, "Almost everyone has evacuated"));
      ((ActiveNPC1*)mActiveItems[1])->m_convo.lines.push_back(Dialog::Line(0, "Our home was destroyed"));
      ((ActiveNPC1*)mActiveItems[1])->m_convo.lines.push_back(Dialog::Line(0, "This is all we have left"));

      ((ActiveNPC1*)mActiveItems[2])->m_convo.lines.push_back(Dialog::Line(0, "A mysterious force is out there"));
      ((ActiveNPC1*)mActiveItems[2])->m_convo.lines.push_back(Dialog::Line(0, "Venture foward if you are strong enough"));

      ((ActiveNPC1*)mActiveItems[3])->m_convo.lines.push_back(Dialog::Line(0, "I have heard of your power"));
      ((ActiveNPC1*)mActiveItems[3])->m_convo.lines.push_back(Dialog::Line(0, "It is unlike anything I have seen"));
      ((ActiveNPC1*)mActiveItems[3])->m_convo.lines.push_back(Dialog::Line(1, "It's a curse on me..."));
      ((ActiveNPC1*)mActiveItems[3])->m_convo.lines.push_back(Dialog::Line(0, "Maybe, but for us it means salvation"));

      ((ActiveNPC1*)mActiveItems[4])->m_convo.lines.push_back(Dialog::Line(0, "I just can't believe our home is gone"));

    }


    void setupScenery()
    {
      SceneNode* sn = 0;

      //
      //  Forest Time
      //

      // create a floor mesh resource
      MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
          Plane(Vector3::UNIT_Y, 0),
          1500 * 4, 1500 * 4,
          50, 50,
          true, 1,
          5, 5,   // UV
          Vector3::UNIT_Z);

      Entity* floor = mSceneMgr->createEntity("Floor", "floor");
      //floor->setMaterialName("Examples/Rockwall");
      floor->setMaterialName("Wolf/ForestBlendX");
      floor->setCastShadows(false);
      mForestNode->attachObject(floor);

      sn = mForestNode->createChildSceneNode(Vector3(0, 0, -4));
      sn->setScale(Vector3(36));
      sn->setOrientation(Quaternion(0, 0, 1, 0));
      Entity*  a= mSceneMgr->createEntity("Arrow.mesh");
      a->setCastShadows(false);
      sn->attachObject(a);

      //
      // Town time
      //

      SceneNode* root = mTownNode->createChildSceneNode();
      root->setScale(Vector3(36));

      sn = root->createChildSceneNode(Vector3(0, 0, -4));
      Entity*  b= mSceneMgr->createEntity("Arrow.mesh");
      b->setCastShadows(false);
      sn->attachObject(b);

      // sn = root->createChildSceneNode(Vector3(-3.444748, 11.612431, -2.526091));
      // sn->setOrientation(Quaternion(1.000000, 0.000000, 0.000000, -0.000000));
      // sn->setScale(Vector3(0.100000, 0.100000, 0.100000));
      sn = root->createChildSceneNode(Vector3(-40.5262, 0, 30.6557));
      sn->attachObject(mSceneMgr->createEntity("CathedralRuinOne300.mesh"));
      sn->yaw(Degree(85.1));
      sn->setScale(Vector3(0.172));
      //sn->setScale(Vector3(0.100000, 0.100000, 0.100000));

      sn = root->createChildSceneNode(Vector3(-28.5066, 0.53212, -32.5137));
      sn->attachObject(mSceneMgr->createEntity("pot.mesh"));
      sn = root->createChildSceneNode(Vector3(-28.5066, 1.28563, -32.5137));
      sn->setScale(Vector3(0.445));
      sn->attachObject(mSceneMgr->createEntity("potCap.mesh"));

      sn = root->createChildSceneNode(Vector3(-30.1719, 0.40176, -30.7661));
      sn->setScale(Vector3(0.739932));
      sn->attachObject(mSceneMgr->createEntity("pot.mesh"));
      sn = root->createChildSceneNode(Vector3(-30.1719, 0.959305, -30.7661));
      sn->setScale(Vector3(0.329));
      sn->attachObject(mSceneMgr->createEntity("potCap.mesh"));

      sn = root->createChildSceneNode(Vector3(-32.3957, 0.40176, -31.5075));
      sn->setScale(Vector3(0.739932));
      sn->attachObject(mSceneMgr->createEntity("pot.mesh"));
      sn = root->createChildSceneNode(Vector3(-32.3957, 0.959305, -31.5075));
      sn->setScale(Vector3(0.329));
      sn->attachObject(mSceneMgr->createEntity("potCap.mesh"));

      sn = root->createChildSceneNode(Vector3(-30.5132, -0.115122, -24.4688));
      sn->setScale(Vector3(0.739932));
      sn->attachObject(mSceneMgr->createEntity("barrel2.mesh"));

      sn = root->createChildSceneNode(Vector3(-28.3302, -0.115122, -20.1078));
      sn->setScale(Vector3(0.739932));
      sn->attachObject(mSceneMgr->createEntity("barrel2.mesh"));

      sn = root->createChildSceneNode(Vector3(-22.352, -0.115122, 29.4283));
      sn->setScale(Vector3(0.739932));
      sn->attachObject(mSceneMgr->createEntity("barrel2.mesh"));

      sn = root->createChildSceneNode(Vector3(-29.5747, -0.115122, 21.1244));
      sn->setScale(Vector3(0.739932));
      sn->attachObject(mSceneMgr->createEntity("barrel2.mesh"));

      sn = root->createChildSceneNode(Vector3(33.44475, 0, 27.5));
      sn->setOrientation(Quaternion(0.306582, 0.306582, 0.637187, -0.637187));
      sn->setScale(Vector3(0.707170, 0.707170, 0.707170));
      sn->attachObject(mSceneMgr->createEntity("prop_env_rock01_Plane.mesh"));

      sn = root->createChildSceneNode(Vector3(33.44475, 0, 27.5));
      sn->setOrientation(Quaternion(0.306582, 0.306582, 0.637187, -0.637187));
      sn->setScale(Vector3(0.707170, 0.707170, 0.707170));
      sn->attachObject(mSceneMgr->createEntity("prop_env_rock01_Plane.mesh"));

      // sn = root->createChildSceneNode(Vector3(-3.444748, 11.612430, -2.526085));
      // sn->setOrientation(Quaternion(1.000000, -0.000000, 0.000000, -0.000000));
      // sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      // sn->attachObject(mSceneMgr->createEntity("CathedralRuinOne300.mesh"));

      sn = root->createChildSceneNode(Vector3(33.44475, 0, 27.5));
      sn->setOrientation(Quaternion(0.306582, 0.306582, 0.637187, -0.637187));
      sn->setScale(Vector3(0.707170, 0.707170, 0.707170));
      sn->attachObject(mSceneMgr->createEntity("prop_env_rock01_Plane.mesh"));

      sn = root->createChildSceneNode(Vector3(27.946651, 0.000000, 33.540115));
      sn->setOrientation(Quaternion(0.500000, 0.500000, -0.500000, 0.500000));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_rock01_Plane.mesh"));

      sn = root->createChildSceneNode(Vector3(13.390029, 0.000000, 37.575119));
      sn->setOrientation(Quaternion(0.566785, 0.566785, -0.422794, 0.422794));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_post_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(13.470168, -0.351187, 37.567772));
      sn->setOrientation(Quaternion(0.085726, 0.085726, -0.701891, 0.701891));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(20.919453, -0.265414, 34.578644));
      sn->setOrientation(Quaternion(0.237372, 0.237372, -0.666074, 0.666074));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(17.475842, -0.265414, 36.617729));
      sn->setOrientation(Quaternion(0.173071, 0.173071, -0.685599, 0.685599));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(24.069561, 0.000000, 32.001068));
      sn->setOrientation(Quaternion(0.220774, 0.220774, -0.671758, 0.671758));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(27.313416, -0.093423, 29.335983));
      sn->setOrientation(Quaternion(0.263428, 0.263428, -0.656205, 0.656205));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(32.974308, 0.000000, 23.566395));
      sn->setOrientation(Quaternion(0.322158, 0.322158, -0.629455, 0.629455));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(30.268314, -0.047868, 26.776203));
      sn->setOrientation(Quaternion(0.281100, 0.281100, -0.648832, 0.648832));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-27.500526, -0.180137, 29.643944));
      sn->setOrientation(Quaternion(0.249139, 0.249139, 0.661763, -0.661763));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-24.203163, -0.180137, 32.242538));
      sn->setOrientation(Quaternion(0.206158, 0.206158, 0.676387, -0.676387));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-27.379517, -0.114907, 29.517776));
      sn->setOrientation(Quaternion(0.224486, 0.224486, 0.670527, -0.670527));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_post_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-34.823494, 0.065230, 19.852592));
      sn->setOrientation(Quaternion(0.318861, 0.318861, 0.631132, -0.631132));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_post_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-32.557194, 0.000000, 23.370750));
      sn->setOrientation(Quaternion(0.301570, 0.301570, 0.639575, -0.639575));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-34.975491, 0.000000, 19.938965));
      sn->setOrientation(Quaternion(0.341992, 0.341992, 0.618904, -0.618904));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(32.680359, -0.614254, -23.470367));
      sn->setOrientation(Quaternion(0.623861, 0.623861, -0.332862, 0.332863));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(25.649200, 0.000000, -30.953161));
      sn->setOrientation(Quaternion(0.651146, 0.651146, -0.275696, 0.275696));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(32.302067, 0.000000, -23.384628));
      sn->setOrientation(Quaternion(0.610790, 0.610790, -0.356280, 0.356280));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_post_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(22.572624, 0.000000, -33.809723));
      sn->setOrientation(Quaternion(0.667497, 0.667497, -0.233341, 0.233341));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(25.518394, 0.065230, -30.837187));
      sn->setOrientation(Quaternion(0.660900, 0.660900, -0.251418, 0.251418));
      sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      sn->attachObject(mSceneMgr->createEntity("prop_env_fence01_post_Cube.mesh"));

      // sn = root->createChildSceneNode(Vector3(9.378317, 0.000000, 17.578728));
      // sn->setOrientation(Quaternion(0.707107, 0.000000, -0.707107, -0.000000));
      // sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      // sn->attachObject(mSceneMgr->createEntity("Cube.004.mesh"));

      // sn = root->createChildSceneNode(Vector3(0.135112, 0.000000, -34.888699));
      // sn->setOrientation(Quaternion(0.707107, 0.000000, -0.707107, -0.000000));
      // sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      // sn->attachObject(mSceneMgr->createEntity("Cube.003.mesh"));

      // sn = root->createChildSceneNode(Vector3(14.612830, 0.000000, -16.448574));
      // sn->setOrientation(Quaternion(0.707107, 0.000000, -0.707107, -0.000000));
      // sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      // sn->attachObject(mSceneMgr->createEntity("Cube.002.mesh"));

      // sn = root->createChildSceneNode(Vector3(-14.495739, 0.000000, -0.073246));
      // sn->setOrientation(Quaternion(0.707107, 0.000000, -0.707107, -0.000000));
      // sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      // sn->attachObject(mSceneMgr->createEntity("Cube.001.mesh"));

      // sn = root->createChildSceneNode(Vector3(-27.832264, 0.000000, -25.838675));
      // sn->setOrientation(Quaternion(0.707107, 0.000000, -0.707107, -0.000000));
      // sn->setScale(Vector3(1.000000, 1.000000, 1.000000));
      // sn->attachObject(mSceneMgr->createEntity("Cube.mesh"));

      sn = root->createChildSceneNode(Vector3(-30.344238, 0.645851, -27.274321));
      sn->setOrientation(Quaternion(0.537595, 0.000000, -0.843203, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-28.743706, 0.645851, -29.023260));
      sn->setOrientation(Quaternion(0.912022, 0.000000, 0.410141, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-26.002647, 0.969553, -31.467382));
      sn->setOrientation(Quaternion(0.855950, 0.000000, 0.517058, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-34.043705, 0.969553, -29.847847));
      sn->setOrientation(Quaternion(0.855950, 0.000000, 0.517058, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-30.023220, 0.645851, -32.952003));
      sn->setOrientation(Quaternion(0.829102, 0.000000, -0.559097, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-31.574585, 0.645851, -33.629700));
      sn->setOrientation(Quaternion(0.646645, 0.000000, -0.762791, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-32.040108, 0.877496, -28.475981));
      sn->setOrientation(Quaternion(0.683691, 0.180465, -0.180465, -0.683690));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-34.791122, 0.645851, -28.374134));
      sn->setOrientation(Quaternion(0.875660, 0.000000, 0.482928, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-34.358524, 0.645851, -26.404715));
      sn->setOrientation(Quaternion(0.481994, 0.000000, 0.876175, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-26.242752, 0.645851, -35.087334));
      sn->setOrientation(Quaternion(0.481994, 0.000000, 0.876175, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-28.614735, 0.877496, -34.062645));
      sn->setOrientation(Quaternion(0.667573, 0.233124, -0.233124, -0.667572));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-33.564526, 0.645851, -32.286163));
      sn->setOrientation(Quaternion(0.955027, 0.000000, -0.296520, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-30.695358, 0.877496, -34.774559));
      sn->setOrientation(Quaternion(0.641820, -0.296761, 0.296761, -0.641820));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-31.424221, 0.645851, -31.051233));
      sn->setOrientation(Quaternion(0.912022, 0.000000, 0.410141, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-27.910648, 0.877496, -34.869812));
      sn->setOrientation(Quaternion(0.215705, -0.673403, 0.673403, -0.215705));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-33.024750, 0.645851, -29.302296));
      sn->setOrientation(Quaternion(0.537595, 0.000000, -0.843203, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-34.193382, 0.877496, -24.765427));
      sn->setOrientation(Quaternion(0.683691, 0.180465, -0.180465, -0.683690));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-32.835079, 0.645851, -25.711624));
      sn->setOrientation(Quaternion(0.966884, 0.000000, -0.255216, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-32.152645, 0.645851, -26.825640));
      sn->setOrientation(Quaternion(0.456921, 0.000000, -0.889507, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-20.390965, 0.877496, -35.621788));
      sn->setOrientation(Quaternion(0.160245, -0.688710, 0.688710, -0.160245));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-24.769737, 0.645851, -35.585899));
      sn->setOrientation(Quaternion(0.226620, 0.000000, 0.973983, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-31.109779, 0.645851, -28.002949));
      sn->setOrientation(Quaternion(0.875660, 0.000000, 0.482928, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-24.235920, 0.645851, -34.032036));
      sn->setOrientation(Quaternion(0.935225, 0.000000, 0.354053, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-26.716915, 0.877496, -33.891449));
      sn->setOrientation(Quaternion(0.380774, 0.595829, -0.595828, -0.380773));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-23.061092, 0.877496, -35.204063));
      sn->setOrientation(Quaternion(0.690117, 0.154077, -0.154077, -0.690116));
      sn->setScale(Vector3(1.476106, 1.476107, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-21.917561, 0.645851, -34.696541));
      sn->setOrientation(Quaternion(0.975972, 0.000000, -0.217897, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-25.661949, 0.645851, -33.336250));
      sn->setOrientation(Quaternion(0.975972, 0.000000, -0.217897, -0.000000));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-27.252483, 0.645851, -32.013145));
      sn->setOrientation(Quaternion(0.226620, 0.000000, 0.973983, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-30.355827, 0.645851, -29.163383));
      sn->setOrientation(Quaternion(0.226620, 0.000000, 0.973983, -0.000000));
      sn->setScale(Vector3(1.476107, 1.476106, 1.476107));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));

      sn = root->createChildSceneNode(Vector3(-29.470158, 0.877496, -29.968981));
      sn->setOrientation(Quaternion(0.160245, -0.688710, 0.688710, -0.160245));
      sn->setScale(Vector3(1.476106, 1.476106, 1.476106));
      sn->attachObject(mSceneMgr->createEntity("crate01.mesh"));


    }

};
