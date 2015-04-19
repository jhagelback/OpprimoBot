#ifndef __PROTOSSMAIN_H__
#define __PROTOSSMAIN_H__

#include "../Commander.h"
#include "../Squad.h"

using namespace BWAPI;
using namespace std;

/**  This is the Commander class for a defensive Protoss tactics.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ProtossMain : public Commander {

private:
	Squad* mainSquad;
	Squad* stealthSquad;
	Squad* detectorSquad;

public:
	ProtossMain();

	/** Destructor. */
	~ProtossMain();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "ProtossMain";
	}
};

#endif
