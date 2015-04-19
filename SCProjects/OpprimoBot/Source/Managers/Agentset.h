#ifndef __AGENTSET_H__
#define __AGENTSET_H__

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>
#include "../MainAgents/BaseAgent.h"

using namespace BWAPI;
using namespace std;

/** Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Agentset : public SetContainer<BaseAgent*, std::hash<void*>> {
public:
	//static const Agentset none;
};

#endif
