#ifndef __SPOTTEDOBJECT_H__
#define __SPOTTEDOBJECT_H__

#include <BWAPI.h>
using namespace BWAPI;
using namespace std;

/** The SpottedObject class is a help class for the ExplorationManager. It contains all details about a spotted
 * enemy unit or neutral resource. 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class SpottedObject {

private:
	UnitType type;
	Position position;
	TilePosition tilePosition;
	int unitID;

public:
	/** Creates an object from a unit reference. */
	SpottedObject(Unit mUnit);

	/** Returns the unique id of the spotted unit. */
	int getUnitID();

	/** Returns the type of the spotted unit. */
	UnitType getType();

	/** Returns the position of the spotted unit. */
	Position getPosition();

	/** Returns the tileposition of the spotted unit. */
	TilePosition getTilePosition();

	/** Returns true if the SpottedObject is at this TilePosition. */
	bool isAt(TilePosition tilePos);

};

#endif
