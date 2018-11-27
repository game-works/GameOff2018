#ifndef __Sinbad_H__
#define __Sinbad_H__

#include "Ogre.h"
#include "OgreInput.h"
#include <stdio.h>
#include <iostream>
#include "Tail.h"


using namespace Ogre;
using namespace OgreBites;


#define CHAR_HEIGHT (2.75 * 30)
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED (17 * 15)           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second


// WolfGirl with all animated controls from Sinbad
class AnimeCharacterController
{
private:

    // all the animations our character has, and a null ID
    // some of these affect separate body parts and will be blended together
    enum AnimID
    {
      ANIM_IDLE_BASE,
      ANIM_IDLE_TOP,
      ANIM_RUN_BASE,
      ANIM_RUN_TOP,
      ANIM_HANDS_CLOSED,
      ANIM_HANDS_RELAXED,
      ANIM_DRAW_SWORDS,
      ANIM_SLICE_VERTICAL,
      ANIM_SLICE_HORIZONTAL,
      ANIM_DANCE,
      ANIM_JUMP_START,
      ANIM_JUMP_LOOP,
      ANIM_JUMP_END,
      NUM_ANIMS,
      ANIM_NONE = NUM_ANIMS
    };

public:

    AnimeCharacterController(Camera* cam) :
      mBaseAnimID(ANIM_NONE),
      mTopAnimID(ANIM_NONE)
    {
      mSceneMgr = cam->getSceneManager();

      setupBody(cam->getSceneManager());

      setupAnimations(mHumanEnt, mHumanAnims);
      setupAnimations(mHumanClothesEnt, mHumanClothesAnims);
      setupAnimations(mWolfEnt, mWolfAnims);

      // start off in the idle state (top and bottom together)
      setBaseAnimation(ANIM_IDLE_BASE);
      setTopAnimation(ANIM_IDLE_TOP);
      mSwordsDrawn = false;

      m_tail = new Tail();
      m_tail->init(mSceneMgr, this);
    }


    const Ogre::Vector3& getPosition() { return mBodyNode->getPosition(); }


    void setPosition(const Ogre::Vector3& p)
    {
      mBodyNode->setPosition(p);
    }


    Ogre::Vector3 getHeadPosition()
    {
      Ogre::Bone* bone = mBodyEnt->getSkeleton()->getBone("Head");
      //Ogre::Bone* bone = mBodyEnt->getSkeleton()->getBone("Root");
      return mBodyNode->_getFullTransform() * bone->_getDerivedPosition();
    }


    Ogre::Vector3 getTailPosition()
    {
      Ogre::Bone* bone = mBodyEnt->getSkeleton()->getBone("Waist");
      //Ogre::Bone* bone = mBodyEnt->getSkeleton()->getBone("Root");
      return mBodyNode->_getFullTransform() * bone->_getDerivedPosition();
    }


    Ogre::Quaternion getTailOrientation()
    {
      //Ogre::Bone* bone = mBodyEnt->getSkeleton()->getBone("Waist");
      //return mBodyNode->_getFullTransform() * bone->_getDerivedOrientation();
      return mBodyNode->_getDerivedOrientation();
    }


    void hybridTransform()
    {
      mHybridTransform = !mHybridTransform;

      // Un-hold weapons and armour
      mBodyEnt->detachAllObjectsFromBone();

      // Switch the body node
      // mBodyNode->detachObject(mBodyEnt);
      // if (mBodyEnt == mHumanEnt) mBodyEnt = mWolfEnt;
      // else mBodyEnt = mHumanEnt;
      // mBodyNode->attachObject(mBodyEnt);

      mBodyNode->removeChild(mBodyNodeAtt);
      mBodyNodeAtt = mHybridTransform ? mWolfNodeAtt : mHumanNodeAtt;
      mBodyNode->addChild(mBodyNodeAtt);

      // Switch Body Entity
      mBodyEnt = mHybridTransform ? mWolfEnt : mHumanEnt;

      // Attach sheathed weapons... or drop them
      if (mSwordsDrawn)
      {

      }
      else
      {
        mBodyEnt->attachObjectToBone("Sheath.L", mWeapon1);
        mBodyEnt->attachObjectToBone("Sheath.R", mWeapon2);
      }

      // Dress human (softbodies,,,)
      if (mHybridTransform)
      {
        // dress Wolf
      }
      else
      {
        dressHuman();
      }
    }


