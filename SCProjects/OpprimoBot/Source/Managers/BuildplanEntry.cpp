#include "BuildplanEntry.h"

BuildplanEntry::BuildplanEntry(UnitType cType, int cSupply)
{
	unittype = cType;
	supply = cSupply;
	type = BUILDING;
}

BuildplanEntry::BuildplanEntry(UpgradeType cType, int cSupply)
{
	upgradetype = cType;
	supply = cSupply;
	type = UPGRADE;
}

BuildplanEntry::BuildplanEntry(TechType cType, int cSupply)
{
	techtype = cType;
	supply = cSupply;
	type = TECH;
}
