#include "stdafx.h"
#include "BranchAndBound.h"

using namespace std;

BranchAndBound::BranchAndBound()
{
	//przygotowanie danych, w tym wrzucienie wierzchołka początkowego
	BnBqueue.empty();
	currentVec.clear();
	tempVec.clear();
	bestRoad.clear();

	bestBound = INT_MAX;

	road.push_back(0);
	BnBqueue.push(road);

	bestRoadLength = INT_MAX;
	bestRoad = road;

	wasInVector = false;
}

BranchAndBound::~BranchAndBound()
{
}

int BranchAndBound::RunBranchAndBound(int numberOfVertices, int **vertices) {

	if (numberOfVertices <= 0) {
		cout << "\nNie wczytano danych\n";
		return 0;
	}

	cout << "Uruchomiono branch and bound \n";

	std::chrono::time_point<std::chrono::system_clock> start, end; //start pomiaru czasu
	start = std::chrono::system_clock::now();

	GreedyAlg(numberOfVertices, vertices);

	while (!BnBqueue.empty()) { //dopóki będą jakieś rozwiązania obiecujące
		currentVec = BnBqueue.top();

		BnBqueue.pop();
		for (int i = 0; i < numberOfVertices; i++) { //dla każdego wierzchołka... // To przerobić na tablicę boola wykorzystanych wierzchołków
			tempVec.clear();
			tempVec = currentVec;
			wasInVector = false;
			for (int j = 0; j < tempVec.size(); j++) //...sprawdzenie czy nie znajduje się już w wektorze
				if (tempVec[j] == i)
					wasInVector = true;
			if (wasInVector == false) { //jeśli znaleziono wierzchołek którego jeszcze nie ma w danym wektorze
				tempVec.push_back(i);

				if (tempVec.size() == numberOfVertices) { //jeśli wektor badany ma wszystkie wierzchołki
					tempVec.shrink_to_fit(); //optymalizacja wektora w pamięci
					int tempRoadLength = CalculateRoad(numberOfVertices, vertices, tempVec);
					
					if (tempRoadLength < bestRoadLength) { //jeśli są wszystkie wierzchołki i ich długość jest lepsza od najlepszej dotychczasowej, to zamieniamy ją na najlepszą
						bestRoad = tempVec; //zapisanie nowego wyniku i wektora
						bestRoadLength = tempRoadLength;
					}
				}

				else if (tempVec.size() < numberOfVertices) {
					//int tempBound = BoundBestIdea(numberOfVertices, vertices, tempVec);
					int tempBound = BoundFillMin2(numberOfVertices, vertices, tempVec); //Funkcja FillMIn daje lepsze wyniki, dla tych samych daje dobre

					if (tempBound <= bestRoadLength) { //jeśli rozwiązanie jest obiecujące (lepsze od dotychczasowego najlepszego)
						BnBqueue.push(tempVec); //wrzucenie wektora na stos do dalszego przeglądu
					}
				}
			}
		}
	}

	end = std::chrono::system_clock::now(); //koniec pomiaru czasu
	std::chrono::duration<double> elapsed_seconds = end - start;

	cout << "\nDlugosc znalezionej drogi: " << bestRoadLength << endl;
	for (int i = 0; i < bestRoad.size(); i++)
		cout << bestRoad[i] << "->";
	cout << "0\n\n";

	cout << "Czas wykonywania : " << elapsed_seconds.count() << endl;
	return 0;
}


