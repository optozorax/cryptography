#include <iostream>
#include <random>
#include <cmath>

using std::cin;
using std::cout;
using std::endl;


// Алгоритм быстрого возведения в степень в поле вычета mod
uint64_t fastPow(uint64_t num, uint64_t deg, uint64_t mod) {

	uint64_t result = 1;
	uint64_t bit = num % mod;

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


// Проверка числа на простоту при помощи решета Эрастофена
bool isPrime(uint64_t num) {

	if (num <= 3) {			// 2 и 3 простые
		return num > 1;		// а 1 - нет
	}
	else if (num % 2 == 0 || num % 3 == 0) {
		return false;
	}
	else {
		for (int i = 5; i * i <= num; i += 6) {
			if (num % i == 0 || num % (i + 2) == 0) {
				return false;
			}
		}
		return true;
	}
}


// Генерация случайного числа
uint64_t getRandomNumber() {

	static std::mt19937 generator(917401);
	static std::uniform_int_distribution<> distribution(0, INT32_MAX);
	return distribution(generator);
}


// Генерация простого числа длинной LENGTH бит
uint64_t getPrimeNumber() {

	uint64_t n;
	do {
		n = getRandomNumber();
	} while (!isPrime(n));
	return n;
}


// Поиск p и k
void calc_p_and_k(uint64_t &p, uint64_t &k) {

	int i = 0;
	k = 1, p = 4;
	while (!isPrime(p)) {
		k = getPrimeNumber();
		p = 2 * k + 1;
	}
}


// Поиск примитивного корня g
uint64_t calcPrimitiveRoot(uint64_t p, uint64_t k) {

	uint64_t g;
	while (true) {

		g = getPrimeNumber();
		if (fastPow(g, 2, p) == 1 || fastPow(g, k, p) == 1)
			continue;
		return g;
	}
}



void main() {

	uint64_t p, g, B;
	cout << "Enter p: ";
	cin >> p;
	cout << "Enter g: ";
	cin >> g;

	uint64_t a = getRandomNumber();
	cout << "a = " << a << endl;
	uint64_t A = fastPow(g, a, p);
	cout << "A = " << A << endl;

	cout << "Enter B: ";
	cin >> B;

	uint64_t K = fastPow(B, a, p);
	cout << "K = " << K << endl;
}