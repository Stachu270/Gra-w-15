//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "gra.h"
#include <string>
#include <vector>

int main()
{	
	Game gra;
	gra.play();
	
	return 0;
}