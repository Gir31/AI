#include "Raven_Feature.h"
#include "../Raven_Bot.h"
#include "../Raven_SensoryMemory.h"
#include "../navigation/Raven_PathPlanner.h"
#include "../armory/Raven_Weapon.h"
#include "../Raven_WeaponSystem.h"
#include "../Raven_ObjectEnumerations.h"
#include "../lua/Raven_Scriptor.h"

//-----------------------------------------------------------------------------
double Raven_Feature::DistanceToItem(Raven_Bot* pBot, int ItemType)
{
  //determine the distance to the closest instance of the item type
  double DistanceToItem = pBot->GetPathPlanner()->GetCostToClosestItem(ItemType);

  //if the previous method returns a negative value then there is no item of
  //the specified type present in the game world at this time.
  if (DistanceToItem < 0 ) return 1;

  //these values represent cutoffs. Any distance over MaxDistance results in
  //a value of 0, and value below MinDistance results in a value of 1
  const double MaxDistance = 500.0;
  const double MinDistance = 50.0;

  Clamp(DistanceToItem, MinDistance, MaxDistance);

  return DistanceToItem / MaxDistance;
}


//----------------------- GetMaxRoundsBotCanCarryForWeapon --------------------
//
//  helper function to tidy up IndividualWeapon method
//  returns the maximum rounds of ammo a bot can carry for the given weapon
//-----------------------------------------------------------------------------
double GetMaxRoundsBotCanCarryForWeapon(int WeaponType)
{
  switch(WeaponType)
  {
  case type_rail_gun:

    return script->GetDouble("RailGun_MaxRoundsCarried");

  case type_rocket_launcher:

    return script->GetDouble("RocketLauncher_MaxRoundsCarried");

  case type_shotgun:

    return script->GetDouble("ShotGun_MaxRoundsCarried");

  default:

    throw std::runtime_error("trying to calculate  of unknown weapon");

  }//end switch
}


//----------------------- IndividualWeaponStrength ----------------------
//-----------------------------------------------------------------------------
double Raven_Feature::IndividualWeaponStrength(Raven_Bot* pBot,
                                               int        WeaponType)
{
  //grab a pointer to the gun (if the bot owns an instance)
  Raven_Weapon* wp = pBot->GetWeaponSys()->GetWeaponFromInventory(WeaponType);

  if (wp)
  {
    return wp->NumRoundsRemaining() / GetMaxRoundsBotCanCarryForWeapon(WeaponType);
  }

  else
  {
   return 0.0;
  }
}

//--------------------- TotalWeaponStrength --------------
//-----------------------------------------------------------------------------
double Raven_Feature::TotalWeaponStrength(Raven_Bot* pBot)
{
  const double MaxRoundsForShotgun = GetMaxRoundsBotCanCarryForWeapon(type_shotgun);
  const double MaxRoundsForRailgun = GetMaxRoundsBotCanCarryForWeapon(type_rail_gun);
  const double MaxRoundsForRocketLauncher = GetMaxRoundsBotCanCarryForWeapon(type_rocket_launcher);
  const double TotalRoundsCarryable = MaxRoundsForShotgun + MaxRoundsForRailgun + MaxRoundsForRocketLauncher;

  double NumSlugs      = (double)pBot->GetWeaponSys()->GetAmmoRemainingForWeapon(type_rail_gun);
  double NumCartridges = (double)pBot->GetWeaponSys()->GetAmmoRemainingForWeapon(type_shotgun);
  double NumRockets    = (double)pBot->GetWeaponSys()->GetAmmoRemainingForWeapon(type_rocket_launcher);

  //the value of the tweaker (must be in the range 0-1) indicates how much
  //desirability value is returned even if a bot has not picked up any weapons.
  //(it basically adds in an amount for a bot's persistent weapon -- the blaster)
  const double Tweaker = 0.1;

  return Tweaker + (1-Tweaker)*(NumSlugs + NumCartridges + NumRockets)/(MaxRoundsForShotgun + MaxRoundsForRailgun + MaxRoundsForRocketLauncher);
}

//------------------------------- HealthScore ---------------------------------
//
//-----------------------------------------------------------------------------
double Raven_Feature::Health(Raven_Bot* pBot)
{
  return (double)pBot->Health() / (double)pBot->MaxHealth();

}

//------------------------------- NoiseScore ---------------------------------
//
//-----------------------------------------------------------------------------
double Raven_Feature::ClosenessToLastNoise(Raven_Bot* pBot)
{
    // 1. 가장 최근 감지된 위치를 가져옵니다.
    const Vector2D LastPos = pBot->GetSensoryMem()->GetMostRecentlySensedPosition();

    // 2. 현재 위치와의 거리를 계산합니다.
    double Dist = Vec2DDistance(pBot->Pos(), LastPos);

    // 3. 점수 계산을 위한 범위 설정 (예: 500 unit 이상이면 0점)
    const double MaxDistance = 500.0;

    // 4. 거리가 0이면(즉, 정보가 없거나 바로 그 위치라면) 1 반환
    //    거리가 MaxDistance 이상이면 0 반환
    if (Dist > MaxDistance) return 0.0;

    // 5. 선형 보간: 거리가 가까울수록 1에 가깝게
    return 1.0 - (Dist / MaxDistance);
}