#include "stdafx.h"
#include "TabuSearch.h"
using namespace std;

TabuSearch::TabuSearch()
{
}

TabuSearch::~TabuSearch()
{
}

int TabuSearch::RunTabuSearch(int numberOfVertices, int ** vertices)
{
	if (numberOfVertices <= 0) {
		cout << "\nNie wczytano danych\n";
		return 0;
	}

	//poni¿ej schowane warunki domyœlnego wczytywania dla Ÿle podanych wartoœci w pliku config.txt
	{
		if (LoadConfigFile(numberOfVertices) == 1) {
			this->endTime = 10;
			this->tabuSize = numberOfVertices * 2;
			this->cadency = numberOfVertices / 2;
			this->aspiration = true;
			this->criticalIterations = 1000;
		}

		if (this->endTime < 0) {
			this->endTime = 10;
			cout << "Zla wartosc czasu!\n";
		}
		if (this->tabuSize < 0) {
			this->tabuSize = numberOfVertices * 2;
			cout << "Zla wartosc tabuSize!\n";
		}
		if (this->cadency < 0) {
			this->cadency = numberOfVertices / 2;
			cout << "Zla wartosc kadencji!\n";
		}
		if (this->criticalIterations < 0) {
			this->criticalIterations = 1000;
			cout << "Zla wartosc krytyczna!\n";
		}
	}

	cout << "\nParametry tabu search:\n";
	cout << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;
	cout << " Rozmiar tabu: " << this->tabuSize << endl;
	cout << " Dlugosc kadencji: " << this->cadency << endl;
	cout << " Iteracje bez poprawy do zresetowania algorytmu: " << this->criticalIterations << endl;
	if (this->aspiration)
		cout << " Z kryterium aspiracji\n\n";
	else
		cout << " Bez kryterium aspiracji\n\n";
	cout << "Uruchomiono tabu search\n";


	this->iterations = 0; //przygotowanie danych algorytmu
	tabuVec.clear();

	std::chrono::time_point<std::chrono::system_clock> start, end; //start pomiaru czasu
	start = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = start - start;

	//-> losowanie pierwszej drogi od której zaczyna dzia³anie algorytm
	RandomStartRoad(numberOfVertices, vertices);
	this->currentRoad = this->bestRoad;
	this->currentRoadLength = this->bestRoadLength;

	//-> warunek d³ugoœci dzia³ania algorytmu
	while (elapsed_seconds.count() < endTime) {

		int smallestVer, min = INT_MAX;
		//-> szukanie najd³u¿szej krawêdzi (raczej do zmiany)
		for (int i = 1; i < numberOfVertices - 1; i++) {
			if (vertices[currentRoad[i]][currentRoad[i + 1]] < min) {
				smallestVer = i - 1;
				min = vertices[currentRoad[smallestVer]][currentRoad[smallestVer + 1]];
			}
		}

		//-> losowanie drugiej krawêdzi
		int randomVer = (rand() % (numberOfVertices - 2));
		while (randomVer == smallestVer || randomVer == (smallestVer + 1)) {
			randomVer = (rand() % (numberOfVertices - 2));
		}
		//nie jestem pewny czy to jest ok
		bool isInTabu = false;
		int tabuIndex;
		//-> sprawdzenie czy wylosowane wektory znajduj¹ siê w tabu
		for (int i = 0; i < tabuVec.size(); i++) {
			if (tabuVec[i].e1.v1 == currentRoad[smallestVer] && tabuVec[i].e1.v2 == currentRoad[smallestVer + 1] && tabuVec[i].e2.v1 == currentRoad[randomVer] && tabuVec[i].e2.v2 == currentRoad[randomVer + 1]) {
				isInTabu = true;
				tabuIndex = i;
				break;
			}
			else if (tabuVec[i].e2.v1 == currentRoad[smallestVer] && tabuVec[i].e2.v2 == currentRoad[smallestVer + 1] && tabuVec[i].e1.v1 == currentRoad[randomVer] && tabuVec[i].e1.v2 == currentRoad[randomVer + 1]) {
				isInTabu = true;
				tabuIndex = i;
				break;
			}
		}

		//-> aspiracja - gdy jest w³¹czona oraz obydwa wierzcho³ki znajduj¹ siê w tabu
		if (aspiration && isInTabu) {
			if (tabuVec[tabuIndex].cadency > (this->cadency / 3) && tabuVec[tabuIndex].cadency < 2 * (this->cadency / 3)) {
				tempRoad = currentRoad;
				SwapEdges(tempRoad[smallestVer], tempRoad[smallestVer + 1], tempRoad[randomVer], tempRoad[randomVer + 1]);
				tempRoadLength = CalculateRoad(numberOfVertices, vertices, tempRoad);
				if (currentRoadLength - tempRoadLength >= currentRoadLength / (numberOfVertices / 4)) { //tutaj kombinowaæ
					isInTabu = true;
				}
			}
		}

		//-> jeœli nie by³o w tabu, zamieñ, oblicz i dodaj zmianê do tabu
		if (isInTabu == false) {
			tempRoad = currentRoad;

			SwapEdges(tempRoad[smallestVer], tempRoad[smallestVer + 1], tempRoad[randomVer], tempRoad[randomVer + 1]);
			tempRoadLength = CalculateRoad(numberOfVertices, vertices, tempRoad);

			//-> sprawdzenie czy zamiana wierzchokow cos dala, jesli tak to zmiana aktualnie badanej dorgi
			if (tempRoadLength < currentRoadLength) {
				currentRoadLength = tempRoadLength;
				currentRoad = tempRoad;
				this->iterations = 0;

				//-> jesli nowa aktualna droga jest lepsza od najlepszej - ustaw j¹ jako najlepsz¹
				if (tempRoadLength < bestRoadLength) {
					bestRoad = tempRoad;
					bestRoadLength = tempRoadLength;
					//this->iterations = 0;
				}
			}

			//-> usuniecie z kadencji jesli jest = 0
			for (int i = 0; i < tabuVec.size(); i++) {
				tabuVec[i].cadency--;
				if (tabuVec[i].cadency <= 0) {
					tabuVec.erase(tabuVec.begin() + i);
				}
			}

			this->iterations++;
			//-> dodanie do tabu jeœli jest miejsce
			if (tabuVec.size() <= this->tabuSize) {
				TabuElement elem;
				elem.cadency = this->cadency;

				elem.e1.v1 = tempRoad[smallestVer];
				elem.e1.v2 = tempRoad[smallestVer + 1];
				elem.e2.v1 = tempRoad[randomVer];
				elem.e2.v2 = tempRoad[randomVer + 1];

				tabuVec.push_back(elem);
			}

		}

		//-> Dywesyfikacja - zmiana otoczenia (reset) gdy bêdzie wiêcej jak X iteracji bez znalezienia lepszego rozwi¹zania (ustawione na pocz¹tku)
		if (this->iterations >= this->criticalIterations) {
			RandomCurrentRoad(numberOfVertices, vertices);
			tabuVec.clear();
			this->iterations = 0;
		}

		end = std::chrono::system_clock::now(); //koniec pomiaru czasu
		elapsed_seconds = end - start;
	}

	cout << "\nDlugosc znalezionej drogi: " << bestRoadLength << endl;
	cout << "Czas wykonywania : " << elapsed_seconds.count() << endl;

	return 0;
}

