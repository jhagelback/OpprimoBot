#include "Profiler.h"
#include <iostream>
#include <fstream>

bool Profiler::instanceFlag = false;
Profiler* Profiler::instance = NULL;

Profiler::Profiler()
{
	active = true;
}

Profiler::~Profiler()
{
	for (ProfilerObj* o : obj)
	{	
		delete o;
	}
}

Profiler* Profiler::getInstance()
{
	if (!instanceFlag)
	{
		instance = new Profiler();
		instanceFlag = true;
	}
	return instance;
}

void Profiler::enable()
{
	active = true;
}

void Profiler::disable()
{
	active = false;
}

ProfilerObj* Profiler::getObj(string mId)
{
	for (ProfilerObj* o : obj)
	{
		if (o->matches(mId))
		{
			return o;
		}
	}
	return NULL;
}


void Profiler::start(string mId)
{
	if (!active) return;

	ProfilerObj* cObj = getObj(mId);
	if (cObj != NULL) 
	{
		cObj->start();
	}
	else
	{
		ProfilerObj* newObj = new ProfilerObj(mId);
		newObj->start();
		obj.push_back(newObj);
	}
}

void Profiler::end(string mId)
{
	if (!active) return;

	ProfilerObj* cObj = getObj(mId);
	if (cObj != NULL) cObj->end();
}

void Profiler::dumpToFile()
{
	if (!active) return;

	ofstream ofile;
	ofile.open("bwapi-data\\AI\\Profiling_OpprimoBot.html");
	
	ofile << "<html><head>\n";
	ofile << "<style type='text/css'>\n";
	ofile << "body{ background-color: #999999;}\n";
	ofile << "table { background-color: white; border: 1px solid; width: 800;}\n";
	ofile << "tr {border: 1px solid;}\n";
	ofile << "td.h {background-color: #ccccff; text-align: center;}\n";
	ofile << "td {background-color: white; text-align: center;}\n";
	ofile << "</style>\n";
	ofile << "<title>OpprimoBot Profiler</title></head><body>\n";
	ofile << "<table>";

	ofile << "<tr><td class='h'>";
	ofile << "Id";
	ofile << "</td><td class='h'>";
	ofile << "AvgCallTime";
	ofile << "</td><td class='h'>";
	ofile << "TotalTime";
	ofile << "</td><td class='h'>";
	ofile << "Calls";
	ofile << "</td><td class='h'>";
	ofile << "MaxTime";
	ofile << "</td><td class='h'>";
	ofile << "TO_10sec";
	ofile << "</td><td class='h'>";
	ofile << "TO_1sec";
	ofile << "</td><td class='h'>";
	ofile << "TO_55ms";
	ofile << "</td>";
	ofile << "</tr>\n";

	for (ProfilerObj* o : obj)
	{
		ofile << o->getDumpStr().c_str();
	}

	ofile << "</table>";
	ofile << "</body></html>";

	ofile.close();
}