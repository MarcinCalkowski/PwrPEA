#include "stdafx.h"
#include "Genetic.h"

using namespace std;

Genetic::Genetic()
{
	this->bestRoadLength = INT_MAX;
	this->worstRoadLength = 0;
}

Genetic::~Genetic()
{
}

int Genetic::RunGenetic(int numberOfVertices, int ** vertices)
{
	if (numberOfVertices <= 0) {
		cout << "\nNie wczytano danych\n";
		return 0;
	}
	//----------------------- Parametry -----------------------------------
	//poni¿ej schowane warunki domyœlnego wczytywania dla Ÿle podanych wartoœci w pliku config.txt
	{
		if (this->LoadConfigFile(numberOfVertices) == 1) {
			this->endTime = 10;
			this->populationSize = numberOfVertices;
			this->numberOfGenerations = numberOfVertices * 2;
			this->mutationProbability = 0.1;
			this->reproductionProbability = 0.6;
		}

		if (this->endTime < 0) {
			this->endTime = 10;
			cout << "Zla wartosc czasu!\n";
		}
		if (this->populationSize < 0) {
			this->populationSize = numberOfVertices;
			cout << "Zla wartosc PopulationSize!\n";
		}
		if (this->numberOfGenerations < 0) {
			this->numberOfGenerations = numberOfVertices * 2;
			cout << "Zla wartosc NumberOfGenerations!\n";
		}
		if (this->mutationProbability < 0) {
			this->mutationProbability = 0.05;
			cout << "Zla wartosc MutationProbability!\n";
		}

		if (this->mutationProbability > 1) {
			this->mutationProbability = 1;
			cout << "Zla wartosc MutationProbability!\n";
		}

		if (this->reproductionProbability <= 0) {
			this->reproductionProbability = 0.1;
			cout << "Zla wartosc ReproductionProbability!\n";
		}

		if (this->reproductionProbability > 1) {
			this->reproductionProbability = 1;
			cout << "Zla wartosc ReproductionProbability!\n";
		}
	}

	cout << "\nParametry algorytmu genetycznego:\n";
	cout << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;
	cout << " Rozmiar populacji: " << this->populationSize << endl;
	cout << " Prawdopodobienstwo krzyzowania: " << this->reproductionProbability << endl;
	cout << " Prawdopodobienstwo mutacji: " << this->mutationProbability << endl;

	//----------------------- ========= -----------------------------------
	cout << "\nUruchumiono algorytm genetyczny\n";

	this->iterations = 0;
	this->badIterations = 0;

	//Generowanie pierwszej populacji, kompletnie losowo
	GenerateFirstPopulation(numberOfVertices, vertices);

	if (population.size() <= 0) {
		cout << "Za maly rozmiar populacji, algorytm przerwano.\n";
		return 0;
	}

	/*for (int i = 0; i < populationSize; i++) {
		cout << "Wynik: " << population[i].roadLength << endl;
	}*/

	std::chrono::time_point<std::chrono::system_clock> start, end; //start pomiaru czasu
	start = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = start - start;

	float mutateRand;
	int generationCount = 0;
	vector <individual> nextGeneration;

	// -----------------------------------------------------------------------------------
	// ---------------------------  Rozpoczecie algorytmu   ------------------------------
	// -----------------------------------------------------------------------------------
	// PAMIÊTAJ !!!!! Dla ka¿dego!!! nowego obiczania drogi, porównywaæ tak samo z najszybsz¹, jak i najwolniejsz¹!
	//while (generationCount < 10) {
	while (elapsed_seconds.count() < endTime) { // Tutaj bêdzie warunek czasu, ¿eby porównaæ go z algorytmem TabuSearch
		//cout << "\nNowe pokolenie\n";

		/*cout << "Rozmiar populacji 1: " << population.size() << endl;
		for (int i = 0; i < population.size(); i++) {
			cout << "Wynik: " << population[i].roadLength << endl;
		}*/

		nextGeneration.clear();
		sort(population.begin(), population.end(), individualObject);

		/*cout << "Populacja po sortowaniu: " << population.size() << endl;
		for (int i = 0; i < population.size(); i++) {
			cout << "Wynik: " << population[i].roadLength << endl;
		}*/

		int indexInPopulation = 0;
		while (nextGeneration.size() < populationSize*0.2) {
			if ((nextGeneration.size() == 0) || (population[indexInPopulation].roadLength != nextGeneration.back().roadLength))
				nextGeneration.push_back(population[indexInPopulation]);
			indexInPopulation++;
		}

		/*cout << "Nowa generacja - 20%: " << nextGeneration.size() << endl;
		for (int i = 0; i < nextGeneration.size(); i++) {
			cout << "Wynik: " << nextGeneration[i].roadLength << endl;
		}*/

		// -----------------------------   SELEKCJA   ---------------------------------
		rouletteSum = 0;
		for (int i = 0; i < population.size(); i++) {

			int x = this->population[i].roadLength;
			int y = this->worstRoadLength;
			int a = y - x;
			int b = a / (long int)numberOfVertices;
			rouletteSum += b;
			//rouletteSum += (this->worstRoadLength - this->population[i].roadLength) / (long int)numberOfVertices;
		} // suma prawdopodobienstw - do ruletki

		while (nextGeneration.size() < populationSize) { //dopóki nie bêdzie nowej generacji - wektor nie bêdzie pe³ny
			vector <int> reproductiveIDsVec;

			while (reproductiveIDsVec.size() == 2) { // TUTAJ daæ warunek - kiedy koñczy siê ruletkowanie
				long int tempSum = rouletteSum;
				long int random = rand() % rouletteSum;
				int sumIndex = 0;
				while (tempSum > random) { //sprawdzanie indexu wyruletkowanej wartoœci
					tempSum -= (this->worstRoadLength - this->population[sumIndex].roadLength) / (long int)numberOfVertices;
					sumIndex++;
				}
				reproductiveIDsVec.push_back(sumIndex - 1);
			}

			float reproductionRand;
			float tempProb1 = rand() % 100000;
			reproductionRand = tempProb1 / 100000;

			// -----------------------------   KROS OX   ----------------------------------

			if (reproductionRand <= reproductionProbability) {
				//cout << "Jest reprodukcja\n";
				individual child1, child2;
				OXCrossing(numberOfVertices, vertices, population.front(), population.back(), child1, child2);

				// -----------------------------   MUTACJA   ----------------------------------
				float tempProb = rand() % 100000; //mo¿liwa mutacja dla pierwszego dziecka
				mutateRand = tempProb / 100000;
				if (mutateRand <= mutationProbability) {
					//cout << "-jest mutacja\n";
					Mutation(numberOfVertices, vertices, child1);
				}

				tempProb = rand() % 100000;
				mutateRand = tempProb / 100000; //mo¿liwa mutacja dla pierwszego dziecka
				if (mutateRand <= mutationProbability) {
					Mutation(numberOfVertices, vertices, child2);
				}

				//podmiana najlepszej, i najgorszej drogi z nowych rozwi¹zañ
				if (child1.roadLength < bestRoadLength) {
					bestRoad = child1.road;
					bestRoadLength = child1.roadLength;
				}

				if (child2.roadLength < bestRoadLength) {
					bestRoad = child2.road;
					bestRoadLength = child2.roadLength;
				}

				if (child1.roadLength > worstRoadLength) {
					worstRoadLength = child1.roadLength;
				}

				if (child2.roadLength > worstRoadLength) {
					worstRoadLength = child2.roadLength;
				}

				nextGeneration.push_back(child1);
				nextGeneration.push_back(child2);
			}
			reproductiveIDsVec.clear();
		}

		/*cout << "Nowa generacja cala: " << nextGeneration.size() << " =? " << populationSize << endl;
		for (int i = 0; i < nextGeneration.size(); i++) {
			cout << "Wynik: " << nextGeneration[i].roadLength << endl;
		}*/
		while (nextGeneration.size() > populationSize) {
			nextGeneration.pop_back();
		}
		population = nextGeneration;


		end = std::chrono::system_clock::now(); //koniec pomiaru czasu
		elapsed_seconds = end - start;
		iterations++;
		badIterations++;
		generationCount++;

	}

	cout << "\nDlugosc najlepszej drogi: " << bestRoadLength << endl;
	cout << "Ilosc wygenerowanych populacji : " << generationCount << endl;

	return 0;
}


