#include <algorithm>
#include <iostream>
#include <iomanip>
#include "genetic.h"

//-----------------------------------------------------------------------------
// Вспомогательные функции для эволюции
std::string getStartKeyByFrequency(const std::string& encrypted);
std::string bruteForce(const std::string& encrypted, std::string startKey, double startEfficiency, bool isWriteLog);

//=============================================================================
//=============================================================================
//=============================================================================

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
	/* Как подсчитывается эффективность данной особи:
			1. Текст расшифровывается ключом текущей особи.
			2. Считается разница частотности расшифрованного текста и частототности русского языка.
			3. Разница в частотности преобразуется в одно вещественное число с помощью особой функции.
	 */

	std::string decrypted = m_cipher.decrypt(*m_text);
	Frequency f2;
	f2.count(decrypted);
	f2.normalize();
	auto diff = getStandardFrequency().countDifference(f2);
	
	m_efficiency = differenceMetric(diff);
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
	/* Как происходит эволюция:
			1. Выбираются лучшие m особей.
			2. Все остальные убиваются.
			3. Эти лучшие создают детей.
			4. Дети мутируют.
			5. Goto 1.
	*/

	//-------------------------------------------------------------------------
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
const Creature& Evolution::getBestCreature(void) const {
	return m_population[0];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string getStartKeyByFrequency(const std::string& encrypted) {
	/* Генерирует ключ, от которого можно отталкиваться при дешифрации. Строит его путем подстановки вместо самых частотных известных букв, самые частотные буквы зашифрованного текста. */

	Frequency f;
	f.count(encrypted);
	f.normalize();

	std::vector<SymbolFrequency1> f1, s1;
	f.getSortedF1(f1);
	getStandardFrequency().getSortedF1(s1);

	std::string key(alphabetSize, '\0');
	for (int i = 0; i < s1.size(); ++i)
		key[s1[i].c1 - rus_a] = f1[i].c1;

	return key;
}

//-----------------------------------------------------------------------------
std::string bruteForce(const std::string& encrypted, std::string startKey, double startEfficiency, bool isWriteLog) {
	/* Доводит ключ до идеала путем перебора всех возможных перестановок. Если такая перестановка стала лучше, чем ключ, данный изначально, повторяет тоже самое. Функция предполагает, что ключ уже почти идеален, иначе число вычислений будет астрономическим. */

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

//-----------------------------------------------------------------------------
std::string decryptByEvolution(const std::string& encrypted, bool isWriteLog) {
	/* Расшифровывает текст при помощи частотного анализа и генетического алгоритма. */
	const int populationSize = 50;
	const int bestInPopulation = 10;
	const int generationCount = 300;

	std::string text = encrypted;

	// Инициализируем эволюцию ключом, сформированным по частотности
	Evolution ev(populationSize, bestInPopulation, &text, getStartKeyByFrequency(encrypted));

	// Цикл по поколениям
	for (int i = 0; i < generationCount; ++i) {
		ev.calcGeneration();
		if (isWriteLog)
			std::cout << "\r" << std::setprecision(3) << std::fixed << std::setw(7)
				<< double(ev.getGenerationsCount() * 100) / generationCount 
				<< "%\t" 
				<< ev.getBestCreature().getEfficiency() 
				<< "    ";
	}
	if (isWriteLog)
		std::cout << std::endl;

	std::string bestKey = ev.getBestCreature().getKey();

	// Возвращаем лучший ключ, доведенный до идеала полным перебором перестановок
	return bruteForce(encrypted, bestKey, ev.getBestCreature().getEfficiency(), isWriteLog);
}