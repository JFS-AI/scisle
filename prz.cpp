#include <deque>
#include <vector>
#include <cassert>
#include <iostream>
#include <functional>
#include <concepts>
#include <type_traits>
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
class MonotonicznaDeque {
	struct Element {
		T wart;
		int id;
	};

	deque<Element> dq;
	Komparator komp;
	const int U;

public:
	MonotonicznaDeque(Komparator k, int u = 0) : komp(k), U(u) {}

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
		assert(dq.size());
		if(id == topId())
			dq.pop_front();
	}
	bool isPushable(T t) const { // sprawdza czy kolejny punkt jest wiekszy od maksimum minus U
		return dq.empty() || komp(t, dq.front().wart - U);
	}
};

template<typename T>
class KolejkaPrzedzialy { // trzeba nazwe zmienic
	int R = -1, L = 0;
	using TypWartosci = conditional_t<is_same_v<T, przedzial>, double, int>;
	MonotonicznaDeque<TypWartosci, less<TypWartosci>> minima;
	MonotonicznaDeque<TypWartosci, greater<TypWartosci>> maksima;
	const vector<T>& v;

	TypWartosci pobierzWartosc(const T& p) const {
        if constexpr (is_same_v<T, przedzial>) {
            return p.jakosc;
        } else { // <punkt>
            return p.y;
        }
    }

public:
	KolejkaPrzedzialy(const vector<T>& x, int u = 0) 
	: minima(less<TypWartosci>{}, -u), maksima(greater<TypWartosci>{}, u), v(x) {}

	void pop() {
		assert(L <= R);
		minima.pop(L);
		maksima.pop(L);
		L++;
	}
	bool isPushable(punkt p) const { // badamy czy kolejny punkt bedzie wciaz tworzyl dobry przedzial
		return minima.isPushable(p.y) && maksima.isPushable(p.y);
	}
	void push() {
		R++;
		minima.push(pobierzWartosc(v[R]), R);
		maksima.push(pobierzWartosc(v[R]), R);
	}
	void wypiszNajlepszyPrzedzial() const {
		const przedzial& p = v[maksima.topId()];
		cout << p.l.index + 1 << " " << p.r.index + 1 << "\n"; // poniewaz liczymy od 1 (nie od 0)
	}
	void wrzucPrzedzialDoVec(vector<przedzial>& vecPrzedzialow) const { // jakosc podnosze do kwadratu, aby uniknac pierwiastka
		assert(L <= R);
		int dlugosc = R - L + 1;
		long long dx = v[R].x - v[L].x;
		double jakosc = pow(dx, 2) / static_cast<double>(dlugosc);
		vecPrzedzialow.emplace_back(v[L], v[R], jakosc);
	}
};

vector<punkt> wczytajWejscie(int n) {
	vector<punkt> v(n);
	v.resize(n);
	for(int i = 0; i < n; i++) {
		cin >> v[i].x >> v[i].y;
		v[i].index = i;
	}
	return v;
}

vector<przedzial> wygenerujPrzedzialy(int u, const vector<punkt>& vecPunktow) {
	KolejkaPrzedzialy<punkt> k(vecPunktow, u);
	vector<przedzial> vecPrzedzialow;
	for(punkt p : vecPunktow) {
		if(!k.isPushable(p)) {
			k.wrzucPrzedzialDoVec(vecPrzedzialow);
			do
				k.pop();
			while(!k.isPushable(p));
		}
		k.push();
	}
	k.wrzucPrzedzialDoVec(vecPrzedzialow); // bo trzeba

	return vecPrzedzialow;
}

int main() {	// za długie nigga
	ios_base::sync_with_stdio(0); 
	cin.tie(0);

	int n, u;
	cin >> n >> u;
	u++; // przydaloby sie slowo komentarza
	vector<punkt> punkty = wczytajWejscie(n);
	vector<przedzial> scislePrzedzialy = wygenerujPrzedzialy(u, punkty); // (sic!) nie robi kopii
	
	const int rozmiarVektora = static_cast<int>(scislePrzedzialy.size());

	assert(scislePrzedzialy[0].l.index == 0);
	assert(scislePrzedzialy[rozmiarVektora - 1].r.index == n-1);

	int indexFirstToPush = 0, indexFirstToPop = 0;
	KolejkaPrzedzialy<przedzial> kolejka(scislePrzedzialy);  // daj jakiś komentarz co się tu dzieje nigga
	for(int i = 0; i < n; i++) {
		if(indexFirstToPush < rozmiarVektora && i == scislePrzedzialy[indexFirstToPush].l.index) {
			kolejka.push();
			indexFirstToPush++;
		}

		kolejka.wypiszNajlepszyPrzedzial();

		if(indexFirstToPop < rozmiarVektora && i == scislePrzedzialy[indexFirstToPop].r.index) {
			kolejka.pop();
			indexFirstToPop++;
		}
	}
	return 0;
}