int TabuSearch::MultiTimeTest(int numberOfVertices, int ** vertices, string filename)
{

	if (numberOfVertices <= 0) {
		cout << "\nNie wczytano danych\n";
		return 0;
	}

	//poni¿ej schowane warunki domyœlnego wczytywania dla Ÿle podanych wartoœci w pliku config.txt
	{
		if (LoadConfigFile(numberOfVertices) == 1) {
			this->endTime = 10;
			this->tabuSize = numberOfVertices * 2;
			this->cadency = numberOfVertices / 2;
			this->criticalIterations = 1000;
		}

		if (this->endTime < 0) {
			this->endTime = 10;
			cout << "Zla wartosc czasu!\n";
		}
		if (this->tabuSize < 0) {
			this->tabuSize = numberOfVertices * 2;
			cout << "Zla wartosc tabuSize!\n";
		}
		if (this->cadency < 0) {
			this->cadency = numberOfVertices / 2;
			cout << "Zla wartosc kadencji!\n";
		}
		if (this->criticalIterations < 0) {
			this->criticalIterations = 1000;
			cout << "Zla wartosc krytyczna!\n";
		}
	}

	int iterations = 10;


	cout << "\nParametry tabu search:\n";
	cout << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;
	cout << " Rozmiar tabu: " << this->tabuSize << endl;
	cout << " Dlugosc kadencji: " << this->cadency << endl;
	cout << " Iteracje bez poprawy do zresetowania algorytmu: " << this->criticalIterations << endl;
	if (this->aspiration)
		cout << " Z kryterium aspiracji\n\n";
	else
		cout << " Bez kryterium aspiracji\n\n";
	cout << "Uruchomiono tabu search ("<<iterations<<" razy)\n";

	// --------------------------- URUCHOMIENIE TEOG X RAZY -------------------------------
	this->aspiration = false;
	int length = 0;
	string name = "Wyniki TS (Bez) - ";
	name += filename;
	name += ".txt";
	fstream outfile;
	outfile.open(name, ios::out);
	outfile << filename << endl;

	outfile << "\nParametry tabu search:\n";
	outfile << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;
	outfile << " Rozmiar tabu: " << this->tabuSize << endl;
	outfile << " Dlugosc kadencji: " << this->cadency << endl;
	outfile << " Iteracje bez poprawy do zresetowania algorytmu: " << this->criticalIterations << endl;
	if (this->aspiration)
		outfile << " Z kryterium aspiracji\n\n";
	else
		outfile << " Bez kryterium aspiracji\n\n";

	for (int i = 0; i < iterations; i++) {
		cout << "Ukonczono " << i << endl;
		length = this->TSTimeTest(numberOfVertices, vertices);
		outfile << length;
		outfile << endl;
	}
	outfile.close();

	this->aspiration = true;
	name = "Wyniki TS (Z) - ";
	name += filename;
	name += ".txt";
	fstream outfile2;
	outfile2.open(name, ios::out);
	outfile2 << filename << endl;

	outfile2 << "\nParametry tabu search:\n";
	outfile2 << " Czas dzialania algorytmu: " << this->endTime << "(s)" << endl;
	outfile2 << " Rozmiar tabu: " << this->tabuSize << endl;
	outfile2 << " Dlugosc kadencji: " << this->cadency << endl;
	outfile2 << " Iteracje bez poprawy do zresetowania algorytmu: " << this->criticalIterations << endl;
	if (this->aspiration)
		outfile2 << " Z kryterium aspiracji\n\n";
	else
		outfile2 << " Bez kryterium aspiracji\n\n";

	for (int i = 0; i < iterations; i++) {
		cout << "Ukonczono " << i << endl;
		length = this->TSTimeTest(numberOfVertices, vertices);
		outfile2 << length;
		outfile2 << endl;
	}
	outfile2.close();

	cout << "Koniec wykonywania testu tabu search \n";

	return 0;

}

