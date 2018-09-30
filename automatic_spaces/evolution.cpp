#include <algorithm>
#include <iostream>
#include <iomanip>
#include "evolution.h"

//-----------------------------------------------------------------------------
Evaluator::Evaluator() {
	// Эти веса подобраны в ходе метаэволюции, они дают самый лучший результат на тестовых текстах: -20%.
	weight11  = 1.0000000000000000;
	weight12  = 0.4628912555735020;
	weight13  = 0.9341480528918292;
	weight21  = 0.5769692396660988;
	weight22  = 0.0000000000000000;
	weight23  = 3.8919697439477732;
	weight31  = 1.2161414607566661;
	weight32  = 0.1165670891614584;
	weight33  = 0.0000000000000000;
	sumWeight = -0.9845191777270004;

	weightSum = weight11 + weight12 + weight13 + 
				weight21 + weight22 + weight23 + 
				weight31 + weight32 + weight33;
}

//-----------------------------------------------------------------------------
void Evaluator::mutate(void) {
	const double min = 0;
	const double max = 100;

	switch (intRandom(0, 10)) {
		case 0: ::mutate(weight11, min, max); break;
		case 1: ::mutate(weight12, min, max); break;
		case 2: ::mutate(weight13, min, max); break;
		case 3: ::mutate(weight21, min, max); break;
		case 4: ::mutate(weight22, min, max); break;
		case 5: ::mutate(weight23, min, max); break;
		case 6: ::mutate(weight31, min, max); break;
		case 7: ::mutate(weight32, min, max); break;
		case 8: ::mutate(weight33, min, max); break;
		case 9: ::mutate(sumWeight, -100, 0); break;
	}

	weightSum = weight11 + weight12 + weight13 + 
				weight21 + weight22 + weight23 + 
				weight31 + weight32 + weight33;
}

//-----------------------------------------------------------------------------
double Evaluator::calcEfficiency(const std::string& text) const {
	return getEfficiency(calcStatSum(text));
}

//-----------------------------------------------------------------------------
std::vector<double> Evaluator::getWeights(void) const {
	std::vector<double> result;
	result.push_back(weight11);
	result.push_back(weight12);
	result.push_back(weight13);
	result.push_back(weight21);
	result.push_back(weight22);
	result.push_back(weight23);
	result.push_back(weight31);
	result.push_back(weight32);
	result.push_back(weight33);
	result.push_back(sumWeight);
	return result;
}

//-----------------------------------------------------------------------------
void Evaluator::calcSum(double& to, double what, double& count) const {
	if (what == 0)
		to += sumWeight;
	else
		to += what;
	count++;
}

//-----------------------------------------------------------------------------
double Evaluator::getEfficiency(StatSum s) const {
	return 
		(s.start1  * weight11 + s.start2  * weight12 + s.start3  * weight13 +
		 s.middle1 * weight21 + s.middle2 * weight22 + s.middle3 * weight23 +
		 s.end1    * weight31 + s.end2    * weight32 + s.end3    * weight33) /
		weightSum;
}

