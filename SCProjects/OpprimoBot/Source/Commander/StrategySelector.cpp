#include "StrategySelector.h"
#include "Protoss/ProtossMain.h"
#include "Terran/TerranMain.h"
#include "Zerg/LurkerRush.h"
#include "Zerg/ZergMain.h"
#include <fstream>

StrategySelector* StrategySelector::instance = NULL;

StrategySelector::StrategySelector()
{
	active = true;

	strategies.push_back(Strategy(Races::Protoss, ProtossMain::getStrategyId()));
	strategies.push_back(Strategy(Races::Terran, TerranMain::getStrategyId()));
	strategies.push_back(Strategy(Races::Zerg, LurkerRush::getStrategyId()));
	strategies.push_back(Strategy(Races::Zerg, ZergMain::getStrategyId()));
	
	loadStats();
}

StrategySelector* StrategySelector::getInstance()
{
	if (instance == NULL)
	{
		instance = new StrategySelector();
	}
	return instance;
}

StrategySelector::~StrategySelector()
{
	instance = NULL;
}

void StrategySelector::enable()
{
	active = true;
}

void StrategySelector::disable()
{
	active = false;
}

void StrategySelector::selectStrategy()
{
	int totWon = 0;
	int totPlay = 0;
	for (int i = 0; i < (int)stats.size(); i++)
	{
		string mOwnRace = Broodwar->self()->getRace().getName();
		
		if (stats.at(i).matches())
		{
			totWon += stats.at(i).won;
			totPlay += stats.at(i).total;
		}
	}
	if (totPlay == 0) totPlay = 1; //To avoid division by zero

	//Random probability select one strategy
	bool found = false;
	int i = 0;
	while (!found)
	{
		i = rand() % (int)stats.size();
		
		//Entry matches
		if (stats.at(i).matches())
		{
			//Calculate probability for this entry.
			int chance = stats.at(i).won * 100 / stats.at(i).getTotal();
			chance = chance * totWon / totPlay;

			//Have 75% chance to try a strategy that
			//hasn't been tested much yet.
			if (stats.at(i).total <= 2) chance = 75;

			//Set a max/min so all strategies have a chance
			//to be played.
			if (chance < 15) chance = 15;
			if (chance > 85) chance = 85;

			//Make the roll!
			int roll = rand() % 100;
			if (roll <= chance)
			{
				currentStrategyId = stats.at(i).strategyId;
				Broodwar << "Strategy selected: " << currentStrategyId << " (Roll: " << roll << " Prob: " << chance << ")" << endl;
				found = true;
				return;
			}
		}		
	}
}

Commander* StrategySelector::getStrategy()
{
	int tot = 0;
	for (int i = 0; i < (int)stats.size(); i++)
	{
		if (stats.at(i).matches()) tot++;
	}
	
	if (tot > 0)
	{
		//Select a strategy among the tested
		//ones.
		selectStrategy();
	}
	else
	{
		//No strategy has been tested for this combo.
		//Return one of the available strategies.
		if (Broodwar->self()->getRace().getID() == Races::Terran.getID()) currentStrategyId = "TerranMain";
		if (Broodwar->self()->getRace().getID() == Races::Protoss.getID()) currentStrategyId = "ProtossMain";
		if (Broodwar->self()->getRace().getID() == Races::Zerg.getID()) currentStrategyId = "LurkerRush";
	}

	//Get Commander for strategy
	if (currentStrategyId == "ProtossMain") return new ProtossMain();
	if (currentStrategyId == "TerranMain") return new TerranMain();
	if (currentStrategyId == "LurkerRush") return new LurkerRush();
	if (currentStrategyId == "ZergMain") return new ZergMain();

	return NULL;
}

void StrategySelector::printInfo()
{
	Broodwar->drawTextScreen(180,5,"\x0F%s", currentStrategyId.c_str());
}

void StrategySelector::loadStats()
{
	string filename = getFilename();

	ifstream inFile;
	inFile.open(filename.c_str());
	if (!inFile)
	{
		//No file found.
		return;
	}
	else
	{
		string line;
		char buffer[256];
		while (!inFile.eof())
		{
			inFile.getline(buffer, 256);
			if (buffer[0] != ';')
			{	
				stringstream ss;
				ss << buffer;
				line = ss.str();
				addEntry(line);
			}
		}
		inFile.close();
	}
}

