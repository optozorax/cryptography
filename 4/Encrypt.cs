using System;
using System.IO;
using System.Security.Cryptography;



class Encrypt
{
    static RSA rsa;
    static Aes aes;

    // Шифруем фото при помощи AES, а ключи AES при помощи RSA 
    static void EncryptPhoto()
    {
        Console.WriteLine("Введите название файла, который нужно зашифровать:");
        string fileName = Console.ReadLine();

        byte[] pictureToEncrypt = File.ReadAllBytes(fileName);
        byte[] encryptedPicture;

        // Обработка ошибок
        if (pictureToEncrypt == null || pictureToEncrypt.Length <= 0)
            throw new ArgumentNullException("pictureToEncrypt");
        if (aes.Key == null || aes.Key.Length <= 0)
            throw new ArgumentNullException("Key");
        if (aes.IV == null || aes.IV.Length <= 0)
            throw new ArgumentNullException("IV");


        var encryptor = aes.CreateEncryptor(aes.Key, aes.IV);
        using (MemoryStream msEncrypt = new MemoryStream())
        {
            using (CryptoStream csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write))
            {
                csEncrypt.Write(pictureToEncrypt, 0, pictureToEncrypt.Length);
                csEncrypt.FlushFinalBlock();
                encryptedPicture = msEncrypt.ToArray();
            }
        }

        byte[] aesEncryptedKey = rsa.Encrypt(aes.Key, RSAEncryptionPadding.Pkcs1);
        byte[] aesEncryptedIV = rsa.Encrypt(aes.IV, RSAEncryptionPadding.Pkcs1);

        
        File.WriteAllBytes("pic_encryted_with_AES.txt", encryptedPicture);
        Console.WriteLine("Файл pic_encryted_with_AES.txt сохранён.");
        File.WriteAllBytes("AES_key_encrypted_with_RSA.txt", aesEncryptedKey);
        Console.WriteLine("Файл AES_key_encrypted_with_RSA.txt сохранён.");
        File.WriteAllBytes("AES_IV_encrypted_with_RSA.txt", aesEncryptedIV);
        Console.WriteLine("Файл AES_IV_encrypted_with_RSA.txt сохранён.");
    }


    static void Main(string[] args)
    {
        // Инициализация
        rsa = RSA.Create();
        aes = Aes.Create();

        //aes.GenerateKey();
        //aes.GenerateIV();
        //File.WriteAllBytes("AES_key.txt", aes.Key);
        //File.WriteAllBytes("AES_IV.txt", aes.IV);

        Console.WriteLine("Введите название файла с чужим публичным ключом в формате XML:");
        string another_public_key = Console.ReadLine();

        Console.WriteLine("Введите название файла с ключом AES которым надо зашифровать:");
        string aes_key = Console.ReadLine();

        Console.WriteLine("Введите название файла с IV AES:");
        string iv_key = Console.ReadLine();

        rsa.FromXmlString(File.ReadAllText(another_public_key));
        aes.Key = File.ReadAllBytes(aes_key);
        aes.IV = File.ReadAllBytes(iv_key);

        EncryptPhoto();

        Console.ReadKey();
    }
}