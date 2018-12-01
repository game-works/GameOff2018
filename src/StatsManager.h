#pragma once


class StatsManager
{
public:

  StatsManager();
  ~StatsManager();

  float hp = 100;
  float maxhp = 100;
  float moon = 0;
  float maxmoon = 100;


  void addHP(int a)
  {
    hp += a;
    if (hp > maxhp) hp = maxhp;
  }


  void addMoon(int a)
  {
    moon += a;
  }


protected:
};
