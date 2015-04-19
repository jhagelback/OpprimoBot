#ifndef __NEXUSAGENT_H__
#define __NEXUSAGENT_H__

#include "../StructureAgent.h"
using namespace BWAPI;
using namespace std;

/** The NexusAgent handles Protoss Nexus buildings.
 * 
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class NexusAgent : public StructureAgent {

private:
	bool hasSentWorkers;

public:
	NexusAgent(Unit mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
