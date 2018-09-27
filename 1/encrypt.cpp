#include <fstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "encrypt.h"

//-----------------------------------------------------------------------------
Frequency::Frequency() {
	clear();
}

//-----------------------------------------------------------------------------
void Frequency::clear() {
	m_last1 = '!';
	m_last2 = '!';
	f1.clear();
	f2.clear();
	f3.clear();
	f1.resize(alphabetSize, 0);
	f2.resize(alphabetSize, std::vector<double>(alphabetSize, 0));
	f3.resize(alphabetSize, std::vector<std::vector<double>>(alphabetSize, std::vector<double>(alphabetSize, 0)));
	m_isNormalized = false;
	m_count1 = 0;
	m_count2 = 0;
	m_count3 = 0;
}

//-----------------------------------------------------------------------------
void Frequency::count(const std::string& text) {
	if (m_isNormalized)
		throw std::exception();

	for (auto i : text) {
		i = toRussianLetters(i);
		if (i != '\0') {
			i -= rus_a;

			f1[i]++;
			m_count1++;

			if (m_last2 != '!') {
				f2[m_last2][i]++;
				m_count2++;
			}

			if (m_last1 != '!') {
				f3[m_last1][m_last2][i]++;
				m_count3++;
			}

			m_last1 = m_last2;
			m_last2 = i;
		}	
	}
}

//-----------------------------------------------------------------------------
void Frequency::normalize() {
	if (m_isNormalized)
		throw std::exception();

	for (auto& i : f1)
		i /= m_count1;

	for (auto& i : f2)
		for (auto& j : i)
			j /= m_count2;

	for (auto& i : f3)
		for (auto& j : i)
			for (auto& k : j)
				k /= m_count3;

	m_isNormalized = true;
}

//-----------------------------------------------------------------------------
void Frequency::write(std::string fileName) const {
	std::vector<SymbolFrequency1> sf1;
	std::vector<SymbolFrequency2> sf2;
	std::vector<SymbolFrequency3> sf3;
	getF1(sf1);
	getF2(sf2);
	getF3(sf3);
	
	std::ofstream fout(fileName);

	for (auto& i : sf1)
		fout << i << std::endl;
	fout << std::endl;
	for (auto& i : sf2)
		fout << i << std::endl;
	fout << std::endl;
	for (auto& i : sf3)
		fout << i << std::endl;

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::writeSorted(std::string fileName) const {
	std::vector<SymbolFrequency1> sf1;
	std::vector<SymbolFrequency2> sf2;
	std::vector<SymbolFrequency3> sf3;
	getSortedF1(sf1);
	getSortedF2(sf2);
	getSortedF3(sf3);
	
	std::ofstream fout(fileName);

	for (auto& i : sf1)
		fout << i << std::endl;
	fout << std::endl;
	for (auto& i : sf2)
		fout << i << std::endl;
	fout << std::endl;
	for (auto& i : sf3)
		fout << i << std::endl;

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::load(std::string fileName) {
	clear();

	m_isNormalized = true;

	std::ifstream fin(fileName);

	SymbolFrequency1 s1;
	for (int i = 0; i < alphabetSize; ++i) {
		fin >> s1;
		f1[i] = s1.f;
	}

	SymbolFrequency2 s2;
	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j) {
			fin >> s2;
			f2[i][j] = s2.f;
		}

	SymbolFrequency3 s3;
	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j)
			for (int k = 0; k < alphabetSize; ++k) {
				fin >> s3;
				f3[i][j][k] = s3.f;
			}

	fin.close();
}

//-----------------------------------------------------------------------------
Difference Frequency::countDifference(const Frequency& a) const {
	if (!m_isNormalized || !a.m_isNormalized)
		throw std::exception();

	Difference result = {0, 0, 0};
	int64_t count1 = 0;
	int64_t count2 = 0;
	int64_t count3 = 0;

	for (int i = 0; i < alphabetSize; ++i)
		result.d1 += metric(f1[i], a.f1[i], count1);

	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j)
			result.d2 += metric(f2[i][j], a.f2[i][j], count2);

	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j)
			for (int k = 0; k < alphabetSize; ++k)
				result.d3 += metric(f3[i][j][k], a.f3[i][j][k], count3);

	result.d1 /= count1;
	result.d2 /= count2;
	result.d3 /= count3;

	return result;
}

//-----------------------------------------------------------------------------
void Frequency::getF1(std::vector<SymbolFrequency1>& f) const {
	if (!m_isNormalized)
		throw std::exception();

	f.clear();
	f.reserve(alphabetSize);
	for (int i = 0; i < f1.size(); ++i)
		f.push_back({char(rus_a + i), f1[i]});
}

