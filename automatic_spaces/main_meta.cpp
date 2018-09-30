#include <iostream>
#include <fstream>
#include "meta_evolution.h"

int main() {
	// Формируем тексты
	const int textCount = 10;
	const int textSize = 100;

	std::vector<std::string> texts;
	TextReader reader(true);
	for (int i = 0; i < reader.size(); ++i) {
		std::string text;
		reader.readNextText(text);

		text = toRussianLettersAndSpace(text);

		for (int i = 0; i < textCount; ++i) {
			texts.push_back(getRandomPart(text, textSize));
		}
	}

	std::cout << "Start evolution" << std::endl;

	// Запускаем эволюцию по этим текстам
	MetaEvolution mev(50, 10, 100, &texts);
	auto result = mev.evolveToResult(true);

	// Выводим результат
	std::ofstream fout("evaluator_best_weights.txt");
	fout.precision(std::numeric_limits<double>::digits10 + 1);
	fout << std::fixed;
	for (int i = 0; i < result.size(); ++i)
		fout << result[i] << std::endl;
	fout << mev.getBestCreature().getCreaturesCount() << std::endl;
	fout << mev.getBestCreature().getBestCreaturesCount() << std::endl;
	fout << mev.getBestCreature().getGenerationsCount() << std::endl;
	fout.close();
}