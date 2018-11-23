#pragma once



#include "SdkSample.h"      // ??
#include "BulletScene.h"
#include "btOgreSoftBody.hpp"
#include "SinbadCharacterController6.h"
// TODO Active Items
#include <stdlib.h>     /* srand, rand */


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



public:

  //
  //  Frame
  //
  bool frameEnded(const FrameEvent& evt)
  {
    // Character handling
    mCharacter->addTime(evt.timeSinceLastFrame);

    //mCameraMan->getTarget()->setPosition(mCharacter->getPosition());
    float dist = 320 * 1.5;
    float height = 420 * 1.5;
    mCamera->getParentNode()->setPosition(
      mCharacter->getPosition() +
      Vector3::UNIT_Z * dist  + Vector3::UNIT_Y * height
    );

    mCamera->getParentSceneNode()->lookAt(mCharacter->getPosition(), Node::TS_PARENT);

    // SDK...
    return SdkSample::frameEnded(evt);
  }


  //
  //  Input
  //
  bool keyPressed(const KeyboardEvent& evt)
  {
    // relay input events to character controller
    mCharacter->injectKeyDown(evt);
    return SdkSample::keyPressed(evt);
  }


  bool keyReleased(const KeyboardEvent& evt)
  {
    // relay input events to character controller
    mCharacter->injectKeyUp(evt);
    return SdkSample::keyReleased(evt);
  }



  //
  // Setup
  //
  // Just override the mandatory create scene method
    void setupContent(void)
    {
      mTrayMgr->hideLogo();
      mTrayMgr->hideFrameStats();

        // do this first so we generate edge lists
        mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);

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

        // Another node?
        SceneNode* node;
        node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

        createSceneItems();

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

        //setupGUI();
        //setDefaultDepthShadowParams();

        //handleShadowTypeChanged();
        mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
        configureLights();
        handleProjectionChanged();

        //...
        mCharacter = new AnimeCharacterController(mCamera);

    }


    void createSceneItems()
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



    void configureLights()
    {
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
        //mCamera->setFarClipDistance(1000);
        //mCamera->setNearClipDistance(0.1);

        mCameraMan->setStyle(CS_MANUAL);
        //mCameraMan->getTarget()->setPosition(Ogre::Vector3(0, -107, 0));
        // mCameraMan->getTarget()->setPosition(Ogre::Vector3(0, 0, 0));
        // mCameraMan->setYawPitchDist(Degree(0), Degree(45), 400);

        //mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
    }


    virtual void cleanupContent()
    {
        MeshManager::getSingleton().remove("Myplane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }






};
