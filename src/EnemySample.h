#ifndef __Enemy_H__
#define __Enemy_H__


#include <Ogre.h>
#include <stdio.h>
#include <iostream>


using namespace Ogre;
using namespace OgreBites;


class EnemySample
{
private:

  enum AnimID
  {
      ANIM_IDLE,
      ANIM_WALK,
      NUM_ANIMS,
      ANIM_NONE = NUM_ANIMS,
  };


public:

    EnemySample(SceneManager* sm, Vector3 pos, String mesh, String anim, Real scale = 1.0)
    {
      mSceneManager = sm;
      m_position = pos;
      setup(mesh, anim, scale);
    }

    ~EnemySample()
    {
      mSceneManager->destroyEntity(mBodyEnt);
      mSceneManager->destroySceneNode(mBodyNode);
    }


    const Ogre::Vector3 & getPosition() { return mBodyNode->getPosition(); }


    void setup(String mesh, String anim, Real scale)
    {
      // BODY
      mBodyNode = mSceneManager->getRootSceneNode()->createChildSceneNode(m_position);
      mBodyNode->setScale(Vector3(scale));
      //mBodyEnt = mSceneManager->createEntity("EnemyBody", "Knight.mesh");
      mBodyEnt = mSceneManager->createEntity(mesh);

      mBodyNode->attachObject(mBodyEnt);

      // ANIMATIONS

      mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
      String animNames[NUM_ANIMS] =
      {anim, anim};

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


    void update(Real deltaTime)
    {
        // Update Animation

        Real baseAnimSpeed = 1;
        Real topAnimSpeed = 1;

        mTimer += deltaTime;

        // increment the current base and top animation times
        if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);

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
  Entity* mBodyEnt;
  AnimationState* mAnims[NUM_ANIMS];    // master animation list
  bool mFadingIn[NUM_ANIMS];            // which animations are fading in
  bool mFadingOut[NUM_ANIMS];           // which animations are fading out
  Real mTimer = 0;                // general timer to see how long animations have been playing
  AnimID mBaseAnimID = ANIM_IDLE;                   // current base (full- or lower-body) animation
  Vector3 m_position;
};

#endif
