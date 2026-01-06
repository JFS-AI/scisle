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
static_assert(sizeof(przedzial) == 32); // wywal to nigga


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


class KolejkaKMax2 { // zrobilem cos strasznego (DRY placze)		//nazwij to lepiej nigga
	struct Element {
		double jakosc;
		int id;
	};

	queue<pair<przedzial, int>> q;
	deque<Element> maksima;

public:
	void pop() {
		assert(q.size());
		assert(maksima.size());

		if(q.front().second == maksima.front().id)
			maksima.pop_front();
		q.pop();
	}
	void push(const przedzial& p) {
		static int i = 0;
		while(maksima.size() && p.jakosc > maksima.back().jakosc) 
			maksima.pop_back();

		maksima.emplace_back(p.jakosc, i);
		q.emplace(p, i);
		i++;
	}
	void wypiszNajlepszy(const vector<przedzial>& v) {
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

int main() {	// za długie nigga
	ios_base::sync_with_stdio(0); 
	cin.tie(0);

	int n, u;
	cin >> n >> u;
	vector<przedzial> scislePrzedzialy;
	KolejkaKMinMax k(u);
	for(int i = 0; i < n; i++) {
		punkt p = wczytajPunkt();
		if(!k.isPushable(p)) {
			k.wrzucPrzedzialDoVec(scislePrzedzialy); // wrzucic do funkcji
			do
				k.pop();
			while(!k.isPushable(p));
		}
		k.push(p);
	}
	k.wrzucPrzedzialDoVec(scislePrzedzialy); // bo trzeba

	int rozmiarVektora = static_cast<int>(scislePrzedzialy.size());

	assert(scislePrzedzialy[0].l.index == 0);
	assert(scislePrzedzialy[rozmiarVektora - 1].r.index == n-1);

	int index_first_to_push = 0, index_first_to_pop = 0;
	KolejkaKMax2 k2;
	for(int i = 0; i < n; i++) {
		if(index_first_to_push < rozmiarVektora && i == scislePrzedzialy[index_first_to_push].l.index)
			k2.push(scislePrzedzialy[index_first_to_push++]);

		k2.wypiszNajlepszy(scislePrzedzialy);

		if(index_first_to_pop < rozmiarVektora && i == scislePrzedzialy[index_first_to_pop].r.index) {
			k2.pop();
			index_first_to_pop++;
		}
	}
	return 0;
}
