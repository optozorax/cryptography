/** ОПИСАНИЕ
		Расшифровывает заданные тексты при помощи эволюции.
		Текстов может и не быть, тогда не расшифровывает.
		Выводит в файлы лучший найденный ключ и расшифрованный по этому ключу текст. 
 */

#include <iostream>
#include "genetic.h"

int main() {
	for (int i = 0; i < 9; i++) {
		std::string text = toRussianLetters(readFromFile("encrypted_" + std::to_string(i+1) + ".txt"));

		if (text.size() == 0)
			continue;

		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		std::cout << "DECRYPT " << i+1 << " TEXT" << std::endl;

		std::string bestKey = decryptByEvolution(text, true);
		writeToFile("best_key_" + std::to_string(i+1) + ".txt", bestKey);

		Cipher cipher;
		cipher.setKey(bestKey);
		auto decrypted = cipher.decrypt(text);
		writeToFile("decrypted_" + std::to_string(i+1) + ".txt", decrypted);
	}
}