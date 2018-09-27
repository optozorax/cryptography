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
	f1.resize(alphabetSize, 0);
	f2.resize(alphabetSize, std::vector<double>(alphabetSize, 0));
	f3.resize(alphabetSize, std::vector<std::vector<double>>(alphabetSize, std::vector<double>(alphabetSize, 0)));
	m_isNormalized = false;
	m_count = 0;
}

//-----------------------------------------------------------------------------
void Frequency::count(const std::string& text) {
	if (m_isNormalized)
		throw std::exception();

	for (auto i : text) {
		i = toRussianLetters(i);
		if (i != '\0') {
			i -= rus_a;
			m_count++;
			f1[i]++;
			if (m_last2 != '!')
				f2[m_last2][i]++;
			if (m_last1 != '!') {
				f3[m_last1][m_last2][i]++;
				if (m_last1 == m_last2 && m_last2 == i && m_last1 == rus_a + 16) {
					std::cout << (rus_a + 16);
					system("pause");
				}
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
		i /= m_count;

	for (auto& i : f2)
		for (auto& j : i)
			j /= m_count;

	for (auto& i : f3)
		for (auto& j : i)
			for (auto& k : j)
				k /= m_count;

	m_isNormalized = true;
}

//-----------------------------------------------------------------------------
void Frequency::write(std::string fileName) const {
	if (!m_isNormalized)
		throw std::exception();

	std::ofstream fout(fileName);

	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	for (int i = 0; i < alphabetSize; ++i) {
		fout 
			<< char(rus_a + i) 
			<< "\t" 
			<< std::fixed 
			<< f1[i] 
			<< std::endl;
	}
	fout << std::endl;

	for (int i = 0; i < alphabetSize; ++i) {
		for (int j = 0; j < alphabetSize; ++j)
			fout 
				<< char(rus_a + i) 
				<< char(rus_a + j) 
				<< "\t" 
				<< std::fixed 
				<< f2[i][j] 
				<< std::endl;		
	}
	fout << std::endl;

	for (int i = 0; i < alphabetSize; ++i) {
		for (int j = 0; j < alphabetSize; ++j)
			for (int k = 0; k < alphabetSize; ++k)
				fout 
					<< char(rus_a + i) 
					<< char(rus_a + j)
					<< char(rus_a + k) 
					<< "\t" 
					<< std::fixed 
					<< f3[i][j][k] 
					<< std::endl;		
	}

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::writeSorted(std::string fileName) const {
	std::vector<Frequency::SymbolFrequency1> sf1;
	std::vector<Frequency::SymbolFrequency2> sf2;
	std::vector<Frequency::SymbolFrequency3> sf3;
	getSortedF1(sf1);
	getSortedF2(sf2);
	getSortedF3(sf3);
	
	std::ofstream fout(fileName);

	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);

	for (auto& i : sf1)
		fout << i.c1 << "\t" << std::fixed << i.f << std::endl;

	fout << std::endl;

	for (auto& i : sf2)
		fout << i.c1 << i.c2 << "\t" << std::fixed << i.f << std::endl;

	fout << std::endl;

	for (auto& i : sf3)
		fout << i.c1 << i.c2 << i.c3 << "\t" << std::fixed << i.f << std::endl;

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::writeAsCode(std::string fileName) const {
	if (!m_isNormalized)
		throw std::exception();

	std::ofstream fout(fileName);

	fout << "std::vector<double> f1 = {";
	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	for (auto& i : f1)
		fout << std::fixed << i << ", ";
	fout << "};" << std::endl;

	fout << "std::vector<std::vector<double>> f2 = {" << std::endl;
	for (auto& i : f2) {
		fout << "\t{";
		for (auto& j : i)
			fout << std::fixed << std::setw(digits10+3) << j << ", ";
		fout << "}," << std::endl;
	}
	fout << "};" << std::endl;

	fout << "std::vector<std::vector<std::vector<double>>> f3 = {" << std::endl;
	for (auto& i : f3) {
		fout << "\t{";
		for (auto& j : i) {
			fout << "{";
			for (auto& k : j)
				fout << std::fixed << std::setw(digits10+3) << k << ", ";
			fout << "}, ";
		}
		fout << "}," << std::endl;
	}
	fout << "};";	

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::load(std::string fileName) {
	clear();

	std::ifstream fin(fileName);

	char c;
	for (int i = 0; i < alphabetSize; ++i)
		fin >> c >> f1[i];

	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j)
			fin >> c >> c >> f2[i][j];

	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j)
			for (int k = 0; k < alphabetSize; ++k)
				fin >> c >> c >> c >> f3[i][j][k];

	fin.close();

	m_isNormalized = true;
}

//-----------------------------------------------------------------------------
Frequency::Difference Frequency::countDifference(const Frequency& a) const {
	if (!m_isNormalized || !a.m_isNormalized)
		throw std::exception();

	Difference result = {0, 0, 0};
	int64_t count1 = 0;
	int64_t count2 = 0;
	int64_t count3 = 0;

	for (int i = 0; i < alphabetSize; ++i) {
		result.d1 += metric(f1[i], a.f1[i], count1);
		for (int j = 0; j < alphabetSize; ++j) {
			result.d2 += metric(f2[i][j], a.f2[i][j], count2);
			for (int k = 0; k < alphabetSize; ++k)
				result.d3 += metric(f3[i][j][k], a.f3[i][j][k], count3);
		}
	}

	result.d1 /= count1;
	result.d2 /= count2;
	result.d3 /= count3;

	return result;
}

//-----------------------------------------------------------------------------
Frequency::Difference Frequency::countDifference(const Frequency& a, char symbol) const {
	if (!m_isNormalized || !a.m_isNormalized)
		throw std::exception();

	symbol -= rus_a;

	Difference result = {};
	int64_t count1 = 0;
	int64_t count2 = 0;
	int64_t count3 = 0;

	result.d1 += metric(f1[symbol], a.f1[symbol], count1);

	for (int i = 0; i < alphabetSize; ++i) {
		result.d2 += metric(f2[symbol][i], a.f2[symbol][i], count2);
		result.d2 += metric(f2[i][symbol], a.f2[i][symbol], count2);
	}

	for (int i = 0; i < alphabetSize; ++i) {
		for (int j = 0; j < alphabetSize; ++j) {
			result.d3 += metric(f3[symbol][i][j], a.f3[symbol][i][j], count3);
			result.d3 += metric(f3[i][symbol][j], a.f3[i][symbol][j], count3);
			result.d3 += metric(f3[i][j][symbol], a.f3[i][j][symbol], count3);
		}
	}

	result.d1 /= count1;
	result.d2 /= count2;
	result.d3 /= count3;

	return result;
}

//-----------------------------------------------------------------------------
int64_t Frequency::getProcessedSymbolsCount(void) const {
	return m_count;
}

//-----------------------------------------------------------------------------
void Frequency::getF1(std::vector<SymbolFrequency1>& f) const {
	f.clear();
	f.reserve(alphabetSize);
	for (int i = 0; i < f1.size(); ++i)
		f.push_back({char(rus_a + i), f1[i]});
}

//-----------------------------------------------------------------------------
void Frequency::getF2(std::vector<SymbolFrequency2>& f) const {
	f.clear();
	f.reserve(alphabetSize * alphabetSize);
	for (int i = 0; i < f2.size(); ++i)
		for (int j = 0; j < f2[i].size(); ++j)
			f.push_back({char(rus_a + i), char(rus_a + j), f2[i][j]});
}

//-----------------------------------------------------------------------------
void Frequency::getF3(std::vector<SymbolFrequency3>& f) const {
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