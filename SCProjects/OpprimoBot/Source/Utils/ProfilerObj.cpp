#include "ProfilerObj.h"

ProfilerObj::ProfilerObj(string mId)
{
	PCFreq = 0.0;
	CounterStart = 0;
	CounterEnd = 0;

	id = mId;
	total = 0.0;
	maxTime = 0.0;
	startCalls = 0;
	endCalls = 0;
	lastShowFrame = 0;

	timeouts_short = 0;
	timeouts_medium = 0;
	timeouts_long = 0;
}

ProfilerObj::~ProfilerObj()
{
	
}

bool ProfilerObj::matches(string mId)
{
	return mId == id;
}

void ProfilerObj::start()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
    
    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;

	startCalls++;
}

void ProfilerObj::end()
{
	LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    double elapsed = (li.QuadPart-CounterStart)/PCFreq;
	
	total += elapsed;

	endCalls++;

	if (elapsed >= 10000.0) timeouts_long++;
	if (elapsed >= 1000.0) timeouts_medium++;
	if (elapsed >= 55.0) timeouts_short++;
	if (elapsed > maxTime) maxTime = elapsed;
}

void ProfilerObj::show()
{
	if (Broodwar->getFrameCount() - lastShowFrame < 400) return;

	lastShowFrame = Broodwar->getFrameCount();

	double avg = (double)total / (double)endCalls;

	Broodwar << id << ": AvgFrame: " << (int)avg << " MaxFrame: " << maxTime << " TO_1min: " << timeouts_long << " TO_1sec: " << timeouts_medium << " TO_55ms: " << timeouts_short << endl;
	if (timeouts_long >= 1 || timeouts_medium >= 10 || timeouts_short >= 320)
	{
		Broodwar << id << ": Timeout fail!!!" << endl;
	}
	if (startCalls != endCalls)
	{
		Broodwar << id << ": Warning! Start- and endcalls mismatch " << startCalls << "/" << endCalls << endl;
	}
}

string ProfilerObj::getDumpStr()
{
	double avg = total / (double)endCalls;

	stringstream ss;
	
	ss << "<tr><td>";
	ss << id;
	ss << "</td><td>";
	ss << avg;
	ss << "</td><td>";
	ss << total;
	ss << "</td><td>";
	if (startCalls == endCalls)
	{
		ss << endCalls;
	}
	else
	{
		ss << "Calls missmatch (";
		ss << startCalls;
		ss << "/";
		ss << endCalls;
		ss << ")";
	}
	ss << "</td><td>";
	ss << maxTime;
	ss << "</td><td>";
	ss << timeouts_long;
	ss << "</td><td>";
	ss << timeouts_medium;
	ss << "</td><td>";
	ss << timeouts_short;
	ss << "</td>";
	ss << "</tr>\n";
	
	return ss.str();
}