    void dressHuman()
    {
      // Attach Hair
      mBodyEnt->attachObjectToBone(
        "Head",
        mHairEnt,
        Ogre::Quaternion(0, 0, 1, 0),
        Ogre::Vector3(0, 8, 0) //Ogre::Quaternion(0, 0, 1, 0) * -mBodyEnt->getSkeleton()->getBone("Head")->_getDerivedPosition()
      );

      // Armour pieces...
    }


    void dressWolf()
    {
      // Something?
    }


    void addTime(Real deltaTime)
    {
        updateBody(deltaTime);

        updateAnimations(deltaTime, mHumanAnims);
        updateAnimations(deltaTime, mHumanClothesAnims);
        updateAnimations(deltaTime, mWolfAnims);

        m_tail->update(deltaTime, this);
    }


    void injectKeyDown(const KeyboardEvent& evt)
    {
        Keycode key = evt.keysym.sym;
        if (key == 'q')
        {
            // take swords out (or put them back, since it's the same animation but reversed)
            mTimer = 0;
        }
        else if (key == 'e' && !mSwordsDrawn)
        {
        }

        // keep track of the player's intended direction
        else if (key == 'w') mKeyDirection.z = -1;
        else if (key == 'a') mKeyDirection.x = -1;
        else if (key == 's') mKeyDirection.z = 1;
        else if (key == 'd') mKeyDirection.x = 1;

        else if (key == 'z')
        {
            // HYBRID
            hybridTransform();
        }

        else if (key == SDLK_SPACE)
        {
            // // jump if on ground
            // setBaseAnimation(ANIM_JUMP_LOOP, true);
            // mTimer = 0;

            // Toggle Weapons
            setTopAnimation(ANIM_DRAW_SWORDS, true);
            mTimer = 0;
        }

        if (!mKeyDirection.isZeroLength())
        {
            // start running if not already moving and the player wants to move
            setBaseAnimation(ANIM_RUN_BASE, true);
            if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
        }
    }


    void injectKeyUp(const KeyboardEvent& evt)
    {
        Keycode key = evt.keysym.sym;
        // keep track of the player's intended direction
        if (key == 'w' && mKeyDirection.z == -1) mKeyDirection.z = 0;
        else if (key == 'a' && mKeyDirection.x == -1) mKeyDirection.x = 0;
        else if (key == 's' && mKeyDirection.z == 1) mKeyDirection.z = 0;
        else if (key == 'd' && mKeyDirection.x == 1) mKeyDirection.x = 0;

        if (mKeyDirection.isZeroLength())
        {
            // stop running if already moving and the player doesn't want to move
            setBaseAnimation(ANIM_IDLE_BASE);
            if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
        }
    }


    void injectMouseMove(const MouseMotionEvent& evt)
    {
    }


    void injectMouseWheel(const MouseWheelEvent& evt)
    {
    }


    void injectMouseDown(const MouseButtonEvent& evt)
    {
      static int c = 0;
        if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
        {
            c = (c + 1) % 3;

            // if swords are out, and character's not doing something weird, then SLICE!
            if (evt.button == BUTTON_LEFT)
            {
              if (c % 3 == 0) setTopAnimation(ANIM_SLICE_VERTICAL, true);
              else            setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
            }
            //else if (evt.button == BUTTON_RIGHT) setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
            mTimer = 0;
        }

    }


private:


