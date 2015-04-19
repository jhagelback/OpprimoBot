#ifndef __SCIENCEVESSELAGENT_H__
#define __SCIENCEVESSELAGENT_H__

#include "../UnitAgent.h"

/** 
 * The ScienceVesselAgent handles abilities for Terran Science Vessels units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ScienceVesselAgent : public UnitAgent {

private:
	BaseAgent* findImportantUnit();
	bool isImportantUnit(BaseAgent* agent);
	bool isEMPtarget(Unit e);
	int lastIrradiateFrame;
	int lastShieldFrame;
	
public:
	ScienceVesselAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "ScienceVesselAgent";
		lastIrradiateFrame = 0;
		lastShieldFrame = 0;
	}

	bool useAbilities();
};

#endif
