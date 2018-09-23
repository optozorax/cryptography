#include <algorithm>
#include <fstream>
#include "encrypt.h"

int main() {
	// Сортируем различие букв на разных текстах

	{
	//-------------------------------------------------------------------------
	// Вводим данные
	std::ifstream fin("difference_letters.txt");

	struct LetterDifference
	{
		char c;
		double a, b;
	};
	std::vector<LetterDifference> ld;
	for (int i = 0; i < alphabetSize; ++i) {
		LetterDifference temp;
		fin >> temp.c >> temp.a >> temp.b;
		ld.push_back(temp);
	}

	fin.close();

	//-------------------------------------------------------------------------
	// Сортируем
	std::sort(ld.begin(), ld.end(), [] (const auto& a, const auto& b) -> bool {
		return a.a > b.a;
	});

	std::vector<LetterDifference> ld2 = ld;
	std::sort(ld2.begin(), ld2.end(), [] (const auto& a, const auto& b) -> bool {
		return a.b > b.b;
	});

	//-------------------------------------------------------------------------
	// Выводим
	std::ofstream fout("difference_letters_sorted.txt");

	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);

	for (auto& i : ld)
		fout << i.c << "\t" << std::fixed << i.a << "\t" << i.b << std::endl;

	fout << std::endl;

	for (auto& i : ld2)
		fout << i.c << "\t" << std::fixed << i.a << "\t" << i.b << std::endl;

	fout.close();
	}

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	// Сортируем различие букв на разных текстах

	{
	//-------------------------------------------------------------------------
	// Вводим данные
	std::ifstream fin("difference_letters_sum.txt");

	struct LetterDifference
	{
		char c;
		double a;
	};
	std::vector<LetterDifference> ld;
	for (int i = 0; i < alphabetSize; ++i) {
		LetterDifference temp;
		fin >> temp.c >> temp.a;
		ld.push_back(temp);
	}

	fin.close();

	//-------------------------------------------------------------------------
	// Сортируем
	std::sort(ld.begin(), ld.end(), [] (const auto& a, const auto& b) -> bool {
		return a.a > b.a;
	});

	//-------------------------------------------------------------------------
	// Выводим
	std::ofstream fout("difference_letters_sum_sorted.txt");

	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);

	for (auto& i : ld)
		fout << i.c << "\t" << std::fixed << i.a << std::endl;

	fout.close();
	}
}