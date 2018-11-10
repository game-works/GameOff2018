#ifndef __Character_H__
#define __Character_H__


#include "SdkSample.h"      // ??
#include <vector>

// Player Character
#include "SinbadCharacterController4.h"

// Active Items
#include "Gem.h"
#include "EnemySample.h"
#include "Projectile.h"


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

      Real dt = evt.timeSinceLastFrame;
      updateActiveItems(dt);
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

        // Action
        //if (mSceneMgr)
        m_projectiles.push_back(
          new Projectile(
            mCamera->getSceneManager(),
            mChara->getPosition(),
            cursorPos )
        );

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
            // SceneManager* sm = mCamera->getSceneManager();
            // m_gems.push_back(new Gem(sm, Ogre::Vector3(15, 15, 0)));
            // m_enemies.push_back(new EnemySample(sm));
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
        i->update(dt);
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
      // for (auto i = m_enemies.begin(); i != m_enemies.end();)
      // {
      //   Real d2 = (*i)->getPosition().squaredDistance(mChara->getPosition());
      //   if (d2 < 25)
      //   {
      //     // Kill enemy, leave a gem
      //     SceneManager* sm = mCamera->getSceneManager();
      //     m_gems.push_back(new Gem(sm, (*i)->getPosition()));
      //     delete *i;
      //
      //     i = m_enemies.erase(i);
      //   }
      //   else
      //   {
      //     ++i;
      //   }
      // }
      for (auto i = m_enemies.begin(); i != m_enemies.end();)
      {
        for (auto j = m_projectiles.begin(); j != m_projectiles.end();)
        {
          Real d2 = (*i)->getPosition().squaredDistance((*j)->getPosition());
          if (d2 < 25)
          {
            // Kill enemy, leave a gem
            SceneManager* sm = mCamera->getSceneManager();
            m_gems.push_back(new Gem(sm, (*i)->getPosition()));
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
      if (m_enemies.size() < 4)
      {
        Real t = Ogre::Math::RangeRandom(0.0, 360.0);
        Real r = 45.0;
        Real x = r * Ogre::Math::Cos(Ogre::Degree(t)) + mChara->getPosition().x;
        Real y = r * Ogre::Math::Sin(Ogre::Degree(t)) + mChara->getPosition().z;

        m_enemies.push_back(
          new EnemySample(
            mCamera->getSceneManager(),
            Ogre::Vector3(x, 5.5, y))
        );
      }

    }



    //
    //  Elements of the scene
    //

    AnimeCharacterController* mChara;
    std::vector<EnemySample*> m_enemies;
    std::vector<Projectile*> m_projectiles;
    std::vector<Gem*> m_gems;
    Vector3 cursorPos = Vector3::ZERO;

    float m_mouseX = 0;
    float m_mouseY = 0;
};

#endif