int Genetic::GenerateRandomRoad(int numberOfVertices, int ** vertices, int index) {

	individual temp;
	//temp.road.push_back(0);

	bool * isInCurrentRoad = new bool[numberOfVertices];
	//isInCurrentRoad[0] = true;
	for (int i = 0; i < numberOfVertices; i++)
		isInCurrentRoad[i] = false;

	while (temp.road.size() < numberOfVertices) {
		int randomVerticle = (rand() % (numberOfVertices));
		if (!isInCurrentRoad[randomVerticle]) {
			temp.road.push_back(randomVerticle);
			isInCurrentRoad[randomVerticle] = true;
		}
	}
	temp.roadLength = CalculateRoad(numberOfVertices, vertices, temp.road);
	if (temp.roadLength < this->bestRoadLength) {
		this->bestRoadLength = temp.roadLength;
		this->bestRoad = temp.road;
	}
	else if (temp.roadLength > this->worstRoadLength) {
		this->worstRoadLength = temp.roadLength;
	}

	this->population.push_back(temp);
	return 0;
}

int Genetic::GenerateFirstPopulation(int numberOfVertices, int ** vertices)
{
	for (int i = 0; i < this->populationSize; i++) {
		GenerateRandomRoad(numberOfVertices, vertices, i);
	}

	return 0;
}

