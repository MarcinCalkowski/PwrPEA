#pragma once
class TabuSearch
{
	struct Edge{
		int v1;
		int v2;
	};

	struct TabuElement{
		Edge e1, e2;
		int cadency;
	};

	std::vector<TabuElement> tabuVec;

	
	std::vector<int> bestRoad; //dane najlepszej drogi
	int bestRoadLength;
	
	std::vector<int> currentRoad;
	int currentRoadLength;

	std::vector<int> tempRoad;
	int tempRoadLength;

	std::vector<int> aspirationTempRoad;
	int aspirationTempRoadLength;
	//Parametry
	int iterations; // ile razy wykona³ siê algorytm
	
	int cadency;
	int tabuSize;
	double endTime;
	bool aspiration;
	int criticalIterations;

	int CalculateRoad(int numberOfVertices, int **vertices, std::vector<int> &currentRoad);
	int RandomStartRoad(int numberOfVertices, int **vertices);
	int RandomCurrentRoad(int numberOfVertices, int ** vertices);
	int SwapVertices(int &vert1, int &vert2);
	int SwapEdges(int & e1v1, int & e1v2, int & e2v1, int & e2v2);
	int LoadConfigFile(int numberOfVertices);

public:
	TabuSearch();
	~TabuSearch();

	int RunTabuSearch(int numberOfVertices, int ** vertices);
	int TSTimeTest(int numberOfVertices, int ** vertices);
	int MultiTimeTest(int numberOfVertices, int ** vertices, std::string filename);
};