//funkcja odchudzona, do przeprowadzania testów czasowych
double BranchAndBound::BnBTimeTest(int numberOfVertices, int **vertices) {

	std::chrono::time_point<std::chrono::system_clock> start, end; //pomiar czasu
	start = std::chrono::system_clock::now();

	GreedyAlg(numberOfVertices, vertices);

	while (!BnBqueue.empty()) { //dopóki będą jakieś rozwiązania obiecujące
		currentVec = BnBqueue.top();

		BnBqueue.pop();
		for (int i = 0; i < numberOfVertices; i++) { //dla każdego wierzchołka... // To przerobić na tablicę boola wykorzystanych wierzchołków
			tempVec.clear();
			tempVec = currentVec;
			wasInVector = false;
			for (int j = 0; j < tempVec.size(); j++) //...sprawdzenie czy nie znajduje się już w wektorze
				if (tempVec[j] == i)
					wasInVector = true;
			if (wasInVector == false) { //jeśli znaleziono wierzchołek którego jeszcze nie ma w danym wektorze
				tempVec.push_back(i);

				if (tempVec.size() == numberOfVertices) { //jeśli wektor badany ma wszystkie wierzchołki
					tempVec.shrink_to_fit(); //optymalizacja wektora w pamięci
					int tempRoadLength = CalculateRoad(numberOfVertices, vertices, tempVec);

					if (tempRoadLength < bestRoadLength) { //jeśli są wszystkie wierzchołki i ich długość jest lepsza od najlepszej dotychczasowej, to zamieniamy ją na najlepszą
						bestRoad = tempVec; //zapisanie nowego wyniku i wektora
						bestRoadLength = tempRoadLength;
					}
				}

				else if (tempVec.size() < numberOfVertices) {
					int tempBound = BoundFillMin2(numberOfVertices, vertices, tempVec); //Funkcja InAndOut daje lepsze wyniki, dla tych samych daje dobre

					if (tempBound <= bestRoadLength) { //jeśli rozwiązanie jest obiecujące (lepsze od dotychczasowego najlepszego)
						BnBqueue.push(tempVec); //wrzucenie wektora na stos do dalszego przeglądu
					}
				}
			}
		}
	}

	end = std::chrono::system_clock::now(); //koniec pomiaru
	std::chrono::duration<double> elapsed_seconds = end - start;

	return elapsed_seconds.count();
}


//Nowa funkcja obliczająca granicę
int BranchAndBound::BoundBestIdea(int numberOfVertices, int **vertices, vector<int> currentRoad) {
	//-------------------------------------------------------------------------------------------------------------------
	//sposób liczenia dodanie drogi do ostatniego wierzchołka + dopełnienie minimalnymi wartościami w tablicy
	//-------------------------------------------------------------------------------------------------------------------
	int sum = 0, minRoad = INT_MAX, minConnection = INT_MAX, minZero = INT_MAX;

	//tablica pokazująca, czy dany wierzchołek został użyty (aby zebrać minima tylko nieodwiedzonych wierzchołków)
	bool * isInCurrentRoad = new bool[numberOfVertices];
	for (int i = 0; i < numberOfVertices; i++)
		isInCurrentRoad[i] = false;

	for (int i = 0; i < currentRoad.size(); i++) {
		isInCurrentRoad[currentRoad[i]] = true;
		if (i < currentRoad.size() - 1)
			sum += vertices[currentRoad[i]][currentRoad[i + 1]];
	}
	
	//obliczanie dopełnienia drogi
	for (int i = 0; i < numberOfVertices; i++) {
		if (!isInCurrentRoad[i]) {
			minRoad = INT_MAX;
			for (int j = 0; j < numberOfVertices; j++) {
				//if (i != j) 
				if (!isInCurrentRoad[j])
					if (vertices[i][j] < minRoad)
						minRoad = vertices[i][j];
			}
			sum += minRoad;

			if (vertices[currentRoad.back()][i] < minConnection && currentRoad.back() != i)
				minConnection = vertices[currentRoad.back()][i];

			if (vertices[i][0] < minZero && i != 0)
				minZero = vertices[i][0];
		}
	}
	delete isInCurrentRoad;
	return sum + minZero +minConnection;
	//return sum  + minConnection;
}


int BranchAndBound::BoundFillMin1(int numberOfVertices, int **vertices, vector<int> currentRoad) {
	//-------------------------------------------------------------------------------------------------------------------
	//sposób liczenia dodanie drogi do ostatniego wierzchołka + dopełnienie minimalnymi wartościami w tablicy
	//-------------------------------------------------------------------------------------------------------------------
	int sum = 0, minRoad = INT_MAX, minConnection = INT_MAX, minZero = INT_MAX;

	//tablica pokazująca, czy dany wierzchołek został użyty (aby zebrać minima tylko nieodwiedzonych wierzchołków)
	/*bool * isInCurrentRoad = new bool[numberOfVertices];
	for (int i = 0; i < numberOfVertices; i++)
	isInCurrentRoad[i] = false;

	for (int i = 0; i < currentRoad.size(); i++)
	isInCurrentRoad[currentRoad[i]] = true;
	*/
	for (int i = 0; i < currentRoad.size() - 1; i++)
		sum += vertices[currentRoad[i]][currentRoad[i + 1]];

	//obliczanie dopełnienia drogi
	for (int i = 0; i < numberOfVertices; i++) {
		bool wasInVector = false;
		for (int j = 0; j < currentRoad.size(); j++) {
			if (currentRoad[j] == i)
				wasInVector = true;
		}

		if (!wasInVector) {
			minRoad = INT_MAX;
			for (int j = 0; j < numberOfVertices; j++) {
				if (i != j) //if (!isInCurrentRoad[j])
					if (vertices[i][j] < minRoad)
						minRoad = vertices[i][j];
			}
			sum += minRoad;

			if (vertices[currentRoad.back()][i] < minConnection && currentRoad.back() != i)
				minConnection = vertices[currentRoad.back()][i];

			if (vertices[i][0] < minZero && i != 0)
				minZero = vertices[i][0];
		}
	}
	//delete isInCurrentRoad;
	return sum + minZero + minConnection;
	//return sum  + minConnection;
}

