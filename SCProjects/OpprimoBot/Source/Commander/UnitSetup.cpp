#include "UnitSetup.h"

UnitSetup::UnitSetup()
{

}

bool UnitSetup::equals(UnitType mType)
{
	return equals(type, mType);
}

bool UnitSetup::equals(UnitType t1, UnitType t2)
{
	UnitType used1 = t1;
	UnitType used2 = t2;

	if (t1.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode) used1 = UnitTypes::Terran_Siege_Tank_Tank_Mode;
	if (t2.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode) used2 = UnitTypes::Terran_Siege_Tank_Tank_Mode;
	if (t1.getID() == UnitTypes::Zerg_Lurker) used1 = UnitTypes::Zerg_Hydralisk;
	if (t2.getID() == UnitTypes::Zerg_Lurker) used2 = UnitTypes::Zerg_Hydralisk;
	if (t1.getID() == UnitTypes::Zerg_Guardian) used1 = UnitTypes::Zerg_Mutalisk;
	if (t2.getID() == UnitTypes::Zerg_Guardian) used2 = UnitTypes::Zerg_Mutalisk;
	if (t1.getID() == UnitTypes::Zerg_Devourer) used1 = UnitTypes::Zerg_Mutalisk;
	if (t2.getID() == UnitTypes::Zerg_Devourer) used2 = UnitTypes::Zerg_Mutalisk;
	if (t1.getID() == UnitTypes::Protoss_High_Templar) used1 = UnitTypes::Protoss_Archon;
	if (t2.getID() == UnitTypes::Protoss_High_Templar) used2 = UnitTypes::Protoss_Archon;

	if (used1.getID() == used2.getID())
	{
		return true;
	}
	return false;
}
