#include <deque>
#include <queue>
#include <vector>
#include <cassert>
#include <iostream>
#include <functional>
#include <concepts>
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

template <typename T, typename Komparator>
requires predicate<Komparator, T, T>
class MonotnicznaDeque {	// polski język trudny język nigga
	struct Element {
		T wart;
		int id;
	};

	deque<Element> dq;
	Komparator komp;
	const int U;

public:
	MonotnicznaDeque(Komparator k, int u = 0) : komp(k), U(u) {}

	int topId() const {
		assert(dq.size());
		return dq.front().id;
	}
	void push(T t, int id) {
		while(dq.size() && komp(t, dq.back().wart)) 
			dq.pop_back();

		dq.emplace_back(t, id);
	}
	void pop(int id) {
		if(id == topId())
			dq.pop_front();
	}
	bool isPushable(T t) const { // jakies wyjasnienie co to robi w komentarzu?
								 // w kolejnej klasie masz funkcję z tą samą nazwą ale robiącą coś innego
								 // confusing af nigga
		return dq.empty() || komp(t, dq.front().wart - U);
	}
};

class KolejkaKMinMax { // nie lepiej połączyć tą klasę z KolejkaPrzedzialy? nigga
	queue<punkt> q;
	MonotnicznaDeque<int, less_equal<int>> minima;
	MonotnicznaDeque<int, greater_equal<int>> maksima;

	double policzJakosc() const { // jakosc podnosze do kwadratu, aby uniknac pierwiastka
		return pow(q.back().x - q.front().x, 2) / static_cast<double>(q.size());
	}

public:
	KolejkaKMinMax(int x) : minima(less_equal<int>{}, -x), maksima(greater_equal<int>{}, x) {}

	void wrzucPrzedzialDoVec(vector<przedzial>& v) const {
		assert(q.size());
		v.emplace_back(q.front(), q.back(), policzJakosc());
	}
	void pop() {
		assert(q.size());

		const int& id = q.front().index;
		minima.pop(id);
		maksima.pop(id);
		q.pop();
	}
	bool isPushable(punkt p) const { // badamy czy kolejny punkt bedzie wciaz tworzyl dobry przedzial
		return minima.isPushable(p.y) && maksima.isPushable(p.y);
	}
	void push(punkt p) {
		minima.push(p.y, p.index);
		maksima.push(p.y, p.index);

		q.push(p);
	}
};


class KolejkaPrzedzialy { // zrobilem cos strasznego (DRY placze (w teorii))  // zmieniłem nazwę klasy na coś bardziej sensownego nigga
	int poczatekGasienicy = -1, koniecGasienicy = 0;
	MonotnicznaDeque<double, greater<double>> maksima;
	const vector<przedzial>& v;

public:
	KolejkaPrzedzialy(const vector<przedzial>& x) : maksima(greater<double>{}), v(x) {}

	void pop() {
		assert(koniecGasienicy <= poczatekGasienicy);
		maksima.pop(koniecGasienicy++);	
	}
	void push() {
		poczatekGasienicy++;
		przedzial p = v[poczatekGasienicy];
		maksima.push(p.jakosc, poczatekGasienicy);
	}
	void wypiszNajlepszy() const {
		const przedzial& p = v[maksima.topId()];
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
		const punkt p = wczytajPunkt();			// nigga wczytujemy w osobnej funkcji
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
	
	const int rozmiarVektora = static_cast<int>(scislePrzedzialy.size());

	assert(scislePrzedzialy[0].l.index == 0);
	assert(scislePrzedzialy[rozmiarVektora - 1].r.index == n-1);

	int indexFirstToPush = 0, indexFirstToPop = 0;
	KolejkaPrzedzialy kolejka(scislePrzedzialy);  // daj jakiś komentarz co się tu dzieje nigga
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