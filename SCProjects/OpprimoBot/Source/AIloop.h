#ifndef __AILOOP_H__
#define __AILOOP_H__

#include "MainAgents/BaseAgent.h"

/** This class handles the main AI loop that is executed each frame. It is 
 * separated from the OpprimoBot::onFrame().
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class AIloop {

private:
	bool debug;
	bool debugUnit;
	bool debugPF;
	bool debugBP;
	int debugSQ;

	void drawTerrainData();

public:
	/** Constructor */
	AIloop();

	/** Destructor */
	~AIloop();

	/** Call this each AI frame. */
	void computeActions();
	
	/** Toggles debug info on/off. */
	void toggleDebug();

	/** Toggles the unit debug info on/off. */
	void toggleUnitDebug();

	/** Toggles the potential field debug info on/off. */
	void togglePFDebug();

	/** Toggles the building placer debug info on/off. */
	void toggleBPDebug();

	/** Shows debug info for the specified squad. Use -1
	 * to disable info. */
	void setDebugSQ(int squadID);

	/** Show debug info. */
	void show_debug();

	/** Called when a new unit is added to the game. */
	void addUnit(Unit unit);

	/** Called when a unit is destroyed in the game. */
	void unitDestroyed(Unit unit);

	/** Called when a unit is morphed in in the game. */
	void morphUnit(Unit unit);
};

#endif