int TabuSearch::TSTimeTest(int numberOfVertices, int ** vertices)
{
	this->iterations = 0; //przygotowanie danych algorytmu
	tabuVec.clear();

	std::chrono::time_point<std::chrono::system_clock> start, end; //start pomiaru czasu
	start = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = start - start;

	//-> losowanie pierwszej drogi od której zaczyna dzia³anie algorytm
	RandomStartRoad(numberOfVertices, vertices);
	this->currentRoad = this->bestRoad;
	this->currentRoadLength = this->bestRoadLength;

	//-> warunek d³ugoœci dzia³ania algorytmu
	while (elapsed_seconds.count() < endTime) {

		int smallestVer, min = INT_MAX;
		//-> szukanie najd³u¿szej krawêdzi (raczej do zmiany)
		for (int i = 1; i < numberOfVertices - 1; i++) {
			if (vertices[currentRoad[i]][currentRoad[i + 1]] < min) {
				smallestVer = i - 1;
				min = vertices[currentRoad[smallestVer]][currentRoad[smallestVer + 1]];
			}
		}

		//-> losowanie drugiej krawêdzi
		int randomVer = (rand() % (numberOfVertices - 2));
		while (randomVer == smallestVer || randomVer == (smallestVer + 1)) {
			randomVer = (rand() % (numberOfVertices - 2));
		}
		//nie jestem pewny czy to jest ok
		bool isInTabu = false;
		int tabuIndex;
		//-> sprawdzenie czy wylosowane wektory znajduj¹ siê w tabu
		for (int i = 0; i < tabuVec.size(); i++) {
			if (tabuVec[i].e1.v1 == currentRoad[smallestVer] && tabuVec[i].e1.v2 == currentRoad[smallestVer + 1] && tabuVec[i].e2.v1 == currentRoad[randomVer] && tabuVec[i].e2.v2 == currentRoad[randomVer + 1]) {
				isInTabu = true;
				tabuIndex = i;
				break;
			}
			else if (tabuVec[i].e2.v1 == currentRoad[smallestVer] && tabuVec[i].e2.v2 == currentRoad[smallestVer + 1] && tabuVec[i].e1.v1 == currentRoad[randomVer] && tabuVec[i].e1.v2 == currentRoad[randomVer + 1]) {
				isInTabu = true;
				tabuIndex = i;
				break;
			}
		}

		//-> aspiracja - gdy jest w³¹czona oraz obydwa wierzcho³ki znajduj¹ siê w tabu
		if (aspiration && isInTabu) {
			if (tabuVec[tabuIndex].cadency > (this->cadency / 3) && tabuVec[tabuIndex].cadency < 2 * (this->cadency / 3)) {
				tempRoad = currentRoad;
				SwapEdges(tempRoad[smallestVer], tempRoad[smallestVer + 1], tempRoad[randomVer], tempRoad[randomVer + 1]);
				tempRoadLength = CalculateRoad(numberOfVertices, vertices, tempRoad);
				if (currentRoadLength - tempRoadLength >= currentRoadLength / (numberOfVertices / 4)) { //tutaj kombinowaæ
					isInTabu = true;
				}
			}
		}

		//-> jeœli nie by³o w tabu, zamieñ, oblicz i dodaj zmianê do tabu
		if (isInTabu == false) {
			tempRoad = currentRoad;

			SwapEdges(tempRoad[smallestVer], tempRoad[smallestVer + 1], tempRoad[randomVer], tempRoad[randomVer + 1]);
			tempRoadLength = CalculateRoad(numberOfVertices, vertices, tempRoad);

			//-> sprawdzenie czy zamiana wierzchokow cos dala, jesli tak to zmiana aktualnie badanej dorgi
			if (tempRoadLength < currentRoadLength) {
				currentRoadLength = tempRoadLength;
				currentRoad = tempRoad;
				this->iterations = 0;

				//-> jesli nowa aktualna droga jest lepsza od najlepszej - ustaw j¹ jako najlepsz¹
				if (tempRoadLength < bestRoadLength) {
					bestRoad = tempRoad;
					bestRoadLength = tempRoadLength;
					//this->iterations = 0;
				}
			}

			//-> usuniecie z kadencji jesli jest = 0
			for (int i = 0; i < tabuVec.size(); i++) {
				tabuVec[i].cadency--;
				if (tabuVec[i].cadency <= 0) {
					tabuVec.erase(tabuVec.begin() + i);
				}
			}

			this->iterations++;
			//-> dodanie do tabu jeœli jest miejsce
			if (tabuVec.size() <= this->tabuSize) {
				TabuElement elem;
				elem.cadency = this->cadency;

				elem.e1.v1 = tempRoad[smallestVer];
				elem.e1.v2 = tempRoad[smallestVer + 1];
				elem.e2.v1 = tempRoad[randomVer];
				elem.e2.v2 = tempRoad[randomVer + 1];

				tabuVec.push_back(elem);
			}

		}

		//-> Dywesyfikacja - zmiana otoczenia (reset) gdy bêdzie wiêcej jak X iteracji bez znalezienia lepszego rozwi¹zania (ustawione na pocz¹tku)
		if (this->iterations >= this->criticalIterations) {
			RandomCurrentRoad(numberOfVertices, vertices);
			tabuVec.clear();
			this->iterations = 0;
		}

		end = std::chrono::system_clock::now(); //koniec pomiaru czasu
		elapsed_seconds = end - start;
	}

	return bestRoadLength;
}



