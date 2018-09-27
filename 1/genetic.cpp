#include <algorithm>
#include <iostream>
#include "genetic.h"

//-----------------------------------------------------------------------------
const Frequency& getStandardFrequency(void) {
	static Frequency f;
	static bool isInitialized = false;
	if (!isInitialized) {
		isInitialized = true;
		f.load("frequency_table.txt");
	}
	return f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Creature::Creature(std::string* text, std::string startKey) : m_text(text) {
	m_cipher.setKey(startKey);
	m_cipher.mutate();
	countEfficiency();
}

//-----------------------------------------------------------------------------
Creature::Creature(std::string* text) : m_text(text) {
	m_cipher.generateRandomKey();
	countEfficiency();
}

//-----------------------------------------------------------------------------
double Creature::getEfficiency(void) const {
	return m_efficiency;
}

//-----------------------------------------------------------------------------
void Creature::mutate(void) {
	m_cipher.mutate();
	countEfficiency();
}

//-----------------------------------------------------------------------------
std::string Creature::getKey(void) const {
	return m_cipher.getKey();
}

//-----------------------------------------------------------------------------
void Creature::countEfficiency(void) {
	std::string decrypted = m_cipher.decrypt(*m_text);
	Frequency f2;
	f2.count(decrypted);
	f2.normalize();
	auto diff = getStandardFrequency().countDifference(f2);

	double weight1 = 1;
	double weight2 = 2;
	double weight3 = 3;
	double weightSum = weight1 + weight2 + weight3;
	m_efficiency = (weight1 * diff.d1 + weight2 * diff.d2 + weight3 * diff.d3) / weightSum;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
Evolution::Evolution(int n, int m, std::string* text) : m_n(n), m_m(m), m_generations(0), m_text(text) {
	for (int i = 0; i < m_n; ++i)
		m_population.push_back(Creature(m_text));
}

//-----------------------------------------------------------------------------
Evolution::Evolution(int n, int m, std::string* text, std::string startKey) : m_n(n), m_m(m), m_generations(0), m_text(text) {
	for (int i = 0; i < m_n; ++i)
		m_population.push_back(Creature(m_text, startKey));
}

//-----------------------------------------------------------------------------
void Evolution::calcGeneration(void) {
	// Получаем эффективность каждой особи
	std::vector<std::pair<int, double>> rank;
	for (int i = 0; i < m_population.size(); ++i)
		rank.push_back({i, m_population[i].getEfficiency()});

	// Сортируем их по эффективности
	std::sort(rank.begin(), rank.end(), [] (auto& a, auto& b) -> bool {
		return a.second < b.second;
	});

	//-------------------------------------------------------------------------
	// Создаем новую популяцию на основе m самых лучших особей

	// Передаем без изменений лучших особей
	std::vector<Creature> newPopulation;
	for (int i = 0; i < m_m; ++i)
		newPopulation.push_back(m_population[rank[i].first]);

	// Создаем потомков для каждого из них
	int descendantCount = (m_n - m_m) / m_m;
	for (int i = 0; i < m_m; ++i) {
		for (int j = 0; j < descendantCount; ++j) {
			newPopulation.push_back(m_population[rank[i].first]);
			newPopulation.back().mutate();		
		}
	}

	// Оставшееся пространство заполняем потомками самого лучшего существа
	int descendantResidue = m_n - m_m - descendantCount * m_m;
	for (int i = 0; i < descendantResidue; ++i) {
		newPopulation.push_back(m_population[rank[0].first]);
		newPopulation.back().mutate();
		newPopulation.back().mutate();
	}

	m_population = newPopulation;

	m_generations++;
}

//-----------------------------------------------------------------------------
int Evolution::getGenerationsCount(void) const {
	return m_generations;
}

//-----------------------------------------------------------------------------
Creature Evolution::getBestCreature(void) const {
	return m_population[0];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string getStartKeyByFrequency(const std::string& encrypted) {
	Frequency f;
	f.count(encrypted);
	f.normalize();

	std::vector<Frequency::SymbolFrequency1> f1, s1;
	f.getSortedF1(f1);
	getStandardFrequency().getSortedF1(s1);

	std::string key(alphabetSize, '\0');
	for (int i = 0; i < s1.size(); ++i)
		key[s1[i].c1 - rus_a] = f1[i].c1;

	return key;
}

//-----------------------------------------------------------------------------
std::string decryptByEvolution(const std::string& encrypted, bool isWriteLog) {
	std::string text = encrypted;
	Evolution ev(50, 10, &text, getStartKeyByFrequency(encrypted));
	for (int i = 0; i < 300; ++i) {
		ev.calcGeneration();
		if (isWriteLog)
			std::cout 
				<< ev.getGenerationsCount() 
				<< "\t" 
				<< ev.getBestCreature().getEfficiency() 
				<< std::endl;
	}

	std::string bestKey = ev.getBestCreature().getKey();

	//return bestKey;
	return bruteForce(encrypted, bestKey, ev.getBestCreature().getEfficiency(), isWriteLog);
}

//-----------------------------------------------------------------------------
std::string bruteForce(const std::string& encrypted, std::string startKey, double startEfficiency, bool isWriteLog) {
	std::string text = encrypted;

	std::vector<std::pair<std::string, double>> population;
	for (int i = 0; i < alphabetSize; ++i) {
		for (int j = i+1; j < alphabetSize; ++j) {
			std::string newKey = startKey;	
			std::swap(newKey[i], newKey[j]);
			
			Creature creature(&text, newKey);
			population.push_back({newKey, creature.getEfficiency()});
		}
	}

	std::sort(population.begin(), population.end(), [] (auto a, auto b) -> bool {
		return a.second < b.second;
	});

	if (isWriteLog)
		std::cout << "BruteForce\t" << population[0].second << std::endl;

	if (population[0].second < startEfficiency)
		return bruteForce(encrypted, population[0].first, population[0].second, isWriteLog);
	else
		return startKey;
}