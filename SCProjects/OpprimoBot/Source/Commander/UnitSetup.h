#ifndef __UNITSETUP_H__
#define __UNITSETUP_H__

#include <BWAPI.h>
using namespace BWAPI;
using namespace std;

/** This class is used by the Squad class to handle the number of units of a specified type is
 * in the squad.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class UnitSetup {

private:
	
public:
	/** Default constructor. */
	UnitSetup();

	/** Type of unit. */
	UnitType type;
	/** Wanted number of the unit. */
	int no;
	/** Current number of the unit. */
	int current;

	/** Checks if this setup equals to the specified type. */
	bool equals(UnitType mType);

	/** Checks if unittypes matches. Needed for morphing units like Siege Tanks and Lurkers. */
	static bool equals(UnitType t1, UnitType t2);

};

#endif
