#include "Sim.h"
#include <windows.h>

int main()
{
	// Set high CPU priority
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	// Set up and run the program
	Sim sim;
	sim.run();

	return 0;
}
