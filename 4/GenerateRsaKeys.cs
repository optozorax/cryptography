using System;
using System.IO;
using System.Security.Cryptography;

class GenerateKeys
{
    static RSA rsa;
    static string to_string(byte[] data)
    {
        return BitConverter.ToString(data).Replace("-", "").ToLowerInvariant();
    }

    static void WriteKeysToFiles()
    {
        // Сначала сохраним XML ключи
        File.WriteAllText("private_key_XML.txt", rsa.ToXmlString(true));
        File.WriteAllText("public_key_XML.txt", rsa.ToXmlString(false));

        string Modulus, Exponent, d, p, q;

        var parameters = rsa.ExportParameters(true);
        Modulus = to_string(parameters.Modulus);
        Exponent = to_string(parameters.Exponent);
        d = to_string(parameters.D);
        p = to_string(parameters.P);
        q = to_string(parameters.Q);


        string privateKeyFile = "private_key.txt",
               publicKeyFile = "public_key.txt";
  
        File.WriteAllText(privateKeyFile, "Private exponent (d):\n" + d);
        File.AppendAllText(privateKeyFile, "\nModulus (n):\n" + Modulus);
        File.AppendAllText(privateKeyFile, "\nP (p):\n" + p);
        File.AppendAllText(privateKeyFile, "\nQ (q):\n" + q);

        File.WriteAllText(publicKeyFile, "Exponent (e):\n" + Exponent);
        File.AppendAllText(publicKeyFile, "\nModulus (n):\n" + Modulus);

    }

    static void Main()
    {
        // Инициализация
        rsa = RSA.Create();

        WriteKeysToFiles();
    }
}