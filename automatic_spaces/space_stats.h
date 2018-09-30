#pragma once

#include <vector>
#include <string>
#include "common.h"

//-----------------------------------------------------------------------------
struct SpaceStat
{
	double start;
	double middle;
	double end;

	void normalize(void);
};

struct Symbol1Stat
{
	char c1;
	SpaceStat s;
};

struct Symbol2Stat
{
	char c1, c2;
	SpaceStat s;
};

struct Symbol3Stat
{
	char c1, c2, c3;
	SpaceStat s;
};

//-----------------------------------------------------------------------------
class AuxiliaryFixedStack
{
public:
	AuxiliaryFixedStack(int size);

	void clear(void);

	void push(char elem);
	int getSpaceFirstPos(void) const;
	int getSpaceSecondPos(void) const;

	char get(int pos) const; // 0 = top, 1 = last top, 2 = last last top, etc.

private:
	std::vector<char> m_stack;
	int n;
};

//-----------------------------------------------------------------------------
class SpaceStatistics
{
public:
	SpaceStatistics();

	void clear(void);

	/*
	Необходимо собрать статистику по предложению:
	и да что тому итого потому

	    и	и.start++
	   и 	и.end++
	  и д	д.start++
	 и да	да.start++
	и да 	да.end++		а.end++
	 да ч	ч.start++
	да чт	чт.start++
	а что	что.start++		т.middle++
	 что 	что.end++		то.end++		о.end++
	что т	т.start++
	то то	то.start++
	о том	том.start++		о.middle++
	 тому	ом.middle++		м.middle++
	тому 	ому.end++		му.end++		у.end++
	ому и   и.start++
	му ит	ит.start++
	у ито	ито.start++		т.middle++
	 итог	то.middle++		о.middle++
	итого	тог.middle++	ог.middle++		г.middle++
	того 	ого.end++		го.end++		о.end++
	ого п	п.start++
	го по 	по.start++
	о пот 	пот.start++		о.middle++
	 пото	от.middle++		т.middle++
	потом	ото.middle++	о.middle++
	отому	том.middle++	м.middle++
	тому 	ому.end++		му.end++		у.end++
	ому  

	*/

	void count(const std::string& text);
	void normalize();

	void save(std::string fileName) const;
	void load(std::string fileName);

	//-------------------------------------------------------------------------
	void getS1(std::vector<Symbol1Stat>& s1) const;
	void getS2(std::vector<Symbol2Stat>& s2) const;
	void getS3(std::vector<Symbol3Stat>& s3) const;

	//-------------------------------------------------------------------------
	std::vector<SpaceStat>								s1;
	std::vector<std::vector<SpaceStat>>					s2;
	std::vector<std::vector<std::vector<SpaceStat>>>	s3;

private:
	bool m_isNormalized;

	AuxiliaryFixedStack m_stack;
};

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const SpaceStat& s);
std::ostream& operator<<(std::ostream& out, const Symbol1Stat& s);
std::ostream& operator<<(std::ostream& out, const Symbol2Stat& s);
std::ostream& operator<<(std::ostream& out, const Symbol3Stat& s);

std::istream& operator>>(std::istream& in, SpaceStat& s);
std::istream& operator>>(std::istream& in, Symbol1Stat& s);
std::istream& operator>>(std::istream& in, Symbol2Stat& s);
std::istream& operator>>(std::istream& in, Symbol3Stat& s);

//-----------------------------------------------------------------------------
const SpaceStatistics& getStandardSpaceStatistics(void);

//-----------------------------------------------------------------------------
// Возвращает число от 0 до 1 какое количество пробелов расставлено правильно. 0 - ни один пробел не расставлен правильно. 1 - все пробелы расставлены правлиьно, и нет ни одного лишнего. -1 - некоторое количество пробелов расставлено абсолютно неправильно.
double getAccuracySpaces(const std::string& text1, const std::string& text2);

/*
и да что тому итого потому - text1
ида что томуи того пот ому - text2
и да что тому и того пот_ому - t1
и_да что тому_и того пот ому - t2
0-00+000+0000+0+0000+000-000
3/7 - returned

и да что тому итого потому - text1
идачтотомуитогопотому - text2
и да что тому итого потому - t1
и_да_что_тому_итого_потому - t2
0-00-000-0000-00000-000000
-1 - returned
*/