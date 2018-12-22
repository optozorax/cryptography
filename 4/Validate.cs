using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography;

class Validate
{
    static RSA rsa;
    
    static void ReadKeysFromFiles()
    {
        rsa.FromXmlString(File.ReadAllText("private_key_XML.txt"));
    }

    static void ValidateFile()
    {
        int signatureSize = 128;
        Console.WriteLine("Введите название файла, который нужно проверить:");
        string sourceFileName = Console.ReadLine();
        byte[] sourceFile = File.ReadAllBytes(sourceFileName);
        byte[] signature = new byte[signatureSize];

        Array.Copy(sourceFile, sourceFile.Length - signatureSize, signature, 0, signatureSize);
        Array.Resize(ref sourceFile, sourceFile.Length - signatureSize);
                
        if (rsa.VerifyData(sourceFile, signature, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1))
            Console.WriteLine("Подпись верна.");
        else
            Console.WriteLine("Подпись не совпала");
    }


    static void Main(string[] args)
    {
        // Инициализация
        rsa = RSA.Create();

        ReadKeysFromFiles();

        ValidateFile();
    }
}