    void setupBody(SceneManager* sceneMgr)
    {
        // create main model
        mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode();

        mHumanNodeAtt = mBodyNode->createChildSceneNode(Vector3::UNIT_Y * 0);
        //mHumanNodeAtt->setScale(Ogre::Vector3(85));
        //mHumanNodeAtt->translate(0, -107, 0);

        mWolfNodeAtt = mBodyNode->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
        mWolfNodeAtt->setScale(Ogre::Vector3(30));
        //mWolfNodeAtt->translate(0, -107, 0);

        mBodyNodeAtt = mHumanNodeAtt;
        mBodyNode->removeChild(mWolfNodeAtt);

        //SceneNode* sn = mBodyNode->createChildSceneNode();
        //sn->setScale(Vector3(6));
        //mBodyEnt = sceneMgr->createEntity("Girl.mesh");
        mHumanEnt = sceneMgr->createEntity("Girl.mesh");
        mHumanClothesEnt = sceneMgr->createEntity("Girl.Clothes.mesh");
        mWolfEnt = sceneMgr->createEntity("WereWolf.mesh");
        mBodyEnt = mHumanEnt;

        mHairEnt = sceneMgr->createEntity("hair.mesh");

        // Set up Human Node
        mHumanNodeAtt->attachObject(mHumanEnt);
        mHumanNodeAtt->attachObject(mHumanClothesEnt);

        // Set up Wolf Node
        mWolfNodeAtt->attachObject(mWolfEnt);

        mKeyDirection = Vector3::ZERO;
        mVerticalVelocity = 0;

        // Create Weapons
        // mWeapon1 = sceneMgr->createEntity("Sword_1.mesh");
        // mWeapon2 = sceneMgr->createEntity("Sword_2.mesh");
        // mWeapon1 = sceneMgr->createEntity("Axe_3.mesh");
        // mWeapon2 = sceneMgr->createEntity("Axe_4.mesh");
        mWeapon1 = sceneMgr->createEntity("Dagger.LoPoly.mesh");
        mWeapon2 = sceneMgr->createEntity("Dagger.LoPoly.mesh");
        // mWeapon1 = sceneMgr->createEntity("Sword.mesh");
        // mWeapon2 = sceneMgr->createEntity("Sword.mesh");

        // Sheathe weapons
        mBodyEnt->attachObjectToBone("Sheath.L", mWeapon1);
        mBodyEnt->attachObjectToBone("Sheath.R", mWeapon2);

        // // Helmet
        // mArmourHead = sceneMgr->createEntity("Barbute_lowpoly.mesh");
        // mBodyEnt->attachObjectToBone("Head", mArmourHead);
        //
        // // Chest Plate
        // mArmourChest = sceneMgr->createEntity("Breastplate_final_low.001.mesh");
        // mBodyEnt->attachObjectToBone("Chest", mArmourChest);

        dressHuman();
    }


