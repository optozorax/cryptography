#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

using std::cin;
using std::cout;
using std::endl;
using std::pair;
typedef uint64_t data;
#define L 0
#define R INT32_MAX


// Алгоритм быстрого возведения в степень в поле вычета mod
data fastPow(data num, data deg, data mod) {

	data result = 1;
	data bit = num % mod;

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
bool isPrime(data num) {

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
data getRandomNumber() {

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(L, R);
	return distribution(generator);
}


// Генерация простого числа длинной LENGTH бит
data getPrimeNumber() {
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(L, R);

	data n;
	do {
		n = distribution(generator);
	} while (!isPrime(n));
	return n;
}


// Поиск p и k
void get_p_and_k(data &p, data &k) {

	int i = 0;
	k = 1, p = 4;
	while (!isPrime(p)) {
		k = getPrimeNumber();
		p = 2 * k + 1;
	}
}


// Поиск примитивного корня g
data getPrimitiveRoot(data p, data k) {

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