#ifndef __BUILDPLANENTRY_H__
#define __BUILDPLANENTRY_H__

#include <BWAPI.h>
using namespace BWAPI;
using namespace std;

/** Helper class for storing buildings/techs/upgrades to construct.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BuildplanEntry {

private:
	
public:
	BuildplanEntry(UnitType cType, int cSupply);
	BuildplanEntry(UpgradeType cType, int cSupply);
	BuildplanEntry(TechType cType, int cSupply);

	int type;
	UnitType unittype;
	UpgradeType upgradetype;
	TechType techtype;
	int supply;

	static const int BUILDING = 0;
	static const int UPGRADE = 1;
	static const int TECH = 2;
};

#endif
