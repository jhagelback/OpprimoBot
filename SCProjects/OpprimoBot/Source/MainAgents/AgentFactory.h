#ifndef __AGENTFACTORY_H__
#define __AGENTFACTORY_H__

#include "BaseAgent.h"
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

/** The agent system is built from a single base agent where all specific agents extends the base agent indirectly or directly.
 * The AgentFactory class is a factory that creates the correct BaseAgent instance for a specific unit. This class shall always
 * be used when a new agent is requested.
 *
 * The AgentFactory is implemented as a singleton class. Each class that needs to access AgentFactory can request an instance,
 * and all classes shares the same AgentFactory instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class AgentFactory {

private:
	AgentFactory();
	static AgentFactory* instance;
	static bool instanceFlag;
	BaseAgent* createTerranAgent(Unit unit);
	BaseAgent* createProtossAgent(Unit unit);
	BaseAgent* createZergAgent(Unit unit);
	
public:
	~AgentFactory();

	/** Returns the instance to the class. */
	static AgentFactory* getInstance();

	/** Creates the BaseAgent */
	BaseAgent* createAgent(Unit unit);

	/** Returns true if the unit is of the specified type. */
	bool isOfType(Unit unit, UnitType type);
	
};

#endif
