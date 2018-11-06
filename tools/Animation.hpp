#include <OgreStableHeaders.h>
#include <OgreSkeleton.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreResourceGroupManager.h>


/// Takes a animation and source skeleton and adds that animation to the target skeleton and returns it
Ogre::Animation* retargetAnimation(
  const Ogre::Animation& animation,
  const Ogre::Skeleton& source,
  Ogre::Skeleton* target);


/// Takes a string with a skeleton name and an optional string with animation name, and retargets it to the target skeleton. If name is empty string, it will load the first animation (allowing entire skeleton packages to become "virtual animations"
Ogre::Animation* createAnimation(
  Ogre::Skeleton* target,
  Ogre::SkeletonManager* creator,
  const Ogre::String& skeletonname,
  const Ogre::String& groupname = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
  const Ogre::String& animationName = Ogre::String());
