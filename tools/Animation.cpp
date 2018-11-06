/// this file was authored by Daniel Horn and resides in the public domain. Feel free to do anything you like to this. It comes with no explicit or implicit warranty. Feel free to reattribute it or whatever you like
#include "Animation.hpp"
#include <OgreSkeletonManager.h>
#include <OgreSkeletonInstance.h>
#include <OgreBone.h>
#include <OgreKeyFrame.h> // new
#include <OgreSkeleton.h> // new
using namespace Ogre;
///replace a zero input component with corresponding fallback
static void MakeNonzeroVector(Vector3&input,const Vector3&fallback) {
  if (input.x==0) input.x=fallback.x;
  if (input.y==0) input.y=fallback.y;
  if (input.z==0) input.z=fallback.z;
}
///divide by the count of nonzero values in each component---make it 1/1 if either is 0
static void AverageVectorComponents(Vector3&input,Vector3 count) {
    MakeNonzeroVector(count,Vector3(1,1,1));
    MakeNonzeroVector(input,Vector3(1,1,1));
    input.x/=count.x;
    input.y/=count.y;
    input.z/=count.z;
}
//increment the average count only if nonzero
static void IncrementCount(Vector3&average,const Vector3&increment, const Vector3&nonzero) {
  if (nonzero.x) average.x+=increment.x;
  if (nonzero.y) average.y+=increment.y;
  if (nonzero.z) average.z+=increment.z;
}
///figures the ratio of the source to target bones as long as they are not centered
static void computeRescale(Bone *targetbone, const Bone *sourcebone, Vector3 *outscale, Vector3*numerator, Vector3 *denominator) {
  Vector3 sourcepos=sourcebone->getPosition();
  Vector3 targetpos=targetbone->getPosition();
  if (sourcepos.x!=0&&targetpos.x!=0) {
    outscale->x=targetpos.x/sourcepos.x;
    numerator->x=targetpos.x;
    denominator->x=sourcepos.x;
  }
  if (sourcepos.y!=0&&targetpos.x!=0) {
    outscale->y=targetpos.y/sourcepos.y;
    numerator->y=targetpos.y;
    denominator->y=sourcepos.y;
  }
  if (sourcepos.z!=0&&targetpos.x!=0) {
    outscale->z=targetpos.z/sourcepos.z;
    numerator->z=targetpos.z;
    denominator->z=sourcepos.z;
  }
}
///scales each component of a by b's corresponding component
static Vector3 Vec3Mul(const Vector3&a, const Vector3&b) {
  return Vector3(a.x*b.x,a.y*b.y,a.z*b.z);
}
NodeAnimationTrack* retargetBoneTrack(unsigned int index, Animation *targetanimation, Ogre::Skeleton*target, const Ogre::Skeleton&source,const Vector3&scale, const NodeAnimationTrack*const animationTrack) {
  //create a track for this particular bone
  NodeAnimationTrack* newtrack=targetanimation->createNodeTrack(index);
  //make sure the animation settings are the same
  newtrack->setUseShortestRotationPath(animationTrack->getUseShortestRotationPath());
  //go through each frame
  unsigned int numframes=animationTrack->getNumKeyFrames();
  for (unsigned int i=0;i<numframes;++i) {
    TransformKeyFrame*sourceframe=animationTrack->getNodeKeyFrame(i);
    TransformKeyFrame*targetframe=newtrack->createNodeKeyFrame(sourceframe->getTime());
    //scale the transformations by the scale passed in.
    targetframe->setTranslate(Vec3Mul(sourceframe->getTranslate(),scale));
    //everything else is passed in vanilla (at least this is how 3ds max seems to do it when importing .bip files
    targetframe->setScale(sourceframe->getScale());//Vec3Mul(sourceframe->getScale(),scale));
    targetframe->setRotation(sourceframe->getRotation());
  }
  return newtrack;
}

