#include "encrypt.h"

int main() {
	Cipher cipher;
	cipher.generateRandomKey();

	auto text = toRussianLetters(readFromFile("text.txt"));
	auto encryptedText = cipher.encrypt(text);
	writeToFile("key.txt", getAlphabet() + "\n" + cipher.getKey());
	writeToFile("encrypted_text.txt", encryptedText);

	Frequency f;
	f.count(encryptedText);
	f.normalize();

	f.writeSorted("encrypted_stat_sorted.txt");
}