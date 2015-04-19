#include "Statistics.h"
#include <fstream>

Statistics* Statistics::instance = NULL;

Statistics::Statistics()
{
	active = true;
}

Statistics* Statistics::getInstance()
{
	if (instance == NULL)
	{
		instance = new Statistics();
	}
	return instance;
}

Statistics::~Statistics()
{
	instance = NULL;
}

void Statistics::enable()
{
	active = true;
}

void Statistics::disable()
{
	active = false;
}

string Statistics::getFilename()
{
	stringstream ss;
	ss << "bwapi-data\\AI\\";
	//ss << "bwapi-data\\write\\"; //Tournament persistent storage version
	ss << "Statistics_OpprimoBot.csv";
	
	return ss.str();
}

void Statistics::saveResult(int win)
{
	if (!active) return;

	stringstream ss;
	ss << Broodwar->self()->getRace().getName();
	ss << ";";
	ss << Broodwar->enemy()->getRace().getName();
	ss << ";";
	ss << Broodwar->mapFileName();
	ss << ";";
	if (win == 1) ss << "Won";
	if (win == 0) ss << "Lost";
	if (win == 2) ss << "Draw";
	ss << ";";
	ss << Broodwar->self()->getUnitScore();
	ss << ";";
	ss << Broodwar->self()->getBuildingScore();
	ss << ";";
	ss << Broodwar->self()->getKillScore();
	ss << ";";
	ss << Broodwar->enemy()->getUnitScore();
	ss << ";";
	ss << Broodwar->enemy()->getBuildingScore();
	ss << ";";
	ss << Broodwar->enemy()->getKillScore();
	ss << "\n";

	//Save the file
	string filename = getFilename();

	ofstream outFile;
	outFile.open(filename.c_str(), ios::out | ios::app); 
	if (!outFile)
	{
		Broodwar << "Error writing to stats file!" << endl;
	}
	else
	{
		outFile << ss.str();
		outFile.close();
	}
}