int Genetic::Mutation(int numberOfVertices, int ** vertices, individual &ind) {
	// Mutacja typu invert
	//losowanie krawedzi
	int randomVer1 = (rand() % numberOfVertices);
	int randomVer2 = (rand() % numberOfVertices);
	while (randomVer2 == randomVer1) {
		randomVer2 = (rand() % numberOfVertices);
	}

	if (randomVer2 < randomVer1)
		swap(randomVer2, randomVer1);

	int invSize = randomVer2 - randomVer1;
	vector <int> citesToInvert;

	for (int i = randomVer1; i < randomVer2; i++) {
		citesToInvert.push_back(ind.road[i]);
	}

	for (int i = randomVer1; i < randomVer2; i++) {
		ind.road[i] = citesToInvert.back();
		citesToInvert.pop_back();
	}
	ind.roadLength = CalculateRoad(numberOfVertices, vertices, ind.road); //obliczenie drogi dla danego elementu

	return 0;
}

//int Genetic::OXCrossing(individual parent1, individual parent2, individual &child1, individual &child2) {
int Genetic::OXCrossing(int numberOfVertices, int ** vertices, individual parent1, individual parent2, individual &child1, individual &child2) {
	//krzy¿owanie OX 
	//cout << "\nRozpoczynamy OXcross\n";
	int size = parent1.road.size();

	bool *childUsed1 = new bool[size]; // tablica booli przechowujaca które wierzcho³ki s¹ w przekopiowanym fragmencie
	bool *childUsed2 = new bool[size];

	individual childAlt1, childAlt2;
	childAlt1.road.resize(size);
	childAlt2.road.resize(size);

	for (int i = 0; i < size; i++) {
		childUsed1[i] = false;
		childUsed2[i] = false;
	}

	int startPos = rand() % size;
	int endPos = -1;
	while (endPos < 0 || endPos == startPos) {
		endPos = rand() % size;
	}
	if (startPos > endPos) {
		swap(startPos, endPos);
	}

	//cout << "startPos: " << startPos << "\tendPos: " << endPos << endl; //->Mamy dobrze wybrane punkty

	vector <int> childCpy1, childCpy2; // wektory tego co jest kopiowane
	childCpy1.resize(endPos - startPos);
	childCpy2.resize(endPos - startPos);

	int tempCounter = 0;
	for (int i = startPos; i < endPos; i++) {
		childAlt2.road[i] = parent1.road[i];
		childAlt1.road[i] = parent2.road[i];
		childCpy1[tempCounter] = parent2.road[i];
		childCpy2[tempCounter] = parent1.road[i];

		childUsed1[parent2.road[i]] = true;
		childUsed2[parent1.road[i]] = true; //zape³nienie tablicy booli

		tempCounter++;
	}

	int c1Counter;
	int c2Counter;

	//teraz wrzucenie tych co siê nie powtarzaj¹
	if (endPos < size) {
		c1Counter = endPos;
		c2Counter = endPos;
	}
	else {
		c1Counter = 0;
		c2Counter = 0;
	}

	//cout << "(endPos == size): " << endPos << " == " << size << endl;
	if (endPos < size)
		for (int i = endPos; i < size; i++) { //wrzucenie koñca ³añcucha
			if (childUsed1[parent1.road[i]] == false) {
				childAlt1.road[c1Counter] = parent1.road[i];
				c1Counter++;
				if (c1Counter >= size)
					c1Counter = 0;
			}

			if (childUsed2[parent2.road[i]] == false) {
				childAlt2.road[c2Counter] = parent2.road[i];
				c2Counter++;
				if (c2Counter >= size)
					c2Counter = 0;
			}
		}

	for (int i = 0; i < startPos; i++) { //wrzucenie pocz¹tku ³añcucha
		if (childUsed1[parent1.road[i]] == false) {
			childAlt1.road[c1Counter] = parent1.road[i];
			c1Counter++;
			if (c1Counter >= size)
				c1Counter = 0;
		}

		if (childUsed2[parent2.road[i]] == false) {
			childAlt2.road[c2Counter] = parent2.road[i];
			c2Counter++;
			if (c2Counter >= size)
				c2Counter = 0;
		}
	}

	for (int i = 0; i < childCpy1.size(); i++) { //wrzucenie tych co s¹ skopiowane do drugiego dziecka
		if (childUsed1[childCpy2[i]] == false) {
			childAlt1.road[c1Counter] = childCpy2[i];
			c1Counter++;
			if (c1Counter >= size)
				c1Counter = 0;
		}

		if (childUsed2[childCpy1[i]] == false) {
			childAlt2.road[c2Counter] = childCpy1[i];
			c2Counter++;
			if (c2Counter >= size)
				c2Counter = 0;
		}
	}

	childAlt1.roadLength = CalculateRoad(numberOfVertices, vertices, childAlt1.road);
	childAlt2.roadLength = CalculateRoad(numberOfVertices, vertices, childAlt2.road);

	child1 = childAlt1;
	child2 = childAlt2;

	delete childUsed1;
	delete childUsed2;
	return 0;
}

