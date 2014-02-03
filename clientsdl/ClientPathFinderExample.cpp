#include "ClientPathFinder.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	GClientPathFinder = new ClientPathFinder();

	PosNode current(0, 0);
	PosNode dest(10, 10);
	while(GClientPathFinder->NextStep(current, PosNode(10, 10), current))
	{
		cout << current.first << " " << current.second << endl;
		if(current == dest) break;
	}

	return 0;
}
