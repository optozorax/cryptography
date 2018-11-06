#include <iostream>
#include <random>  
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

using namespace boost::multiprecision;
using std::cin;
using std::cout;
using std::endl;

const int ITER = 25;
const int LENGTH = 1024;


// Алгоритм быстрого возведения в степень в поле вычета mod
cpp_int fastPow(cpp_int &num, const cpp_int &deg_param, const cpp_int &mod) {

	cpp_int result = 1;
	cpp_int bit = num % mod;
	cpp_int deg = deg_param;

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
cpp_int getRandomNumber() {

	static boost::random::mt19937 generator(917401);
	static boost::random::uniform_int_distribution<cpp_int> distribution(0, cpp_int(1) << LENGTH);
	return distribution(generator);
}


// Генерация простого числа длинной LENGTH бит
cpp_int getPrimeNumber() {

	cpp_int n;
	do {
		n = getRandomNumber();
	} while (!miller_rabin_test(n, ITER));
	return n;
}


// Поиск p и k
void calc_p_and_k(cpp_int &p, cpp_int &k) {

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
cpp_int calcPrimitiveRoot(cpp_int p, cpp_int k) {

	cpp_int g;
	while (true) {

		g = getPrimeNumber();
		if (fastPow(g, 2, p) == 1 || fastPow(g, k, p) == 1)
			continue;
		return g;
	}
}




void main() {

	cpp_int p, g, B;
	cout << "Enter p: ";
	cin >> p;
	cout << "Enter g: ";
	cin >> g;

	cpp_int a = getRandomNumber();
	cout << "a = " << a << endl;
	cpp_int A = fastPow(g, a, p);
	cout << "A = " << A << endl;

	cout << "Enter B: ";
	cin >> B;

	cpp_int K = fastPow(B, a, p);
	cout << "K = " << K << endl;
}