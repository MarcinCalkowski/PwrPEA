// PEA-Projekt.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "Menu.h"
using namespace std;

int main()
{
	srand(time(NULL));
	Menu menu;
	menu.ShowMenu();
	return 0;
}