    void setupAnimations(Entity* entity, AnimationState* anims[NUM_ANIMS])
    {
        // this is very important due to the nature of the exported animations
        std::cout << "setupAnimations" << std::endl;
        std::cout << entity << std::endl;
        std::cout << entity->getSkeleton() << std::endl;
        entity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

        String animNames[NUM_ANIMS] =
        {"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
        "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

        // populate our animation list
        for (int i = 0; i < NUM_ANIMS; i++)
        {
          std::cout << animNames[i] << std::endl;
          (anims)[i] = entity->getAnimationState(animNames[i]);
          std::cout << (anims)[i] << std::endl;
          // Check existence of animation?
          (anims)[i]->setLoop(true);
          mFadingIn[i] = false;
          mFadingOut[i] = false;
        }

        // relax the hands since we're not holding anything
        (anims)[ANIM_HANDS_RELAXED]->setEnabled(true);
    }


    void updateBody(Real deltaTime)
    {
        mGoalDirection = Vector3::ZERO;   // we will calculate this

        if (mKeyDirection != Vector3::ZERO)
        {
            // calculate actually goal direction in world based on player's key directions
            mGoalDirection += mKeyDirection.z * Ogre::Vector3::UNIT_Z;
            mGoalDirection += mKeyDirection.x * Ogre::Vector3::UNIT_X;
            mGoalDirection.y = 0;
            mGoalDirection.normalise();

            Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

            // calculate how much the character has to turn to face goal direction
            Real yawToGoal = toGoal.getYaw().valueDegrees();
            // this is how much the character CAN turn this frame
            Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
            // reduce "turnability" if we're in midair
            //if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

            // turn as much as we can, but not more than we need to
            if (yawToGoal < 0) yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
            else if (yawToGoal > 0) yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);

            mBodyNode->yaw(Degree(yawToGoal));

            // move in current body direction (not the goal direction)
            // HACK
            mBodyNode->translate(
                0,
                0,
                deltaTime * RUN_SPEED *
                mHumanAnims[mBaseAnimID]->getWeight(),
                Node::TS_LOCAL);
        }
    }


    void updateAnimations(Real deltaTime, AnimationState* mAnims[NUM_ANIMS])
    {
      Real baseAnimSpeed = 1;
      Real topAnimSpeed = 1;

      mTimer += deltaTime;

      //
      // Jump?
      //
      if (mBaseAnimID == ANIM_JUMP_LOOP)
      {
          if (mTimer == 0)
          {
              mVerticalVelocity = JUMP_ACCEL;
          }
      }

      //
      //  Toggle Swords
      //
      if (mTopAnimID == ANIM_DRAW_SWORDS)
      {
          // flip the draw swords animation if we need to put it back
          topAnimSpeed = mSwordsDrawn ? -1 : 1;

          // half-way through the animation is when the hand grasps the handles...
          if (mTimer >= mAnims[mTopAnimID]->getLength() / 2 &&
              mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2)
          {
              // so transfer the swords from the sheaths to the hands
              mBodyEnt->detachAllObjectsFromBone();
              mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mWeapon1);
              mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mWeapon2);
              // change the hand state to grab or let go
              mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
              mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

              // Hmmm... Re-apply armour items
              if (mHybridTransform) ;
              else dressHuman();
              // mBodyEnt->attachObjectToBone("Head", mArmourHead);
              // mBodyEnt->attachObjectToBone("Chest", mArmourChest);

              // toggle sword trails
              // if (mSwordsDrawn)
              // {
              //     mSwordTrail->setVisible(false);
              //     mSwordTrail->removeNode(mWeapon1->getParentNode());
            //     mSwordTrail->removeNode(mWeapon2->getParentNode());
              // }
              // else
              // {
              //     mSwordTrail->setVisible(true);
              //     mSwordTrail->addNode(mWeapon1->getParentNode());
              //     mSwordTrail->addNode(mWeapon2->getParentNode());
              // }
          }

