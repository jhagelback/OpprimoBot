#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include "../Commander/Commander.h"
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

/** This class saves results (winner, building score, unit score, kill score) from
 * played games. The results are stored to a semicolon-separated csv file in the bwapi-data/AI/
 * folder. Note that fog-of-war must be disabled to see opponent score.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Statistics {

private:
	static Statistics* instance;
	Statistics();

	string getFilename();
	bool active;
	
public:
	/** Returns the instance of the class. */
	static Statistics* getInstance();

	/** Destructor */
	~Statistics();

	/** Saves result from a game to file. */
	void saveResult(int win);

	/** Enable statistics. */
	void enable();

	/** Disable statistics. */
	void disable();
};

#endif
