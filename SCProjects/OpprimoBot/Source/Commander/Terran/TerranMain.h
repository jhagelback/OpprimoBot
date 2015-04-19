#ifndef __TERRANMAIN_H__
#define __TERRANMAIN_H__

#include "../Commander.h"
#include "../Squad.h"

using namespace BWAPI;
using namespace std;

/**  This is the Commander class for a defensive Marine/Siege Tank/Goliath
 * based strategy.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class TerranMain : public Commander {

private:
	Squad* mainSquad;
	Squad* secondarySquad;
	Squad* backupSquad1;
	Squad* backupSquad2;
	Squad* sc1;
	Squad* sc2;
	
public:
	TerranMain();

	/** Destructor. */
	~TerranMain();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Returns the unique id for this strategy. */
	static string getStrategyId()
	{
		return "TerranMain";
	}
};

#endif
