#include "stdafx.h"
#include "Menu.h"
#include "BranchAndBound.h"
#include "TabuSearch.h"
#include "Genetic.h"
using namespace std;

Menu::Menu()
{
	option = ' ';
	numberOfVertices = 0;
	filename = "Brak wczytanego pliku";
}


Menu::~Menu()
{
	if (numberOfVertices != 0) {
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
	}
}

int Menu::ShowVertices() {
	cout << "\nLiczba wierzcholkow: " << numberOfVertices << endl << endl;
	if (numberOfVertices > 50) {
		cout << "Za duza liczba wierzcholkow\n";
		return 0;
	}
	
	else if (numberOfVertices > 0)
		for (int i = 0; i < numberOfVertices; i++)
		{
			for (int j = 0; j < numberOfVertices; j++)
			{
				cout << vertices[i][j] << "\t";
			}
			cout << endl << endl;
		}
	return 0;
}

void Menu::ShowMenu(){

	while (option != '0') {
		if (option != ' ')
			cout << "\n----------------------------------------------\n\n";
		cout << "Projektowanie efektywnych algorytmow - Projekt etap 3\nMarcin Calkowski 218191\n\nParametry dla Tabu Search oraz algorytmu genetycznego dostepne do zmiany w pliku 'config.txt'\nWczytany plik: " << filename << "\nLiczba wierzcholkow: " << numberOfVertices << "\n\nWybierz opcje:\n\n1. Wczytaj graf z pliku\n2. Pokaz graf (Macierz sasiedztwa)\n\n--- Pojedyncze uruchomienie algorytmu ---\n3. Algorytm Branch and Bound\n4. Algorytm Tabu Search\n5. Algorytm genetyczny\n\n7. Testy wielokrotne\nR. Losuj graf\n\n0. Wyjscie\n";
		option = _getch();
		
		switch (option) {
		case '1': {cout << "\nWpisz nazwe pliku: "; 
			 string file;
			 cin >> file;
			 LoadFile(file);
		}  break;
		case '2': ShowVertices(); break;
		case '3': { BranchAndBound bnb;
			bnb.RunBranchAndBound(numberOfVertices, vertices);
			} break;
		case '4': { 
			if (ConfigFileCheck()) {
				TabuSearch ts;
				ts.RunTabuSearch(numberOfVertices, vertices);
			}
			else "Nie mozna uruchomic algorytmu - problem pliku konfiguracyjnego\n";
		} break;


		case '5': { 
			if (ConfigFileCheck()) {
				Genetic ga;
				ga.RunGenetic(numberOfVertices, vertices);
			}
			else "Nie mozna uruchomic algorytmu - problem pliku konfiguracyjnego\n";
		} break;

		
		case '7': { TabuSearch ts;
			this->Testmenu();
		} break;

		case ('r'): {BranchAndBound bnb;
			cout << "Podaj liczbe wierzcholkow do wygenerowania:\n";
			int verts;
			cin >> verts;
			if (verts < 3){
				cout << "Podano za malo wierzcholkow\n";
				break;
			}
			cout << "Czy dorga ma byc symetryczna? (t/n):\n";
			char road;
			bool sym;
			cin >> road;
			if (road == 'n')
				sym = false;
			else sym = true;

			GenerateMatrix(verts, sym);
		} break;
		case ('R'): {BranchAndBound bnb;
			cout << "Podaj liczbe wierzcholkow do wygenerowania:\n";
			int verts;
			cin >> verts;
			if (verts < 3) {
				cout << "Podano za malo wierzcholkow\n";
				break;
			}
			cout << "Czy dorga ma byc symetryczna? (t/n):\n";
			char road;
			bool sym;
			cin >> road;
			if (road == 'n')
				sym = false;
			else sym = true;

			GenerateMatrix(verts, sym);
		} break;
		
		}
	}
	return;
}

int Menu::Testmenu() {
	char w=' ';
	while (w != '0') {
		if (w != ' ')
			cout << "\n----------------------------------------------\n\n";
		cout << "\nWielokrotne wykonywanie algorytmow\nWybierz test:\n1. Branch and bound\n2. Tabu Search\n3. Genetyczny\nt. Test losowych wartosci dla BnB\n\n0. Powrot do menu glownego\n";
		w = _getch();
		switch (w) {
			case '1': { BranchAndBound bnb;
			BnBTimeTest(this->filename);
			} break;

			case '2': { TabuSearch ts;
			ts.MultiTimeTest(numberOfVertices, vertices, this->filename);
			} break;
			
			case '3': {
				Genetic gen;
				gen.RunGeneticMultiTest(numberOfVertices, vertices, filename);
			} break;

			case ('t'): {BranchAndBound bnb;
				cout << "Podaj liczbe wierzcholkow do wygenerowania:\n";
				int verts;
				cin >> verts;
				if (verts < 3) {
					cout << "Podano za malo wierzcholkow\n";
					break;
				}
				else
					BnBRandomTest(verts, true);
			} break;

			case ('A'): { cout << "Testowanie wszystkich plików:\n";
				TestAllFilesGenetic(); } break;
		}
	}
	return 0;
}