//-----------------------------------------------------------------------------
Evaluator::StatSum Evaluator::calcStatSum(const std::string& text) const {
	StatSum sum = {};
	StatSum count = {};

	auto isLetter = [] (char a) -> bool {
		return a != rus_space && a != '\0';
	};

	AuxiliaryFixedStack stack(5);

	const auto& s1 = getStandardSpaceStatistics().s1;
	const auto& s2 = getStandardSpaceStatistics().s2;
	const auto& s3 = getStandardSpaceStatistics().s3;

	for (auto c : text) {
		stack.push(c);
		int firstSpace = stack.getSpaceFirstPos();
		int secondSpace = stack.getSpaceSecondPos();

		if (firstSpace == 0) {
			// Значит слово закончилось
			int wordSize = secondSpace - firstSpace - 1;
			int first = stack.get(firstSpace + 1);
			int second = stack.get(firstSpace + 2);
			int third = stack.get(firstSpace + 3);
			switch (wordSize) {
				case 0: {
					
				} break;
				case 1: {
					calcSum(sum.end1, s1[first].end, count.end1);
				} break;
				case 2: {
					calcSum(sum.end1, s1[first].end, count.end1);
					calcSum(sum.end2, s2[second][first].end, count.end2);
				} break;
				case 3:
				case 4: {
					calcSum(sum.end1, s1[first].end, count.end1);
					calcSum(sum.end2, s2[second][first].end, count.end2);
					calcSum(sum.end3, s3[third][second][first].end, count.end3);
				} break;
			}
		} else {
			int first = stack.get(0);
			int second = stack.get(1);
			int third = stack.get(2);
			int fourth = stack.get(3);
			int fift = stack.get(4);
			switch (firstSpace) {
				case 0: {
					throw std::exception();
				} break;
				case 1: {
					calcSum(sum.start1, s1[first].start, count.start1);
				} break;
				case 2: {
					calcSum(sum.start2, s2[second][first].start, count.start2);
				} break;
				case 3: {
					calcSum(sum.middle1, s1[second].middle, count.middle1);
					calcSum(sum.start3, s3[third][second][first].start, count.start3);
				} break;
				case 4: {
					calcSum(sum.middle1, s1[second].middle, count.middle1);
					calcSum(sum.middle2, s2[third][second].middle, count.middle2);
				} break;
				case 5: {			
					calcSum(sum.middle1, s1[second].middle, count.middle1);
					calcSum(sum.middle2, s2[third][second].middle, count.middle2);
					calcSum(sum.middle3, s3[fourth][third][second].middle, count.middle3);
				} break;
			}
		}
	}

	sum.start1 /= count.start1;
	sum.start2 /= count.start2;
	sum.start3 /= count.start3;
	sum.middle1 /= count.middle1;
	sum.middle2 /= count.middle2;
	sum.middle3 /= count.middle3;
	sum.end1 /= count.end1;
	sum.end2 /= count.end2;
	sum.end3 /= count.end3;

	return sum;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Creature::Creature(const std::string& text, const Evaluator& eval) : m_text(text), m_eval(eval) {
	m_spaces.push_back(intRandom(0, text.size()));
	makeText();
	countEfficiency();
}

//-----------------------------------------------------------------------------
void Creature::countEfficiency(void) {
	m_efficiency = m_eval.calcEfficiency(m_result);
}

//-----------------------------------------------------------------------------
void Creature::makeText(void) {
	m_result = m_text;
	sort(m_spaces.begin(), m_spaces.end(), [] (auto& a, auto& b) -> bool {
		return a < b;
	});
	int sum = 0;
	for (auto& i : m_spaces) {
		m_result.insert(sum + i, " ");
		sum++;
	}
}

//-----------------------------------------------------------------------------
double Creature::getEfficiency(void) const {
	return m_efficiency;
}

//-----------------------------------------------------------------------------
void Creature::mutate(void) {
	/*
	потому      - изначальный текст

	по то му    - пример расставления пробелов
	  1  3      - массив пробелов

	п о т о м у - максимально занято
	 0 1 2 3 4  - массив пробелов

	Есть несколько вариантов мутации:
		Добавить пробел
			Пробел добавляется в случайное место, если оно еще не занято
		Убрать пробел
			Если массив не нулевой длины
				Удаляется случайный пробел
		Подвинуть пробел вправо или влево
			Если все не занято максимально
				Выбрать случайный пробел
				Выбрать случайное направление
				Если под этим направлением нет другого пробела, то 
					Подвинуть
				Иначе начать заного генерацию
	*/

	// Формируем массив свободных мест
	std::vector<bool> isSpaceFree(m_text.size(), true);
	for (int i = 0; i < m_spaces.size(); ++i)
		isSpaceFree[m_spaces[i]] = false;

	std::vector<int> freeSpaces;
	for (int i = 0; i < isSpaceFree.size(); ++i) 
		if (isSpaceFree[i])
			freeSpaces.push_back(i);

	startMutate:
	int randSpaceNo = intRandom(0, m_spaces.size());
	int randFreeNo = intRandom(0, freeSpaces.size());
	switch (getRandomCase({1, 1, 1})) {
		case 0: {
			// Добавить пробел
			if (m_spaces.size() != m_text.size() - 1)
				m_spaces.push_back(freeSpaces[randFreeNo]);
			else
				goto startMutate;
		} break;
		case 1: {
			// Удалить пробел
			if (m_spaces.size() != 1)
				m_spaces.erase(m_spaces.begin() + randSpaceNo);
			else
				goto startMutate;
		} break;
		case 2: {
			// Передвинуть пробел на случайное свободное место
			if (freeSpaces.size() != 0)
				m_spaces[randSpaceNo] = freeSpaces[randFreeNo];
			else
				goto startMutate;
		} break;
	}

	makeText();
	countEfficiency();
}

//-----------------------------------------------------------------------------
std::string Creature::getText(void) const {
	return m_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
Evolution::Evolution(int n, int m, int g, const std::string& text, const Evaluator& eval) : m_n(n), m_m(m), m_g(g), m_generations(0), m_text(text), m_eval(eval) {
	for (int i = 0; i < m_n; ++i)
		m_population.push_back(Creature(m_text, m_eval));
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
		return a.second > b.second;
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
std::string Evolution::evolveToResult(void) {
	for (int i = 0; i < m_g; ++i)
		calcGeneration();
	return getBestCreature().getText();
}

//-----------------------------------------------------------------------------
int Evolution::getGenerationsCount(void) const {
	return m_generations;
}

//-----------------------------------------------------------------------------
const Creature& Evolution::getBestCreature(void) const {
	return m_population[0];
}