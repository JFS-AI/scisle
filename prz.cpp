#include <queue>
#include <deque>
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

struct punkt {
	int x, y;
};

struct przedzial {
	punkt l, r;
	double jakosc;
};

static_assert(sizeof(punkt) == 8);
static_assert(sizeof(przedzial) == 24);

int n, U;
vector<przedzial> v;

class kolejka_kmax {
	queue<punkt> q;
	deque<int> minima, maksima;

public:
	void wrzuc_przedzial_do_vectora() {
		v.push_back({q.front(), q.back(), (q.back().x - q.front().x) / sqrt(q.size())});
	}
	void pop() {
		int y = q.front().y;
		if(y == minima.front())
			minima.pop_front();
		if(y == maksima.front())
			maksima.pop_front();
		q.pop();
	}
	bool is_bushable(punkt p) {
		return p.y <= minima.front() + U && p.y >= maksima.front() - U;
	}
	void push(punkt p) {
		if(!is_pushable(p)) {
			wrzuc_przedzial_do_vectora();
			while(!is_pushable(p))
				pop();
		}

		while(p.y < minima.back() && minima.size()) 
			minima.pop_back();

		minima.push_back(p.y);

		while(p.y > maksima.back() && maksima.size()) 
			maksima.pop_back();

		maksima.push_back(p.y);

		q.push(p);
	}

};

int main() {
	cin >> n >> U;

	return 0;
}
