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
class Frequency
{
public:
	Frequency();

	void clear();

	void count(const std::string& text);
	void normalize();

	void write(std::string fileName) const;
	void writeSorted(std::string fileName) const;
	void writeAsCode(std::string fileName) const;

	void load(std::string fileName);

	std::pair<double, double> countDifference(const Frequency& a) const;
	std::pair<double, double> countDifference(const Frequency& a, char symbol) const;

	int64_t getCount(void) const;

	//-------------------------------------------------------------------------
	struct LetterAndNumber
	{
		char c;
		double f;
	};

	struct TwoLetterAndNumber
	{
		char c1, c2;
		double f;
	};

	void getF1(std::vector<LetterAndNumber>& f1) const;
	void getF2(std::vector<TwoLetterAndNumber>& f2) const;

	void getSortedF1(std::vector<LetterAndNumber>& sortedF1) const;
	void getSortedF1(std::vector<TwoLetterAndNumber>& sortedF1) const;

	//-------------------------------------------------------------------------
	std::vector<double> f1;
	std::vector<std::vector<double>> f2;
private:
	char m_last;
	int64_t m_count;
	bool m_isNormalized;
};

//-----------------------------------------------------------------------------
class Cipher
{
public:
	Cipher();

	void generateHalfRandomKey(int swapCount);
	void generateRandomKey(void);
	void setKey(std::string key);
	std::string getKey(void) const;

	char encrypt(char symbol) const;
	std::string encrypt(std::string text) const;

	char decrypt(char symbol) const;
	std::string decrypt(std::string text) const;
private:
	std::string m_key;
	std::string m_reverseKey;
};

//-----------------------------------------------------------------------------
double random(void);
int intRandom(int min, int max);
double metric(double a, double b);

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName);
void writeToFile(std::string fileName, const std::string& text);

std::vector<std::string> readTexts(void);

char toRussianLetters(char c);
std::string toRussianLetters(std::string text);

std::string getRandomPart(const std::string& text, int size);

std::string getAlphabet(void);

//-----------------------------------------------------------------------------
struct RandomValueCharacterization {
	double min;
	double max;
	double expected;
	double dispersion;
};

RandomValueCharacterization characterizeRandomValueByData(const std::vector<double>& data);

/*void countTextDifference(
	std::vector<double>& diff1,
	std::vector<double>& diff2,
	const Frequency& f,
	std::string text
);*/

void writeSortedFrequency(Frequency f, std::string fileName);