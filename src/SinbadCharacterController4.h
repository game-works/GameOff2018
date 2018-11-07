#ifndef __Sinbad_H__
#define __Sinbad_H__

#include "Ogre.h"
#include "OgreInput.h"
#include "Animation.hpp"
#include <stdio.h>
#include <iostream>

using namespace Ogre;
using namespace OgreBites;

#define CHAR_HEIGHT 5
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED 17           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second


class AnimeCharacterController
{
private:

    // all the animations our character has, and a null ID
    // some of these affect separate body parts and will be blended together
    enum AnimID
    {
        ANIM_IDLE_L,
        ANIM_IDLE_R,
        ANIM_PICK,
        ANIM_JUMP,
        ANIM_WALK,
        NUM_ANIMS,
        ANIM_NONE = NUM_ANIMS,
    };

public:

    AnimeCharacterController(Camera* cam) :
      mBaseAnimID(ANIM_NONE),
      mTopAnimID(ANIM_NONE)
    {
        setupBody(cam->getSceneManager());
        setupCamera(cam);
        setupAnimations();
    }


    void addTime(Real deltaTime)
    {
        updateBody(deltaTime);
        updateAnimations(deltaTime);
        updateCamera(deltaTime);
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

        else if (key == SDLK_SPACE)
        {
            // jump if on ground
            setBaseAnimation(ANIM_JUMP, true);
            mTimer = 0;
        }

        if (!mKeyDirection.isZeroLength())
        {
            // start running if not already moving and the player wants to move
            setBaseAnimation(ANIM_WALK, true);
            //if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
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
            setBaseAnimation(ANIM_NONE);
            //if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
        }
    }


    void injectMouseMove(const MouseMotionEvent& evt)
    {
        // update camera goal based on mouse movement
        updateCameraGoal(-0.05f * evt.xrel, -0.05f * evt.yrel, 0);
    }


    void injectMouseWheel(const MouseWheelEvent& evt)
    {
        // update camera goal based on mouse movement
        updateCameraGoal(0, 0, -0.05f * evt.y);
    }


    void injectMouseDown(const MouseButtonEvent& evt)
    {
        // if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
        // {
        //     // if swords are out, and character's not doing something weird, then SLICE!
        //     if (evt.button == BUTTON_LEFT) setTopAnimation(ANIM_SLICE_VERTICAL, true);
        //     else if (evt.button == BUTTON_RIGHT) setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
        //     mTimer = 0;
        // }
    }


private:


    void setupBody(SceneManager* sceneMgr)
    {
        // create main model
        mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
        mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");

        mBodyNode->attachObject(mBodyEnt);

        mKeyDirection = Vector3::ZERO;
        mVerticalVelocity = 0;
    }


    void setupAnimations()
    {
        // this is very important due to the nature of the exported animations
        mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

        String animNames[NUM_ANIMS] =
        {"IdleBase", "IdleBase", "IdleBase", "JumpLoop", "RunBase"};

        // populate our animation list
        for (int i = 0; i < NUM_ANIMS; i++)
        {
            mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
            // Check existence of animation?
            mAnims[i]->setLoop(true);
            mFadingIn[i] = false;
            mFadingOut[i] = false;
        }

        // start off in the idle state (top and bottom together)
        setBaseAnimation(ANIM_IDLE_L);
        //setTopAnimation(ANIM_IDLE_L);

        // relax the hands since we're not holding anything
        //mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

        mSwordsDrawn = false;
    }


    void setupCamera(Camera* cam)
    {
        // create a pivot at roughly the character's shoulder
        mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        // this is where the camera should be soon, and it spins around the pivot
        mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
        // this is where the camera actually is
        mCameraNode = cam->getParentSceneNode();
        mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

        mCameraPivot->setFixedYawAxis(true);
        mCameraGoal->setFixedYawAxis(true);
        mCameraNode->setFixedYawAxis(true);

        // our model is quite small, so reduce the clipping planes
        cam->setNearClipDistance(0.1);
        cam->setFarClipDistance(1000);

        mPivotPitch = 0;
    }


    void updateBody(Real deltaTime)
    {
        mGoalDirection = Vector3::ZERO;   // we will calculate this

        if (mKeyDirection != Vector3::ZERO)
        {
            // calculate actually goal direction in world based on player's key directions
            mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
            mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
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
            mBodyNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),
                Node::TS_LOCAL);
        }
    }


    void updateAnimations(Real deltaTime)
    {
      Real baseAnimSpeed = 1;
      Real topAnimSpeed = 1;

      mTimer += deltaTime;

      if (mBaseAnimID == ANIM_JUMP)
      {
          if (mTimer == 0)
          {
              mVerticalVelocity = JUMP_ACCEL;
          }
      }

      // increment the current base and top animation times
      if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
      if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

      // apply smooth transitioning between our animations
      fadeAnimations(deltaTime);
    }


    void fadeAnimations(Real deltaTime)
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


    void updateCamera(Real deltaTime)
    {
        // place the camera pivot roughly at the character's shoulder
        mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);

        // move the camera smoothly to the goal
        //Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
        float dist = 42;
        float height = 9;
        Vector3 goalOffset = (mCameraPivot->getPosition() + Vector3::UNIT_Z * dist  + Vector3::UNIT_Y * height) - mCameraNode->getPosition();
        mCameraNode->translate(goalOffset * deltaTime * 9.0f);

        // always look at the pivot
        mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_PARENT);
    }


    void updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom)
    {
        // mCameraPivot->yaw(Degree(deltaYaw), Node::TS_PARENT);
        //
        // // bound the pitch
        // if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
        //     !(mPivotPitch + deltaPitch < -60 && deltaPitch < 0))
        // {
        //     mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
        //     mPivotPitch += deltaPitch;
        // }
        //
        // Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
        // Real distChange = deltaZoom * dist;
        //
        // // bound the zoom
        // if (!(dist + distChange < 8 && distChange < 0) &&
        //     !(dist + distChange > 25 && distChange > 0))
        // {
        //     mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
        // }
    }


    void setBaseAnimation(AnimID id, bool reset = false)
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


    SceneNode* mBodyNode;
    SceneNode* mCameraPivot;
    SceneNode* mCameraGoal;
    SceneNode* mCameraNode;
    Real mPivotPitch;
    Entity* mBodyEnt;
    AnimationState* mAnims[NUM_ANIMS];    // master animation list
    AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
    AnimID mTopAnimID;                    // current top (upper-body) animation
    bool mFadingIn[NUM_ANIMS];            // which animations are fading in
    bool mFadingOut[NUM_ANIMS];           // which animations are fading out
    bool mSwordsDrawn;
    Vector3 mKeyDirection;      // player's local intended direction based on WASD keys
    Vector3 mGoalDirection;     // actual intended direction in world-space
    Real mVerticalVelocity;     // for jumping
    Real mTimer;                // general timer to see how long animations have been playing
};

#endif
