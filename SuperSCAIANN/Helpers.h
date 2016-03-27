#pragma once
#include <BWAPI.h>
using namespace BWAPI;

class Helpers
{
public:

	// TODO: A complete mapping of threat ratings for all units
	//  if we're actually thinking about adding more units
	static int GetUnitThreatRating(BWAPI::UnitType InType)
	{
		int threatRating = 0;
		if (InType == BWAPI::UnitTypes::Terran_Marine)
		{
			threatRating = 6;
		}
		else if (InType == BWAPI::UnitTypes::Terran_Firebat)
		{
			threatRating = 8;
		}
		else if (InType == BWAPI::UnitTypes::Terran_Medic)
		{
			threatRating = 10;
		}

		return threatRating;
	}


	static float GetMaxUnitAttackRange(BWAPI::UnitType InType)
	{
		const WeaponType gw = InType.groundWeapon();
		const WeaponType aw = InType.airWeapon();
		return(gw.maxRange() > aw.maxRange() ? (1.f / 32.f) * gw.maxRange() : (1.f / 32.f) * aw.maxRange());
	}
};