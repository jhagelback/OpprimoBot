#ifndef __LURKERRUSH_H__
#define __LURKERRUSH_H__

#include "../Commander.h"
#include "../Squad.h"

using namespace BWAPI;
using namespace std;

/** This is the Commander class for the Zerg Lurker Rush tactic.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class LurkerRush : public Commander {

private:
	Squad* mainSquad;
	Squad* l1;
	Squad* sc1;
	Squad* sc2;

public:
	LurkerRush();

	/** Destructor. */
	~LurkerRush();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "LurkerRush";
	}
};

#endif
