#pragma once

#include <vector>
#include "common.h"
#include "space_stats.h"

//-----------------------------------------------------------------------------
class Evaluator
{
public:
	Evaluator();

	void mutate(void);

	double calcEfficiency(const std::string& text) const;

	std::vector<double> getWeights(void) const;

private:
	double weight11; double weight21; double weight31;
	double weight12; double weight22; double weight32;
	double weight13; double weight23; double weight33;
	double sumWeight;

	double weightSum;

	struct StatSum
	{
		double start1,  start2,	 start3;
		double middle1, middle2, middle3;
		double end1,    end2,    end3;
	};

	void calcSum(double& to, double what, double& count) const;
	double getEfficiency(StatSum s) const;
	StatSum calcStatSum(const std::string& text) const;
};

//-----------------------------------------------------------------------------
class Creature
{
public:
	Creature(const std::string& text, const Evaluator& eval);

	void mutate(void);

	double getEfficiency(void) const;
	std::string getText(void) const;

private:
	Evaluator m_eval;

	std::string m_text;
	std::string m_result;
	std::vector<int> m_spaces;
	double m_efficiency;

	void countEfficiency(void);
	void makeText(void);
};

//-----------------------------------------------------------------------------
class Evolution
{
public:
	Evolution(int n, int m, int g, const std::string& text, const Evaluator& eval);

	void calcGeneration(void);
	std::string evolveToResult(void);

	int getGenerationsCount(void) const;
	const Creature& getBestCreature(void) const;

private:
	Evaluator m_eval;

	std::string m_text;
	std::vector<Creature> m_population;

	int m_generations;
	int m_m, m_n, m_g;
};