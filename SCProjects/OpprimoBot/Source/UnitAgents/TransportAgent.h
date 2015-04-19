#ifndef __TRANSPORTAGENT_H__
#define __TRANSPORTAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The TransportAgent handles transport units (Terran Dropship and Protoss Shuttle).
 *
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class TransportAgent : public UnitAgent {

private:
	int maxLoad;
	int currentLoad;
	int getCurrentLoad();
	bool isValidLoadUnit(BaseAgent* a);
	BaseAgent* findUnitToLoad(int spaceLimit);

public:
	TransportAgent(Unit mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
