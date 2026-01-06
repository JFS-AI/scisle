#include <deque>
#include <queue>
#include <vector>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace std;

struct punkt {
	int x, y;
	int index;
};

struct przedzial {
	punkt l, r;
	double jakosc;
};

static_assert(sizeof(punkt) == 12);
static_assert(sizeof(przedzial) == 32);


class KolejkaKMinMax {
	const int U;
	queue<punkt> q;
	deque<int> minima, maksima;

	double policzJakosc() { // jakosc podnosze do kwadratu, aby uniknac pierwiastka
		return pow(q.back().x - q.front().x, 2) / static_cast<double>(q.size());
	}

public:
	KolejkaKMinMax(int x) : U(x) {}

	void wrzucPrzedzialDoVec(vector<przedzial>& v) {
		assert(q.size());
		v.emplace_back(q.front(), q.back(), policzJakosc());
	}
	void pop() {
		assert(q.size());
		assert(minima.size());
		assert(maksima.size());

		int y = q.front().y;
		if(y == minima.front())
			minima.pop_front();
		if(y == maksima.front())
			maksima.pop_front();
		q.pop();
	}
	bool isPushable(punkt p) { // badamy czy kolejny punkt bedzie wciaz tworzyl dobry przedzial
		return (minima.empty() || p.y <= minima.front() + U) && (maksima.empty() || p.y >= maksima.front() - U);
	}
	void push(punkt p) {
		while(minima.size() && p.y < minima.back()) 
			minima.pop_back();

		minima.push_back(p.y);

		while(maksima.size() && p.y > maksima.back()) 
			maksima.pop_back();

		maksima.push_back(p.y);

		q.push(p);
	}
};


class KolejkaKMax2 { // zrobilem cos strasznego (DRY placze (w teorii))		//nazwij to lepiej nigga
	struct Element {
		double jakosc;
		int id;
	};

	int poczatekGasienicy = -1, koniecGasienicy = 0;
	deque<Element> maksima;
	const vector<przedzial>& v;

public:
	KolejkaKMax2(const vector<przedzial>& x) : v(x) {}

	void pop() {
		assert(koniecGasienicy <= poczatekGasienicy);
		assert(maksima.size());

		if(koniecGasienicy == maksima.front().id)
			maksima.pop_front();

		koniecGasienicy++;
	}
	void push() {
		poczatekGasienicy++;
		przedzial p = v[poczatekGasienicy];
		while(maksima.size() && p.jakosc > maksima.back().jakosc) 
			maksima.pop_back();

		maksima.emplace_back(p.jakosc, poczatekGasienicy);
	}
	void wypiszNajlepszy() {
		assert(maksima.size());
		przedzial p = v[maksima.front().id];
		cout << p.l.index + 1 << " " << p.r.index + 1 << "\n"; // poniewaz liczymy od 1 (nie od 0)
	}
};

punkt wczytajPunkt() {
	static int i = 0;
	int x, y;
	cin >> x >> y;
	return {x, y, i++};
}

vector<przedzial> wygenerujPrzedzialy(int n) {
	int u;
	cin >> u;
	KolejkaKMinMax k(u);
	vector<przedzial> v;
	for(int i = 0; i < n; i++) {
		punkt p = wczytajPunkt();
		if(!k.isPushable(p)) {
			k.wrzucPrzedzialDoVec(v);
			do
				k.pop();
			while(!k.isPushable(p));
		}
		k.push(p);
	}
	k.wrzucPrzedzialDoVec(v); // bo trzeba

	return v;
}

int main() {	// za długie nigga
	ios_base::sync_with_stdio(0); 
	cin.tie(0);

	int n;
	cin >> n;
	vector<przedzial> scislePrzedzialy = wygenerujPrzedzialy(n); // (sic!) nie robi kopii
	
	int rozmiarVektora = static_cast<int>(scislePrzedzialy.size());

	assert(scislePrzedzialy[0].l.index == 0);
	assert(scislePrzedzialy[rozmiarVektora - 1].r.index == n-1);

	int indexFirstToPush = 0, indexFirstToPop = 0;
	KolejkaKMax2 kolejka(scislePrzedzialy);
	for(int i = 0; i < n; i++) {
		if(indexFirstToPush < rozmiarVektora && i == scislePrzedzialy[indexFirstToPush].l.index) {
			kolejka.push();
			indexFirstToPush++;
		}

		kolejka.wypiszNajlepszy();

		if(indexFirstToPop < rozmiarVektora && i == scislePrzedzialy[indexFirstToPop].r.index) {
			kolejka.pop();
			indexFirstToPop++;
		}
	}
	return 0;
}
