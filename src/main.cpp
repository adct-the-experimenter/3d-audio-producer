
#include "viewer.h"

Viewer viewer;

int main(int argc, char* args[])
{
	if(!viewer.OnInit())
	{
		std::cout << "Failed to initialize program!\n";
	}
	return 0;
}
