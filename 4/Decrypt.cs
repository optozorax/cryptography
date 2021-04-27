using System;
using System.IO;
using System.Security.Cryptography;



class Decrypt
{
    static RSA rsa;
    static Aes aes;

    static void DecryptPhoto()
    {
        Console.WriteLine("Введите название файла, содержащего приватный ключ в формате XML:");
        string private_key = Console.ReadLine();

        Console.WriteLine("Введите название файла, который нужно расшифровать:");
        string pic_file = Console.ReadLine();

        Console.WriteLine("Введите название файла, где находится зашифрованный ключ:");
        string key_file = Console.ReadLine();

        Console.WriteLine("Введите название файла, где находится зашифрованное IV:");
        string iv_file = Console.ReadLine();

        rsa.FromXmlString(File.ReadAllText(private_key));

        byte[] pictureToDecrypt = File.ReadAllBytes(pic_file);
        byte[] decryptedPicture;

        aes.Key = rsa.Decrypt(File.ReadAllBytes(key_file), RSAEncryptionPadding.Pkcs1);
        aes.IV = rsa.Decrypt(File.ReadAllBytes(iv_file), RSAEncryptionPadding.Pkcs1);

        // Обработка ошибок
        if (pictureToDecrypt == null || pictureToDecrypt.Length <= 0)
            throw new ArgumentNullException("pictureToDecrypt");
        if (aes.Key == null || aes.Key.Length <= 0)
            throw new ArgumentNullException("Key");
        if (aes.IV == null || aes.IV.Length <= 0)
            throw new ArgumentNullException("IV");

        var decryptor = aes.CreateDecryptor(aes.Key, aes.IV);
        using (MemoryStream msDecrypt = new MemoryStream())
        {
            using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Write))
            {
                csDecrypt.Write(pictureToDecrypt, 0, pictureToDecrypt.Length);
                csDecrypt.FlushFinalBlock();
                decryptedPicture = msDecrypt.ToArray();
            }
        }

        File.WriteAllBytes("Decrypted.jpg", decryptedPicture);
        Console.WriteLine("Файл Decrypted.jpg сохранён.");
    }

    static void Main(string[] args)
    {
        // Инициализация
        rsa = RSA.Create();
        aes = Aes.Create();

        DecryptPhoto();

        Console.ReadKey();
    }
}