#ifndef __SETS_H__
#define __SETS_H__

#include <BWAPI.h>
#include <BWTA.h>
#include <BWAPI/SetContainer.h>
#include "../MainAgents/BaseAgent.h"

using namespace BWAPI;
using namespace BWTA;

/** Author: Johan Hagelback (johan.hagelback@gmail.com)
 */

class BaseLocationItem
{
public:
	BaseLocationItem(TilePosition pos)
	{
		baseLocation = pos;
		frameVisited = Broodwar->getFrameCount();
	};

	TilePosition baseLocation;
	int frameVisited;
};

class BaseLocationSet : public SetContainer<BaseLocationItem*, std::hash<void*>> 
{
public:
	
};

class Agentset : public SetContainer<BaseAgent*, std::hash<void*>> {
public:
	
};

#endif
