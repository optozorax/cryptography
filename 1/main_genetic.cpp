#include <iostream>
#include "genetic.h"

int main() {
	std::string text = toRussianLetters(readFromFile("encrypted.txt"));

	Evolution ev(50, 10, &text);
	for (int i = 0; i < 1000; ++i) {
		ev.calcGeneration();
		std::cout 
			<< ev.getGenerationsCount() 
			<< "\t" 
			<< ev.getBestCreature().getEfficiency() 
			<< std::endl;
	}
	std::string bestKey = ev.getBestCreature().getKey();
	
	writeToFile("best_creature.txt", bestKey);

	Cipher cipher;
	cipher.setKey(bestKey);
	auto decrypted = cipher.decrypt(text);

	writeToFile("decrypted.txt", decrypted);

	system("pause");
}