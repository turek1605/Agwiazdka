#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
using namespace std;

enum Pole {
	PUSTE,
	LISTA_OTW,
	LISTA_ZAM,
	TRASA,
	PRZESZKODA
};

enum Rodzic {
	BRAK,
	NA_GORZE,
	NA_DOLE,
	PO_LEWEJ,
	PO_PRAWEJ
};

int main() {
	ifstream plansza;

	// tablice programu, kazda odpowiednio zerowana
	Pole pola[20][20] = { PUSTE };
	float heurystyki[20][20] = { 0.0f };
	short drogi[20][20] = { 0 };
	float sumy[20][20] = { 0 };
	Rodzic rodzice[20][20] = { BRAK };

	bool znaleziono_trase = false;
	bool brak_trasy = false;

	unsigned short poczatek_wiersz = 0,
		poczatek_kolumna = 0,
		koniec_wiersz = 19,
		koniec_kolumna = 19;

	plansza.open("grid.txt");

	if (!plansza.is_open()) {
		cout << "blad przy otwieraniu pliku\n";
		system("PAUSE");
		return 0;
	}
	// bez bledu, plik otwarty
	for (unsigned short wiersz = 0; wiersz < 20; wiersz++) {
		for (unsigned short kolumna = 0; kolumna < 20; kolumna++) {
			short liczba;
			plansza >> liczba;

			switch (liczba) {
			case 0:
				pola[wiersz][kolumna] = PUSTE;
				break;
			case 5:
				pola[wiersz][kolumna] = PRZESZKODA;
				break;
			}
		}
	}

	// mapa zbudowana, obliczamy heurystyki dla wszystkich pol, ktore nie sa przeszkodami

	for (unsigned short wiersz = 0; wiersz < 20; wiersz++) {
		for (unsigned short kolumna = 0; kolumna < 20; kolumna++) {
			if (pola[wiersz][kolumna] == PUSTE) {
				// dlugosc odcinka w linii prostej -> (y2-y1)*(y2-y1) + (x2-x1)*(x2-x1)
				heurystyki[wiersz][kolumna] = sqrt((float)((koniec_kolumna - kolumna)*(koniec_kolumna - kolumna) + (koniec_wiersz - wiersz)*(koniec_wiersz - wiersz)));
			}
		}
	}

	//heurystyki policzone, dodaje pierwszy element do listy otwartej:
	pola[poczatek_wiersz][poczatek_kolumna] = LISTA_OTW;
	drogi[poczatek_wiersz][poczatek_kolumna] = 0;
	sumy[poczatek_wiersz][poczatek_kolumna] = heurystyki[poczatek_wiersz][poczatek_kolumna] + drogi[poczatek_wiersz][poczatek_kolumna];

	while (!znaleziono_trase && !brak_trasy) {
		float najlepsza_suma = 9001.0f; // wiecej niz najwieksza mozliwa suma
		unsigned short najlepszy_wiersz,
			najlepsza_kolumna;

		for (unsigned short wiersz = 0; wiersz < 20; wiersz++) {
			for (unsigned short kolumna = 0; kolumna < 20; kolumna++) {
				if (pola[wiersz][kolumna] == LISTA_OTW) {
					if (sumy[wiersz][kolumna] < najlepsza_suma) { // znaleziono lepszy element w liscie otwartej
						najlepsza_suma = sumy[wiersz][kolumna];
						najlepszy_wiersz = wiersz;
						najlepsza_kolumna = kolumna;
					}
				}
			}
		}

		if (najlepsza_suma == 400.0f) { // nie znaleziono zadnego elementu w liscie otwartej - czyli jest pusta
			brak_trasy = true;
			continue; // pomijam dalsze polecenia, nie ma trasy z punktu poczatkowego do koncowego
		}

		if (najlepszy_wiersz == koniec_wiersz && najlepsza_kolumna == koniec_kolumna) { // doszedlem do mety
			znaleziono_trase = true;
			continue; // pomijam dalsze polecenia, znaleziono najlepsza trase z punktu poczatkowego do koncowego
		}

		// zamykam najlepszy element
		pola[najlepszy_wiersz][najlepsza_kolumna] = LISTA_ZAM;

		if (najlepsza_kolumna - 1 >= 0) { //czy moge pojsc w lewo
			unsigned short lewy_sasiad_wiersz = najlepszy_wiersz;
			unsigned short lewy_sasiad_kolumna = najlepsza_kolumna - 1;

			if (pola[lewy_sasiad_wiersz][lewy_sasiad_kolumna] == PUSTE
				|| (pola[lewy_sasiad_wiersz][lewy_sasiad_kolumna] == LISTA_OTW
					&& drogi[lewy_sasiad_wiersz][lewy_sasiad_kolumna] > drogi[najlepszy_wiersz][najlepsza_kolumna] + 1)) {

				pola[lewy_sasiad_wiersz][lewy_sasiad_kolumna] = LISTA_OTW; // dodaje do otwartej
				drogi[lewy_sasiad_wiersz][lewy_sasiad_kolumna] = drogi[najlepszy_wiersz][najlepsza_kolumna] + 1;
				sumy[lewy_sasiad_wiersz][lewy_sasiad_kolumna] = heurystyki[lewy_sasiad_wiersz][lewy_sasiad_kolumna] + drogi[lewy_sasiad_wiersz][lewy_sasiad_kolumna];
				rodzice[lewy_sasiad_wiersz][lewy_sasiad_kolumna] = PO_PRAWEJ;
			}
		}

		if (najlepsza_kolumna + 1 <= 19) { //czy moge pojsc w prawo
			unsigned short prawy_sasiad_wiersz = najlepszy_wiersz;
			unsigned short prawy_sasiad_kolumna = najlepsza_kolumna + 1;
			if (pola[prawy_sasiad_wiersz][prawy_sasiad_kolumna] == PUSTE
				|| (pola[prawy_sasiad_wiersz][prawy_sasiad_kolumna] == LISTA_OTW
					&& drogi[prawy_sasiad_wiersz][prawy_sasiad_kolumna] > drogi[najlepszy_wiersz][najlepsza_kolumna] + 1)) {
				pola[prawy_sasiad_wiersz][prawy_sasiad_kolumna] = LISTA_OTW; // dodaje do otwartej
				drogi[prawy_sasiad_wiersz][prawy_sasiad_kolumna] = drogi[najlepszy_wiersz][najlepsza_kolumna] + 1;
				sumy[prawy_sasiad_wiersz][prawy_sasiad_kolumna] = heurystyki[prawy_sasiad_wiersz][prawy_sasiad_kolumna] + drogi[prawy_sasiad_wiersz][prawy_sasiad_kolumna];
				rodzice[prawy_sasiad_wiersz][prawy_sasiad_kolumna] = PO_LEWEJ;
			}
		}

		if (najlepszy_wiersz - 1 >= 0) { //czy moge pojsc w gore
			unsigned short gorny_sasiad_wiersz = najlepszy_wiersz - 1;
			unsigned short gorny_sasiad_kolumna = najlepsza_kolumna;

			if (pola[gorny_sasiad_wiersz][gorny_sasiad_kolumna] == PUSTE
				|| (pola[gorny_sasiad_wiersz][gorny_sasiad_kolumna] == LISTA_OTW
					&& drogi[gorny_sasiad_wiersz][gorny_sasiad_kolumna] > drogi[najlepszy_wiersz][najlepsza_kolumna] + 1)) {
				pola[gorny_sasiad_wiersz][gorny_sasiad_kolumna] = LISTA_OTW; // dodaje do otwartej
				drogi[gorny_sasiad_wiersz][gorny_sasiad_kolumna] = drogi[najlepszy_wiersz][najlepsza_kolumna] + 1;
				sumy[gorny_sasiad_wiersz][gorny_sasiad_kolumna] = heurystyki[gorny_sasiad_wiersz][gorny_sasiad_kolumna] + drogi[gorny_sasiad_wiersz][gorny_sasiad_kolumna];
				rodzice[gorny_sasiad_wiersz][gorny_sasiad_kolumna] = NA_DOLE;
			}
		}

		if (najlepszy_wiersz + 1 <= 19) { //czy moge pojsc w dol
			unsigned short dolny_sasiad_wiersz = najlepszy_wiersz + 1;
			unsigned short dolny_sasiad_kolumna = najlepsza_kolumna;

			if (pola[dolny_sasiad_wiersz][dolny_sasiad_kolumna] == PUSTE
				|| (pola[dolny_sasiad_wiersz][dolny_sasiad_kolumna] == LISTA_OTW
					&& drogi[dolny_sasiad_wiersz][dolny_sasiad_kolumna] > drogi[najlepszy_wiersz][najlepsza_kolumna] + 1)) {
				pola[dolny_sasiad_wiersz][dolny_sasiad_kolumna] = LISTA_OTW; // dodaje do otwartej
				drogi[dolny_sasiad_wiersz][dolny_sasiad_kolumna] = drogi[najlepszy_wiersz][najlepsza_kolumna] + 1;
				sumy[dolny_sasiad_wiersz][dolny_sasiad_kolumna] = heurystyki[dolny_sasiad_wiersz][dolny_sasiad_kolumna] + drogi[dolny_sasiad_wiersz][dolny_sasiad_kolumna];
				rodzice[dolny_sasiad_wiersz][dolny_sasiad_kolumna] = NA_GORZE;
			}
		}
	}

	if (znaleziono_trase) {
		unsigned short biezacy_wiersz = koniec_wiersz;
		unsigned short biezaca_kolumna = koniec_kolumna;
		while (biezaca_kolumna != poczatek_kolumna || biezacy_wiersz != poczatek_wiersz) { // dopoki nie dojdziemy do startu
			pola[biezacy_wiersz][biezaca_kolumna] = TRASA;
			if (rodzice[biezacy_wiersz][biezaca_kolumna] == NA_GORZE) biezacy_wiersz--;
			else if (rodzice[biezacy_wiersz][biezaca_kolumna] == NA_DOLE) biezacy_wiersz++;
			else if (rodzice[biezacy_wiersz][biezaca_kolumna] == PO_LEWEJ) biezaca_kolumna--;
			else if (rodzice[biezacy_wiersz][biezaca_kolumna] == PO_PRAWEJ) biezaca_kolumna++;
		}
		pola[poczatek_wiersz][poczatek_kolumna] = TRASA;


		//wypisanie wyniku na ekran
		for (unsigned short wiersz = 0; wiersz < 20; wiersz++) {
			for (unsigned short kolumna = 0; kolumna < 20; kolumna++) {
				switch (pola[wiersz][kolumna]) {
				case TRASA: // element trasy
					cout << "* ";
					break;
				case PRZESZKODA: // przeszkoda
					cout << "# ";
					break;
				default: // pozostale
					cout << "  ";
					break;
				}
			}

			cout << "\n";
		}


		system("PAUSE");
		return 0;
	}

	else if (brak_trasy) {
		cout << "nie ma trasy z punktu poczatkowego do koncowego\n";
		system("PAUSE");
		return 0;
	}
}
