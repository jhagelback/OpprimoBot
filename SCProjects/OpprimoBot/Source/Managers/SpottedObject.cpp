#include "SpottedObject.h"

SpottedObject::SpottedObject(Unit mUnit)
{
	type = mUnit->getType();
	position = mUnit->getPosition();
	tilePosition = mUnit->getTilePosition();
	unitID = mUnit->getID();
}

int SpottedObject::getUnitID()
{
	return unitID;
}

UnitType SpottedObject::getType()
{
	return type;
}

Position SpottedObject::getPosition()
{
	return position;
}

TilePosition SpottedObject::getTilePosition()
{
	return tilePosition;
}

bool SpottedObject::isAt(TilePosition tilePos)
{
	if (tilePos.x == tilePosition.x && tilePos.y == tilePosition.y)
	{
		return true;
	}
	return false;
}
