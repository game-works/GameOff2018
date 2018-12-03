#pragma once


class StatsManager
{
public:

  static float hp;
  static float maxhp;
  static float moon;
  static float maxmoon;
  static bool wantsTransform;
  static bool hybrid;


  static void addHP(float a)
  {
    hp += a;
    if (hp > maxhp) hp = maxhp;
  }


  static void addMoon(float a)
  {
    moon += a;
    if (moon >= maxmoon)
    {
      moon = maxmoon;
      wantsTransform = true;
    }
  }


protected:
};
