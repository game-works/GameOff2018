#pragma once


#include "Agent.h"
#include "Ogre.h"


class Chicken : public Agent
{
  enum AnimID
  {
    ANIM_WALK_BACK,
    ANIM_WALK,
    ANIM_STAND_WALK,
    ANIM_PICK,
    ANIM_JUMP,
    ANIM_IDLE_LOOK_L,
    ANIM_IDLE_LOOK_R,
    ANIM_FLY,
    ANIM_CLOSE_FOOT_L,
    ANIM_CLOSE_FOOT_R,
    NUM_ANIMS,
    ANIM_NONE = NUM_ANIMS
  };

  const Ogre::Real RUN_SPEED = 7 * 15;
  const Real TURN_SPEED = 667.0f;      // character turning in degrees per second
  const Real ANIM_FADE_SPEED = 7.5f;   // animation crossfade speed in % of full weight per second


public:

  Chicken(SceneManager* sm, Vector3 startPos) :
    mAnimID(ANIM_NONE)
  {
    mSceneMgr = sm;

    setupBody("Hen.mesh");
    setPosition(startPos);

    setupAnimations(mBodyEnt, mAnims);

    // start off in the idle state
    setAnimation(ANIM_PICK, true);
  }


  const Ogre::Vector3& getPosition() { return mBodyNode->getPosition(); }


  void setPosition(const Ogre::Vector3& p)
  {
    mBodyNode->setPosition(p);
  }


  void addTime(Real deltaTime)
  {
    updateBody(deltaTime);

    updateAnimations(deltaTime, mAnims);
  }


  String classType() { return "Hen"; }


  private:


      void setupBody(String meshName)
      {
          // create main model
          mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
          mBodyEnt = mSceneMgr->createEntity(meshName);
          mBodyNode->attachObject(mBodyEnt);
      }


      void setupAnimations(Entity* entity, AnimationState* anims[NUM_ANIMS])
      {
          // this is very important due to the nature of the exported animations
          std::cout << "Chicken animations" << std::endl;
          std::cout << entity << std::endl;
          std::cout << entity->getSkeleton() << std::endl;
          entity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

          String animNames[NUM_ANIMS] =
          { "Walk.Back", "Walk", "StandWalk", "Pick", "Jump", "Idle.Look.L", "Idle.Look.R", "fly", "CloseFoot.L", "CloseFoot.R" };

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
      }


      void newAction()
      {
        mTimer = 0;

        if (mAction == 0)
        {
          if (rand() % 10 < 5) newIdleAction();
          else newWalkAction();
        }
        else if (mAction == 1)
        {
          if (rand() % 10 < 9) newIdleAction();
          else newWalkAction();
        }
      }


      void newIdleAction()
      {
        mAction = 0;

        int anim = rand() % 5;

        if (anim == 0)      setAnimation(ANIM_PICK, true);
        else if (anim == 1) setAnimation(ANIM_IDLE_LOOK_L, true);
        else if (anim == 2) setAnimation(ANIM_IDLE_LOOK_R, true);
        else if (anim == 3) setAnimation(ANIM_FLY, true);
        else if (anim == 4) setAnimation(ANIM_JUMP, true);

      }


      void newWalkAction()
      {
        mAction = 1;
        setAnimation(ANIM_WALK, true);

        // Set new random goal direction

        Real z1 = Math::RangeRandom(-1.0, 1.0);
        Real z2 = Math::RangeRandom(-1.0, 1.0);
        Real zd = Math::RangeRandom(14.0, 24.0) / (z1*z1 + z2*z2);
        z1 *= zd;
        z2 *= zd;

        mGoalDirection = getPosition() + Vector3(z1, 0, z2);
      }


      void updateBody(Real deltaTime)
      {
        if (mAction != 1) return;

        if (rand() % 180 == 0) newWalkAction();

        // End of walk to target
        if (mGoalDirection.squaredDistance(getPosition()) < 1)
        {
          newAction();
        }
        else // keep walking
        {
          Vector3 goalNormal = mGoalDirection - getPosition();
          goalNormal.normalise();

          Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(goalNormal);

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
              mAnims[mAnimID]->getWeight(),
              Node::TS_LOCAL);
        }
      }


      void updateAnimations(Real deltaTime, AnimationState* mAnims[NUM_ANIMS])
      {
        Real baseAnimSpeed = 1;
        Real topAnimSpeed = 1;

        mTimer += deltaTime;

        // End of animation... time for a new action
        if (mAction == 0 && mTimer >= mAnims[mAnimID]->getLength())
        {
          newAction();
        }

        // increment the current base and top animation times
        if (mAnimID != ANIM_NONE) mAnims[mAnimID]->addTime(deltaTime * baseAnimSpeed);

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


      void setAnimation(AnimID id, bool reset = false)
      {
        setAnimation(id, mAnims, reset);
      }


      void setAnimation(AnimID id, AnimationState* mAnims[NUM_ANIMS], bool reset)
      {
          if (mAnimID != ANIM_NONE)
          {
              // if we have an old animation, fade it out
              mFadingIn[mAnimID] = false;
              mFadingOut[mAnimID] = true;
          }

          mAnimID = id;

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
      Entity* mBodyEnt;

      AnimationState* mAnims[NUM_ANIMS];    // master animation list
      AnimID mAnimID;                   // current base (full- or lower-body) animation
      bool mFadingIn[NUM_ANIMS];            // which animations are fading in
      bool mFadingOut[NUM_ANIMS];           // which animations are fading out
      Vector3 mGoalDirection;     // actual intended direction in world-space
      Real mTimer;                // general timer to see how long animations have been playing

      SceneManager* mSceneMgr = 0;    // Added

      // 0 - idling
      // 1 - walking
      int mAction = 0;

};