          if (mTimer >= mAnims[mTopAnimID]->getLength())
          {
              // animation is finished, so return to what we were doing before
              if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
              else
              {
                  setTopAnimation(ANIM_RUN_TOP);
                  mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
              }
              mSwordsDrawn = !mSwordsDrawn;
          }
      }
      else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
      {
          if (mTimer >= mAnims[mTopAnimID]->getLength())
          {
              // animation is finished, so return to what we were doing before
              if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
              else
              {
                  setTopAnimation(ANIM_RUN_TOP);
                  mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
              }
          }

          // don't sway hips from side to side when slicing. that's just embarrassing.
          if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
      }

      // increment the current base and top animation times
      if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
      if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

      // apply smooth transitioning between our animations
      fadeAnimations(deltaTime, mAnims);
    }


    void fadeAnimations(Real deltaTime, AnimationState* mAnims[NUM_ANIMS])
    {
      for (int i = 0; i < NUM_ANIMS; i++)
      {
          if (mFadingIn[i])
          {
              // slowly fade this animation in until it has full weight
              Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
              mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
              if (newWeight >= 1) mFadingIn[i] = false;
          }
          else if (mFadingOut[i])
          {
              // slowly fade this animation out until it has no weight, and then disable it
              Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
              mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
              if (newWeight <= 0)
              {
                  mAnims[i]->setEnabled(false);
                  mFadingOut[i] = false;
              }
          }
      }
    }


    void setBaseAnimation(AnimID id, bool reset = false)
    {
      setBaseAnimation(id, mHumanAnims, reset);
      setBaseAnimation(id, mHumanClothesAnims, reset);
      setBaseAnimation(id, mWolfAnims, reset);
    }


    void setBaseAnimation(AnimID id, AnimationState* mAnims[NUM_ANIMS], bool reset)
    {
        if (mBaseAnimID != ANIM_NONE)
        {
            // if we have an old animation, fade it out
            mFadingIn[mBaseAnimID] = false;
            mFadingOut[mBaseAnimID] = true;
        }

        mBaseAnimID = id;

        if (id != ANIM_NONE)
        {
            // if we have a new animation, enable it and fade it in
            mAnims[id]->setEnabled(true);
            mAnims[id]->setWeight(0);
            mFadingOut[id] = false;
            mFadingIn[id] = true;
            if (reset) mAnims[id]->setTimePosition(0);
        }
    }


    void setTopAnimation(AnimID id, bool reset = false)
    {
      setTopAnimation(id, mHumanAnims, reset);
      setTopAnimation(id, mHumanClothesAnims, reset);
      setTopAnimation(id, mWolfAnims, reset);
    }


    void setTopAnimation(AnimID id, AnimationState* mAnims[NUM_ANIMS], bool reset)
    {
        if (mTopAnimID != ANIM_NONE)
        {
            // if we have an old animation, fade it out
            mFadingIn[mTopAnimID] = false;
            mFadingOut[mTopAnimID] = true;
        }

        mTopAnimID = id;

        if (id != ANIM_NONE)
        {
            // if we have a new animation, enable it and fade it in
            mAnims[id]->setEnabled(true);
            mAnims[id]->setWeight(0);
            mFadingOut[id] = false;
            mFadingIn[id] = true;
            if (reset) mAnims[id]->setTimePosition(0);
        }
    }


    SceneNode* mBodyNode;
    SceneNode* mBodyNodeAtt;
    SceneNode* mHumanNodeAtt;
    SceneNode* mWolfNodeAtt;
    Entity* mBodyEnt;
    Entity* mHumanEnt;
    Entity* mHumanClothesEnt;
    Entity* mWolfEnt;

    Entity* mHairEnt;


    Entity* mWeapon1;
    Entity* mWeapon2;
    Entity* mArmourHead;
    Entity* mArmourChest;

    AnimationState* mHumanAnims[NUM_ANIMS];    // master animation list
    AnimationState* mHumanClothesAnims[NUM_ANIMS];    // master animation list
    AnimationState* mWolfAnims[NUM_ANIMS];    // master animation list
    AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
    AnimID mTopAnimID;                    // current top (upper-body) animation
    bool mFadingIn[NUM_ANIMS];            // which animations are fading in
    bool mFadingOut[NUM_ANIMS];           // which animations are fading out
    bool mSwordsDrawn;
    Vector3 mKeyDirection;      // player's local intended direction based on WASD keys
    Vector3 mGoalDirection;     // actual intended direction in world-space
    Real mVerticalVelocity;     // for jumping
    Real mTimer;                // general timer to see how long animations have been playing

    SceneManager* mSceneMgr = 0;    // Added

    bool mHybridTransform = false;

    Tail* m_tail;

public:
    btRigidBody* mCharacterBody;
};

#endif
