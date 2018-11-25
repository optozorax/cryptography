using System;
using System.IO;
using System.Security.Cryptography;
using System.Linq;
using System.Collections;


public class MyHMAC : IDisposable
{
    public MyHMAC(byte[] new_key)
    {
        key = new_key;
        ipad = 0x36;
        opad = 0x5C;
        blockSize = 64;
    }

    private byte[] StringToByteArray(string str)
    {
        return Enumerable.Range(0, str.Length)
                         .Where(x => x % 2 == 0)
                         .Select(x => Convert.ToByte(str.Substring(x, 2), 16))
                         .ToArray();
    }

    private byte[] XOR(byte[] buffer1, byte buffer2)
    {
        byte[] result = new byte[blockSize];
        for (int i = 0; i < blockSize; i++)
            result[i] = (byte)(buffer1[i] ^ buffer2);
        return result;
    }

    public byte[] ComputeHash(Stream s)
    {
        byte[] text = StreamToByteArray(s);
        byte[] K = key;

        if (K.Length > blockSize)
            K = H(K);

        if (K.Length < blockSize)
        {
            byte[] newArray = new byte[blockSize];
            K.CopyTo(newArray, 0);
            for (int i = K.Length; i < blockSize; ++i)
                newArray[i] = 0;

            K = newArray;
        }

        return H(concat(XOR(K, opad), H(concat(XOR(K, ipad), text))));
    }

    private static byte[] concat(byte[] a, byte[] b)
    {
        byte[] c = new byte[a.Length + b.Length];
        a.CopyTo(c, 0);
        b.CopyTo(c, a.Length);

        return c;
    }

    private byte[] H(byte[] arr)
    {
        using (var sha256 = SHA256.Create())
            return sha256.ComputeHash(arr);
    }


    private byte[] StreamToByteArray(Stream input)
    {
        using (MemoryStream ms = new MemoryStream())
        {
            input.CopyTo(ms);
            return ms.ToArray();
        }
    }

    public void Dispose()
    {
    }

    private byte ipad;
    private byte opad;
    private int blockSize;
    private byte[] key;
}




class Program
{
    static byte[] StringToByteArray(string hash)
    {
        return Enumerable.Range(0, hash.Length)
                         .Where(x => x % 2 == 0)
                         .Select(x => Convert.ToByte(hash.Substring(x, 2), 16))
                         .ToArray();
    }

    static byte[] MySignFile(string filename, byte[] key)
    {
        using (MyHMAC HMAC = new MyHMAC(key))
        {
            using (var file = new FileStream(filename, FileMode.Open))
            {
                return HMAC.ComputeHash(file);
            }
        }
    }

    static bool MyValidateFile(string filename, byte[] key, byte[] expectedHash)
    {
        byte[] realHash;

        using (var HMAC = new MyHMAC(key))
        {
            using (var file = new FileStream(filename, FileMode.Open))
            {
                realHash = HMAC.ComputeHash(file);
            }
        }

        return StructuralComparisons.StructuralEqualityComparer.Equals(realHash, expectedHash);
    }

    static void MySignAndValidate()
    {
        Console.WriteLine("Enter name of a file");
        string filename = Console.ReadLine();
        Console.WriteLine("Enter the key");
        string str = Console.ReadLine();
        byte[] key = StringToByteArray(str);
        byte[] hmac = MySignFile(filename, key);

        Console.WriteLine("HMAC = {0}", BitConverter.ToString(hmac).Replace("-", "").ToLowerInvariant());


        Console.WriteLine("Enter HMAC:");
        hmac = StringToByteArray(Console.ReadLine());
        bool isVerified = MyValidateFile(filename, key, hmac);

        if (isVerified)
            Console.WriteLine("HMAC matched");
        else
            Console.WriteLine("HMAC didn't matched");
    }




    static void Main(string[] args)
    {
        // Task 4
        MySignAndValidate();
    }
}