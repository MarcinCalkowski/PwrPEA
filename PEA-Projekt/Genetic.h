#pragma once
class Genetic
{

	struct individual {
		int roadLength;
		std::vector<int> road;
		
		bool operator() (individual i1, individual i2) {
			return (i1.roadLength < i2.roadLength);
		}
	} individualObject;

	// parametry
	int populationSize; //rozmiar populacji
	int numberOfGenerations;
	float mutationProbability;
	float reproductionProbability;

	double endTime;
	long int rouletteSum;
	// -----
	int iterations;
	int badIterations;
	//int *individual[]; //osobnik - jedno rozwi¹zanie 

	std::vector<int> bestRoad; //dane najlepszej drogi
	int bestRoadLength;
	int worstRoadLength;

	std::vector<individual> population; //wektor osobników aktualnej populacji

	int Mutation(int numberOfVertices, int ** vertices, individual &ind); //todo
	int OXCrossing(int numberOfVertices, int ** vertices, individual parent1, individual parent2, individual &child1, individual &child2);

	int CalculateRoad(int numberOfVertices, int **vertices, std::vector<int> &currentRoad);
	//int SwapEdges(int & e1v1, int & e1v2, int & e2v1, int & e2v2);

	int GenerateRandomRoad(int numberOfVertices, int ** vertices, int populationIndex);
	int GenerateFirstPopulation(int numberOfVertices, int ** vertices);

	int LoadConfigFile(int numberOfVertices);

	int GeneticForMultiTest(int numberOfVertices, int ** vertices);

public:
	Genetic();
	~Genetic();

	int RunGenetic(int numberOfVertices, int ** vertices);
	int RunGeneticMultiTest(int numberOfVertices, int ** vertices, std::string filename);
};

