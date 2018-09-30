#pragma once

#include <vector>
#include <string>

//-----------------------------------------------------------------------------
// Russian letters
const char rus_a = -32;
const char rus_A = -64;
const char rus_z = -1;
const char rus_Z = -33;
const char rus_spare_e = -72;
const char rus_spare_E = -88;
const char rus_space = ' ';
const int rus_size = rus_z - rus_a + 1;

//-----------------------------------------------------------------------------
double random(void);
int intRandom(int min, int max);
int getRandomCase(const std::vector<double>& casesWeight);
void mutate(double& number, double min, double max);
void mutate(int& number, int min, int max);

//-----------------------------------------------------------------------------
char toRussianLetters(char c);
std::string toRussianLetters(std::string text);

char toRussianLettersAndSpace(char c);
std::string toRussianLettersAndSpace(std::string text);

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName);
void writeToFile(std::string fileName, const std::string& text);

// Возвращает случайную часть строки размером size
std::string getRandomPart(const std::string& text, int size);

//-----------------------------------------------------------------------------
/** Класс для ввода текстов. */
class TextReader
{
public:
	TextReader(bool isTestTexts);

	void readNextText(std::string& text);
	int size(void) const;

private:
	std::string path;
	int count;
	int current;
};

//-----------------------------------------------------------------------------
/** В ходе анализа встречается множество случайных величин. В программе измеряются их значения. Единственное, что мы можем сказать насчет этих случайных величин - некоторые их характеристики.

	Тип RandomValueCharacterization как раз обозначает характеристики случайной величины.

	Функция characterizeRandomValueByData получает эти характеристики по массиву измеренных значений случайной величины.

	Так же есть переопределение оператора << для удобного вывода характеристик случайной величины.
 */

struct RandomValueCharacterization {
	double min; // Минимальное значение
	double max; // Максимальное значение
	double expected; // Среднее значение
	double dispersion; // Среднеквадратичное отклонение
};

RandomValueCharacterization characterizeRandomValueByData(
	const std::vector<double>& data
);

std::ofstream& operator<<(std::ofstream& out, const RandomValueCharacterization& c);