//-----------------------------------------------------------------------------
void Frequency::getF2(std::vector<SymbolFrequency2>& f) const {
	if (!m_isNormalized)
		throw std::exception();

	f.clear();
	f.reserve(alphabetSize * alphabetSize);
	for (int i = 0; i < f2.size(); ++i)
		for (int j = 0; j < f2[i].size(); ++j)
			f.push_back({char(rus_a + i), char(rus_a + j), f2[i][j]});
}

//-----------------------------------------------------------------------------
void Frequency::getF3(std::vector<SymbolFrequency3>& f) const {
	if (!m_isNormalized)
		throw std::exception();

	f.clear();
	f.reserve(alphabetSize * alphabetSize * alphabetSize);
	for (int i = 0; i < f3.size(); ++i)
		for (int j = 0; j < f3[i].size(); ++j)
			for (int k = 0; k < f3[i][j].size(); ++k)
				f.push_back({char(rus_a + i), char(rus_a + j), char(rus_a + k), f3[i][j][k]});
}

//-----------------------------------------------------------------------------
void Frequency::getSortedF1(std::vector<SymbolFrequency1>& sortedF1) const {
	getF1(sortedF1);
	std::sort(sortedF1.begin(), sortedF1.end(), [] (auto& a, auto& b) -> bool {
		return a.f > b.f;
	});
}

//-----------------------------------------------------------------------------
void Frequency::getSortedF2(std::vector<SymbolFrequency2>& sortedF2) const {
	getF2(sortedF2);
	std::sort(sortedF2.begin(), sortedF2.end(), [] (auto& a, auto& b) -> bool {
		return a.f > b.f;
	});
}

//-----------------------------------------------------------------------------
void Frequency::getSortedF3(std::vector<SymbolFrequency3>& sortedF3) const {
	getF3(sortedF3);
	std::sort(sortedF3.begin(), sortedF3.end(), [] (auto& a, auto& b) -> bool {
		return a.f > b.f;
	});
}

//-----------------------------------------------------------------------------
std::ofstream& operator<<(std::ofstream& out, const SymbolFrequency1& s) {
	int digits10 = std::numeric_limits<double>::digits10;
	out.precision(digits10);
	out << s.c1 << "\t" << std::fixed << std::setw(digits10 + 3) << s.f * 100.0;
	return out;
}

//-----------------------------------------------------------------------------
std::ofstream& operator<<(std::ofstream& out, const SymbolFrequency2& s) {
	int digits10 = std::numeric_limits<double>::digits10;
	out.precision(digits10);
	out << s.c1 << s.c2 << "\t" << std::fixed << std::setw(digits10 + 3) << s.f * 100.0;
	return out;
}

//-----------------------------------------------------------------------------
std::ofstream& operator<<(std::ofstream& out, const SymbolFrequency3& s) {
	int digits10 = std::numeric_limits<double>::digits10;
	out.precision(digits10);
	out << s.c1 << s.c2 << s.c3 << "\t" << std::fixed << std::setw(digits10 + 3) << s.f * 100;
	return out;
}

//-----------------------------------------------------------------------------
std::ifstream& operator>>(std::ifstream& in, SymbolFrequency1& s) {
	char c;
	in >> c >> s.f;
	s.f /= 100;
	return in;
}

//-----------------------------------------------------------------------------
std::ifstream& operator>>(std::ifstream& in, SymbolFrequency2& s) {
	char c;
	in >> c >> c >> s.f;
	s.f /= 100;
	return in;
}

