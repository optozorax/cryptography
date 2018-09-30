#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include "space_stats.h"

//-----------------------------------------------------------------------------
void SpaceStat::normalize(void) {
	double sum = start + middle + end;
	if (sum != 0) {
		start /= sum;
		middle /= sum;
		end /= sum;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
AuxiliaryFixedStack::AuxiliaryFixedStack(int size) : n(size) {
	clear();
}

//-----------------------------------------------------------------------------
void AuxiliaryFixedStack::clear(void) {
	m_stack.clear();
	m_stack.resize(n, 0);
}

//-----------------------------------------------------------------------------
void AuxiliaryFixedStack::push(char elem) {
	if (elem == rus_space)
		elem = 0;
	else
		elem = elem - rus_a + 1;
	m_stack.erase(m_stack.begin());
	m_stack.push_back(elem);
}

//-----------------------------------------------------------------------------
int AuxiliaryFixedStack::getSpaceFirstPos(void) const {
	for (int i = 0; i < n; ++i) {
		if (m_stack[n - i - 1] == 0)
			return i;
	}
	return n;
}

//-----------------------------------------------------------------------------
int AuxiliaryFixedStack::getSpaceSecondPos(void) const {
	for (int i = getSpaceFirstPos() + 1; i < n; ++i) {
		if (m_stack[n - i - 1] == 0)
			return i;	
	}
	return n;
}

//-----------------------------------------------------------------------------
char AuxiliaryFixedStack::get(int pos) const {
	if (pos > n - 1)
		return 0;
	else
		return m_stack[n - pos - 1] - 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SpaceStatistics::SpaceStatistics() : m_stack(5) {
	clear();
}

//-----------------------------------------------------------------------------
void SpaceStatistics::clear() {
	m_isNormalized = false;
	m_stack.clear();
	s1.clear();
	s2.clear();
	s3.clear();
	s1.resize(rus_size, {0, 0, 0});
	s2.resize(rus_size, s1);
	s3.resize(rus_size, s2);
}

//-----------------------------------------------------------------------------
void SpaceStatistics::count(const std::string& text) {
	if (m_isNormalized)
		throw std::exception();

	std::string text1 = toRussianLettersAndSpace(text);

	auto isLetter = [] (char a) -> bool {
		return a != rus_space && a != '\0';
	};

	for (auto c : text1) {
		m_stack.push(c);
		int firstSpace = m_stack.getSpaceFirstPos();
		int secondSpace = m_stack.getSpaceSecondPos();

		if (firstSpace == 0) {
			// Значит слово закончилось
			int wordSize = secondSpace - firstSpace - 1;
			int first = m_stack.get(firstSpace + 1);
			int second = m_stack.get(firstSpace + 2);
			int third = m_stack.get(firstSpace + 3);
			switch (wordSize) {
				case 0: {
					
				} break;
				case 1: {
					s1[first].end++;
				} break;
				case 2: {
					s1[first].end++;
					s2[second][first].end++;
				} break;
				case 3: {
					s1[first].end++;
					s2[second][first].end++;
					s3[third][second][first].end++;
				} break;
			}
		} else {
			int first = m_stack.get(0);
			int second = m_stack.get(1);
			int third = m_stack.get(2);
			int fourth = m_stack.get(3);
			int fift = m_stack.get(4);
			switch (firstSpace) {
				case 0: {
					throw std::exception();
				} break;
				case 1: {
					s1[first].start++;
				} break;
				case 2: {
					s2[second][first].start++;
				} break;
				case 3: {
					s1[second].middle++;
					s3[third][second][first].start++;
				} break;
				case 4: {
					s1[second].middle++;
					s2[third][second].middle++;
				} break;
				case 5: {			
					s1[second].middle++;
					s2[third][second].middle++;
					s3[fourth][third][second].middle++;
				} break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
void SpaceStatistics::normalize() {
	if (m_isNormalized)
		throw std::exception();

	for (auto& i : s1)
		i.normalize();

	for (auto& i : s2)
		for (auto& j : i)
			j.normalize();

	for (auto& i : s3)
		for (auto& j : i)
			for (auto& k : j)
				k.normalize();

	m_isNormalized = true;
}

//-----------------------------------------------------------------------------
void SpaceStatistics::save(std::string fileName) const {
	if (!m_isNormalized)
		throw std::exception();

	std::vector<Symbol1Stat> ss1;
	std::vector<Symbol2Stat> ss2;
	std::vector<Symbol3Stat> ss3;
	getS1(ss1);
	getS2(ss2);
	getS3(ss3);
	
	std::ofstream fout(fileName);

	for (auto& i : ss1)
		fout << i << std::endl;
	fout << std::endl;
	for (auto& i : ss2)
		fout << i << std::endl;
	fout << std::endl;
	for (auto& i : ss3)
		fout << i << std::endl;

	fout.close();
}

//-----------------------------------------------------------------------------
void SpaceStatistics::load(std::string fileName) {
	clear();

	m_isNormalized = true;

	std::ifstream fin(fileName);

	Symbol1Stat r1;
	for (int i = 0; i < rus_size; ++i) {
		fin >> r1;
		s1[i] = r1.s;
	}

	Symbol2Stat r2;
	for (int i = 0; i < rus_size; ++i)
		for (int j = 0; j < rus_size; ++j) {
			fin >> r2;
			s2[i][j] = r2.s;
		}

	Symbol3Stat r3;
	for (int i = 0; i < rus_size; ++i)
		for (int j = 0; j < rus_size; ++j)
			for (int k = 0; k < rus_size; ++k) {
				fin >> r3;
				s3[i][j][k] = r3.s;
			}

	fin.close();
}

//-----------------------------------------------------------------------------
void SpaceStatistics::getS1(std::vector<Symbol1Stat>& s) const {
	if (!m_isNormalized)
		throw std::exception();	

	s.clear();
	s.resize(rus_size);
	for (int i = 0; i < s1.size(); ++i)
		s[i] = {char(i + rus_a), s1[i]};
}

//-----------------------------------------------------------------------------
void SpaceStatistics::getS2(std::vector<Symbol2Stat>& s) const {
	if (!m_isNormalized)
		throw std::exception();

	s.clear();
	s.resize(rus_size * rus_size);
	for (int i = 0; i < s2.size(); ++i)
		for (int j = 0; j < s2[i].size(); ++j)
			s[i * rus_size + j] = {char(i + rus_a), char(j + rus_a), s2[i][j]};	
}

//-----------------------------------------------------------------------------
void SpaceStatistics::getS3(std::vector<Symbol3Stat>& s) const {
	if (!m_isNormalized)
		throw std::exception();

	s.clear();
	s.resize(rus_size * rus_size * rus_size);
	for (int i = 0; i < s3.size(); ++i)
		for (int j = 0; j < s3[i].size(); ++j)
			for (int k = 0; k < s3[i][j].size(); ++k)
				s[(i * rus_size + j) * rus_size + k] = 
					{char(i + rus_a), char(j + rus_a), char(k + rus_a), s3[i][j][k]};	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const SpaceStat& s) {
	int digits10 = std::numeric_limits<double>::digits10;
	out.precision(digits10);
	out << std::fixed << std::setw(digits10 + 3) << s.start * 100 << "\t";
	out << std::fixed << std::setw(digits10 + 3) << s.middle * 100 << "\t";
	out << std::fixed << std::setw(digits10 + 3) << s.end * 100 << "\t";
	return out;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Symbol1Stat& s) {
	out << s.c1 << "\t" << s.s;
	return out;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Symbol2Stat& s) {
	out << s.c1 << s.c2 << "\t" << s.s;
	return out;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Symbol3Stat& s) {
	out << s.c1 << s.c2 << s.c3 << "\t" << s.s;
	return out;
}

//-----------------------------------------------------------------------------
std::istream& operator>>(std::istream& in, SpaceStat& s) {
	in >> s.start;
	in >> s.middle;
	in >> s.end;
	s.start /= 100.0;
	s.middle /= 100.0;
	s.end /= 100.0;
	return in;
}

//-----------------------------------------------------------------------------
std::istream& operator>>(std::istream& in, Symbol1Stat& s) {
	in >> s.c1 >> s.s;
	return in;
}

//-----------------------------------------------------------------------------
std::istream& operator>>(std::istream& in, Symbol2Stat& s) {
	in >> s.c1 >> s.c2 >> s.s;
	return in;
}

//-----------------------------------------------------------------------------
std::istream& operator>>(std::istream& in, Symbol3Stat& s) {
	in >> s.c1 >> s.c2 >> s.c3 >> s.s;
	return in;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const SpaceStatistics& getStandardSpaceStatistics(void) {
	static SpaceStatistics s;
	static bool isInitialized = false;
	if (!isInitialized) {
		isInitialized = true;
		s.load("frequency_space_table.txt");
	}
	return s;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double getAccuracySpaces(const std::string& text1, const std::string& text2) {
	std::string t1 = text1;
	std::string t2 = text2;

	bool isEnd = false;
	int i = 0;
	while (!isEnd) {
		if (t1[i] == rus_space && t2[i] == rus_space) {
			// all is ok
		} else if (t1[i] == rus_space && t2[i] != rus_space) {
			t2.insert(i, "_");
		} else if (t1[i] != rus_space && t2[i] == rus_space) {
			t1.insert(i, "_");
		} else if (t1[i] != rus_space && t2[i] != rus_space) {
			// all is ok
		} else {
			throw std::exception();
		}

		i++;
		isEnd = i == t1.size() || i == t2.size();
	}

	int result = 0;
	int count = 0;
	for (int i = 0; i < t1.size(); ++i) {
		if (t2[i] == rus_space) {
			if (t1[i] == rus_space) {
				result++;
			} else {
				result--;
			}
			count++;
		} else if (t2[i] == '_') {
			result--;
			count++;
		} else {
			// all is ok
		}
	}

	return double(result)/double(count);
}