int BranchAndBound::BoundFillMin2(int numberOfVertices, int **vertices, vector<int> currentRoad) {
	//-------------------------------------------------------------------------------------------------------------------
	//sposób liczenia dodanie drogi do ostatniego wierzchołka + dopełnienie minimalnymi wartościami w tablicy
	//-------------------------------------------------------------------------------------------------------------------
	int sum = 0, minRoad = INT_MAX, minConnection = INT_MAX, minZero = INT_MAX;

	bool * isInCurrentRoad = new bool[numberOfVertices];
	for (int i = 0; i < numberOfVertices; i++)
		isInCurrentRoad[i] = false;

	for (int i = 0; i < currentRoad.size(); i++) {
		isInCurrentRoad[currentRoad[i]] = true;
		if (i < currentRoad.size() - 1)
			sum += vertices[currentRoad[i]][currentRoad[i + 1]];
	}

	//obliczanie dopełnienia drogi
	for (int i = 0; i < numberOfVertices; i++) {
		if (!isInCurrentRoad[i]) {
			minRoad = INT_MAX;
			for (int j = 0; j < numberOfVertices; j++) {
				if (i != j) 
				//if (!isInCurrentRoad[j])
					if (vertices[i][j] < minRoad)
						minRoad = vertices[i][j];
			}
			sum += minRoad;

			if (vertices[currentRoad.back()][i] < minConnection && currentRoad.back() != i)
				minConnection = vertices[currentRoad.back()][i];

			if (vertices[i][0] < minZero && i != 0)
				minZero = vertices[i][0];
		}
	}
	delete isInCurrentRoad;
	return sum + minZero + minConnection;
}


//obliczanie drogi z wektora
int BranchAndBound::CalculateRoad(int numberOfVertices, int **vertices, vector<int> &currentRoad) {

	int temp = currentRoad.size();
	if (temp > 1) {
		int length = 0;
		for (int i = 0; i < temp - 1; i++)
				length += vertices[currentRoad[i]][currentRoad[i + 1]];

		if (temp == numberOfVertices)
			length += vertices[currentRoad[temp - 1]][currentRoad[0]];

		//cout << "Length: " << length << endl;
		return length;
	}
	return 0;
}

//obliczanie pierwszej górnej granicy przy pomocy algorytmu zachłannego
int BranchAndBound::GreedyAlg(int numberOfVertices, int **vertices) {

	this->bestRoad.clear();
	this->bestRoad.push_back(0);
	while (this->bestRoad.size() < numberOfVertices) {
		bool wasTaken;
		int bestInIteration;
		int bestGreedyRoad = INT_MAX;
		for (int i = 1; i < numberOfVertices; i++) {
			wasTaken = false;
			for (int j = 0; j < bestRoad.size(); j++) {
				if (bestRoad[j] == i)
					wasTaken = true;
			}
			if (!wasTaken) {
				int roadTempValue = vertices[bestRoad[bestRoad.size() - 1]][i];

				if (roadTempValue < bestGreedyRoad && (vertices[bestRoad[bestRoad.size() - 1]][i] != -1 && vertices[bestRoad[bestRoad.size() - 1]][i] != 0)) {
					bestGreedyRoad = roadTempValue;
					bestInIteration = i;
				}
			}
		}
		if (bestInIteration > 0)
			bestRoad.push_back(bestInIteration);
	}

	bestRoadLength = CalculateRoad(numberOfVertices, vertices, bestRoad);

	return 0;
}