///this function takes as input a source skeleton and source animation and a skeleton that needs the animation applied to it
Ogre::Animation * retargetAnimation(const Ogre::Animation &animation, const Ogre::Skeleton & source,Ogre::Skeleton *target){
  if (target->hasAnimation(animation.getName())){
    //remove the current animation, we're on a mission to replace it
    target->removeAnimation(animation.getName());
  }
  target->reset(true);
  //this creates an animation with the same name as the source animation
  Animation *retval=target->createAnimation(animation.getName(),animation.getLength());
  //these two values will be used to compute the overall scale of the model
  Vector3 average_scale_count(0,0,0);
  Vector3 rescale_factor(0,0,0);
  //this loop goes through all bones twice...the first time finds overall model scale, and the second time retargets the bones. Think of compute_animation_retarget as a bool
  for (unsigned int compute_animation_retarget=0;compute_animation_retarget<2;++compute_animation_retarget) {
    //iterate through all nodes in the source animation
    Animation::NodeTrackIterator nodes=animation.getNodeTrackIterator();
    int i=0;
    while (nodes.hasMoreElements()) {
      const Animation::NodeTrackIterator::KeyType key=nodes.peekNextKey();
      const NodeAnimationTrack *const next=nodes.getNext();
      //make sure the node animates a valid bone
      if (key<source.getNumBones()) {
        Bone * sourcebone=source.getBone(key);
        unsigned int numbones= target->getNumBones();
        unsigned int index=numbones-1-key,count;
        //go through all bones in the target skeleton, starting with the source number
        //this applies it to another skeleton by name if they are not the exact same skeleton
        //( I need this because I use LOD skeletons that have subsets of the bones)
        for (count=0;count<numbones;++count,++index) {
          unsigned int boneindex=numbones-1-(index%numbones);
          Bone *targetbone=target->getBone(boneindex);
          if (targetbone->getName()==sourcebone->getName()){
            //now we know which bone we have...and if bones are setup the same we're gold
            Vector3 curscale(0,0,0);
            Vector3 curnumerator(0,0,0);
            Vector3 curdenominator(0,0,0);
            //first compute how different the target and source bone are in position
            computeRescale(targetbone,sourcebone,&curscale,&curnumerator,&curdenominator);
            if (compute_animation_retarget) {
              //if we're in the second pass, go ahead and use the current bone difference (or if the node is in the center of the model, use the average bone differece for the scaling value
              MakeNonzeroVector(curscale,rescale_factor);
              //retarget a whole animation track to the skeleton
              retargetBoneTrack(boneindex,retval, target, source,curscale,next);
            }else {
              //apply the difference in bone centers to the overall average as long as neither is at the center of the world.
              IncrementCount(average_scale_count,curdenominator,curscale);
              IncrementCount(rescale_factor,curnumerator,curscale);
            }
            break;
          }
        }
      }
    }
    ///averages the scales that were nonzero for an overall scaling value
    AverageVectorComponents(rescale_factor,average_scale_count);
  }
  return retval;
}

///Takes a string with a skeleton name and an optional string with animation name, and retargets it to the target skeleton. If name is empty string, it will load the first animation (allowing entire skeleton packages to become "virtual animations"
Ogre::Animation *createAnimation(Ogre::Skeleton *target, Ogre::SkeletonManager* creator, const Ogre::String &skeletonname, const Ogre::String &groupname, const Ogre::String &animationName){

  ResourcePtr resultskeleton;
  if (creator->resourceExists(skeletonname)){
    resultskeleton=creator->getByName(skeletonname);
  }else {
    resultskeleton=creator->load(skeletonname,groupname);
  }
  //load the source skeleton by name...if it's not already in the system, load it manually,
  //SkeletonPtr source= resultskeleton;
  ResourcePtr sourceRes = resultskeleton;
  SkeletonPtr source = sourceRes.staticCast<Skeleton>();

  if (source.get()) {
    Animation * sourceAnimation=NULL;
    if (animationName.length()) {//if the name was passed in, get animation by name
      sourceAnimation=source->getAnimation(animationName);
    }else if (source->getNumAnimations()) {//otherwise assume artist saved skeleton an animation as a pair
      sourceAnimation=source->getAnimation(0);
    }
    if (sourceAnimation!=NULL) {//load animation
      return retargetAnimation(*sourceAnimation,*source.get(),target);
    }
  }
  //skeleton does not have requested animation (or any if animationName is not specified
  return NULL;
}
