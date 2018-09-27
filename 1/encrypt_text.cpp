/** ОПИСАНИЕ
		Эта программа зашифровывает текст text.txt случайным ключом и выводит в файл как ключ, так и зашифрованный текст.
 */

#include "encrypt.h"

int main() {
	srand(153);

	Cipher cipher;
	cipher.generateRandomKey();

	auto text = toRussianLetters(readFromFile("text.txt"));
	auto encryptedText = cipher.encrypt(text);
	writeToFile("key.txt", getAlphabet() + "\n" + cipher.getKey());
	writeToFile("encrypted.txt", encryptedText);
}