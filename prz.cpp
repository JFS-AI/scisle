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
	const auto wartoscDoPorownan() const { return y; }
};

struct przedzial {
	int l, r;
	double jakosc;
	const auto wartoscDoPorownan() const { return jakosc; }
};

// to jest tak naprawde kolejka-kmax - w czasie O(1) zwraca maksimum

template <typename T, typename Komparator>
requires strict_weak_order<Komparator, T, T>
class MonotonicznaDeque {
	struct Element {
		T wart;
		int id;
	};

	deque<Element> dq; 
	Komparator komp;
	const int U;

public:
	explicit MonotonicznaDeque(Komparator k, int u = 0) : komp(k), U(u) {}

	int topId() const {
		assert(dq.size());
		return dq.front().id;
	}
	void push(T wart, int id) {
		while(dq.size() && komp(wart, dq.back().wart)) 
			dq.pop_back();

		dq.emplace_back(wart, id);
	}
	void pop(int id) {
		assert(dq.size());
		if(id == topId())
			dq.pop_front();
	}
	bool isPushable(T wart) const { // sprawdza czy kolejny punkt jest większy od maksimum minus U
		return dq.empty() || komp(wart, dq.front().wart - U);
	}
};

// rozbudowanie kolejki-kmax

template<typename T>
class KolejkaPrzedzialy {
	int R = -1, L = 0;
	using TypWartosci = decltype(declval<T>().wartoscDoPorownan());
	MonotonicznaDeque<TypWartosci, less<TypWartosci>> minima;
	MonotonicznaDeque<TypWartosci, greater<TypWartosci>> maksima;
	const vector<T>& v;

public:
	explicit KolejkaPrzedzialy(const vector<T>& x, int u = 0) 
	: minima(less<TypWartosci>{}, -u), maksima(greater<TypWartosci>{}, u), v(x) {}

	void pop() {
		assert(L <= R);
		minima.pop(L);
		maksima.pop(L);
		L++;
	}
	bool isPushable(punkt p) const { // badamy czy kolejny punkt będzie wciąż tworzył dobry przedział
		return minima.isPushable(p.y) && maksima.isPushable(p.y);
	}
	void push() {
		R++;
		minima.push(v[R].wartoscDoPorownan(), R);
		maksima.push(v[R].wartoscDoPorownan(), R);
	}
	pair<int, int> zwrocNajlepszyPrzedzial() const {
		const przedzial& p = v[maksima.topId()];
		return {p.l + 1, p.r + 1}; // ponieważ liczymy od 1 (nie od 0)
	}
	void wrzucPrzedzialDoVec(vector<przedzial>& vecPrzedzialow) const {
		assert(L <= R);
		int dlugosc = R - L + 1;
		long long dx = v[R].x - v[L].x;
		// jakość podnoszę do kwadratu, aby uniknać pierwiastka
		double jakosc = pow(dx, 2) / static_cast<double>(dlugosc);
		vecPrzedzialow.emplace_back(L, R, jakosc);
	}
};

vector<punkt> wczytajWejscie(int n) {
	vector<punkt> v;
	v.reserve(n);
	for(int i = 0; i < n; i++) {
		int x, y;
		cin >> x >> y;
		v.emplace_back(x, y);
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

vector<pair<int, int>> solve(const int n, const vector<przedzial>& przedzialy) {
	const int rozmiarVektora = static_cast<int>(przedzialy.size());
	vector<pair<int, int>> acc;
	acc.reserve(n);
	int indexFirstToPush = 0, indexFirstToPop = 0;
	KolejkaPrzedzialy<przedzial> kolejka(przedzialy);
	// dla każdego punktu chcemy wypisać najlepszy przedział
	for(int i = 0; i < n; i++) {
		// jeżeli na tym indeksie rozpoczyna się nowy przedział, to go wpychamy na kolejke (*)
		if(indexFirstToPush < rozmiarVektora && i == przedzialy[indexFirstToPush].l) {
			kolejka.push();
			indexFirstToPush++;
		}

		// autodeskryptywna metoda
		acc.emplace_back(kolejka.zwrocNajlepszyPrzedzial());

		// * analogicznie jak się jakiś kończy
		if(indexFirstToPop < rozmiarVektora && i == przedzialy[indexFirstToPop].r) {
			kolejka.pop();
			indexFirstToPop++;
		}
	}
	return acc;
}

int main() {
	ios_base::sync_with_stdio(0); 
	cin.tie(0);

	int n, u;
	cin >> n >> u;
	u++; // zwiększamy U o jeden, ponieważ korzystamy z ostrych nierówności
	vector<punkt> punkty = wczytajWejscie(n);
	vector<przedzial> scislePrzedzialy = wygenerujPrzedzialy(u, punkty); // (sic!) nie robi kopii

	assert(scislePrzedzialy[0].l == 0);
	assert(scislePrzedzialy[scislePrzedzialy.size() - 1].r == n-1);

	vector<pair<int, int>> wynik = solve(n, scislePrzedzialy);

	for(auto [l, r] : wynik) {
		cout << l << " " << r << "\n";
	}

	return 0;
}