int Genetic::CalculateRoad(int numberOfVertices, int **vertices, vector<int> &currentRoad) {

	int temp = currentRoad.size();
	if (temp > 1) {
		int length = 0;
		for (int i = 0; i < temp - 1; i++)
			length += vertices[currentRoad[i]][currentRoad[i + 1]];

		if (temp == numberOfVertices)
			length += vertices[currentRoad[temp - 1]][currentRoad[0]];

		return length;
	}
	return 0;
}

/*int Genetic::SwapEdges(int & e1v1, int & e1v2, int & e2v1, int & e2v2) {
	int temp1 = e1v1, temp2 = e1v2;
	e1v1 = e2v1;
	e1v2 = e2v2;
	e2v1 = temp1;
	e2v2 = temp2;
	return 0;
}*/


int Genetic::LoadConfigFile(int numberOfVertices)
{
	//Tutaj bêdê wczytywa³ parametry, wrzuci siê do konstruktora metody TS
	ifstream File;
	File.open("config.txt", ios::in);
	if (!File.good()) {
		cout << "\nNie mozna wczytac pliku konfiguracyjnego!\n";
		return 1;
	}

	string line;
	getline(File, line);
	if (line != ";PEA Projekt - plik konfiguracyjny") {
		cout << "To nie jest poprawny plik konfiguracyjny. Wczytane zostan¹ dane domyœlne.\n";
		return 1;
	}
	while (!File.eof()) {
		getline(File, line);

		if (!line.compare(0, 1, ";"));
		else if (!line.compare(0, 9, "[Genetic]")) {
			while (line != "GeneticEnd;" && !File.eof()) {
				getline(File, line);
				if (!line.compare(0, 10, "TimeLimit=")) {
					if (!line.compare(10, 2, "n*")) {
						string number = line.substr(12, 20);
						this->endTime = numberOfVertices*stof(number);
					}
					else if (!line.compare(10, 2, "n/")) {
						string number = line.substr(12, 20);
						this->endTime = numberOfVertices / stof(number);
					}
					else if (!line.compare(10, 1, "n")) {
						this->endTime = numberOfVertices;
					}
					else {
						string number = line.substr(10, 20);
						this->endTime = stof(number);
					}
				}

				if (!line.compare(0, 15, "PopulationSize=")) {
					if (!line.compare(15, 2, "n*")) {
						string number = line.substr(17, 20);
						this->populationSize = numberOfVertices*stoi(number);
					}
					else if (!line.compare(15, 2, "n/")) {
						string number = line.substr(17, 20);
						this->populationSize = numberOfVertices / stoi(number);
					}
					else if (!line.compare(15, 1, "n")) {
						this->populationSize = numberOfVertices;
					}
					else {
						string number = line.substr(15, 20);
						this->populationSize = stoi(number);
					}
				}

				if (!line.compare(0, 20, "NumberOfGenerations=")) {
					if (!line.compare(20, 2, "n*")) {
						string number = line.substr(22, 20);
						this->numberOfGenerations = numberOfVertices*stoi(number);
					}
					else if (!line.compare(20, 2, "n/")) {
						string number = line.substr(22, 20);
						this->numberOfGenerations = numberOfVertices / stoi(number);
					}
					else if (!line.compare(20, 1, "n")) {
						this->numberOfGenerations = numberOfVertices;
					}
					else {
						string number = line.substr(22, 20);
						this->numberOfGenerations = stoi(number);
					}
				}
				if (!line.compare(0, 20, "MutationProbability=")) {
					if (!line.compare(20, 2, "n*")) {
						string number = line.substr(22, 20);
						this->mutationProbability = numberOfVertices*stof(number);
					}
					else if (!line.compare(20, 2, "n/")) {
						string number = line.substr(22, 20);
						this->mutationProbability = numberOfVertices / stof(number);
					}
					else if (!line.compare(20, 1, "n")) {
						this->mutationProbability = numberOfVertices;
					}
					else {
						string number = line.substr(20, 20);
						this->mutationProbability = stof(number);
					}
				}

				if (!line.compare(0, 24, "ReproductionProbability=")) {
					if (!line.compare(24, 2, "n*")) {
						string number = line.substr(26, 20);
						this->reproductionProbability = numberOfVertices*stof(number);
					}
					else if (!line.compare(24, 2, "n/")) {
						string number = line.substr(26, 20);
						this->reproductionProbability = numberOfVertices / stof(number);
					}
					else if (!line.compare(24, 1, "n")) {
						this->reproductionProbability = numberOfVertices;
					}
					else {
						string number = line.substr(24, 20);
						this->reproductionProbability = stof(number);
					}
				}

				/*if (!line.compare(0, 11, "Aspiration=")) {
					if (!line.compare(11, 1, "0")) {
						this->aspiration = false;
					}
					else if (!line.compare(11, 1, "1")) {
						this->aspiration = true;
					}
					else {
						cout << "nie mozna odczytac wartosci aspiracji - ustawiono automatycznie na true\n";
						this->aspiration = true;
					}
				}*/
			}
			File.close();
			return 0;
		}
	}
	File.close();
	cout << "\nWczytano plik!\n";
	return 1;
}