int TabuSearch::RandomStartRoad(int numberOfVertices, int ** vertices)
{
	bestRoad.clear();
	bestRoad.push_back(0);

	bool * isInCurrentRoad = new bool[numberOfVertices];
	isInCurrentRoad[0] = true;
	for (int i = 1; i < numberOfVertices; i++)
		isInCurrentRoad[i] = false;

	while (bestRoad.size() < numberOfVertices) {
		int randomVerticle = (rand() % (numberOfVertices - 1)) + 1;
		if (!isInCurrentRoad[randomVerticle]) {
			bestRoad.push_back(randomVerticle);
			isInCurrentRoad[randomVerticle] = true;
		}
	}
	this->bestRoadLength = CalculateRoad(numberOfVertices, vertices, bestRoad);
	return 0;
}

int TabuSearch::RandomCurrentRoad(int numberOfVertices, int ** vertices)
{
	currentRoad.clear();
	currentRoad.push_back(0);

	bool * isInCurrentRoad = new bool[numberOfVertices];
	isInCurrentRoad[0] = true;
	for (int i = 1; i < numberOfVertices; i++)
		isInCurrentRoad[i] = false;

	while (currentRoad.size() < numberOfVertices) {
		int randomVerticle = (rand() % (numberOfVertices - 1)) + 1;
		if (!isInCurrentRoad[randomVerticle]) {
			currentRoad.push_back(randomVerticle);
			isInCurrentRoad[randomVerticle] = true;
		}
	}
	this->currentRoadLength = CalculateRoad(numberOfVertices, vertices, currentRoad);
	return 0;
}


