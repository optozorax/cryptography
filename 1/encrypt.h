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
	int64_t getProcessedSymbolsCount(void) const;

	void write(std::string fileName) const;
	void writeSorted(std::string fileName) const;
	void writeAsCode(std::string fileName) const;

	void load(std::string fileName);

	//-------------------------------------------------------------------------
	struct Difference
	{
		double d1, d2, d3;
	};

	Difference countDifference(const Frequency& a) const;
	Difference countDifference(const Frequency& a, char symbol) const;

	//-------------------------------------------------------------------------
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

	void getF1(std::vector<SymbolFrequency1>& f1) const;
	void getF2(std::vector<SymbolFrequency2>& f2) const;
	void getF3(std::vector<SymbolFrequency3>& f3) const;

	void getSortedF1(std::vector<SymbolFrequency1>& sortedF1) const;
	void getSortedF2(std::vector<SymbolFrequency2>& sortedF2) const;
	void getSortedF3(std::vector<SymbolFrequency3>& sortedF3) const;

	//-------------------------------------------------------------------------
	std::vector<double> f1;
	std::vector<std::vector<double>> f2;
	std::vector<std::vector<std::vector<double>>> f3;
private:
	char m_last1, m_last2;
	int64_t m_count;
	bool m_isNormalized;
};

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
double metric(double a, double b, int64_t& count);

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName);
void writeToFile(std::string fileName, const std::string& text);

char toRussianLetters(char c);
std::string toRussianLetters(std::string text);

std::string getRandomPart(const std::string& text, int size);

std::string getAlphabet(void);

//-----------------------------------------------------------------------------
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

double getAccuracy(const std::string& text1, const std::string& text2);