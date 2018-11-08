#ifndef __Character_H__
#define __Character_H__


#include "SdkSample.h"      // ??
#include "SinbadCharacterController4.h"
#include <vector>
#include "Gem.h"
#include "EnemySample.h"


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
        i->update(dt);
      }

      // Check proximity of active items
      // ... Gems
      for (auto i = m_gems.begin(); i != m_gems.end();)
      {
        Real d2 = (*i)->getPosition().squaredDistance(mChara->getPosition());
        if (d2 < 15)
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
        if (d2 < 25)
        {
          // Kill enemy, leave a gem
          SceneManager* sm = mCamera->getSceneManager();
          m_gems.push_back(new Gem(sm, (*i)->getPosition()));
          delete *i;

          i = m_enemies.erase(i);
        }
        else
        {
          ++i;
        }
      }

      // Check Enemy Spawn potential
      Real x = Ogre::Math::RangeRandom(-5.0, 5.0);


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
        return SdkSample::mouseMoved(evt);
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
        return SdkSample::mousePressed(evt);
    }


protected:


    void setupContent()
    {
        // set background and some fog
        mViewport->setBackgroundColour(ColourValue(1.0f, 1.0f, 0.8f));
        mSceneMgr->setFog(
          Ogre::FOG_LINEAR,
          ColourValue(1.0f, 1.0f, 0.8f),
          0,    // expDensity
          35,   // linear start
          160); // linear end

        // set shadow properties
        //mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
        mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
        mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
        mSceneMgr->setShadowTextureSize(1024);
        mSceneMgr->setShadowTextureCount(1);

        // disable default camera control so the character can do its own
        mCameraMan->setStyle(CS_MANUAL);

        // use a small amount of ambient lighting
        mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

        // add a bright light above the scene
        Light* light = mSceneMgr->createLight();
        //light->setType(Light::LT_POINT);
        // mSceneMgr->getRootSceneNode()
        //     ->createChildSceneNode(Vector3(-10, 40, 20))
        //     ->attachObject(light);
        light->setType(Light::LT_DIRECTIONAL);
        SceneNode* lsn =
        mSceneMgr->getRootSceneNode()
            ->createChildSceneNode(Vector3(0, 40, 0));
        lsn->attachObject(light);
        lsn->yaw(Degree(8));
        lsn->pitch(Degree(-59));

        light->setSpecularColour(ColourValue::White);

        // create a floor mesh resource
        MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Plane(Vector3::UNIT_Y, 0),
            320, 320, // Size
            10, 10,     // Divs
            true, 1,
            20, 20,   // UV
            Vector3::UNIT_Z);

        // create a floor entity, give it a material, and place it at the origin
        Entity* floor = mSceneMgr->createEntity("Floor", "floor");
        floor->setMaterialName("Examples/Rockwall");
        floor->setCastShadows(false);
        mSceneMgr->getRootSceneNode()->attachObject(floor);

        //      LogManager::getSingleton().logMessage("creating sinbad");
        // create our character controller
        mChara = new AnimeCharacterController(mCamera);

        //      LogManager::getSingleton().logMessage("toggling stats");
        mTrayMgr->toggleAdvancedFrameStats();

        //      LogManager::getSingleton().logMessage("creating panel");
        StringVector items;
        items.push_back("Help");
        ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
        help->setParamValue("Help", "H / F1");

        //      LogManager::getSingleton().logMessage("all done");


            //
            // Game Setup
            //
            SceneManager* sm = mCamera->getSceneManager();
            m_gems.push_back(new Gem(sm, Ogre::Vector3(15, 15, 0)));
            m_enemies.push_back(new EnemySample(sm));
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


    //
    //  Elements of the scene
    //

    AnimeCharacterController* mChara;
    std::vector<EnemySample*> m_enemies;
    std::vector<Gem*> m_gems;
};

#endif