int Menu::BnBTimeTest(string &filename) {

	if (this->numberOfVertices <= 0) {
		cout << "\nNie wczytano danych\n";
		return 1;
	}

	double time = 0;
	string name = "Czasy BnB - ";
	name += filename;
	name += ".txt";
	fstream outfile;
	outfile.open(name, ios::out);
	outfile << filename << endl;
	for (int i = 0; i < 100; i++) {
		if (i % 10 == 0)
			cout << "Ukonczono " << i << endl;
		BranchAndBound bnbTime;
		time = bnbTime.BnBTimeTest(numberOfVertices, vertices);
		outfile << time;
		outfile << endl;
	}
	outfile.close();

	return 0;
}

int Menu::BnBRandomTest(int numberOfVerts, bool symetric) {
	double time = 0;
	string name = "Czasy BnB - rand";
	name += to_string(numberOfVerts);
	if (symetric)
		name += "s";
	else
		name += "a";
	name += ".txt";
	fstream outfile;
	outfile.open(name, ios::out);
	outfile << "Random " + to_string(numberOfVerts) << endl;
	int symetry;
	bool sym = true;;
	for (int i = 0; i < 50; i++) {
		if (i % 10 == 0)
			cout << "Ukonczono " << i << endl;
		GenerateMatrix(numberOfVerts, symetric);
		BranchAndBound bnbTime;
		time = bnbTime.BnBTimeTest(numberOfVertices, vertices);
		outfile << time;
		outfile << endl;
	}
	outfile.close();

	return 0;
}

int Menu::LoadFile(string &filename) {

	/*if (numberOfVertices != 0) {
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
	}*/
	string fileExt = filename.substr(filename.find_last_of(".") + 1);
	if (fileExt != "txt" && fileExt != "tsp" && fileExt != "atsp") //dodanie do nazwy pliku rozszerzenia .txt gdy go nie ma
	{
		filename += ".txt";
	}
	fileExt = filename.substr(filename.find_last_of(".") + 1);

	if (fileExt == "txt") {
		return Menu::LoadFileTXT(filename);
	}

	if (fileExt == "tsp") {
		return Menu::LoadFileTSP(filename);
	}

	if (fileExt == "atsp") {
		return Menu::LoadFileATSP(filename);
	}

	return 0;
}

int Menu::LoadFileTXT(std::string & filename)
{
	fstream File;
	File.open(filename, ios::in);
	if (!File.good()) {
		this->filename = "Brak wczytanego pliku";
		cout << "\nNie mozna wczytac pliku!\n";
		return 0;
	}

	if (numberOfVertices != 0) {
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
	}

	this->filename = filename;
	File >> numberOfVertices;
	vertices = new int*[numberOfVertices];
	int line = 0;

	for (int i = 0; i < numberOfVertices; i++)
		vertices[i] = new int[numberOfVertices];

	while (!File.eof()) {
		for (int i = 0; i < numberOfVertices; i++) {
			for (int j = 0; j < numberOfVertices; j++) {
				File >> vertices[i][j];
			}

			line++;
		}
	}
	File.close();

	if (line != numberOfVertices && line != numberOfVertices * 2) { // zabezpieczenie pir
		cout << "-> Zla ilosc danych <-\n";
		this->filename = "Brak wczytanego pliku";
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
		return 1;
	}
	cout << "\nWczytano plik!\n";
	return 0;
}

