using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography;


class PutASignature
{
    static RSA rsa;
    
    static void ReadKeysFromFiles()
    {
        rsa.FromXmlString(File.ReadAllText("private_key_XML.txt"));
    }
    
    static void SignFile()
    {
        Console.WriteLine("Введите название файла, который нужно подписать:");
        string fileName = Console.ReadLine();
        byte[] fileToSign = File.ReadAllBytes(fileName);
        byte[] signature = rsa.SignData(fileToSign, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
        byte[] signedFile = new byte[fileToSign.Length + signature.Length];
        fileToSign.CopyTo(signedFile, 0);
        signature.CopyTo(signedFile, fileToSign.Length);

        File.WriteAllBytes("sign_for_" + fileName, signature);      // Отдельно подпись
        File.WriteAllBytes("signed_" + fileName, signedFile);    // И файл с подписью в конце

        Console.WriteLine("Файл подписан.");
    }
    
    static void Main(string[] args)
    {
        // Инициализация
        rsa = RSA.Create();

        ReadKeysFromFiles();

        SignFile();
    }
}