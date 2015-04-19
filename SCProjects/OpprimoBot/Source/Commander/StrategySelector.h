#ifndef __STRATEGYSELECTOR_H__
#define __STRATEGYSELECTOR_H__

#include "Commander.h"

using namespace BWAPI;
using namespace std;

struct StrategyStats {
	string mapHash;
	string mapName;
	string strategyId;
	string ownRace;
	string opponentRace;
	int won;
	int lost;
	int draw;
	int total;

	StrategyStats()
	{
		won = 0;
		lost = 0;
		draw = 0;
		total = 0;
	}

	int getTotal()
	{
		if (total == 0) return 1; //To avoid division by zero.
		return total;
	}

	bool matches()
	{
		string mMapHash = Broodwar->mapHash();
		string mOwnRace = Broodwar->self()->getRace().getName();
		if (mMapHash == mapHash && mOwnRace == ownRace)
		{
			Race oRace = Broodwar->enemy()->getRace();
			if (oRace.getID() != Races::Unknown.getID())
			{
				//Opponent race is known. Match race as well.
				if (oRace.getName() == opponentRace)
				{
					return true;
				}
				else
				{
					return false;
				}
			}	
			return true;
		}
		return false;
	}
};

struct Strategy {
	Race race;
	string strategyId;

	Strategy(Race mRace, string mId)
	{
		race = mRace;
		strategyId = mId;
	}
};

/** When a game is started a strategy is selected depending on the map and, if known,
 * the opponent race. After each game the result is stored to a statistics file
 * (bwapi-data/AI/Strategies_OpprimoBot.csv). Strategies that previously have been
 * successful have a higher probability of being selected, but all strategies have
 * at least 15% chance of being used. 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class StrategySelector {

private:
	vector<Strategy> strategies;
	vector<StrategyStats> stats;
	
	static StrategySelector* instance;
	StrategySelector();

	string currentStrategyId;
	
	string getFilename();
	string getWriteFilename();
	void addEntry(string line);
	int toInt(string &str);

	void selectStrategy();

	bool active;

public:
	/** Returns the instance of the class. */
	static StrategySelector* getInstance();

	/** Destructor */
	~StrategySelector();

	/** Returns the selected strategy for this game. */
	Commander* getStrategy();

	/** Loads the stats file. */
	void loadStats();

	/** Prints debug info to the screen. */
	void printInfo();

	/** Adds the result after a game is finished. */
	void addResult(int win);

	/** Saves the stats file. */
	void saveStats();

	/** Enable strategy updates. */
	void enable();

	/** Disable strategy updates. */
	void disable();
};

#endif