int TabuSearch::CalculateRoad(int numberOfVertices, int **vertices, vector<int> &currentRoad) {

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


int TabuSearch::SwapVertices(int & vert1, int & vert2) //nieu¿ywane po zmianie konceptu
{
	int temp = vert1;
	vert1 = vert2;
	vert2 = temp;
	return 0;
}

int TabuSearch::SwapEdges(int & e1v1, int & e1v2, int & e2v1, int & e2v2) {
	int temp1 = e1v1, temp2 = e1v2;
	e1v1 = e2v1;
	e1v2 = e2v2;
	e2v1 = temp1;
	e2v2 = temp2;
	return 0;
}


int TabuSearch::LoadConfigFile(int numberOfVertices)
{
	//Tutaj bêdê wczytywa³ parametry, wrzuci siê do konstruktora metody TS
	ifstream File;
	File.open("config.txt", ios::in);
	if (!File.good()) {
		cout << "\nNie mozna wczytac pliku konfiguracyjnego!\n";
		return 0;
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
		else if (!line.compare(0, 12, "[TabuSearch]")) {
			while (line != "TabuEnd;" && !File.eof()) {
				getline(File, line);
				if (!line.compare(0, 5, "Time=")) {
					if (!line.compare(5, 2, "n*")) {
						string number = line.substr(7, 20);
						this->endTime = numberOfVertices*stof(number);
					}
					else if (!line.compare(5, 2, "n/")) {
						string number = line.substr(7, 20);
						this->endTime = numberOfVertices / stof(number);
					}
					else if (!line.compare(5, 1, "n")) {
						this->endTime = numberOfVertices;
					}
					else {
						string number = line.substr(5, 20);
						this->endTime = stof(number);
					}
				}

				if (!line.compare(0, 9, "TabuSize=")) {
					if (!line.compare(9, 2, "n*")) {
						string number = line.substr(11, 20);
						this->tabuSize = numberOfVertices*stoi(number);
					}
					else if (!line.compare(9, 2, "n/")) {
						string number = line.substr(11, 20);
						this->tabuSize = numberOfVertices / stoi(number);
					}
					else if (!line.compare(9, 1, "n")) {
						this->tabuSize = numberOfVertices;
					}
					else {
						string number = line.substr(9, 20);
						this->tabuSize = stoi(number);
					}
				}

				if (!line.compare(0, 8, "Cadency=")) {
					if (!line.compare(8, 2, "n*")) {
						string number = line.substr(10, 20);
						this->cadency = numberOfVertices*stoi(number);
					}
					else if (!line.compare(8, 2, "n/")) {
						string number = line.substr(10, 20);
						this->cadency = numberOfVertices / stoi(number);
					}
					else if (!line.compare(8, 1, "n")) {
						this->cadency = numberOfVertices;
					}
					else {
						string number = line.substr(8, 20);
						this->cadency = stoi(number);
					}
				}
				if (!line.compare(0, 19, "CriticalIterations=")) {
					if (!line.compare(20, 2, "n*")) {
						string number = line.substr(21, 20);
						this->criticalIterations = numberOfVertices*stoi(number);
					}
					else if (!line.compare(19, 2, "n/")) {
						string number = line.substr(21, 20);
						this->criticalIterations = numberOfVertices / stoi(number);
					}
					else if (!line.compare(19, 1, "n")) {
						this->criticalIterations = numberOfVertices;
					}
					else {
						string number = line.substr(19, 20);
						this->criticalIterations = stoi(number);
					}
				}

				if (!line.compare(0, 11, "Aspiration=")) {
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
				}
			}
			File.close();
			return 0;
		}
	}
	File.close();
	cout << "\nWczytano plik!\n";
	return 1;
}
