#include <iostream>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>


using namespace boost::multiprecision;
using namespace boost::random;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
typedef cpp_int data;
#define ITER 25
#define LENGTH 1024


// Алгоритм быстрого возведения в степень в поле вычета mod
data& fastPow(data &num, const data &deg_param, const data &mod) {

	data result = 1;
	data bit = num % mod;
	data deg = deg_param;

	while (deg > 0) {

		if ((deg & 1) == 1) {
			result *= bit;
			result %= mod;
		}

		bit *= bit;
		bit %= mod;
		deg >>= 1;
	}
	return result;
}


// Генерация случайного числа
data& getRandomNumber() {

	mt11213b base_gen(clock());
	independent_bits_engine<mt11213b, LENGTH, data> gen(base_gen);
	return gen();
}

// Генерация простого числа длинной LENGTH бит
data& getPrimeNumber() {

	mt11213b base_gen(clock());
	independent_bits_engine<mt11213b, LENGTH, data> gen(base_gen);
	data n;
	do {
		n = gen();
	} while (!miller_rabin_test(n, ITER));
	return n;
}


// Поиск p и k
void get_p_and_k(data &p, data &k) {

	int i = 0;
	k = 1, p = 4;
	while (!miller_rabin_test(p, ITER)) {
		k = getPrimeNumber();
		p = 2 * k + 1;
		cout << ++i << "\r";
	}
	cout << "Iterations count:" << i << endl;
}


// Поиск примитивного корня g
data& getPrimitiveRoot(data p, data k) {

	data g;
	while (true) {

		g = getPrimeNumber();
		if (fastPow(g, 2, p) == 1 || fastPow(g, k, p) == 1)
			continue;
		return g;
	}
}



void main() {

	data p, g, B;
	cout << "Enter p: ";
	cin >> p;
	cout << "Enter g: ";
	cin >> g;

	data a = getRandomNumber();
	cout << "a = " << a << endl;
	data A = fastPow(g, a, p);
	cout << "A = " << A << endl;

	cout << "Enter B: ";
	cin >> B;

	data K = fastPow(B, a, p);
	cout << "K = " << K << endl;
}