#include <iostream>
#include <algorithm>
#include "meta_evolution.h"

//-----------------------------------------------------------------------------
MetaCreature::MetaCreature(std::vector<std::string>* texts) : m_texts(texts), m_creaturesCount(43), m_bestCreaturesCount(12), m_generationsCount(97) {
	countEfficiency();
}

//-----------------------------------------------------------------------------
void MetaCreature::mutate(void) {
	switch (getRandomCase({5, 1, 1, 1})) {
		case 0: {
			m_eval.mutate();
		} break;
		case 1: {
			::mutate(m_creaturesCount, 3, 100);
		} break;
		case 2: {
			::mutate(m_bestCreaturesCount, 1, m_creaturesCount);
		} break;
		case 3: {
			::mutate(m_generationsCount, 2, 300);
		} break;
	}
	countEfficiency();
}

//-----------------------------------------------------------------------------
double MetaCreature::getEfficiency(void) const {
	return m_efficiency;
}

//-----------------------------------------------------------------------------
int MetaCreature::getCreaturesCount(void) const {
	return m_creaturesCount;
}

//-----------------------------------------------------------------------------
int MetaCreature::getBestCreaturesCount(void) const {
	return m_bestCreaturesCount;
}

//-----------------------------------------------------------------------------
int MetaCreature::getGenerationsCount(void) const {
	return m_generationsCount;
}

//-----------------------------------------------------------------------------
std::vector<double> MetaCreature::getWeights(void) const {
	return m_eval.getWeights();
}

//-----------------------------------------------------------------------------
void MetaCreature::countEfficiency(void) {
	double sum = 0;
	for (int i = 0; i < m_texts->size(); ++i) {
		std::string text = (*m_texts)[i];
		std::string text1 = toRussianLetters(text);

		Evolution ev(m_creaturesCount, m_bestCreaturesCount, m_generationsCount, text1, m_eval);
		std::string text2 = ev.evolveToResult();

		sum += getAccuracySpaces(text, text2);
	}
	m_efficiency = sum / m_texts->size();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MetaEvolution::MetaEvolution(int n, int m, int g, std::vector<std::string>* texts) : m_texts(texts), m_n(n), m_m(m), m_g(g) {
	for (int i = 0; i < m_n; ++i)
		m_population.push_back(MetaCreature(m_texts));
}

//-----------------------------------------------------------------------------
void MetaEvolution::calcGeneration(void) {
	/* Как происходит эволюция:
			1. Выбираются лучшие m особей.
			2. Все остальные убиваются.
			3. Эти лучшие создают детей.
			4. Дети мутируют.
			5. Goto 1.
	*/

	//-------------------------------------------------------------------------
	// Получаем эффективность каждой особи
	struct RankElem
	{
		int i;
		double efficiency;
		int generations;
		int creatures;
	};
	std::vector<RankElem> rank;
	for (int i = 0; i < m_population.size(); ++i)
		rank.push_back({
			i, 
			m_population[i].getEfficiency(), 
			m_population[i].getGenerationsCount(), 
			m_population[i].getCreaturesCount()
		});

	// Сортируем их по эффективности
	std::sort(rank.begin(), rank.end(), [] (auto& a, auto& b) -> bool {
		if (a.efficiency == b.efficiency)
			return (a.generations * a.creatures) < 
				   (b.generations * b.creatures);
		else
			return a.efficiency > b.efficiency;
	});

	//-------------------------------------------------------------------------
	// Создаем новую популяцию на основе m самых лучших особей

	// Передаем без изменений лучших особей
	std::vector<MetaCreature> newPopulation;
	for (int i = 0; i < m_m; ++i)
		newPopulation.push_back(m_population[rank[i].i]);

	// Создаем потомков для каждого из них
	int descendantCount = (m_n - m_m) / m_m;
	for (int i = 0; i < m_m; ++i) {
		for (int j = 0; j < descendantCount; ++j) {
			newPopulation.push_back(m_population[rank[i].i]);
			newPopulation.back().mutate();
		}
	}

	// Оставшееся пространство заполняем потомками самого лучшего существа
	int descendantResidue = m_n - m_m - descendantCount * m_m;
	for (int i = 0; i < descendantResidue; ++i) {
		newPopulation.push_back(m_population[rank[0].i]);
		newPopulation.back().mutate();
		newPopulation.back().mutate();
	}

	m_population = newPopulation;

	m_generations++;
}

//-----------------------------------------------------------------------------
std::vector<double> MetaEvolution::evolveToResult(bool isWriteLog) {
	for (int i = 0; i < m_g; ++i) {
		calcGeneration();
		if (isWriteLog)
			std::cout 
				<< getGenerationsCount() 
				<< "\t"
				<< getBestCreature().getEfficiency()
				<< std::endl;
	}
	return getBestCreature().getWeights();
}

//-----------------------------------------------------------------------------
int MetaEvolution::getGenerationsCount(void) const {
	return m_generations;
}

//-----------------------------------------------------------------------------
const MetaCreature& MetaEvolution::getBestCreature(void) const {
	return m_population[0];
}