int Genetic::GeneticForMultiTest(int numberOfVertices, int ** vertices)
{
	if (numberOfVertices <= 0) {
		cout << "\nNie wczytano danych\n";
		return 0;
	}

	srand(time(NULL));

	// Tutaj BRAK wczytywania parametrów

	/*cout << "\nParametry algorytmu genetycznego:\n";
	cout << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;
	cout << " Rozmiar populacji: " << this->populationSize << endl;
	cout << " Prawdopodobienstwo krzyzowania: " << this->reproductionProbability << endl;
	cout << " Prawdopodobienstwo mutacji: " << this->mutationProbability << endl;
	*/

	this->iterations = 0;
	this->badIterations = 0;

	GenerateFirstPopulation(numberOfVertices, vertices);

	if (population.size() <= 0) {
		cout << "Za maly rozmiar populacji, algorytm przerwano.\n";
		return 0;
	}

	std::chrono::time_point<std::chrono::system_clock> start, end; //start pomiaru czasu
	start = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = start - start;

	float mutateRand;
	int generationCount = 0;
	vector <individual> nextGeneration;

	while (elapsed_seconds.count() < endTime) {

		nextGeneration.clear();
		sort(population.begin(), population.end(), individualObject);

		int indexInPopulation = 0;
		while (nextGeneration.size() < populationSize*0.2) {
			if ((nextGeneration.size() == 0) || (population[indexInPopulation].roadLength != nextGeneration.back().roadLength))
				nextGeneration.push_back(population[indexInPopulation]);
			indexInPopulation++;
		}

		rouletteSum = 0;
		for (int i = 0; i < population.size(); i++) {
			rouletteSum += (worstRoadLength - population[i].roadLength) / numberOfVertices;
		}

		while (nextGeneration.size() < populationSize) {
			vector <int> reproductiveIDsVec;

			while (reproductiveIDsVec.size() == 2) {
				long int tempSum = rouletteSum;
				long int random = rand() % rouletteSum;
				int sumIndex = 0;
				while (tempSum > random) {
					tempSum -= (worstRoadLength - population[sumIndex].roadLength) / numberOfVertices;
					sumIndex++;
				}
				reproductiveIDsVec.push_back(sumIndex - 1);
			}

			float reproductionRand;
			float tempProb1 = rand() % 100000;
			reproductionRand = tempProb1 / 100000;

			if (reproductionRand <= reproductionProbability) {
				individual child1, child2;
				OXCrossing(numberOfVertices, vertices, population.front(), population.back(), child1, child2);

				float tempProb = rand() % 100000;
				mutateRand = tempProb / 100000;
				if (mutateRand <= mutationProbability) {
					Mutation(numberOfVertices, vertices, child1);
				}

				tempProb = rand() % 100000;
				mutateRand = tempProb / 100000;
				if (mutateRand <= mutationProbability) {
					Mutation(numberOfVertices, vertices, child2);
				}

				if (child1.roadLength < bestRoadLength) {
					bestRoad = child1.road;
					bestRoadLength = child1.roadLength;
				}

				if (child2.roadLength < bestRoadLength) {
					bestRoad = child2.road;
					bestRoadLength = child2.roadLength;
				}

				if (child1.roadLength > worstRoadLength) {
					worstRoadLength = child1.roadLength;
				}

				if (child2.roadLength > worstRoadLength) {
					worstRoadLength = child2.roadLength;
				}

				nextGeneration.push_back(child1);
				nextGeneration.push_back(child2);
			}
			reproductiveIDsVec.clear();
		}

		while (nextGeneration.size() > populationSize) {
			nextGeneration.pop_back();
		}
		population = nextGeneration;

		end = std::chrono::system_clock::now(); //koniec pomiaru czasu
		elapsed_seconds = end - start;
		iterations++;
		badIterations++;
		generationCount++;
	}

	return bestRoadLength;
}