void StrategySelector::addEntry(string line)
{
	if (line == "") return;

	StrategyStats s = StrategyStats();
	int i;
	string t;

	i = line.find(";");
	t = line.substr(0, i);
	s.strategyId = t;
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.ownRace = t;
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.opponentRace = t;
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.won = toInt(t);
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.lost = toInt(t);
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.draw = toInt(t);
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.total = toInt(t);
	line = line.substr(i + 1, line.length());

	i = line.find(";");
	t = line.substr(0, i);
	s.mapName = t;
	line = line.substr(i + 1, line.length());
	
	i = line.find(";");
	t = line.substr(0, i);
	s.mapHash = t;
	line = line.substr(i + 1, line.length());

	stats.push_back(s);
}

int StrategySelector::toInt(string &str)
{
	stringstream ss(str);
	int n;
	ss >> n;
	return n;
}

string StrategySelector::getFilename()
{
	stringstream ss;
	ss << "bwapi-data\\AI\\";
	//ss << "bwapi-data\\read\\"; //Tournament persistent storage version
	ss << "Strategies_OpprimoBot.csv";
	
	return ss.str();
}

string StrategySelector::getWriteFilename()
{
	stringstream ss;
	ss << "bwapi-data\\AI\\";
	//ss << "bwapi-data\\write\\"; //Tournament persistent storage version
	ss << "Strategies_OpprimoBot.csv";
	
	return ss.str();
}

void StrategySelector::addResult(int win)
{
	if (!active) return;

	string opponentRace = Broodwar->enemy()->getRace().getName();
	string mapHash = Broodwar->mapHash();

	//Check if we have the entry already
	for (int i = 0; i < (int)stats.size(); i++)
	{
		if (mapHash == stats.at(i).mapHash && opponentRace == stats.at(i).opponentRace && currentStrategyId == stats.at(i).strategyId)
		{
			stats.at(i).total++;
			if (win == 0) stats.at(i).lost++;
			if (win == 1) stats.at(i).won++;
			if (win == 2) stats.at(i).draw++;
			return;
		}
	}

	StrategyStats s = StrategyStats();
	s.total++;
	if (win == 0) s.lost++;
	if (win == 1) s.won++;
	if (win == 2) s.draw++;
	s.strategyId = currentStrategyId;
	s.mapHash = mapHash;
	s.mapName = Broodwar->mapFileName();
	s.ownRace = Broodwar->self()->getRace().getName();
	s.opponentRace = opponentRace;
	stats.push_back(s);
}

void StrategySelector::saveStats()
{
	if (!active) return;

	//Fill entries in stats file for combinations that have
	//not yet been played.
	string mapHash = Broodwar->mapHash();
	string opponentRace = Broodwar->enemy()->getRace().getName();
	string ownRace = Broodwar->self()->getRace().getName();

	for (int i = 0; i < (int)strategies.size(); i++)
	{
		bool found = false;
		for (int s = 0; s < (int)stats.size(); s++)
		{
			if (strategies.at(i).strategyId == stats.at(s).strategyId && mapHash == stats.at(s).mapHash && opponentRace == stats.at(s).opponentRace)
			{
				//Matches
				found = true;
				break;
			}
		}

		if (!found)
		{
			//Only fill in the strategies for
			//the same race
			if (ownRace == strategies.at(i).race.getName())
			{
				//Add entry
				StrategyStats s = StrategyStats();
				s.mapHash = mapHash;
				s.mapName = Broodwar->mapFileName();
				s.opponentRace = opponentRace;
				s.ownRace = strategies.at(i).race.getName();
				s.strategyId = strategies.at(i).strategyId;

				stats.push_back(s);
			}
		}
	}

	//Save the file
	string filename = getWriteFilename();

	ofstream outFile;
	outFile.open(filename.c_str());
	if (!outFile)
	{
		Broodwar << "Error writing to stats file!" << endl;
	}
	else
	{
		for (int i = 0; i < (int)stats.size(); i++)
		{
			stringstream s2;
			s2 << stats.at(i).strategyId;
			s2 << ";";
			s2 << stats.at(i).ownRace;
			s2 << ";";
			s2 << stats.at(i).opponentRace;
			s2 << ";";
			s2 << stats.at(i).won;
			s2 << ";";
			s2 << stats.at(i).lost;
			s2 << ";";
			s2 << stats.at(i).draw;
			s2 << ";";
			s2 << stats.at(i).total;
			s2 << ";";
			s2 << stats.at(i).mapName;
			s2 << ";";
			s2 << stats.at(i).mapHash;
			s2 << ";\n";

			outFile << s2.str();
		}
		outFile.close();
	}
}
