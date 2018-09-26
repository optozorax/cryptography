#pragma once

#include <vector>
#include "encrypt.h"

//-----------------------------------------------------------------------------
const Frequency& getStandardFrequency(void);

//-----------------------------------------------------------------------------
class Creature
{
public:
	Creature(std::string* text);

	double getEfficiency(void) const;
	void mutate(void);

	std::string getKey(void) const;

private:
	std::string* m_text;
	Cipher m_cipher;
	double m_efficiency;

	void countEfficiency(void);
};

//-----------------------------------------------------------------------------
class Evolution
{
public:
	Evolution(int n, int m, std::string* text);

	void calcGeneration(void);
	int getGenerationsCount(void) const;

	Creature getBestCreature(void) const;

private:
	std::string* m_text;
	std::vector<Creature> m_population;
	int m_generations;
	int m_m, m_n;
};

//-----------------------------------------------------------------------------
std::string decryptByEvolution(const std::string& encrypted, bool isWriteLog);