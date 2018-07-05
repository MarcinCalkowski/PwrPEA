#pragma once
class Menu
{
	char option;
	// zmienne macierzy wierzcho³ków w grafie
	int numberOfVertices;
	int **vertices;

	bool configGood;
	std::string filename;

	int LoadFile(std::string &filename);
	int LoadFileTXT(std::string &filename);
	int LoadFileTSP(std::string &filename);
	int LoadFileATSP(std::string &filename);
	int GenerateMatrix(int numberOfVerts, bool sym);
	bool ConfigFileCheck();
	int BnBTimeTest(std::string &filename);
	int BnBRandomTest(int numberOfVerts, bool symetric);
	int ShowVertices();
	int Testmenu();
	int TestAllFilesGenetic();


public:

	Menu();
	~Menu();


	void ShowMenu();
	friend class BranchAndBound;
};


