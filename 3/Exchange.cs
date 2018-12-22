using System;
using System.IO;
using System.Security.Cryptography;
using System.Linq;

class Program
{
    static byte[] StringToByteArray(string hash)
    {
        return Enumerable.Range(0, hash.Length)
                         .Where(x => x % 2 == 0)
                         .Select(x => Convert.ToByte(hash.Substring(x, 2), 16))
                         .ToArray();
    }

    static void keysExchenge()
    {
        byte[] inputBuffer = new byte[1024];
        Stream inputStream = Console.OpenStandardInput(inputBuffer.Length);
        Console.SetIn(new StreamReader(inputStream, Console.InputEncoding, false, inputBuffer.Length));

        byte[] OurPublicKey;
        byte[] TheirPublicKey;

        using (var ecd = new ECDiffieHellmanCng())
        {
            ecd.KeyDerivationFunction = ECDiffieHellmanKeyDerivationFunction.Hash;
            ecd.HashAlgorithm = CngAlgorithm.Sha256;
            OurPublicKey = ecd.PublicKey.ToByteArray();
            Console.WriteLine("Our public key: {0}", BitConverter.ToString(OurPublicKey).Replace("-", "").ToLowerInvariant());

            Console.Write("Input their public key:");
            string str = Console.ReadLine();
            TheirPublicKey = StringToByteArray(str);
            byte[] ourSecret = ecd.DeriveKeyMaterial(CngKey.Import(TheirPublicKey, CngKeyBlobFormat.EccPublicBlob));
            Console.WriteLine("Our secret key: {0}", BitConverter.ToString(ourSecret).Replace("-", "").ToLowerInvariant());
        }
    }

    static void Main(string[] args)
    {
        // Task 2
        keysExchenge();
    }
}