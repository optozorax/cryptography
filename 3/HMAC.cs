using System;
using System.IO;
using System.Security.Cryptography;
using System.Linq;
using System.Collections;


class Program
{
    static byte[] StringToByteArray(string hash)
    {
        return Enumerable.Range(0, hash.Length)
                         .Where(x => x % 2 == 0)
                         .Select(x => Convert.ToByte(hash.Substring(x, 2), 16))
                         .ToArray();
    }


    static byte[] SignFile(string filename, byte[] key)
    {
        using (var HMAC = new HMACSHA256(key))
        {
            using (var file = new FileStream(filename, FileMode.Open))
            {
                return HMAC.ComputeHash(file);
            }
        }
    }

    static bool ValidateFile(string filename, byte[] key, byte[] expectedHash)
    {
        byte[] realHash;

        using (var HMAC = new HMACSHA256(key))
        {
            using (var file = new FileStream(filename, FileMode.Open))
            {
                realHash = HMAC.ComputeHash(file);
            }
        }

        return StructuralComparisons.StructuralEqualityComparer.Equals(realHash, expectedHash);
    }

    static void SignAndValidate()
    {
        Console.WriteLine("Enter name of a file");
        string filename = Console.ReadLine();
        Console.WriteLine("Enter the key");
        byte[] key = StringToByteArray(Console.ReadLine());
        byte[] hmac = SignFile(filename, key);

        Console.WriteLine("HMAC = {0}", BitConverter.ToString(hmac).Replace("-", "").ToLowerInvariant());


        Console.WriteLine("Enter HMAC:");
        hmac = StringToByteArray(Console.ReadLine());
        bool isVerified = ValidateFile(filename, key, hmac);

        if (isVerified)
            Console.WriteLine("HMAC matched");
        else
            Console.WriteLine("HMAC didn't matched");
    }


    static void Main(string[] args)
    {
        // Task 3
        SignAndValidate();
    }
}
