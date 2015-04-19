#ifndef __ZERGMAIN_H__
#define __ZERGMAIN_H__

#include "../Commander.h"
#include "../Squad.h"

using namespace BWAPI;
using namespace std;

/** This is the Commander class for a balanced Zerg tactic
 * based on Hydralisks and Mutalisks.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ZergMain : public Commander {

private:
	Squad* mainSquad;
	Squad* l1;
	Squad* sc1;
	Squad* sc2;

public:
	ZergMain();

	/** Destructor. */
	~ZergMain();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "ZergMain";
	}
};

#endif