//-----------------------------------------------------------------------------
std::ifstream& operator>>(std::ifstream& in, SymbolFrequency3& s) {
	char c;
	in >> c >> c >> c >> s.f;
	s.f /= 100;
	return in;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Cipher::Cipher() {
}

//-----------------------------------------------------------------------------
void Cipher::mutate(void) {
	auto key = getKey();
	int a = intRandom(0, alphabetSize);
	int b = intRandom(0, alphabetSize);
	std::swap(key[a], key[b]);
	setKey(key);
}

//-----------------------------------------------------------------------------
void Cipher::generateHalfRandomKey(int swapCount) {
	setKey(getAlphabet());

	for (int i = 0; i < swapCount; ++i)
		mutate();
}

//-----------------------------------------------------------------------------
void Cipher::generateRandomKey(void) {
	std::string alphabet = getAlphabet();

	std::random_shuffle(alphabet.begin(), alphabet.end());

	setKey(alphabet);
}

//-----------------------------------------------------------------------------
void Cipher::setKey(std::string key) {
	if (key.size() == alphabetSize) {
		m_key = key;
		std::string reverseKey = key;
		for (int i = 0; i < alphabetSize; ++i)
			reverseKey[key[i] - rus_a] = i + rus_a;
		m_reverseKey = reverseKey;
	}
}

//-----------------------------------------------------------------------------
std::string Cipher::getKey(void) const {
	return m_key;
}

//-----------------------------------------------------------------------------
char Cipher::encrypt(char symbol) const {
	if (symbol >= rus_a && symbol <= rus_z)
		return m_key[symbol - rus_a];
	else
		return '\0';
}

//-----------------------------------------------------------------------------
std::string Cipher::encrypt(std::string text) const {
	for (auto& i : text)
		i = encrypt(i);
	return text;
}

//-----------------------------------------------------------------------------
char Cipher::decrypt(char symbol) const {
	if (symbol >= rus_a && symbol <= rus_z)
		return m_reverseKey[symbol - rus_a];
	else
		return '\0';
}

//-----------------------------------------------------------------------------
std::string Cipher::decrypt(std::string text) const {
	for (auto& i : text)
		i = decrypt(i);
	return text;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double random(void) {
	return std::rand() / double(RAND_MAX + 1);
}

//-----------------------------------------------------------------------------
int intRandom(int min, int max) {
	return min + random() * (max - min);
}

//-----------------------------------------------------------------------------
double differenceMetric(Difference diff) {
	// В ходе экспериментов было обнаружено, что такие веса к переменным разницы дают самую большую эффективность расшифровки
	
	double weight1 = 1;
	double weight2 = 2;
	double weight3 = 3;
	double weightSum = weight1 + weight2 + weight3;

	return (
		weight1 * diff.d1 + 
		weight2 * diff.d2 + 
		weight3 * diff.d3
	) / weightSum;
}

//-----------------------------------------------------------------------------
double metric(double should, double was, int64_t& count) {
	if (should == 0 && was != 0) {
		count++;
		return 1;
	} else
	if (was != 0) {
		count++;
		return fabs(should-was) / std::max(should, was);
	} else
		return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName) {
	std::ifstream fin(fileName);
	std::string result;
	result.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	fin.close();

	return result;
}

//-----------------------------------------------------------------------------
void writeToFile(std::string fileName, const std::string& text) {
	std::ofstream fout(fileName);

	for (auto i : text)
		fout << i;
	 
	fout.close();
}

//-----------------------------------------------------------------------------
char toRussianLetters(char c) {
	if (c == -72)
		return rus_a + 6;
	
	if (c >= rus_a && c <= rus_z)
		return c;
	else {
		if (c >= rus_A && c <= rus_Z)
			return rus_a + (c - rus_A);
		else
			return '\0';
	}
}

//-----------------------------------------------------------------------------
std::string toRussianLetters(std::string text) {
	std::string result;
	for (auto i : text) {
		i = toRussianLetters(i);
		if (i != '\0')
			result.push_back(i);
	}

	return result;
}

//-----------------------------------------------------------------------------
std::string getRandomPart(const std::string& text, int size) {
	if (text.size() < size)
		return text;
	else {
		int pos = intRandom(0, text.size() - size);
		return text.substr(pos, size);
	}
}

//-----------------------------------------------------------------------------
std::string getAlphabet(void) {
	std::string alphabet;
	for (char i = rus_a; i <= rus_z; i++)
		alphabet.push_back(i);
	return alphabet;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TextReader::TextReader(bool isTestTexts) {
	if (!isTestTexts) {
		path = "text/0 (";
		count = 220;
	} else {
		path = "test_text/0 (";
		count = 8;
	}
	current = 0;
}

//-----------------------------------------------------------------------------
void TextReader::readNextText(std::string& text) {
	current++;

	std::stringstream sout;
	sout << path;
	sout << current << ").txt";

	text = toRussianLetters(readFromFile(sout.str()));
}

//-----------------------------------------------------------------------------
int TextReader::size(void) const {
	return count;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RandomValueCharacterization characterizeRandomValueByData(const std::vector<double>& data) {
	RandomValueCharacterization result;
	result.min = data[0];
	result.max = data[0];
	result.expected = 0;
	result.dispersion = 0;

	for (auto& i : data) {
		if (i < result.min)
			result.min = i;
		if (i > result.max)
			result.max = i;
		result.expected += i;
	}
	result.expected /= data.size();

	for (auto& i : data) 
		result.dispersion += (i - result.expected) * (i - result.expected);
	result.dispersion /= data.size();
	result.dispersion = sqrt(result.dispersion);

	return result;
}

//-----------------------------------------------------------------------------
std::ofstream& operator<<(std::ofstream& out, const RandomValueCharacterization& c) {
	out << std::fixed << std::setprecision(4);
	out << "expected:   " << c.expected   * 100 << "%" << std::endl;
	out << "dispersion: " << c.dispersion * 100 << "%" << std::endl;
	out << "min:        " << c.min        * 100 << "%" << std::endl;
	out << "max:        " << c.max        * 100 << "%" << std::endl;

	return out;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double getAccuracy(const std::string& text1, const std::string& text2) {
	if (text1.size() != text2.size())
		throw std::exception();

	int count = 0;
	for (int i = 0; i < text1.size(); ++i) {
		if (text1[i] == text2[i])
			count++;
	}

	return double(count)/text1.size();
}