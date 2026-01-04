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

int U;
vector<przedzial> v;

class KolejkaKMinMax {
	queue<punkt> q;
	deque<int> minima, maksima;

	double policz_jakosc() { // jakosc podnosze do kwadratu, aby uniknac pierwiastka
		return pow(q.back().x - q.front().x, 2) / static_cast<double>(q.size());
	}

public:
	void wrzucPrzedzialDoVec() {
		assert(q.size());
		v.emplace_back(q.front(), q.back(), policz_jakosc());
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


class KolejkaKMax2 { // zrobilem cos strasznego (DRY placze)
	queue<pair<przedzial, int>> q;
	deque<pair<double, int>> maksima;

public:
	void pop() {
		assert(q.size());
		assert(maksima.size());

		if(q.front().second == maksima.front().second)
			maksima.pop_front();
		q.pop();
	}
	void push(const przedzial& p) {
		static int i = 0;
		while(maksima.size() && p.jakosc > maksima.back().first) 
			maksima.pop_back();

		maksima.emplace_back(p.jakosc, i);
		q.emplace(p, i);
		i++;
	}
	pair<int, int> najlepszy() {
		assert(maksima.size());
		przedzial p = v[maksima.front().second];
		return {p.l.index + 1, p.r.index + 1}; // poniewaz liczymy od 1 (nie od 0)
	}
};

ostream& operator<<(ostream& os, const pair<int, int>& p) {
    os << p.first << " " << p.second;
    return os;
}

int main() {
	int n;
	cin >> n >> U;
	KolejkaKMinMax k;
	for(int i = 0; i < n; i++) {
		punkt p = {0, 0, i};
		cin >> p.x >> p.y;
		if(!k.isPushable(p)) {
			k.wrzucPrzedzialDoVec();
			while(!k.isPushable(p))
				k.pop();
		}
		k.push(p);
	}
	k.wrzucPrzedzialDoVec(); // bo trzeba

	assert(v[0].l.index == 0);
	assert(v[v.size() - 1].r.index == n-1);
	int index_first_to_push = 0, index_first_to_pop = 0;
	KolejkaKMax2 k2;
	for(int i = 0; i < n; i++) {
		if(index_first_to_push < static_cast<int>(v.size()) && i == v[index_first_to_push].l.index)
			k2.push(v[index_first_to_push++]);

		cout << k2.najlepszy() << "\n";

		if(index_first_to_pop < static_cast<int>(v.size()) && i == v[index_first_to_pop].r.index) {
			k2.pop();
			index_first_to_pop++;
		}
	}
	return 0;
}
