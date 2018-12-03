#ifndef __EnemySlime_H__
#define __EnemySlime_H__


#include <Ogre.h>
#include <stdio.h>
#include <iostream>
#include "Enemy.h"


using namespace Ogre;
using namespace OgreBites;


class EnemySlime : public Enemy
{
private:

  enum AnimID
  {
      ANIM_IDLE,
      ANIM_WALK,
      ANIM_ATTACK,
      ANIM_DEATH,
      NUM_ANIMS,
      ANIM_NONE = NUM_ANIMS,
  };

  const Real RUN_SPEED = (7 * 15);           // character running speed in units per second
  const Real TURN_SPEED = 500.0f;      // character turning in degrees per second
  const Real ANIM_FADE_SPEED = 7.5f;   // animation crossfade speed in % of full weight per second

  int mHP = 2;
  Real mImmuneTimer = 0;

public:

    EnemySlime(SceneManager* sm, SceneNode* root, Vector3 pos)
    {
      mSceneManager = sm;
      mRoot = root;
      m_position = pos;
      setup();
    }


    ~EnemySlime()
    {
      mSceneManager->destroyEntity(mBodyEnt);
      mSceneManager->destroySceneNode(mBodyNode);
    }


    const Ogre::Vector3 & getPosition() { return mBodyNode->getPosition(); }


    String classType() { return "Slime"; }


    void hit()
    {
      if (StatsManager::hybrid)
      {
        mRemove = true;
        return;
      }

      if (mImmuneTimer <= 0)
      {
        --mHP;
        if (mHP == 0) mRemove = true;
        mImmuneTimer = 0.3;

        mTimer = 0;
        setBaseAnimation(ANIM_DEATH, true);
      }
    }


    void hitRange()
    {
        mRemove = true;
    }


    void setup()
    {
      // BODY
      mBodyNode = mRoot->createChildSceneNode(m_position);
      mBodyNode->setScale(Vector3(16));
      //mBodyEnt = mSceneManager->createEntity("EnemyBody", "Knight.mesh");
      mBodyEnt = mSceneManager->createEntity("Slime.mesh");

      SceneNode* att = mBodyNode->createChildSceneNode();
      att->yaw(Degree(-90));
      att->attachObject(mBodyEnt);

      // ANIMATIONS

      mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
      String animNames[NUM_ANIMS] =
      {
        "Slime_Idle",
        "Slime_Walk",
        "Slime_Attack",
        "Slime_Death",
      };

      for (int i = 0; i < NUM_ANIMS; i++)
      {
          mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
          // Check existence of animation?
          mAnims[i]->setLoop(true);
          mFadingIn[i] = false;
          mFadingOut[i] = false;
      }

      // start off in the idle state
      std::cout << "Setting enemy anim to idle" << std::endl;

      setBaseAnimation(ANIM_IDLE);

      std::cout << "Ready" << std::endl;

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


    void addTime(Real deltaTime)
    {
      if (mImmuneTimer > 0) mImmuneTimer -= deltaTime;

      Vector3 characterTarget = Continuity::characterPosition;
      characterTarget.y = 0;

      // Logic
      Real range = getPosition().squaredDistance(characterTarget);

      if (mBaseAnimID != ANIM_DEATH && mBaseAnimID != ANIM_ATTACK)
      {
        if (range < 35 * 35)
        {
          mTimer = 0;
          setBaseAnimation(ANIM_ATTACK, true);
        }
        else if (range < 400 * 400)
        {
          setBaseAnimation(ANIM_WALK);
        }
        else if (range > 1000 * 1000)
        {
          mRemove = true;
        }
        else
        {
          setBaseAnimation(ANIM_IDLE);
        }
      }

      // Walking
      if (mBaseAnimID == ANIM_WALK)
      {
        Vector3 goalNormal = characterTarget - getPosition();
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
            deltaTime * RUN_SPEED,
            //* mAnims[mBaseAnimID]->getWeight(),
            Node::TS_LOCAL);
      }


      // Update Animation
      Real baseAnimSpeed = 1;
      Real topAnimSpeed = 1;

      mTimer += deltaTime;

      // increment the current base and top animation times
      if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);

      // ...
      if (mBaseAnimID == ANIM_DEATH && mTimer >= mAnims[ANIM_DEATH]->getLength())
      {
        setBaseAnimation(ANIM_IDLE);
      }

      if (mTimer - deltaTime < 0.5 * mAnims[ANIM_ATTACK]->getLength() &&
          mTimer >= 0.5 * mAnims[ANIM_ATTACK]->getLength())
      {
        mDamaging = true;
      }

      if (mBaseAnimID == ANIM_ATTACK && mTimer >= mAnims[ANIM_ATTACK]->getLength())
      {
        setBaseAnimation(ANIM_IDLE);
      }

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


protected:

  SceneManager* mSceneManager;
  SceneNode* mBodyNode;
  SceneNode* mRoot;
  Entity* mBodyEnt;
  AnimationState* mAnims[NUM_ANIMS];    // master animation list
  bool mFadingIn[NUM_ANIMS];            // which animations are fading in
  bool mFadingOut[NUM_ANIMS];           // which animations are fading out
  Real mTimer = 0;                // general timer to see how long animations have been playing
  AnimID mBaseAnimID = ANIM_IDLE;                   // current base (full- or lower-body) animation
  Vector3 m_position;
};

#endif
