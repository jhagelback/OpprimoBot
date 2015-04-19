#ifndef __MEDICAGENT_H__
#define __MEDICAGENT_H__

#include "../UnitAgent.h"

/** 
 * The MedicAgent handles abilities for Terran Medics.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MedicAgent : public UnitAgent {

private:
	
	/** Returns true if the specified own unit is a good target for the medic to follow and heal. Good targets
	* must be biological, must be in the attack force, and must not be loaded into a building or transport. */
	bool isMedicTarget(Unit mUnit);

public:
	MedicAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "MedicAgent";
	}

	bool useAbilities();
};

#endif
