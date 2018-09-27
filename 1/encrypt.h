#pragma once

#include <vector>
#include <string>
#include <cstdlib>

//-----------------------------------------------------------------------------
// Russian letters
const char rus_a = -32;
const char rus_A = -64;
const char rus_z = -1;
const char rus_Z = -33;
const int alphabetSize = rus_z - rus_a + 1;

//-----------------------------------------------------------------------------
struct SymbolFrequency1
{
	char c1;
	double f;
};

struct SymbolFrequency2
{
	char c1, c2;
	double f;
};

struct SymbolFrequency3
{
	char c1, c2, c3;
	double f;
};

struct Difference
{
	double d1, d2, d3;
};

class Frequency
{
public:
	Frequency();

	void clear();

	void count(const std::string& text);
	void normalize();

	void write(std::string fileName) const;
	void writeSorted(std::string fileName) const;

	void load(std::string fileName);

	//-------------------------------------------------------------------------
	Difference countDifference(const Frequency& a) const;

	//-------------------------------------------------------------------------
	void getF1(std::vector<SymbolFrequency1>& f1) const;
	void getF2(std::vector<SymbolFrequency2>& f2) const;
	void getF3(std::vector<SymbolFrequency3>& f3) const;

	void getSortedF1(std::vector<SymbolFrequency1>& sortedF1) const;
	void getSortedF2(std::vector<SymbolFrequency2>& sortedF2) const;
	void getSortedF3(std::vector<SymbolFrequency3>& sortedF3) const;
private:
	char m_last1;
	char m_last2;
	bool m_isNormalized;
	int64_t m_count1;
	int64_t m_count2;
	int64_t m_count3;

	std::vector<double>								f1;
	std::vector<std::vector<double>>				f2;
	std::vector<std::vector<std::vector<double>>>	f3;
};

std::ofstream& operator<<(std::ofstream& out, const SymbolFrequency1& s);
std::ofstream& operator<<(std::ofstream& out, const SymbolFrequency2& s);
std::ofstream& operator<<(std::ofstream& out, const SymbolFrequency3& s);

std::ifstream& operator>>(std::ifstream& in, SymbolFrequency1& s);
std::ifstream& operator>>(std::ifstream& in, SymbolFrequency2& s);
std::ifstream& operator>>(std::ifstream& in, SymbolFrequency3& s);

//-----------------------------------------------------------------------------
class Cipher
{
public:
	Cipher();

	void mutate(void);
	void generateHalfRandomKey(int swapCount);
	void generateRandomKey(void);
	void setKey(std::string key);

	std::string getKey(void) const;

	char		encrypt(char symbol) const;
	std::string encrypt(std::string text) const;

	char		decrypt(char symbol) const;
	std::string decrypt(std::string text) const;
private:
	std::string m_key;
	std::string m_reverseKey;
};

//-----------------------------------------------------------------------------
double random(void);
int intRandom(int min, int max);

double differenceMetric(Difference diff);

double metric(double a, double b, int64_t& count);

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName);
void writeToFile(std::string fileName, const std::string& text);

char toRussianLetters(char c);
std::string toRussianLetters(std::string text);

// Возвращает случайную часть строки размером size
std::string getRandomPart(const std::string& text, int size);

// Возвращает число от 0 до 1 насколько тексты отличаются. 1 - не отличаются, 0 - полностью разные.
double getAccuracy(const std::string& text1, const std::string& text2);

std::string getAlphabet(void);

//-----------------------------------------------------------------------------
/** Класс для ввода текстов. */
class TextReader
{
public:
	TextReader(bool isTestTexts);

	void readNextText(std::string& text);
	int size(void) const;

private:
	std::string path;
	int count;
	int current;
};

//-----------------------------------------------------------------------------
/** В ходе анализа встречается множество случайных величин. В программе измеряются их значения. Единственное, что мы можем сказать насчет этих случайных величин - некоторые их характеристики.

	Тип RandomValueCharacterization как раз обозначает характеристики случайной величины.

	Функция characterizeRandomValueByData получает эти характеристики по массиву измеренных значений случайной величины.

	Так же есть переопределение оператора << для удобного вывода характеристик случайной величины.
 */

struct RandomValueCharacterization {
	double min; // Минимальное значение
	double max; // Максимальное значение
	double expected; // Среднее значение
	double dispersion; // Среднеквадратичное отклонение
};

RandomValueCharacterization characterizeRandomValueByData(
	const std::vector<double>& data
);

std::ofstream& operator<<(std::ofstream& out, const RandomValueCharacterization& c);