int Menu::LoadFileTSP(std::string & filename)
{
	ifstream File;
	File.open(filename, ios::in);
	if (!File.good()) {
		this->filename = "Brak wczytanego pliku";
		cout << "\nNie mozna wczytac pliku!\n";
		return 0;
	}
	this->filename = filename;

	if (numberOfVertices != 0) {
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
	}

	string line;
	while (!File.eof()) {
		getline(File, line);
		{
			if (!line.compare(0, 6, "TYPE: ")) {
				if (!line.compare(7, 3, "TSP")) {
					cout << "\n To nie jest plik TSP\n"; // czy typ pliku to TSP
					return 1;
				}

			}
			else if (!line.compare(0, 7, "TYPE : ")) {
				if (!line.compare(8, 3, "TSP")) {
					cout << "\n To nie jest plik TSP\n"; // czy typ pliku to TSP
					return 1;
				}
			}

			if (!line.compare(0, 11, "DIMENSION: ")) {
				string number = line.substr(11, 100);
				numberOfVertices = stoi(number); //zapisanie iloœci wierzcho³ków
			}
			else if (!line.compare(0, 12, "DIMENSION : ")) {
				string number = line.substr(12, 100);
				numberOfVertices = stoi(number); //zapisanie iloœci wierzcho³ków
			}


			if (!line.compare(0, 18, "EDGE_WEIGHT_TYPE: ")) {
				if (!line.compare(19, 8, "EXPLICIT")) {
					cout << "\n Nieobslugiwany typ danych\n"; // czy zapisano jako wagi krawêdzi
					return 1;
				}
			}
			else if (!line.compare(0, 19, "EDGE_WEIGHT_TYPE : ")) {
				if (!line.compare(20, 8, "EXPLICIT")) {
					cout << "\n Nieobslugiwany typ danych\n"; // czy zapisano jako wagi krawêdzi
					return 1;
				}
			}


			if (!line.compare(0, 20, "EDGE_WEIGHT_FORMAT: ")) { //Obs³uga ró¿nego typu zapisu macierzy
				if (!line.compare(20, 11, "FULL_MATRIX")) {
					
					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {
							for (int j = 0; j < numberOfVertices; j++) 
								for (int k = 0; k < numberOfVertices; k++) 
									File >> vertices[j][k];	
						}	
					}
				}
				else if (!line.compare(20, 14, "LOWER_DIAG_ROW")) {
					
					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					int line = 1;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {	
							for (int j = 0; j < numberOfVertices; j++) {
								for (int k = 0; k < line; k++) {
									File >> vertices[j][k];
									if (j != k)
										vertices[k][j] = vertices[j][k];
								}
								if (line < numberOfVertices)
									line++;
							}
								
						}
					}
				}
				else if (!line.compare(20, 14, "UPPER_DIAG_ROW")) {
					
					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					int line = numberOfVertices-1;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {
							for (int j = numberOfVertices-1; j >= 0; j--) {
								for (int k = line; k >= 0; k--) {
									File >> vertices[j][k];
									if (j != k)
										vertices[k][j] = vertices[j][k];
								}
								if (line >= 0)
									line--;
							}
						}
					}
				}
				else {
					cout << "\nNieobs³ugiwany format macierzy\n";
					return 1;
				}
			}

			else if (!line.compare(0, 21, "EDGE_WEIGHT_FORMAT : ")) { //Obs³uga ró¿nego typu zapisu macierzy
				if (!line.compare(21, 11, "FULL_MATRIX")) {

					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {
							for (int j = 0; j < numberOfVertices; j++)
								for (int k = 0; k < numberOfVertices; k++)
									File >> vertices[j][k];
						}
					}
				}
				else if (!line.compare(21, 14, "LOWER_DIAG_ROW")) {

					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					int line = 1;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {
							for (int j = 0; j < numberOfVertices; j++) {
								for (int k = 0; k < line; k++) {
									File >> vertices[j][k];
									if (j != k)
										vertices[k][j] = vertices[j][k];
								}
								if (line < numberOfVertices)
									line++;
							}

						}
					}
				}
				else if (!line.compare(21, 14, "UPPER_DIAG_ROW")) {

					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					int line = numberOfVertices - 1;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {
							for (int j = numberOfVertices - 1; j >= 0; j--) {
								for (int k = line; k >= 0; k--) {
									File >> vertices[j][k];
									if (j != k)
										vertices[k][j] = vertices[j][k];
								}
								if (line >= 0)
									line--;
							}
						}
					}
				}
				else {
					cout << "\nNieobs³ugiwany format macierzy\n";
					return 1;
				}
			}
		}

	}
	File.close();
	cout << "\nWczytano plik!\n";
	return 0;
}

