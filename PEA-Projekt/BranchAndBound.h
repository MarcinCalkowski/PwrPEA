#pragma once
#include "Menu.h"

using namespace std;

class BranchAndBound
{
	struct queueVector {
		vector<int> currentRoad;
		int lowerBound;

		void add(vector<int> vec, int bound) {
			currentRoad = vec;
			lowerBound = bound;
		}
	};

	struct compareQueueVector {
		bool operator () (const queueVector & vec1, const queueVector & vec2) {
			if (vec1.lowerBound < vec2.lowerBound)
				return true;
			else return false;
		}
	};

	std::vector<int> road; //dane najlepszej drogi
	float roadLength;

	//std::priority_queue < queueVector, std::vector<queueVector>, compareQueueVector> BnBqueue;
	std::stack<vector<int>> BnBqueue; //stos dla algorytmu, zawieraj¹cy rozwi¹zania obiecuj¹ce

	std::vector<int> currentVec;
	std::vector<int> tempVec;
	std::vector<int> bestRoad;

	int bestRoadLength;
	int bestBound;

	bool wasInVector;

	int GreedyAlg(int numberOfVertices, int **vertices);

	int BoundBestIdea(int numberOfVertices, int **vertices, vector<int> currentRoad);
	int BoundFillMin1(int numberOfVertices, int ** vertices, vector<int> currentRoad);
	int BoundFillMin2(int numberOfVertices, int ** vertices, vector<int> currentRoad);

	int CalculateRoad(int numberOfVertices, int ** vertices, vector<int>& currentRoad);

public:
	BranchAndBound();
	~BranchAndBound();
	int RunBranchAndBound(int numberOfVertices, int ** vertices); //W³aœciwy algorytm branch and bound
	double BnBTimeTest(int numberOfVertices, int ** vertices); //Algorytm bnb w wersji do testowania czasowego
};

