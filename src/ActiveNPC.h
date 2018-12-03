#ifndef __ActiveNPC1_H__
#define __ActiveNPC1_H__

#include "Ogre.h"
#include "OgreInput.h"
#include <stdio.h>
#include <iostream>
#include "ActiveItem.h"
#include "ConvoManager.h"


using namespace Ogre;
using namespace OgreBites;


// WolfGirl with all animated controls from Sinbad
class ActiveNPC1 : public ActiveItem
{
private:

    // all the animations our character has, and a null ID
    // some of these affect separate body parts and will be blended together
    enum AnimID
    {
      ANIM_IDLE,
      ANIM_WALK,
      ANIM_ATTACK,
      ANIM_DIE,
      ANIM_HIT,
      NUM_ANIMS,
      ANIM_NONE = NUM_ANIMS
    };

    const Real ANIM_FADE_SPEED = 7.5f;   // animation crossfade speed in % of full weight per second


public:

  Dialog m_convo;
  bool m_hasConvo = true;

    ActiveNPC1(SceneManager* sm, SceneNode* root, Vector3 startPos, String meshName) :
      mAnimID(ANIM_NONE)
    {
      mSceneMgr = sm;

      setupBody(meshName, root);
      setPosition(startPos);

      setupAnimations(mBodyEnt, mAnims);

      // start off in the idle state
      setAnimation(ANIM_IDLE);
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

    String classType() { return "NPC"; }


private:


    void setupBody(String meshName, SceneNode* root)
    {
        // create main model
        mBodyNode = root->createChildSceneNode();
        mBodyNode->setScale(Vector3(8.5));
        mBodyEnt = mSceneMgr->createEntity(meshName);
        mBodyNode->attachObject(mBodyEnt);
    }


    void setupAnimations(Entity* entity, AnimationState* anims[NUM_ANIMS])
    {
        // this is very important due to the nature of the exported animations
        std::cout << "setupAnimations" << std::endl;
        std::cout << entity << std::endl;
        std::cout << entity->getSkeleton() << std::endl;
        entity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

        String animNames[NUM_ANIMS] =
        { "Idle", "Walk", "Attack", "Die", "Hit" };

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


    void updateBody(Real deltaTime)
    {
    }


    void updateAnimations(Real deltaTime, AnimationState* mAnims[NUM_ANIMS])
    {
      Real baseAnimSpeed = 1;
      Real topAnimSpeed = 1;

      mTimer += deltaTime;

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

};

#endif