int Genetic::RunGeneticMultiTest(int numberOfVertices, int ** vertices, string filename)
{
	srand(time(NULL));

	this->endTime = 30;

	cout << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;

	for (int l = 0; l < 4; l++) {
		switch (l) {
		case (3): {this->populationSize = numberOfVertices * 2; } break;
		case (2): {this->populationSize = numberOfVertices; } break;
		case (1): {this->populationSize = numberOfVertices / 2; } break;
		case (0): {this->populationSize = numberOfVertices / 4; } break;
		}
		for (int k = 0; k < 5; k++) {
			//zmiany prawdopodobienstwa krzyzowania
			switch (k) {
			case (9): {this->reproductionProbability = 0.1; } break;
			case (8): {this->reproductionProbability = 0.2; } break;
			case (7): {this->reproductionProbability = 0.3; } break;
			case (6): {this->reproductionProbability = 0.4; } break;
			case (5): {this->reproductionProbability = 0.5; } break;
			case (4): {this->reproductionProbability = 0.6; } break;
			case (3): {this->reproductionProbability = 0.7; } break;
			case (2): {this->reproductionProbability = 0.8; } break;
			case (1): {this->reproductionProbability = 0.9; } break;
			case (0): {this->reproductionProbability = 1.0; } break;
			}
			cout << "\n" << k;

			this->mutationProbability = 0.1;

			int length = 0;
			stringstream s1, s2, s3;
			string name = "Genetic; ";
			string param = " ";

			name += filename;

			s1 << fixed << setprecision(1) << this->endTime;
			param += s1.str();
			param += ", ";

			param += to_string(this->populationSize);
			param += ", ";

			s2 << fixed << setprecision(1) << this->reproductionProbability;
			param += s2.str();
			//param += ", ";

			s3 << fixed << setprecision(1) << this->mutationProbability;
			param += s3.str();

			name += param;
			name += "; ";

			name += ".txt";
			fstream outfile;
			outfile.open(name, ios::out);

			length = 0;
			for (int i = 0; i < 10; i++) {
				length += GeneticForMultiTest(numberOfVertices, vertices);
			}
			length = length / 10;

			outfile << length << endl;

			outfile.close();
		}
	}

	return 0;
}

