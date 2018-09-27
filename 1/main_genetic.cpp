#include <iostream>
#include "genetic.h"

int main() {
	std::string text = toRussianLetters(readFromFile("encrypted.txt"));
	std::string bestKey = decryptByEvolution(text, true);
	writeToFile("best_creature.txt", bestKey);

	Cipher cipher;
	cipher.setKey(bestKey);
	auto decrypted = cipher.decrypt(text);
	writeToFile("decrypted.txt", decrypted);

	system("pause");
}