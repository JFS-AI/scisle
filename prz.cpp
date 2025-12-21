#include <queue>
#include <deque>
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

long long sq(long long l) {
	return l*l;
}

struct punkt {
	int x, y;
};

struct przedzial {
	punkt l, r;
	double jakosc; // do kwadratu
};

static_assert(sizeof(punkt) == 8);
static_assert(sizeof(przedzial) == 24);

int n, U;
vector<przedzial> v;

class kolejka_kmax {
	queue<punkt> q;
	deque<int> minima, maksima;

	void wrzuc_przedzial_do_vectora() {
		v.push_back({q.front(), q.back(), (q.back().x - q.front().x) / sqrt(q.size())});
	}
	void push(punkt p) {
		return;
	}

};

int main() {
	cin >> n >> U;

	return 0;
}
