#include <iostream>
#include "genetic.h"

int main() {
	for (int i = 0; i < 9; i++) {
		std::string text = toRussianLetters(readFromFile("encrypted_" + std::to_string(i+1) + ".txt"));

		if (text.size() == 0)
			continue;

		std::string bestKey = decryptByEvolution(text, true);
		writeToFile("best_creature_" + std::to_string(i+1) + ".txt", bestKey);

		Cipher cipher;
		cipher.setKey(bestKey);
		auto decrypted = cipher.decrypt(text);
		writeToFile("decrypted_" + std::to_string(i+1) + ".txt", decrypted);

		system("pause");
	}
}