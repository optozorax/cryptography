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
    //--------------------------------------------------------------------------------
    // Task 1
    public static string CalculateMD5(string filename)
    {
        using (var md5 = MD5.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = md5.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    public static string CalculateSHA1(string filename)
    {
        using (var sha1 = SHA1.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = sha1.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    public static string CalculateSHA256(string filename)
    {
        using (var sha256 = SHA256.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = sha256.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    public static string CalculateSHA512(string filename)
    {
        using (var sha512 = SHA512.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = sha512.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }



    //--------------------------------------------------------------------------------
    // Task 2
    static byte[] StringToByteArray(string hash)
    {
        return Enumerable.Range(0, hash.Length)
                         .Where(x => x % 2 == 0)
                         .Select(x => Convert.ToByte(hash.Substring(x, 2), 16))
                         .ToArray();
    }

    static void KeysExchenge()
    {
        byte[] inputBuffer = new byte[1024];
        Stream inputStream = Console.OpenStandardInput(inputBuffer.Length);
        Console.SetIn(new StreamReader(inputStream, Console.InputEncoding, false, inputBuffer.Length));

        byte[] OurPublicKey;
        byte[] TheirPublicKey;

        using (var ec = new ECDiffieHellmanCng())
        {
            ec.KeyDerivationFunction = ECDiffieHellmanKeyDerivationFunction.Hash;
            ec.HashAlgorithm = CngAlgorithm.Sha256;
            OurPublicKey = ec.PublicKey.ToByteArray();
            Console.WriteLine("Our public key: {0}", BitConverter.ToString(OurPublicKey).Replace("-", "").ToLowerInvariant());

            Console.Write("Input their public key:");
            string str = Console.ReadLine();
            TheirPublicKey = StringToByteArray(str);
            byte[] sendKey = ec.DeriveKeyMaterial(CngKey.Import(TheirPublicKey, CngKeyBlobFormat.EccPublicBlob));
            Console.WriteLine("Our secret key: {0}", BitConverter.ToString(OurPublicKey).Replace("-", "").ToLowerInvariant());
        }
    }



    //--------------------------------------------------------------------------------
    // Task 3
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
        //string filename = Console.ReadLine();
        string filename = "npp.7.3.Installer.x64.exe";
        Console.WriteLine("Enter the key");
        //string str = Console.ReadLine();
        string kstr = "857a71b794695839181706c689b36f23a8ffa64eb060b49cb860a2dfe8315e38";
        byte[] key = StringToByteArray(kstr);
        //Console.WriteLine("Enter name of a file");
        //string filename = Console.ReadLine();
        //Console.WriteLine("Enter the key");
        //byte[] key = StringToByteArray(Console.ReadLine());
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



    //--------------------------------------------------------------------------------
    // Task 4
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
        //string filename = Console.ReadLine();
        string filename = "npp.7.3.Installer.x64.exe";
        Console.WriteLine("Enter the key");
        //string str = Console.ReadLine();
        string kstr = "857a71b794695839181706c689b36f23a8ffa64eb060b49cb860a2dfe8315e38";
        byte[] key = StringToByteArray(kstr);
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
        // Task 1
        //string filename = "npp.7.3.Installer.x64.exe";
        //Console.WriteLine("MD5 checksum: " + CalculateMD5(filename));
        //Console.WriteLine("SHA-1 checksum: " + CalculateSHA1(filename));
        //Console.WriteLine("SHA256 checksum: " + CalculateSHA256(filename));
        //Console.WriteLine("SHA512 checksum: " + CalculateSHA512(filename));


        // Task 2
        //KeysExchenge();


        // Task 3
        //SignAndValidate();

        // npp.7.3.Installer.x64.exe
        // Task 4
        MySignAndValidate();
    }
}