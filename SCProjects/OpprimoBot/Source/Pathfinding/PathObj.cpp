#include "PathObj.h"

PathObj::PathObj(TilePosition cStart, TilePosition cEnd)
{
	start = cStart;
	end = cEnd;
	finished = false;
}

PathObj::~PathObj()
{
	
}

bool PathObj::matches(TilePosition cStart, TilePosition cEnd)
{
	//Check if end almost matches
	double dist = cEnd.getDistance(end);
	if (dist > 3) return false;
	
	//Check if start almost matches.
	dist = cStart.getDistance(start);
	if (dist > 5) return false;
	return true;
}

bool PathObj::isFinished()
{
	return finished;
}

void PathObj::calculatePath() 
{
	path = getShortestPath(start, end);
	finished = true;
}

vector<TilePosition> PathObj::getPath()
{
	return path;
}