/* STARA FUNKCJA - Backup
int Genetic::RunGeneticMultiTest(int numberOfVertices, int ** vertices, string filename)
{
	//int repeats = 2; // ile razy uruchomiæ to ustrojstwo
					 // Przed uruchomieniem (wielokrotnym) musz¹ byæ parametry
					 // W Genetycznym do testów NIE MA KONFIGU!!!!!!!
	int length;

	this->endTime = 30;

	cout << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;

	for (int l = 0; l < 4; l++) {
		switch (l) {
		case (0): {this->populationSize = numberOfVertices * 2; } break;
		case (1): {this->populationSize = numberOfVertices; } break;
		case (2): {this->populationSize = numberOfVertices / 2; } break;
		case (3): {this->populationSize = numberOfVertices / 4; } break;
		}

		cout << "\n" << l;

		int length = 0;
		stringstream s1, s2, s3;
		string name = "Genetic; ";
		name += "; ";
		name += filename;
		string param = " ";

		s1 << fixed << setprecision(1) << this->endTime;
		param += s1.str();
		param += ", ";

		param += to_string(this->populationSize);
		param += ", ";

		name += param;
		name += ".txt";
		fstream outfile;
		outfile.open(name, ios::out);

		for (int k = 0; k < 6; k++) {
			//zmiany prawdopodobienstwa krzyzowania
			switch (k) {
			case (0): {this->reproductionProbability = 0.5; } break;
			case (1): {this->reproductionProbability = 0.6; } break;
			case (2): {this->reproductionProbability = 0.7; } break;
			case (3): {this->reproductionProbability = 0.8; } break;
			case (4): {this->reproductionProbability = 0.9; } break;
			case (5): {this->reproductionProbability = 1.0; } break;
			}

			//outfile << filename << endl;

			this->mutationProbability = 0.1;

			cout << ".";


			length = GeneticForMultiTest(numberOfVertices, vertices);

			outfile << length << endl;

		}
		outfile.close();
	}

	return 0;
}*/