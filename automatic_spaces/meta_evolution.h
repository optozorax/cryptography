#pragma once

#include <vector>
#include "evolution.h"

//-----------------------------------------------------------------------------
class MetaCreature
{
public:
	MetaCreature(std::vector<std::string>* texts);

	void mutate(void);

	double getEfficiency(void) const;
	int getCreaturesCount(void) const;
	int getBestCreaturesCount(void) const;
	int getGenerationsCount(void) const;
	std::vector<double> getWeights(void) const;

private:
	std::vector<std::string>* m_texts;

	Evaluator m_eval;
	int m_creaturesCount;
	int m_bestCreaturesCount;
	int m_generationsCount;
	double m_efficiency;

	void countEfficiency(void);
};

//-----------------------------------------------------------------------------
class MetaEvolution
{
public:
	MetaEvolution(int n, int m, int g, std::vector<std::string>* texts);

	void calcGeneration(void);
	std::vector<double> evolveToResult(bool isWriteLog);

	int getGenerationsCount(void) const;
	const MetaCreature& getBestCreature(void) const;
private:
	std::vector<std::string>* m_texts;

	std::vector<MetaCreature> m_population;

	int m_generations;
	int m_m, m_n, m_g;
};