int Menu::LoadFileATSP(std::string & filename)
{
	ifstream File;
	File.open(filename, ios::in);
	if (!File.good()) {
		this->filename = "Brak wczytanego pliku";
		cout << "\nNie mozna wczytac pliku!\n";
		return 0;
	}
	this->filename = filename;

	if (numberOfVertices != 0) {
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
	}

	string line;
	while (!File.eof()) {
		getline(File, line);
		{
			if (!line.compare(0, 6, "TYPE: "))
				if (!line.compare(7, 4, "ATSP")) {
					cout << "\n To nie jest plik ATSP\n"; // czy typ pliku to ATSP
					return 1;
				}

			if (!line.compare(0, 11, "DIMENSION: ")) {
				string number = line.substr(11, 100);
				numberOfVertices = stoi(number); //zapisanie iloœci wierzcho³ków
			}
			if (!line.compare(0, 18, "EDGE_WEIGHT_TYPE: "))
				if (!line.compare(19, 8, "EXPLICIT")) {
					cout << "\n Nieobslugiwany typ danych\n"; // czy zapisano jako wagi krawêdzi
					return 1;
				}
			if (!line.compare(0, 20, "EDGE_WEIGHT_FORMAT: ")) { //Obs³uga ró¿nego typu zapisu macierzy
				if (!line.compare(20, 11, "FULL_MATRIX")) {

					vertices = new int*[numberOfVertices];
					for (int i = 0; i < numberOfVertices; i++)
						vertices[i] = new int[numberOfVertices];

					string temp;
					while (!File.eof()) {
						File >> temp;
						if (temp == "EDGE_WEIGHT_SECTION") {
							for (int j = 0; j < numberOfVertices; j++)
								for (int k = 0; k < numberOfVertices; k++)
									File >> vertices[j][k];
						}
					}
				}
				else {
					cout << "\nNieobs³ugiwany format macierzy\n";
					return 1;
				}
			}
		}

	}
	File.close();
	cout << "\nWczytano plik!\n";
	return 0;
}

int Menu::GenerateMatrix(int numberOfVerts, bool sym)
{
	if (numberOfVertices != 0) {
		for (int i = 0; i < numberOfVertices; i++)
			delete vertices[i];
		delete[] vertices;
		vertices = NULL;
		numberOfVertices = 0;
	}
	
	this->numberOfVertices = numberOfVerts;
	this->filename = "Losowe dane - ";
	this->filename += to_string(numberOfVerts);

	vertices = new int*[numberOfVertices];
	for (int i = 0; i < numberOfVertices; i++)
		vertices[i] = new int[numberOfVertices];

	if (sym){
		int line = 1;
		for (int j = 0; j < numberOfVertices; j++) {
			for (int k = 0; k < line; k++) {
				vertices[j][k] = (rand() % 500) + 1;
				if (j != k)
					vertices[k][j] = vertices[j][k];
				else if (j == k)
					vertices[k][j] = -1;
			}
			if (line < numberOfVertices)
				line++;
		}
	}

	else {
		for (int i = 0; i < numberOfVertices; i++)
			for (int j = 0; j < numberOfVertices; j++) {
				if (i == j)
					vertices[i][j] = -1;
				else
					vertices[i][j] = (rand() % 500) + 1;
			}
	}


	return 0;
}

bool Menu::ConfigFileCheck() {
	fstream Config;
	Config.open("config.txt", ios::in);
	if (!Config.good()) {
		cout << "\nNie mozna odnalezc pliku konfiguracyjnego 'config.txt'\n";
		Config.close();
		return 0;
	}
	else {
		string line;
		getline(Config, line);
		if (line != ";PEA Projekt - plik konfiguracyjny") {
			cout << "\nZla zawartosc pliku 'config.txt'\n";
			Config.close();
			_getch();
			return 0;
		}
	}
	Config.close();
	return 1;
}

int Menu::TestAllFilesGenetic() {
	string name0 = "gr17.tsp", name1 = "br17.atsp", name2 = "ftv170.atsp", name3="rbg443.atsp", name4="pa561.tsp",name5="si1032.tsp";

	for (int i = 0; i < 6; i++) {
		if (i == 0) {
			LoadFile(name0);
			Genetic gen;
			cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
			gen.RunGeneticMultiTest(numberOfVertices, vertices, this->filename);
		}
		else if (i == 1) {
			LoadFile(name1);
			Genetic gen;
			cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
			gen.RunGeneticMultiTest(numberOfVertices, vertices, this->filename);
		}
		else if (i == 2) {
			LoadFile(name2);
			Genetic gen;
			cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
			gen.RunGeneticMultiTest(numberOfVertices, vertices, this->filename);
		}
		else if (i == 3) {
			LoadFile(name3);
			Genetic gen;
			cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
			gen.RunGeneticMultiTest(numberOfVertices, vertices, this->filename);
		}
		else if (i == 4) {
			LoadFile(name4);
			Genetic gen;
			cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
			gen.RunGeneticMultiTest(numberOfVertices, vertices, this->filename);
		}
		else if (i == 5) {
			LoadFile(name5);
			Genetic gen;
			cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
			gen.RunGeneticMultiTest(numberOfVertices, vertices, this->filename);
		}

		//cout << "Wczytano plik: " << this->filename << "   rozmiar:" << this->numberOfVertices << endl;
	}
	return 0;
}