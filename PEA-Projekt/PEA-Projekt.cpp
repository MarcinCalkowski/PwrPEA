// PEA-Projekt.cpp: Okre�la punkt wej�cia dla aplikacji konsoli.
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
